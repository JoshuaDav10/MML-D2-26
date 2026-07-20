#!/usr/bin/env bash
# Positional score of a full-TU 53B40 candidate: compile via the permuter
# pipeline, slice func_80053B40 from both objects, count same-index row
# mismatches (register-level, branch targets as-is). Lower = better; the
# absolute value tracks the permuter's metric to within a small constant.
# Usage: tools/score53b40.sh <candidate.c>
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/.venv/bin/activate"
CAND="$(realpath "$1")"
TMPO=$(mktemp /tmp/score53b40.XXXX.o)
trap 'rm -f "$TMPO"' EXIT
"$ROOT/tools/decomp-permuter/mml_53B40/compile.sh" "$CAND" -o "$TMPO" >/dev/null 2>&1
python3 - "$TMPO" "$ROOT/tools/decomp-permuter/mml_53B40/target.o" <<'EOF'
import subprocess, sys, re
def rows(obj):
    out = subprocess.run(["mipsel-elf-objdump", "-d", obj],
                         capture_output=True, text=True).stdout
    lines, started = [], False
    for l in out.splitlines():
        if "<func_80053B40>:" in l:
            started = True; continue
        if not started: continue
        m = re.match(r"\s+[0-9a-f]+:\s+[0-9a-f]{8}\s+(.*)", l)
        if m:
            lines.append(re.sub(r"<[^>]*>", "", m.group(1)).strip())
        elif re.match(r"^[0-9a-f]+ <", l):
            break
    return lines
a, b = rows(sys.argv[1]), rows(sys.argv[2])
n = min(len(a), len(b))
mism = sum(1 for i in range(n) if a[i] != b[i])
print(f"len {len(a)} vs {len(b)}; positional row mismatches: {mism}")
EOF
