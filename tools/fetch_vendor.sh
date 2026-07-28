#!/usr/bin/env bash
# Fetch third-party reference sources into vendor/ (gitignored — ~62 MB).
#
# These let us MATCH library code from published source instead of reverse-engineering it.
# Sony's PSY-Q library and the gcc runtime helpers are not Capcom code; the originals are
# public, so hand-RE'ing them is wasted effort.
set -uo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."
mkdir -p vendor && cd vendor
get() { [ -d "$2" ] && { echo "  have $2"; return; }; git clone --depth 1 -q "$1" "$2" && echo "  cloned $2"; }

get https://github.com/FoxdieTeam/psyq_sdk.git        psyq_sdk        # 74 .LIB, psyq 4.3/4.4/4.5
get https://github.com/sozud/psy-q-decomp.git         psy-q-decomp    # reference C for SDK routines
get https://github.com/decompals/mips-gcc-2.7.2.git   mips-gcc-2.7.2  # libgcc2.c = __divdi3 etc.
if [ ! -d psyq-obj-parser-src ]; then
  git clone --depth 1 --filter=blob:none --sparse -q \
    https://github.com/grumpycoders/pcsx-redux.git psyq-obj-parser-src \
    && (cd psyq-obj-parser-src && git sparse-checkout set tools/psyq-obj-parser) \
    && echo "  cloned psyq-obj-parser-src"   # converts Sony .OBJ/.LIB -> ELF .o
fi
cat <<'NOTE'

  CAVEAT: our cc1 reports GNU C 2.7.2.SN32.3.7, which per wiki.deco.mp ships from
  PSY-Q 4.0. This SDK mirror has 4.3/4.4/4.5 only — 4.3 is the closest available, NOT
  an exact version match. Verify any library identification against our own bytes before
  believing it. SN-patched builds can also differ from stock FSF libgcc.
NOTE
