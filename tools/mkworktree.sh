#!/usr/bin/env bash
# Create a BUILDABLE git worktree so several agents can work without racing.
#
#     tools/mkworktree.sh <branch-name> [engine|all]
#
# Why this is not just `git worktree add`: everything a build needs is gitignored.
# disks/ (949M), asm/ (319M), assets/, build/, *.ld and .venv are all absent from a fresh
# checkout, so the worktree cannot compile anything. This wires them up:
#   disks/   symlink  — read-only game image, safe to share, never copied
#   .venv/   symlink  — same interpreter, same packages
#   assets/  symlink  — small, read-only during a build
#   asm/     OWN COPY — splat rewrites it, so sharing it would let one agent corrupt
#                       every other agent's tree (this exact failure happened 2026-07-26)
#   build/   OWN      — one shared build dir means concurrent `make` races
#
# Each worktree is fully independent: it can `make CPP=cpp check_rock_neo_only` and get a
# real byte-for-byte answer that says nothing about anyone else's tree.
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
ROOT=$PWD
BR=${1:?usage: mkworktree.sh <branch> [engine|all]}
SCOPE=${2:-engine}
# CC_FLAGS carries -gcoff, which embeds the ABSOLUTE SOURCE PATH in every C object. A
# worktree at a different-length path therefore produces a different-sized binary and can
# NEVER match — measured 2026-07-28: a 5-character-longer path shifted the exe by 172
# bytes and every C object by 32. So the worktree directory name must be EXACTLY as long
# as the main one. Pass a 2-char tag; the name becomes MML-WT-xx, same length as MML-D2-26.
TAG=${BR##*-}
[ ${#TAG} -eq 2 ] || { echo "branch must end in a 2-char tag (e.g. wt-a1) so the path length matches"; exit 1; }
WT="$(dirname "$ROOT")/MML-WT-$TAG"
[ ${#WT} -eq ${#ROOT} ] || { echo "path length mismatch: $WT (${#WT}) vs $ROOT (${#ROOT}) — would never match"; exit 1; }

[ -d "$WT" ] && { echo "worktree already exists: $WT"; exit 0; }

git worktree add -q -b "$BR" "$WT" HEAD || exit 1
cd "$WT"

# `ln -sfn X dir` when dir EXISTS puts the link INSIDE it. disks/ is partially tracked
# (.gitignore keeps disks/README.md) so git materialises it as a real directory — link the
# payload subdir instead of the parent. Use -T elsewhere to refuse the nest-inside trap.
mkdir -p disks
ln -sfnT "$ROOT/disks/us" disks/us
for d in assets .venv; do rm -rf "$d"; ln -sfnT "$ROOT/$d" "$d"; done
mkdir -p build logs

# Submodules are not populated in a new worktree; symlink the parent's checkouts.
for s in tools/splat tools/maspx tools/asm-differ; do
  [ -e "$s/.git" ] || { rm -rf "$s"; ln -sfn "$ROOT/$s" "$s"; }
done

echo "  splitting asm (own copy, so splat here cannot corrupt other trees)..."
source .venv/bin/activate 2>/dev/null
python3 tools/splat/split.py config/splat.us.rock_neo.yaml >logs/split.log 2>&1 \
  || { echo "  SPLIT FAILED — see $WT/logs/split.log"; exit 1; }
if [ "$SCOPE" = all ]; then
  for y in config/overlay/*/*.yaml; do python3 tools/splat/split.py "$y" >>logs/split.log 2>&1; done
fi

echo "  building..."
make CPP=cpp build_rock_neo_only >logs/build.log 2>&1 \
  || { echo "  BUILD FAILED — see $WT/logs/build.log"; tail -5 logs/build.log; exit 1; }
if make CPP=cpp check_rock_neo_only 2>&1 | tail -1 | grep -q OK; then
  echo "  READY: $WT  (branch $BR, baseline hash OK)"
else
  echo "  WARNING: $WT built but does NOT match — do not trust results from it"; exit 1
fi
