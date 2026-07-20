#!/usr/bin/env bash
# Land a winning func_80053B40 candidate into src/rock_neo/moji.c.
# Usage: tools/land53b40.sh <flat-or-plain candidate.c> [--keep]
#   Splices the function + needed decls over the INCLUDE_ASM line, does a
#   clean-ish rebuild, runs check_rock_neo_only, prints the verdict.
#   Default: REVERTS moji.c afterwards (dry-run). Pass --keep to leave the
#   splice in place (only after bytecmp shows 0 hard mismatches!).
# The full ritual (mutation test, audit_count.sh -> 275, docs, commit) stays
# MANUAL — this script only mechanizes the splice + hash check.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CAND="$1"; KEEP="${2:-}"
MOJI="$ROOT/src/rock_neo/moji.c"
[[ -f "$CAND" ]] || { echo "no such candidate: $CAND"; exit 1; }
grep -q 'INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80053B40);' "$MOJI" \
  || { echo "moji.c INCLUDE_ASM line for 53B40 not found (already landed?)"; exit 1; }

cp "$MOJI" "$MOJI.land_backup"
python3 - "$CAND" "$MOJI" <<'EOF'
import sys, re
cand, moji_path = sys.argv[1], sys.argv[2]
src = open(cand).read()
i = src.index('void func_80053B40')
# candidates may be FULL TUs (permuter tree-objective bases): extract just the
# function by brace matching from its opening brace.
b = src.index('{', i)
depth = 0
j = b
while True:
    if src[j] == '{': depth += 1
    elif src[j] == '}':
        depth -= 1
        if depth == 0: break
    j += 1
body = src[i:j+1] + '\n'
# tree-environment decls the function needs beyond moji.c's existing ones
decls = '''/* --- func_80053B40 support decls (landed) --- */
extern s32 D_80098B2C;
extern u16 D_800C0C26;
extern u16 D_800C0C2A;
typedef UnkStruc_80098934 DRAWCTX; /* game.h owns the real struct */
extern RECT D_80097F50[];
extern u16 D_80097F30[];
extern u8 D_8008AE7C[];
extern s32 (*D_8008A91C[])();
extern s32 (*D_8008AAC4[])();
extern s32 (*D_8008AAE4[])();
extern MOJI_TASK D_800BB9C8;
void SetDrawArea(u32 *, RECT *);

'''
moji = open(moji_path).read()
line = 'INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80053B40);'
moji = moji.replace(line, decls + body.rstrip() + '\n')
open(moji_path, 'w').write(moji)
print("spliced", len(body), "bytes of function text")
EOF

cd "$ROOT"
source .venv/bin/activate
touch src/rock_neo/*.c && rm -f build/rock_neo.elf
if ! make CPP=cpp build_rock_neo_only > /tmp/land53b40_build.log 2>&1; then
    echo "BUILD FAILED — see /tmp/land53b40_build.log (moji.c reverted)"
    grep -iE "error" /tmp/land53b40_build.log | head -5
    mv "$MOJI.land_backup" "$MOJI"
    exit 1
fi
RES=$(make CPP=cpp check_rock_neo_only 2>&1 | tail -1)
echo "hash check: $RES"
if [[ "$KEEP" == "--keep" ]]; then
    rm -f "$MOJI.land_backup"
    echo "splice KEPT. Continue the ritual: mutation test, audit_count.sh (must print 275), docs, commit."
else
    mv "$MOJI.land_backup" "$MOJI"
    touch src/rock_neo/*.c && rm -f build/rock_neo.elf
    make CPP=cpp build_rock_neo_only > /dev/null 2>&1 || true
    RES2=$(make CPP=cpp check_rock_neo_only 2>&1 | tail -1)
    echo "moji.c reverted; post-revert hash: $RES2"
fi
