# Sub_screen_status_calc (123 insn) — parked at 82 rows

First deliberate attempt at the 120-200 instruction band WITHOUT original source
(2026-07-28). Not matched. Parking the state so nobody re-treads it.

## What it does — fully understood, this is not the blocker
Five equipment slots. For slot k: `icon = D_8008D7E8[ D_8008DB08[idx]*0x14 + 4*k +
(s8)Player_work_bytes[idx*8 + 0x4F4 + k] ]`. An icon of 0xFF means empty, which becomes
icon 0x58 plus an "empty" flag. A negative index clears all five slots and returns.
Display struct: five slots at 0x11, stride 8, icon at +0 and flag at +5.

m2c produced a clean, essentially correct draft immediately — **understanding the function
was not the hard part.** Register allocation is.

## The blocker
The target keeps `&Player_work` in `$a0` and `&D_8008D7E8` in `$a1` alive across the whole
body and uses **no stack frame at all**. Every shape I tried emits `addiu sp,sp,-8`,
i.e. gcc spills instead of holding those two bases in registers.

## Attempts (do not repeat)
| version | shape | rows |
|---|---|---|
| v1 | globals indexed inline, unused `u16 set` local | ~19 by a different (broken) metric — not comparable |
| v2 | `(tbl + 0x04)[...]` per-slot base pointers | 81 |
| v3 | globals indexed inline, no spare locals | **82 (best)** |
| v4 | v3 + table base held in a local `u8 *tbl` | 93 |

⚠️ Methodology note: v1's "19" was measured with a different grep than v2-v4 and is NOT
comparable. Only v2/v3/v4 share a metric. Do not read v1 as the best result.

## Next levers, untried
- Force the two base addresses to live in registers without adding a stack slot — the
  `register` keyword, or restructuring so both are function parameters of a helper.
- Check whether the display struct shape is forcing the spill (try a plain `u8 *`).
- decomp-permuter: structure is right and registers are wrong, which is exactly the
  documented case where the permuter is the correct tool.

## Best draft so far (v3, 82 rows)
```c
/* Status readout: five equipment slots. For each slot k, look up an icon byte in the
 * 3-level table D_8008D7E8[set][k][sub] and copy it into the display struct; a value of
 * 0xFF means "empty", which becomes icon 0x58 plus a flag. Negative index clears all
 * five slots. Written unrolled because the target is unrolled — a loop changes codegen. */
extern u8  D_8008D7E8[];   /* [set][5][4], stride 0x14 */
extern u16 D_8008DB08[];   /* per-index set selector */

typedef struct {
    u8 pad00[0x11];
    struct {
        u8 icon;           /* +0x11 + 8k */
        u8 pad[4];
        u8 empty;          /* +0x16 + 8k */
        u8 pad2[2];
    } slot[5];
} STATUS_DISP;

void Sub_screen_status_calc(STATUS_DISP *sd, s32 idx) {
    s8 *pl;
    u8 *tbl;

    if (idx < 0) {
        sd->slot[0].empty = 0; sd->slot[0].icon = 0;
        sd->slot[1].empty = 0; sd->slot[1].icon = 0;
        sd->slot[2].empty = 0; sd->slot[2].icon = 0;
        sd->slot[3].empty = 0; sd->slot[3].icon = 0;
        sd->slot[4].empty = 0; sd->slot[4].icon = 0;
        return;
    }

    pl  = (s8 *)&Player_work + idx * 8;
    tbl = D_8008D7E8;

    sd->slot[0].icon = tbl[D_8008DB08[idx] * 0x14 + 0x0  + pl[0x4F4]];
    sd->slot[1].icon = tbl[D_8008DB08[idx] * 0x14 + 0x4  + pl[0x4F5]];
    sd->slot[2].icon = tbl[D_8008DB08[idx] * 0x14 + 0x8  + pl[0x4F6]];
    sd->slot[3].icon = tbl[D_8008DB08[idx] * 0x14 + 0xC  + pl[0x4F7]];
    sd->slot[4].icon = tbl[D_8008DB08[idx] * 0x14 + 0x10 + pl[0x4F8]];

    if (sd->slot[0].icon == 0xFF) { sd->slot[0].icon = 0x58; sd->slot[0].empty = 1; } else { sd->slot[0].empty = 0; }
    if (sd->slot[1].icon == 0xFF) { sd->slot[1].icon = 0x58; sd->slot[1].empty = 1; } else { sd->slot[1].empty = 0; }
    if (sd->slot[2].icon == 0xFF) { sd->slot[2].icon = 0x58; sd->slot[2].empty = 1; } else { sd->slot[2].empty = 0; }
    if (sd->slot[3].icon == 0xFF) { sd->slot[3].icon = 0x58; sd->slot[3].empty = 1; } else { sd->slot[3].empty = 0; }
    if (sd->slot[4].icon == 0xFF) { sd->slot[4].icon = 0x58; sd->slot[4].empty = 1; } else { sd->slot[4].empty = 0; }
}

```
