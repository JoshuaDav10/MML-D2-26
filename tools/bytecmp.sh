#!/usr/bin/env bash
# Assemble a scratch TU, compare instruction words vs splat .s comment column.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

usage() {
    cat >&2 <<'EOF'
usage: tools/bytecmp.sh FUNC_NAME SCRATCH.c [REFERENCE.s]

  FUNC_NAME   symbol to compare (e.g. func_8001CB30)
  SCRATCH.c   single-function translation unit
  REFERENCE.s splat nonmatching asm (default: asm/rock_neo/nonmatchings/**/FUNC.s)

Writes a scratch object (BYTECMP_OBJ, default build/scratch/bytecmp.o) only.
  Set BYTECMP_OBJ to a unique path when running several instances concurrently.
  Exits 0 when hard mismatches == 0.
Reloc-sensitive slots (%hi/%lo/jal/branch labels) are flagged, not hard-fail.
EOF
    exit 1
}

[[ $# -ge 2 ]] || usage

FUNC="$1"
SCRATCH="$2"
REF="${3:-}"

[[ -f "$SCRATCH" ]] || { echo "bytecmp.sh: no such file: $SCRATCH" >&2; exit 1; }

if [[ -z "$REF" ]]; then
    mapfile -t matches < <(find asm/rock_neo/nonmatchings -name "${FUNC}.s" | sort)
    if [[ ${#matches[@]} -eq 0 ]]; then
        echo "bytecmp.sh: no asm/rock_neo/nonmatchings/**/${FUNC}.s" >&2
        exit 1
    fi
    if [[ ${#matches[@]} -gt 1 ]]; then
        echo "bytecmp.sh: ambiguous reference; pass REFERENCE.s explicitly:" >&2
        printf '  %s\n' "${matches[@]}" >&2
        exit 1
    fi
    REF="${matches[0]}"
fi

[[ -f "$REF" ]] || { echo "bytecmp.sh: no such file: $REF" >&2; exit 1; }

# Unique-able so parallel agents do not clobber each other's scratch object.
OBJ="${BYTECMP_OBJ:-build/scratch/bytecmp.o}"
mkdir -p "$(dirname "$OBJ")"

tools/tryfn.sh "$SCRATCH" "$OBJ" >/dev/null

python3 - "$FUNC" "$REF" "$OBJ" <<'PY'
import re
import subprocess
import sys

func, ref_path, obj_path = sys.argv[1:4]

word_re = re.compile(r"/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+\*/")
reloc_re = re.compile(
    r"%hi|%lo|\bjal\b|\bj\b|\bbeq\b|\bbne\b|\bble\b|\bbgt\b|\bblt\b|\bbge\b|"
    r"\bbeql\b|\bbnel\b|\bbgtl\b|\bbltl\b|\bbgel\b|\bbgtz\b|\bblez\b|"
    r"\bbgtzl\b|\bblezl\b|\bbnezl\b|\bbeqzl\b|\bb\b|\bbal\b|"
    r"\bbranch\b|\b\.L[0-9A-Fa-f]+\b",
    re.IGNORECASE,
)

expected: list[tuple[str, str, bool]] = []
with open(ref_path, encoding="utf-8") as f:
    for line in f:
        m = word_re.search(line)
        if not m:
            continue
        word = m.group(1).upper()
        maybe_reloc = bool(reloc_re.search(line.split("*/", 1)[-1]))
        expected.append((word, line.rstrip(), maybe_reloc))

if not expected:
    print(f"bytecmp: no instruction words in {ref_path}", file=sys.stderr)
    sys.exit(1)

dump = subprocess.run(
    ["mipsel-elf-objdump", "-dr", obj_path],
    check=True,
    capture_output=True,
    text=True,
).stdout

def be_word(le_hex: str) -> str:
    """objdump prints LE file bytes; splat comments use BE insn words."""
    b = bytes.fromhex(le_hex)
    return b[::-1].hex().upper()


in_func = False
actual: list[str] = []
func_hdr = re.compile(rf"<{re.escape(func)}>:")
for line in dump.splitlines():
    if func_hdr.search(line):
        in_func = True
        continue
    if in_func and re.match(r"^[0-9a-f]+ <", line):
        break
    if not in_func:
        continue
    m = re.match(r"^\s*[0-9a-f]+:\s+([0-9a-f]{8})\b", line)
    if m:
        actual.append(be_word(m.group(1)))

if not actual:
    print(f"bytecmp: no instructions for {func} in {obj_path}", file=sys.stderr)
    sys.exit(1)

n = max(len(expected), len(actual))
hard = 0
reloc = 0
print(f"bytecmp: {func}")
print(f"  scratch: {obj_path}")
print(f"  reference: {ref_path}")
print(f"  words: expected={len(expected)} actual={len(actual)}")
print()

for i in range(n):
    exp_word, exp_line, exp_reloc = (
        expected[i] if i < len(expected) else ("--------", "(missing)", False)
    )
    act_word = actual[i] if i < len(actual) else "--------"
    if i >= len(expected):
        print(f"  [{i:3d}] EXTRA  actual={act_word}")
        hard += 1
        continue
    if i >= len(actual):
        print(f"  [{i:3d}] MISSING expected={exp_word}  {exp_line.strip()}")
        hard += 1
        continue
    if exp_word == act_word:
        continue
    if exp_reloc:
        print(f"  [{i:3d}] RELOC  expected={exp_word} actual={act_word}")
        print(f"         {exp_line.strip()}")
        reloc += 1
    else:
        print(f"  [{i:3d}] MISMATCH expected={exp_word} actual={act_word}")
        print(f"         {exp_line.strip()}")
        hard += 1

print()
print(f"summary: {hard} hard mismatch(es), {reloc} reloc flag(s)")
if hard:
    sys.exit(1)
PY
