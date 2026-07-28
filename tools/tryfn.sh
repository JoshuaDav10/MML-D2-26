#!/usr/bin/env bash
# Pipe one C file through the rock_neo .c.o pipeline (Makefile line 266) and objdump.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

CPP="${CPP:-cpp}"
AS="${CROSS:-mipsel-elf-}as"
OBJDUMP="${CROSS:-mipsel-elf-}objdump"

CPP_FLAGS=(
    -Iinclude -undef -Wall -lang-c -fno-builtin
    -Dmips -D__GNUC__=2 -D__OPTIMIZE__ -D__mips__ -D__mips
    -Dpsx -D__psx__ -D__psx -D_PSYQ -D__EXTENSIONS__ -D_MIPSEL
    -D_LANGUAGE_C -DLANGUAGE_C -DHACKS
)
# MUST stay in sync with the Makefile's CC_FLAGS (Makefile line 25).
# -mel added 2026-07-29: cc1-27 defaults to BIG-endian for UNALIGNED access, so
# without it lwl/lwr and swl/swr carry swapped byte offsets. The Makefile has
# always passed it; this harness did not, so bytecmp.sh silently disagreed with
# the real build for any function containing an unaligned struct copy — found on
# ST03 func_80108648, which showed 8 phantom hard mismatches on correct C.
CC_FLAGS=(
    -mcpu=3000 -quiet -w -O2 -funsigned-char -fpeephole -ffunction-cse
    -fpcc-struct-return -fcommon -fverbose-asm -fgnu-linker -mgas -msoft-float
    -G8 -gcoff -mel
)
AS_FLAGS=(-Iinclude -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0)

usage() {
    echo "usage: tools/tryfn.sh FILE.c [OUT.o]" >&2
    exit 1
}

[[ $# -ge 1 ]] || usage
SRC="$1"
OUT="${2:-build/scratch/tryfn.o}"

[[ -f "$SRC" ]] || { echo "tryfn.sh: no such file: $SRC" >&2; exit 1; }

mkdir -p build/scratch

"${CPP}" "${CPP_FLAGS[@]}" "$SRC" \
    | ./bin/cc1-27 "${CC_FLAGS[@]}" \
    | python3 tools/maspx/maspsx.py --no-macro-inc --expand-div \
    | python3 tools/gprel.py \
    | python3 tools/patchasm.py \
    | "${AS}" "${AS_FLAGS[@]}" -o "$OUT"

echo "=== ${OUT} ==="
"${OBJDUMP}" -dr "$OUT"
