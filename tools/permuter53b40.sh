#!/usr/bin/env bash
# Managed launcher for the 53B40 permuter. NEVER launch by hand — this script
# exists because a hand-launch without the venv died instantly on `import toml`
# while zombie forkservers kept `pgrep permuter` alive, masking the death for
# an hour (2026-07-20). This launcher:
#   1. kills anything stale (by pidfile AND pattern),
#   2. activates the venv and PROVES the import works,
#   3. launches, records the real PID to mml_53B40/permuter.pid,
#   4. blocks until "base score" appears in the log (or fails loudly).
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WD="$ROOT/tools/decomp-permuter"
PIDFILE="$WD/mml_53B40/permuter.pid"
LOG="$WD/mml_53B40/permuter.log"

# 1. cleanup
if [[ -f "$PIDFILE" ]]; then
    oldpid=$(cat "$PIDFILE")
    kill -9 "$oldpid" 2>/dev/null || true
    rm -f "$PIDFILE"
fi
pkill -9 -f "permuter.py mml_53B40" 2>/dev/null || true
sleep 1

# 2. environment sanity — fail HERE, not silently in nohup
source "$ROOT/.venv/bin/activate"
python3 -c "import toml" || { echo "FATAL: venv broken (toml missing)"; exit 1; }

# 3. launch (PERMUTER_ONLY_FUNC: score just this function out of a full-TU base)
cd "$WD"
export PERMUTER_ONLY_FUNC=func_80053B40
nohup python3 permuter.py mml_53B40 -j"${JOBS:-8}" --best-only --stop-on-zero \
    > "$LOG" 2>&1 &
pid=$!
echo "$pid" > "$PIDFILE"

# 4. verify it actually started working
for i in $(seq 1 60); do
    if ! kill -0 "$pid" 2>/dev/null; then
        echo "FATAL: permuter (pid $pid) died during startup. Log tail:"
        tail -5 "$LOG"
        rm -f "$PIDFILE"
        exit 1
    fi
    if grep -q "base score" "$LOG" 2>/dev/null; then
        echo "OK: permuter pid $pid running, $(grep -m1 'base score' "$LOG")"
        exit 0
    fi
    sleep 2
done
echo "FATAL: no 'base score' after 120s. Log tail:"
tail -5 "$LOG"
exit 1
