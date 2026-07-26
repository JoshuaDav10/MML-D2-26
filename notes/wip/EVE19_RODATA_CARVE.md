# eve19 — the rodata carve (blocks the last 6 functions)

**State 2026-07-26: 6 of 12 eve19 functions matched.** The remaining six all need one
thing, and it is the same thing.

## Who is blocked and why

| function | needs |
|---|---|
| `eve19_main_area_00` | jump table `jtbl_801006C8` (7-case switch on `routine_1`) |
| `eve19_main_area_01` | `CHECK_BLOCK` local initializer → `D_801006E4` |
| `eve19_main_area_02` | → `D_801006E8` |
| `eve19_main_area_05` | → `D_801006EC` |
| `eve19_main_area_06` | → `D_801006F0` |
| `eve19_main_area_09` | → `D_801006F4` |

Each item is **rodata the C compiler would emit itself**, but which currently lives in
the hand-carved asm segment `asm/ST1A/ovl0__progbin_r3_st1a.bin/data/800.rodata.s`.
Write the C without carving and the constant exists twice — once from the asm segment,
once from `eve19.c.o(.rodata)` — and the overlay mismatches.

## The boundary (measured, clean)

- eve19 owns exactly `0x801006C8 .. 0x801006F8` → file offset **`0xEC8 .. 0xEF8`**, 0x30 bytes.
- That is the **tail** of the rodata segment (`[0x800, rodata]`, which ends at `0xEF8`),
  so the carve is a pure truncation — no splitting a segment down the middle.
- The preceding symbol `jtbl_801006B4` is referenced by `EF8.s`, a **different**
  function. Verified by grep: the only jtbl any eve19 function references is
  `jtbl_801006C8`, and the only `D_8010xxxx` it references are the five above.

## The recipe

1. `config/overlay/splat.us.ST1A/ovl0__progbin_r3_st1a.bin.yaml` — append a rodata
   subsegment owned by eve19, immediately after the existing rodata line:
   ```yaml
         - [0x800, rodata]
         - [0xEC8, .rodata, eve19]     # jtbl_801006C8 + 5 CHECK_BLOCK consts
   ```
   (Same shape as the proven engine recipe: `[0x10C0, .rodata, scene]`, see
   LESSONS.md "jump-table infrastructure".)
2. Re-split ST1A so `800.rodata.s` regenerates **without** the `0xEC8..0xEF8` tail.
   ⚠️ `asm/` is gitignored — **back it up first** (`cp -r asm/ST1A /tmp/asm_ST1A.bak`).
   A prior session lost asm to a bad splat re-run and had to re-extract.
3. Write **all six** remaining functions in one go. Partial conversion cannot work: the
   carved region must be filled completely and in the right order, and gcc emits rodata
   in order of first use — jump table (area_00) first, then the CHECK_BLOCKs in function
   order 01, 02, 05, 06, 09. That ordering matches the observed addresses exactly, which
   is good evidence the original file had these functions in this order too.
4. `make CPP=cpp chunks` → **check the exit code**, then `check_overlays` must say
   205/205, then mutation-test.

## Gotcha that already bit once

A failed `make chunks` leaves the previous BINs in place and `check_overlays` then
happily reports **205/205 on stale files**. Always read the build exit code before
believing the gate.

## Source

Capcom's own `eve19.c`, left uncompiled in the JP/PSP `ST1E.BIN`, transcribed on TCRF
(page static since 2021-08-06). Full text saved in the session scratchpad; the six
matched functions came out of it essentially verbatim, with `check_block` matching
instruction-for-instruction on the first compile.
