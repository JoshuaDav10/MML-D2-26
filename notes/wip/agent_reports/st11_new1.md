# ST11 Batch "new1" — from-scratch bodies

5/5 solved and proven byte-exact. Typedefs omitted from the C blocks as requested.

**Read the two "REQUIRED ADDITIONS" items before landing** — one struct field and two
extern declarations are needed, otherwise cc1 silently accepts an unknown member and
deletes a body.

---

## REQUIRED ADDITIONS (please apply these to the file's existing decl block)

**1. `BOSS_WORK` needs a byte at offset 0x0B** (for `func_8010908C`).
Currently the typedef has `u8 padB[0x14 - 0xB];`. Split it:

```c
    u8      phase;               /* 0x00A intro/spawn phase counter */
    u8      xB;                  /* 0x00B  <-- NEW: 4th byte of the state block */
    u8      padC[0x14 - 0xC];    /* was padB[0x14 - 0xB] */
```

No other struct changes. `SWEEP_WORK.x22`, `ENTITY_WORK.routine`, and
`FLAG_WORK.xFF / .x103 / .x107` all already exist and are used as-is.

**2. Two extern declarations** (neither is currently in the file):

```c
extern volatile u16 System_timer;        /* 0x1F800002 — scratchpad frame counter */
extern void (*D_8010A1BC[])(void);
```

`volatile` on `System_timer` is **load-bearing, not decoration** — see the note under
func_80109034. Both symbols resolve inside ST11's own chunk: `System_timer` is in
`config/overlay/splat.us.ST11/generated.syms.us.ovl0__progbin_r3_st11.bin.txt`, and
`D_8010A1BC` is defined in `asm/ST11/ovl0__progbin_r3_st11.bin/data/9C40.data.s:1224`.

---

## func_80109034 — 7 insn [x7] — VERIFIED (by real link, not by bytecmp)

```c
void func_80109034(SWEEP_WORK *work) {
    /* System_timer MUST be volatile: without it gcc sees that (x << 3) & 0x60
     * keeps only bits 2-3 and narrows the halfword load to `lbu 0(v0)`,
     * costing the match. volatile pins the access width -> `lhu 2(v0)`. */
    work->x22 = (System_timer << 3) & 0x60;
}
```

**bytecmp: reports 2 hard mismatches — but this is a bytecmp artifact, and the body is
correct.** Do not let the red verdict stop you landing it. Explanation:

bytecmp decides "is this slot a relocation?" by regex-matching the *reference asm text*
for `%hi`/`%lo`. Splat never attached the symbol to this function, so it rendered the
line as literal arithmetic `lui $v0, (0x1F800002 >> 16)` instead of `%hi(System_timer)`.
The regex finds no marker, so it grades the two unresolved reloc slots as hard failures.

My object *does* carry the correct relocs (`objdump -r`):
```
00000000 R_MIPS_HI16       System_timer
00000004 R_MIPS_LO16       System_timer
```
Linked at the real address with the symbol defined, all 7 words are exact:
```
80109034: 3c021f80  lui  v0,0x1f80      -> 801F023C  expected 801F023C
80109038: 94420002  lhu  v0,2(v0)       -> 02004294  expected 02004294
8010903c: 00000000  nop                 -> 00000000  expected 00000000
80109040: 000210c0  sll  v0,v0,0x3      -> C0100200  expected C0100200
80109044: 30420060  andi v0,v0,0x60     -> 60004230  expected 60004230
80109048: 03e00008  jr   ra             -> 0800E003  expected 0800E003
8010904c: a0820022  sb   v0,0x22(a0)    -> 220082A0  expected 220082A0
```

Side note: ST0C has this same body **PARKED** as `func_8010BE64` with a comment saying
the symbol isn't in that chunk's linker inputs. That reason does **not** apply to ST11 —
`System_timer` is in ST11's own generated syms. Worth un-parking ST0C's copy separately.
(I also tried the LESSONS "lui/%lo => unsized array extern" idiom here; it does *not*
apply — the array form emits an extra `addiu` and comes out 8 words instead of 7.)

---

## func_8010908C — 7 insn [x5] — PASS (bytecmp exit 0, 0 hard, 0 reloc)

Requires the new `BOSS_WORK.xB` field above.

```c
void func_8010908C(BOSS_WORK *work) {
    work->routine++;
    work->step = 0;
    work->phase = 0;
    work->xB = 0;
}
```

The increment is written **first** even though its store lands in the delay slot last —
the `lbu 0x8` is hoisted to the top and the `sb 0x8` sinks into the `jr` delay slot.

---

## func_8010489C — 8 insn [x2] — PASS (bytecmp exit 0, 0 hard, 1 reloc)

```c
void func_8010489C(FLAG_WORK *p, u8 flag) {
    if (flag) {
        p->x107 = 0;
    } else {
        p->x107 = -1;
    }
}
```

The single reloc flag is the intra-function `j .L801048B4`, which encodes relative to the
object's base of 0 in scratch. Linked at 0x8010489C it resolves to `0804122d` =
`j 801048b4`, exactly the expected `2D120408`. Verified.

---

## func_801048BC — 10 insn [x2] — PASS (bytecmp exit 0, 0 hard, 1 reloc)

```c
void func_801048BC(FLAG_WORK *p, u8 flag) {
    if (flag) {
        p->xFF = 0;
        p->x103 = 0;
    } else {
        p->xFF = -1;
        p->x103 = -1;
    }
}
```

Same story: the `j .L801048DC` reloc resolves to `08041237` = `j 801048dc`, matching the
expected `37120408`. Verified by link.

---

## func_801015F4 — 15 insn [x2] — PASS (bytecmp exit 0, 0 hard, 2 reloc)

Standard jal-table dispatcher. Requires the `D_8010A1BC` declaration above.

```c
void func_801015F4(ENTITY_WORK *work) {
    D_8010A1BC[work->routine]();
}
```

The two reloc flags are `%hi`/`%lo` on `D_8010A1BC`. Linked with the symbol at
0x8010A1BC they resolve to `3c018011` (-> `1180013C`) and `8c22a1bc` (-> `BCA1228C`),
both exactly as expected. Verified.

Note the callee takes no argument in the C. `$a0` still happens to hold `work` at the
call, so a `(work)` argument form would assemble identically here — I used the no-arg
form to match the shape of the dispatchers already solved elsewhere in the tree.

---

## Summary

| Target | Insns | Copies | bytecmp | Proven byte-exact | Needs |
|---|---|---|---|---|---|
| func_80109034 | 7 | x7 | 2 hard (tool artifact) | YES, by real link | `volatile u16 System_timer` |
| func_8010908C | 7 | x5 | exit 0 | YES | `BOSS_WORK.xB` field |
| func_8010489C | 8 | x2 | exit 0 | YES, by real link | — |
| func_801048BC | 10 | x2 | exit 0 | YES, by real link | — |
| func_801015F4 | 15 | x2 | exit 0 | YES, by real link | `D_8010A1BC` decl |

5/5 solved. Nothing left unresolved.

**Honest caveat on the one red verdict:** func_80109034 is the only one whose correctness
rests on my link-and-compare rather than on a green bytecmp. I ran that comparison
explicitly and all 7 words matched; the reasoning for why bytecmp disagrees is above and
is a property of how splat rendered the reference, not of the C. If you would rather not
take that on trust, land the other four first and gate func_80109034 on the full hash by
itself — it is a 1-line body and cheap to back out.
