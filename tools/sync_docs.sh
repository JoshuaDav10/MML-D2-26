#!/usr/bin/env bash
# Rewrite the <!-- BEGIN GENERATED COUNTS --> block in progress.md and HANDOFF.md from the
# built objects, and regenerate notes/COUNTS.md. Run this instead of editing counts by hand.
#
# Pairs with tools/check_docs.sh, which byte-diffs the same block and fails on drift.
# If a doc has no block yet, one is inserted after its first heading.
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
source .venv/bin/activate 2>/dev/null || true

tools/gen_counts.sh >/dev/null || { echo "sync_docs: gen_counts failed (build first?)"; exit 2; }
JSON=$(tools/gen_counts.sh --json)
get() { printf '%s' "$JSON" | python3 -c "import json,sys;print(json.load(sys.stdin)['$1'])"; }

python3 - "$(get matched)" "$(get c_mapped)" "$(get active_stubs)" "$(get unsplit_raw)" \
           "$(get main_exe_total)" "$(get whole_game)" "$(get pct_slice)" \
           "$(get pct_main_exe)" "$(get pct_whole_game)" <<'PY'
import sys, re, pathlib
N, CMAP, S, UNSPLIT, TOTAL, WHOLE, PS, PE, PG = sys.argv[1:10]
BEGIN, END = "<!-- BEGIN GENERATED COUNTS -->", "<!-- END GENERATED COUNTS -->"
body = (
    f"| matched functions | **{N}** |\n"
    f"| C-mapped slice | {N} / **{CMAP}** = {PS}% |\n"
    f"| main executable | {N} / **{TOTAL}** = **{PE}%** |\n"
    f"| **whole game** | {N} / **~{WHOLE}** = **~{PG}%** |\n"
    f"| active INCLUDE_ASM stubs | **{S}** |\n"
    f"| still unsplit raw asm | **{UNSPLIT}** functions in `asm/rock_neo/*.s` |\n"
)
header = "| metric | value |\n|---|---|\n"
blk = f"{BEGIN}\n{header}{body}{END}"

for name in ("progress.md", "HANDOFF.md"):
    p = pathlib.Path(name)
    if not p.is_file():
        continue
    t = p.read_text()
    if BEGIN in t and END in t:
        t = re.sub(re.escape(BEGIN) + r".*?" + re.escape(END), blk, t, flags=re.S)
    else:
        lines = t.split("\n")
        at = next((i for i, l in enumerate(lines) if l.startswith("# ")), -1) + 1
        lines.insert(at, "\n" + blk + "\n")
        t = "\n".join(lines)
    p.write_text(t)
    print(f"sync_docs: updated {name}")
PY
echo "sync_docs: notes/COUNTS.md regenerated"
