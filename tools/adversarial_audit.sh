#!/usr/bin/env bash
# ADVERSARIAL AUDIT — run at session start, and after every ~5 claimed matches.
#
# Written 2026-07-26 after an audit found that BOTH existing gates exited 0 on failure
# and that the project's completion denominator had excluded 64.6% of the binary for
# months. The lesson: a gate only catches what it checks, and a broken gate is worse
# than no gate because it manufactures confidence.
#
# So this script does three things the others don't:
#   A. SELF-TEST the gates (prove they still FAIL when they should)
#   B. ACCOUNTING completeness (is every byte of the target attributed to something?)
#   C. Ask what is NOT being measured
#
# Exit 0 only if everything is attributable and the gates demonstrably work.
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
source .venv/bin/activate 2>/dev/null || true
FAIL=0
hdr() { printf '\n=== %s ===\n' "$1"; }
bad() { printf '  FAIL: %s\n' "$1"; FAIL=1; }
ok()  { printf '  ok: %s\n' "$1"; }

hdr "A. GATE SELF-TEST (do the gates still fail when they should?)"
# audit_count.sh must exit nonzero when raw cmp differs.
if [ -f tools/audit_count.sh ]; then
  sed 's|^CMP=.*|CMP="DIFFERS"  # self-test injection|' tools/audit_count.sh > tools/_selftest_ac.sh
  chmod +x tools/_selftest_ac.sh
  tools/_selftest_ac.sh >/dev/null 2>&1
  [ $? -ne 0 ] && ok "audit_count.sh fails on a bad cmp" || bad "audit_count.sh EXITS 0 ON A BAD CMP — it is not a gate"
  rm -f tools/_selftest_ac.sh
fi
# check_docs.sh must exit nonzero on an injected stale count.
if [ -f tools/check_docs.sh ] && [ -f HANDOFF.md ]; then
  cp HANDOFF.md /tmp/_hb.md
  printf '\n> self-test injection: 999 matched\n' >> HANDOFF.md
  tools/check_docs.sh >/dev/null 2>&1
  [ $? -ne 0 ] && ok "check_docs.sh fails on an injected stale count" || bad "check_docs.sh EXITS 0 ON A STALE COUNT — it is not a gate"
  cp /tmp/_hb.md HANDOFF.md; rm -f /tmp/_hb.md
fi

hdr "B. ACCOUNTING COMPLETENESS (is the whole target attributed?)"
C_TXT=$(mipsel-elf-size -A build/src/rock_neo/*.o 2>/dev/null | awk '/^\.text/{s+=$2} END{print s+0}')
A_TXT=$(mipsel-elf-size -A build/asm/rock_neo/*.o 2>/dev/null | awk '/^\.text/{s+=$2} END{print s+0}')
C_FN=$(mipsel-elf-objdump -t build/src/rock_neo/*.o 2>/dev/null | grep -c " F \.text")
A_FN=$(grep -h '^glabel' asm/rock_neo/*.s 2>/dev/null | wc -l | tr -d ' ')
MATCHED=$(python3 tools/census.py --matched 2>/dev/null | grep -oE '[0-9]+ matched' | grep -oE '[0-9]+')
TOT_FN=$(( C_FN + A_FN )); TOT_TXT=$(( C_TXT + A_TXT ))
printf "  C TUs      : %5s fns  %8s bytes\n" "$C_FN" "$C_TXT"
printf "  raw asm    : %5s fns  %8s bytes   <- NEVER split into per-function files\n" "$A_FN" "$A_TXT"
printf "  TOTAL      : %5s fns  %8s bytes\n" "$TOT_FN" "$TOT_TXT"
if [ "$A_FN" -gt 0 ]; then
  pct_fn=$(awk "BEGIN{printf \"%.1f\", 100*$MATCHED/$TOT_FN}")
  pct_slice=$(awk "BEGIN{printf \"%.1f\", 100*$MATCHED/$C_FN}")
  printf "  COMPLETION : %s%% of the whole exe (%s%% of the C-mapped slice only)\n" "$pct_fn" "$pct_slice"
  grep -q "$TOT_FN" notes/COUNTS.md 2>/dev/null \
    && ok "notes/COUNTS.md carries the true denominator $TOT_FN" \
    || bad "notes/COUNTS.md omits the true denominator $TOT_FN — reporting only /$C_FN overstates completion $(awk "BEGIN{printf \"%.1fx\", $TOT_FN/$C_FN}")"
fi

hdr "C. WHAT IS NOT BEING MEASURED (answer these out loud each run)"
cat <<'Q'
  1. Is the denominator defined by the TARGET BINARY, or by whatever a tool happens to
     read? (The 2026-07-26 miss: census.py reads only the C objects, so 64.6% of the
     game was structurally invisible to every count this project ever produced.)
  2. Has any doc claimed a number that no script regenerates? Hand-written numbers rot.
  3. Has any gate been added since the last audit WITHOUT a negative test proving it
     fails? An untested gate is a confidence generator, not a check.
  4. Which claims made to the user in the last session were NOT adjudicated by a
     mechanical oracle? Those are where every unnoticed error has lived (20/20 binary
     claims were self-caught; 0/6 progress/process claims were).
  5. Is any "impossible / exhausted" claim older than one session still unchallenged?
     Two such claims have already been overturned by one external idiom.
Q

hdr "VERDICT"
[ "$FAIL" = 0 ] && echo "  gates demonstrably work; accounting complete" || echo "  AUDIT FAILED — fix the above before reporting any progress"
exit $FAIL
