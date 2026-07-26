#!/usr/bin/env bash
# Doc-consistency gate.
#
# 2026-07-26 REWRITE. The first version was BROKEN in the exact way this project keeps
# failing: it *looked* like it worked (it caught 3 real defects) but two of its five
# checks piped into `while read`, which runs in a SUBSHELL, so their `fail=1` could never
# propagate — it exited 0 with six stale "276"s in HANDOFF.md. No pipelines-into-while
# here; findings accumulate in a plain array in the current shell.
#
# Cheap (no rebuild). Run at session start and before any commit that claims a count.
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
source .venv/bin/activate 2>/dev/null || true

DEFECTS=()
def() { DEFECTS+=("$1"); }

N=$(python3 tools/census.py --matched 2>/dev/null | grep -oE '[0-9]+ matched' | grep -oE '[0-9]+')
S=$(python3 tools/census.py --matched 2>/dev/null | grep -oE '[0-9]+ active stubs' | grep -oE '[0-9]+')
[ -n "${N:-}" ] || { echo "cannot get census — build first (make CPP=cpp build_rock_neo_only)"; exit 2; }

# The TRUE denominators. 484 is only the functions currently split into C TUs; the rest of
# the game is still linked as raw asm and has never been counted. (2026-07-26 audit.)
UNSPLIT=$(grep -h '^glabel' asm/rock_neo/*.s 2>/dev/null | wc -l | tr -d ' ')
TOTAL_FN=$(( 484 + UNSPLIT ))
echo "authoritative: $N matched / 484 C-mapped ($(awk "BEGIN{printf \"%.1f\", 100*$N/484}")%)"
echo "               $N / $TOTAL_FN game functions in exe ($(awk "BEGIN{printf \"%.1f\", 100*$N/$TOTAL_FN}")%) — $UNSPLIT still unsplit raw asm"
echo "               $S active stubs"

# 1. Any 3-digit count-shaped claim in a doc must equal N. No subshell.
for f in progress.md HANDOFF.md notes/COUNTS.md; do
  [ -f "$f" ] || continue
  while IFS= read -r hit; do
    ln=${hit%%:*}; txt=${hit#*:}
    # A line explicitly marked [SUPERSEDED] is preserved history, not a live claim.
    case "$txt" in *"[SUPERSEDED"*) continue;; esac
    # take the number ADJACENT to the count word, not the first 3 digits on the line
    # (a date like 2026-07-25 would otherwise parse as "202")
    num=$(printf '%s' "$txt" | grep -oE '(\*\*)?[0-9]{3}(\*\*)? matched|Matched: [0-9]{3}|count = \*\*[0-9]{3}' | grep -oE '[0-9]{3}' | head -1)
    [ "$num" = "$N" ] || def "$f:$ln states count $num, authoritative is $N -> $(printf '%.90s' "$txt")"
  done < <(grep -nE '(\*\*[0-9]{3}\*\*|[0-9]{3}) matched|Matched: [0-9]{3}|AUTHORITATIVE count = \*\*[0-9]{3}' "$f" 2>/dev/null)
done

# 2. Stub-count claims.
c=$(grep -oE '\*\*[0-9]+\*\* active INCLUDE_ASM stubs' progress.md 2>/dev/null | grep -oE '[0-9]+' | head -1)
[ -z "${c:-}" ] || [ "$c" = "$S" ] || def "progress.md says $c active stubs, authoritative is $S"

# 3. COUNTS.md must carry the current count AND both denominators.
if [ -f notes/COUNTS.md ]; then
  grep -q "\b$N\b" notes/COUNTS.md || def "notes/COUNTS.md lacks the current count $N (it exists only to hold numbers)"
  grep -q "\b$TOTAL_FN\b" notes/COUNTS.md || def "notes/COUNTS.md lacks the TRUE denominator $TOTAL_FN ($UNSPLIT unsplit fns are excluded from 484 — reporting only 484 overstates completion ~2.3x)"
fi

# 4. The rulebook must not contradict itself about which tool is authoritative.
a=$(grep -c 'ONLY authoritative matched count is `tools/audit_count.sh`' CLAUDE.md 2>/dev/null)
b=$(grep -c 'ONLY authoritative matched count is `tools/census.py' notes/LESSONS.md 2>/dev/null)
[ "${b:-0}" = 0 ] || def "notes/LESSONS.md names census.py as THE authority while CLAUDE.md names audit_count.sh — census alone misses link/register nits (it accepted the func_8001F828 false transfer)"

# 5. Re-inflation trap.
raw=$(grep -c INCLUDE_ASM src/rock_neo/*.c 2>/dev/null | awk -F: '{s+=$2} END{print s}')
[ "${raw:-$S}" = "$S" ] && : || echo "note: raw grep sees $raw stubs vs $S real — $((raw-S)) shadowed by an active #else (un-gating one is a NO-OP, not a match)"

if [ ${#DEFECTS[@]} -eq 0 ]; then
  echo "docs consistent"; exit 0
fi
printf '  DEFECT: %s\n' "${DEFECTS[@]}"
echo "DOCS INCONSISTENT (${#DEFECTS[@]}) — fix before claiming progress"
exit 1
