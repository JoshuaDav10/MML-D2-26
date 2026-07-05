# Function count reconciliation (rock_neo main exe)

Authoritative method (see `notes/LESSONS.md` “Counting matched functions”):

1. **In object file** — count `F .text` symbols in `build/src/rock_neo/<tu>.c.o`
   (`tools/census.py --matched`, column `in .o`).
2. **Active stubs** — count `.include nonmatchings` paths after cpp on each TU
   (`tools/census.py`, default mode).
3. **Matched per TU** — `in .o − stubs` (same as `--matched` column).
4. **Hash** — the only match authority; this file is bookkeeping only.

## Why 475 ≠ 484

| Count | Value | What it measures |
|------:|------:|------------------|
| Splat nonmatchings `.s` files | **475** | Functions extracted to `asm/rock_neo/nonmatchings/` (historical asm reference) |
| `F .text` in all rock_neo `.o` | **484** | Functions actually emitted from current `src/rock_neo/*.c` |
| Active INCLUDE_ASM stubs | **261** | Stubs still pulled in after cpp (ifdef-aware) |
| Matched (C in tree) | **223** | `484 − 261`; per-TU sum from `--matched` |

**484 = 475 + 9.** The nine extras are compiled from `game.c` and never had a
nonmatching `.s` (upstream C carried in the port):

| Function | TU |
|----------|-----|
| `func_800131FC` | game.c |
| `func_8001326C` | game.c |
| `func_80016BC0` | game.c |
| `func_80016BF4` | game.c |
| `func_80016D0C` | game.c |
| `func_80016D38` | game.c |
| `func_80016D64` | game.c |
| `func_80016DAC` | game.c |
| `func_80016E90` | game.c |

Every nonmatching `.s` (475/475) corresponds to a symbol still present in some
`.o` (either active stub or replaced by matching C). None are orphaned.

## Per-TU breakdown (`census.py --matched`)

| TU | in .o | stubs | matched |
|----|------:|------:|----------:|
| Code800133D8.c | 16 | 11 | 5 |
| cd.c | 35 | 20 | 15 |
| debug.c | 10 | 8 | 2 |
| game.c | 22 | 10 | 12 |
| main.c | 28 | 11 | 17 |
| moji.c | 145 | 65 | 80 |
| player.c | 91 | 59 | 32 |
| scene.c | 59 | 30 | 29 |
| sound.c | 34 | 18 | 16 |
| sub_scrn.c | 44 | 29 | 15 |
| **total** | **484** | **261** | **223** |

## progress.md mapping

- **Total mapped (authoritative):** 484 functions in linked rock_neo from source.
- **Splat asm library:** 475 `.s` files (subset; does not include the nine game.c-only symbols).
- **Do not use** naive `grep INCLUDE_ASM` or `475 − matched` for stub totals —
  cpp census is required (`#ifndef ACCEPT_REORDERING_BULLSHIT` blocks, etc.).

Regenerate: `python3 tools/census.py` and `python3 tools/census.py --matched`
(requires built tree).
