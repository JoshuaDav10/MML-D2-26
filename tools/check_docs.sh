#!/usr/bin/env bash
# Doc-consistency gate. Catches the error classes that the 2026-07-25 audit found
# NOBODY had caught: stale count claims and write-only docs drifting from reality.
#
# Cheap (no rebuild) — run it before any commit that claims a count, and at session start.
# Exits nonzero if any doc disagrees with the built objects.
#
# It does NOT rebuild; run tools/audit_count.sh first if the build may be stale.
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
source .venv/bin/activate 2>/dev/null || true

fail=0
say() { printf '%s\n' "$*"; }
bad() { printf '  DEFECT: %s\n' "$*"; fail=1; }

N=$(python3 tools/census.py --matched 2>/dev/null | grep -oE '[0-9]+ matched' | grep -oE '[0-9]+')
S=$(python3 tools/census.py --matched 2>/dev/null | grep -oE '[0-9]+ active stubs' | grep -oE '[0-9]+')
if [ -z "$N" ]; then say "cannot get census (build first: make CPP=cpp build_rock_neo_only)"; exit 2; fi
say "authoritative: $N matched, $S active stubs"

# 1. Headline claims must equal N.
for f in progress.md HANDOFF.md activity.md; do
  [ -f "$f" ] || continue
  head -3 "$f" | grep -oE '\*\*[0-9]{3}\*\* matched|[0-9]{3} matched' | grep -oE '[0-9]{3}' | while read -r c; do
    [ "$c" = "$N" ] || echo "  DEFECT: $f headline says $c, authoritative is $N"
  done
done
head -3 HANDOFF.md 2>/dev/null | grep -qE "\b$N\b" || bad "HANDOFF.md title does not carry the authoritative count $N"
grep -qE "rock_neo main: \*\*$N\*\*" progress.md 2>/dev/null || bad "progress.md 'rock_neo main' count != $N"

# 2. Stub count claims.
if grep -qE '\*\*[0-9]+\*\* active INCLUDE_ASM stubs' progress.md 2>/dev/null; then
  c=$(grep -oE '\*\*[0-9]+\*\* active INCLUDE_ASM stubs' progress.md | grep -oE '[0-9]+' | head -1)
  [ "$c" = "$S" ] || bad "progress.md says $c active stubs, authoritative is $S"
fi

# 3. A "verified/authoritative" banner must not carry a stale number.
awk '/verified|authoritative|not aspirational/{ctx=NR} ctx && NR<=ctx+12 && /Matched: [0-9]{3}/{print NR": "$0}' HANDOFF.md 2>/dev/null \
  | grep -oE 'Matched: [0-9]{3}' | grep -oE '[0-9]{3}' | while read -r c; do
      [ "$c" = "$N" ] || echo "  DEFECT: HANDOFF.md 'verified' section states Matched: $c (authoritative $N)"
    done

# 4. COUNTS.md freshness — it exists only to hold numbers, so stale == wrong.
if [ -f notes/COUNTS.md ]; then
  cd_last=$(git log -1 --format=%cd --date=short -- notes/COUNTS.md 2>/dev/null)
  src_last=$(git log -1 --format=%cd --date=short -- src/rock_neo 2>/dev/null)
  grep -qE "\b$N\b" notes/COUNTS.md || bad "notes/COUNTS.md has no $N (last touched $cd_last, src last $src_last) — it is the count-reconciliation doc; refresh or delete it"
fi

# 5. Re-inflation trap: stubs shadowed by an active #else body read as +1 to a naive grep.
raw=$(grep -c INCLUDE_ASM src/rock_neo/*.c 2>/dev/null | awk -F: '{s+=$2} END{print s}')
if [ -n "$raw" ] && [ "$raw" != "$S" ]; then
  say "note: raw grep sees $raw stubs vs $S real — $((raw-S)) are shadowed by an active #else."
  say "      Un-gating any of those is a NO-OP, not a match (this caused the 2026-07-19 inflation)."
fi

[ "$fail" = 0 ] && say "docs consistent" || say "DOCS INCONSISTENT — fix before claiming progress"
exit $fail
