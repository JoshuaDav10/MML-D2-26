# Scratch translation-unit tools

Single-function scratch files for iterating decomp matches without touching
the main build graph. All outputs go under `build/scratch/` only.

Requires repo root, venv active (`source .venv/bin/activate`), and
`make CPP=cpp` toolchain on PATH.

## tryfn.sh

Pipe one `.c` file through the real rock_neo pipeline (Makefile line 266):

`cpp | cc1-27 | maspsx | gprel.py | patchasm.py | as`

```bash
tools/tryfn.sh build/scratch/myfn.c
tools/tryfn.sh build/scratch/myfn.c build/scratch/myfn.o
```

Prints `mipsel-elf-objdump -dr` for the resulting object.

## bytecmp.sh

Assemble a scratch TU, then compare instruction words against the third
column of splat `.s` comments (`/* rom vram word */`).

```bash
tools/bytecmp.sh func_8001CB30 build/scratch/func_8001CB30.c
tools/bytecmp.sh func_8001CB30 build/scratch/func_8001CB30.c asm/rock_neo/nonmatchings/cd/func_8001B3E4.s
```

- **Hard mismatch**: word differs and the asm line is not reloc-sensitive.
- **Reloc flag**: `%hi`, `%lo`, `jal`, or branch — eyeball only.
- Exit 0 when hard mismatches == 0.

## census.py

Accurate active-stub list: preprocess each `src/rock_neo/*.c` with the build's
`CPP_FLAGS`, extract `.include`d nonmatchings paths, count insn comment lines
in each `.s`, print sorted (longest first).

```bash
python3 tools/census.py
python3 tools/census.py | head
```

Do not use naive `grep INCLUDE_ASM` — `#ifndef ACCEPT_REORDERING_BULLSHIT`
blocks inflate the count.
