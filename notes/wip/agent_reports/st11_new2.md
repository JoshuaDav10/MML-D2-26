# ST11 New Batch 2 — jal-table dispatcher family

All 6 verified with `tools/bytecmp.sh`, exit 0 each.

## Family verdict: YES, one family — and it is bigger than 6

I diffed func_801019CC against func_80101B70 first, as asked. They are
instruction-for-instruction identical except for the table symbol
(D_8010A1EC vs D_8010A23C). All six in the batch are the same 15-instruction
shape. This IS the proven dispatcher family the note mentioned.

Proven body, already landed in ST0C/ST0CB/ST0CC at func_80102CB8:

    void func_80102CB8(u8 *o) { D_8010D414[o[0x8]](); }

Only two things vary across the family:
  1. the table symbol
  2. the byte offset indexed into `o`

I did NOT write anything from scratch — this is the ST0C body with the
symbol and offset swapped, both read from each TARGET's own asm.

### Family size in ST11: 13 total, 7 still unassigned

I scanned every .s in Code801000B4 for the shape (15 insns + `jalr $v0` +
`sll $v0, $v0, 2`). Full inventory:

| function | table | offset | status |
|---|---|---|---|
| func_801015F4 | D_8010A1BC | 0x8   | UNASSIGNED |
| func_801019CC | D_8010A1EC | 0xA   | mine, PASS |
| func_80101B70 | D_8010A23C | 0xA   | mine, PASS |
| func_8010225C | D_8010A2D8 | 0xA   | mine, PASS |
| func_80102898 | D_8010A334 | 0xA   | mine, PASS |
| func_80102E7C | D_8010A3A0 | 0x34B | mine, PASS |
| func_80103264 | D_8010A3B0 | 0x34B | mine, PASS |
| func_80103524 | D_8010A3BC | 0x34B | UNASSIGNED |
| func_80103768 | D_8010A3D4 | 0x34B | UNASSIGNED |
| func_80103B70 | D_8010A3E8 | 0x34B | UNASSIGNED |
| func_80107EBC | D_8010E138 | 0x8   | UNASSIGNED |
| func_80108394 | D_8010E14C | 0x8   | UNASSIGNED |
| func_80109050 | D_8010E26C | 0x8   | UNASSIGNED |

Three offset variants (0x8, 0xA, 0x34B), one template. The 7 unassigned ones
are pure fill-in-the-blanks from the table above — I can do them in one pass
if you want them. Note the note said "appears twice game-wide", so ST11B
presumably mirrors all 13.

## Externs needed — ACTION REQUIRED

None of my six table symbols are declared in
src/ST11/ovl0__progbin_r3_st11.bin/Code801000B4.c yet. Please add to the
extern block (same form the file already uses for D_8010D414 etc.):

    extern void (*D_8010A1EC[])(void);
    extern void (*D_8010A23C[])(void);
    extern void (*D_8010A2D8[])(void);
    extern void (*D_8010A334[])(void);
    extern void (*D_8010A3A0[])(void);
    extern void (*D_8010A3B0[])(void);

No new struct fields needed — the body takes `u8 *o` and indexes raw, so it
touches no typedef at all. I have stripped typedefs from the blocks below as
requested; the `extern` line is shown per function only so you can see which
table each needs, it belongs in the shared extern block.

---

## func_801019CC  PASS

```c
void func_801019CC(u8 *o) {
    D_8010A1EC[o[0xA]]();
}
```
bytecmp: 0 hard mismatch(es), 2 reloc flag(s).

## func_80101B70  PASS

```c
void func_80101B70(u8 *o) {
    D_8010A23C[o[0xA]]();
}
```
bytecmp: 0 hard mismatch(es), 2 reloc flag(s).

## func_8010225C  PASS

```c
void func_8010225C(u8 *o) {
    D_8010A2D8[o[0xA]]();
}
```
bytecmp: 0 hard mismatch(es), 2 reloc flag(s).

## func_80102898  PASS

```c
void func_80102898(u8 *o) {
    D_8010A334[o[0xA]]();
}
```
bytecmp: 0 hard mismatch(es), 2 reloc flag(s).

## func_80102E7C  PASS

```c
void func_80102E7C(u8 *o) {
    D_8010A3A0[o[0x34B]]();
}
```
bytecmp: 0 hard mismatch(es), 2 reloc flag(s).

## func_80103264  PASS

```c
void func_80103264(u8 *o) {
    D_8010A3B0[o[0x34B]]();
}
```
bytecmp: 0 hard mismatch(es), 2 reloc flag(s).

---

## Honest caveat on the relocs

Every one of these reports 2 reloc flags, both on the table symbol:
`lui $at, %hi(D_xxx)` and `lw $v0, %lo(D_xxx)($at)`. bytecmp cannot check
these in a single-function scratch TU because the symbol is unresolved — it
compares 0000013C against the oracle's 1180013C and flags rather than fails.

So what bytecmp PROVES here is the instruction shape and the offset, not the
table address. The addresses are correct by construction (I read each symbol
out of the target's own .s, not from ST10B), but the reloc column is the one
thing only a real link can confirm. If a table symbol is missing or lands at
the wrong address the build, not bytecmp, is what will catch it.

The 0x8/0xA/0x34B offsets ARE fully proven — those are in the hard-compared
`lbu` word.
