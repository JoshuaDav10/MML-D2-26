#!/usr/bin/env bash
# ADVERSARIAL AUDIT — the DEEP tier. Deliberately NOT the session-start hook (see below).
# Run on demand, or after a batch of claimed matches.
#
# Written 2026-07-26 after an audit found BOTH existing gates exited 0 on failure and the
# project's completion denominator had excluded 64.6% of the binary for months. A gate only
# catches what it checks, and a broken gate is worse than none: it manufactures confidence.
#
#   A. SELF-TEST the gates (prove they still FAIL when they should)
#   B. ACCOUNTING completeness (is every byte of the target attributed?)
#   C. Ask what is NOT being measured
#
# 2026-07-26 FIXES — this script was itself guilty of three defects it exists to hunt:
#   (1) DESTRUCTIVE. Its self-test sed-copied audit_count.sh and EXECUTED it — including
#       the `rm -rf build` and full `make`. Wired to SessionStart, that nuked and rebuilt
#       build/ at EVERY session start. Now passes AUDIT_SKIP_BUILD=1.
#   (2) IT MUTATED HANDOFF.md. It appended a fake "999 matched" line, backed the file up to
#       a fixed /tmp path, then restored. A session killed mid-audit — precisely the
#       usage-limit case — left the fake line in the tree. Now runs check_docs.sh against a
#       throwaway DOC_ROOT copy and asserts the real file is untouched.
#   (3) PARTIAL DEMONSTRATION. It injected "999 matched", a string shape occurring NOWHERE
#       in the real docs, and asserted only `$? -ne 0`. So a BUILD FAILURE (gate never
#       reached) scored as "ok: the gate works". Now it injects a realistic edit in the
#       real generated-block format and requires exit code exactly 1 plus the right message.
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
source .venv/bin/activate 2>/dev/null || true
FAIL=0
TMP=$(mktemp -d); trap 'rm -rf "$TMP" tools/_selftest_ac.sh' EXIT INT TERM
hdr() { printf '\n=== %s ===\n' "$1"; }
bad() { printf '  FAIL: %s\n' "$1"; FAIL=1; }
ok()  { printf '  ok: %s\n' "$1"; }

HANDOFF_BEFORE=$(md5sum HANDOFF.md 2>/dev/null | cut -d' ' -f1)

hdr "A. GATE SELF-TEST (do the gates still fail when they should?)"

# --- audit_count.sh must exit 1 (gate failure, not 2 = build failure) AND say why.
if [ -f tools/audit_count.sh ]; then
  sed 's|^CMP=.*|CMP="DIFFERS"  # self-test injection|' tools/audit_count.sh > tools/_selftest_ac.sh
  chmod +x tools/_selftest_ac.sh
  out=$(AUDIT_SKIP_BUILD=1 tools/_selftest_ac.sh 2>&1); rc=$?
  if [ "$rc" = 1 ] && printf '%s' "$out" | grep -q 'raw cmp is not byte-identical'; then
    ok "audit_count.sh exits 1 with the correct reason on a bad cmp"
  else
    bad "audit_count.sh did not fail correctly on a bad cmp (rc=$rc) — it is not a gate"
  fi
  rm -f tools/_selftest_ac.sh
fi

# --- check_docs.sh must exit 1 on a REALISTIC stale count, against a throwaway copy.
if [ -f tools/check_docs.sh ]; then
  mkdir -p "$TMP/notes" "$TMP/.claude/agents"
  for f in progress.md HANDOFF.md CLAUDE.md; do [ -f "$f" ] && cp "$f" "$TMP/$f"; done
  for f in notes/COUNTS.md notes/LESSONS.md notes/WORK_MAP.md notes/STRATEGY.md; do
    [ -f "$f" ] && cp "$f" "$TMP/$f"; done
  cp .claude/agents/*.md "$TMP/.claude/agents/" 2>/dev/null || true
  cur=$(tools/gen_counts.sh --json 2>/dev/null | python3 -c 'import json,sys;print(json.load(sys.stdin)["matched"])' 2>/dev/null)
  if [ -n "${cur:-}" ]; then
    # Realistic bait: bump the matched number INSIDE the real generated-block format —
    # the way an actually-stale doc looks, not a synthetic string the format never produces.
    sed -i "s/| matched functions | \*\*$cur\*\* |/| matched functions | **$((cur+1))** |/" "$TMP/HANDOFF.md"
    out=$(DOC_ROOT="$TMP" tools/check_docs.sh 2>&1); rc=$?
    if [ "$rc" = 1 ] && printf '%s' "$out" | grep -q 'generated block is stale'; then
      ok "check_docs.sh exits 1 on a realistic stale count"
    else
      bad "check_docs.sh did not fail on a realistic stale count (rc=$rc) — it is not a gate"
    fi
  else
    bad "could not read the current count — build missing; self-test could not run"
  fi
  # Compare against a snapshot taken BEFORE the self-test, not against HEAD: the question
  # is "did the audit touch it?", not "does the working tree have edits?". Using `git diff`
  # here flagged the user's own legitimate uncommitted work as an audit bug.
  now=$(md5sum HANDOFF.md 2>/dev/null | cut -d' ' -f1)
  [ "$now" = "$HANDOFF_BEFORE" ] \
    && ok "self-test left the real HANDOFF.md byte-identical" \
    || bad "self-test MODIFIED HANDOFF.md — the destructive-audit bug is back"
fi

hdr "B. ACCOUNTING COMPLETENESS (is the whole target attributed?)"
if ! command -v mipsel-elf-size >/dev/null 2>&1 || ! command -v mipsel-elf-objdump >/dev/null 2>&1; then
  bad "mipsel-elf binutils absent — section B cannot attribute bytes (it would report 0 and divide by zero)"
else
  C_TXT=$(mipsel-elf-size -A build/src/rock_neo/*.o 2>/dev/null | awk '/^\.text/{s+=$2} END{print s+0}')
  A_TXT=$(mipsel-elf-size -A build/asm/rock_neo/*.o 2>/dev/null | awk '/^\.text/{s+=$2} END{print s+0}')
  C_FN=$(mipsel-elf-objdump -t build/src/rock_neo/*.o 2>/dev/null | grep -c " F \.text")
  A_FN=$(grep -h '^glabel' asm/rock_neo/*.s 2>/dev/null | wc -l | tr -d ' ')
  MATCHED=$(tools/gen_counts.sh --json 2>/dev/null | python3 -c 'import json,sys;print(json.load(sys.stdin)["matched"])' 2>/dev/null)
  TOT_FN=$(( C_FN + A_FN )); TOT_TXT=$(( C_TXT + A_TXT ))
  printf "  C TUs      : %5s fns  %8s bytes\n" "$C_FN" "$C_TXT"
  printf "  raw asm    : %5s fns  %8s bytes   <- NEVER split into per-function files\n" "$A_FN" "$A_TXT"
  printf "  TOTAL      : %5s fns  %8s bytes\n" "$TOT_FN" "$TOT_TXT"
  if [ "${C_FN:-0}" -eq 0 ] || [ -z "${MATCHED:-}" ]; then
    bad "no build present — the completion math would be meaningless (this used to print 0.0% silently)"
  else
    printf "  COMPLETION : %s%% of the whole exe (%s%% of the C-mapped slice only)\n" \
      "$(awk "BEGIN{printf \"%.1f\", 100*$MATCHED/$TOT_FN}")" \
      "$(awk "BEGIN{printf \"%.1f\", 100*$MATCHED/$C_FN}")"
    grep -q "$TOT_FN" notes/COUNTS.md 2>/dev/null \
      && ok "notes/COUNTS.md carries the true denominator $TOT_FN" \
      || bad "notes/COUNTS.md omits the true denominator $TOT_FN"
  fi
fi

hdr "C. WHAT IS NOT BEING MEASURED (answer these out loud each run)"
cat <<'Q'
  1. Is the denominator defined by the TARGET BINARY, or by whatever a tool happens to
     read? (The 2026-07-26 miss: census.py reads only the C objects, so 64.6% of the
     game was structurally invisible to every count this project ever produced.)
  2. Has any doc claimed a number that no script regenerates? Hand-written numbers rot.
     (Counts now live in notes/COUNTS.md, GENERATED by tools/gen_counts.sh and byte-diffed.)
  3. Has any gate been added since the last audit WITHOUT a negative test proving it
     fails? An untested gate is a confidence generator, not a check.
  4. Which claims made to the user in the last session were NOT adjudicated by a
     mechanical oracle? Those are where every unnoticed error has lived (20/20 binary
     claims were self-caught; 0/6 progress/process claims were).
  5. Is any "impossible / exhausted" claim older than one session still unchallenged?
     Two such claims have already been overturned by one external idiom.
Q

hdr "VERDICT"
[ "$FAIL" = 0 ] && echo "  gates demonstrably work; accounting complete" \
                || echo "  AUDIT FAILED — fix the above before reporting any progress"
exit $FAIL
