#!/usr/bin/env bash
# AUTHORITATIVE progress audit. The ONLY trusted source of the matched count.
# Does a from-scratch rebuild so no stale object can produce a false pass.
# Usage: tools/audit_count.sh   (takes a few minutes; that's the point)
set -uo pipefail
rc=0
cd "$(dirname "${BASH_SOURCE[0]}")/.."
source .venv/bin/activate 2>/dev/null || true

echo "=== AUTHORITATIVE AUDIT ($(git rev-parse --short HEAD), branch $(git branch --show-current)) ==="
echo "[1/4] clean rebuild (rm -rf build)..."
rm -rf build
if ! make CPP=cpp build_rock_neo_only >/tmp/audit_build.log 2>&1; then
  echo "  BUILD FAILED — see /tmp/audit_build.log"; grep -iE "error" /tmp/audit_build.log | head; exit 1
fi
echo "[2/4] hash check..."
HASH=$(make CPP=cpp check_rock_neo_only 2>&1 | tail -1)
echo "[3/4] raw byte compare vs original..."
CMP=$(cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe 2>&1 && echo "byte-identical" || echo "DIFFERS")
echo "[4/4] authoritative matched count (census reads built .o)..."
COUNT=$(python3 tools/census.py --matched 2>&1 | grep -iE "total:")

echo
echo "----------------------------------------"
echo "  hash check : $HASH"
echo "  raw cmp    : $CMP"
echo "  count      : $COUNT"
echo "----------------------------------------"
# 2026-07-26 audit: these were previously CAPTURED BUT NEVER TESTED, so the script
# exited 0 even when raw cmp said DIFFERS — it could not be used as a gate/hook.
case "$HASH" in *OK*) ;; *) echo "  FAIL: hash check did not print OK"; rc=1;; esac
case "$CMP"  in *byte-identical*) ;; *) echo "  FAIL: raw cmp is not byte-identical"; rc=1;; esac
[ "$rc" = 0 ] && echo "Trust THIS number. Never a grep of INCLUDE_ASM." \
              || echo "BUILD DOES NOT MATCH — the count above is meaningless."
exit $rc
