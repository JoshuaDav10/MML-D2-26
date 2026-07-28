# LANE B — m2c drafts + cleaned C, ranked best-first

**Generated:** 2026-07-28. Read-only pass; nothing in the repo tree was touched.

## What was done

* Pool: `tools/gen_map.py --query UNSPLIT --max-insns 30` → **244 engine functions**.
  (`--query STUB --max-insns 30` returns only 4, all known-parked:
  func_80042044, func_80041EF4, func_80013578, func_8001A6DC — skipped.)
* **m2c was run on all 244** (`tools/m2c/m2c.py --target mips-gcc-c --context …`).
  0 hard failures; 244 produced output; 183 were structurally clean.
* Context file was generated to **/tmp** (NOT `ctx.c` in the tree — `tools/m2ctx.py`
  writes `<root>/ctx.c`, so it was not used; a hand-rolled `cpp -E -P` of
  `common.h` + all `include/rock_neo/*.h` was used instead).

## Artifacts (all in /tmp)

| path | contents |
|---|---|
| `/tmp/lane_b_drafts.md` | this file |
| `/tmp/lane_b/m2c_raw.json` | all 244 raw m2c outputs + insn counts + asm file + dup counts |
| `/tmp/lane_b/review.txt` | all 244 raw m2c outputs, sorted by instruction count |
| `/tmp/lane_b/ctx.c` | the m2c context file |
| `/tmp/lane_b/getasm.py` | `python3 /tmp/lane_b/getasm.py <func>…` prints target asm |
| `/tmp/lane_b/run.py` | the batch driver (re-runnable) |
| `/tmp/lane_b/gp_syms.txt` | census of every symbol gp-accessed anywhere in `asm/rock_neo/**/*.s` |

---

# READ FIRST — three findings that change how you land these

### 1. gp-vs-lui is decidable *mechanically* for every draft below

I built `/tmp/lane_b/gp_syms.txt` (181 symbols) = every symbol that appears as
`%gp_rel(SYM)` anywhere in the extracted asm. For **every** symbol in every draft below
I checked it against the target asm's own addressing, and the two agree 100%:

* symbol in `gp_syms.txt`  ⇒ target uses `%gp_rel(SYM)($gp)`  ⇒ **plain scalar `extern`**
* symbol not in it         ⇒ target uses `lui $at,%hi / %lo($at)` ⇒ **plain scalar `extern`
  also works** (gprel.py leaves it alone; GAS -G0 expands to the same lui/$at bytes)

So the *only* case that needs the unsized-array spelling is
**address materialisation** — `lui $r,%hi(SYM); addiu $r,$r,%lo(SYM); … 0($r)`.
That occurs in exactly three drafts below (func_8005D4F8, func_8002037C, func_800178FC)
and each is called out.

### 2. TOOLING PREREQUISITE — `ASPSX_REGFIRST_SYM_PREFIXES` is incomplete

`tools/maspx/maspsx/__init__.py:21` has `ASPSX_REGFIRST_SYM_PREFIXES = ("D_801F8",)`.
Census of the whole asm corpus shows the reg-first `addu $at, $reg, $at` order is used
for **far more prefixes** than that:

```
42  D_8015C008     6  D_8013A004     4  D_800DD000     4  D_800DC300
16  D_8015C006     4  D_8015C004     4  D_800DCB80     4  D_800DB800
 2  D_8015C007     4  D_800D9D00     4  D_800DC780     4  STAGE_IDX_LOAD_ADDRESS
 2  D_8015C003     …  D_80164004-8   …  D_801F81xx (already covered)
```

**Pattern:** every reg-first symbol is at an address *above the end of the loaded image*
(0x800D9xxx+) — i.e. an undefined/absolute extern, not a symbol defined in the binary.
Every symbol using the normal `addu $at,$at,$reg` order is a real in-image symbol
(0x8008xxxx / 0x8009xxxx). That is almost certainly the ASPSX rule.

**Consequence:** func_8003ACF4, func_8003786C, func_80037C14 (`D_8015C0xx`) **cannot match
from any C source** until `"D_8015C"` is added to that tuple. They are listed in Tier D
below as *blocked-on-tooling*, not as bad drafts. Do the one-line tool change first;
it likely unblocks a whole family (58 reg-first `D_8015C` sites in the corpus).

### 3. `asm/rock_neo/77D4.s` is hand-written assembly — do not assign it

`func_800175FC` loads `$sp` from a global; `func_800177CC` branches to its own `glabel`;
`func_800175D8` stores `$sp` to memory. This is a task/context-switch runtime. m2c emits
`M2C_BREAK`, "Cannot find branch target <function itself>", and bogus `sp` locals for
several of them — those are *correct* failures, not m2c bugs. **Exclude**
func_800175D8, func_800175FC, func_80017668, func_80017684, func_800176A4, func_800177CC,
func_800177E0, func_800177F8, func_80017820, func_8001782C, func_80017860.
(func_800178FC and func_80017E94 in the same file DO have normal compiler frames and are
kept below, flagged.)

### 4. Adjacent-function groups (land as one TU each, not one TU per function)

These are address-contiguous in the asm, so one `Code<ADDR>.c` carve covers all of them:

| TU | functions | total insns |
|---|---|---|
| `Code8005C218.c` | func_8005C218, C224, C230, C23C, C24C | 17 |
| `Code8005D4C0.c` | func_8005D4C0, D4D4, D4E8, (D4F8) | 24 |
| `Code8005D974.c` | func_8005D974, D97C, (D98C, D9C4, D9EC) | 6 (+27) |
| `Code8005EBA4.c` | func_8005EBA4, EBB4, EBC4, EBD4 | 16 |

⚠️ If you put func_8005D844 and func_8005D87C in one TU you need **two views** of
D_800BBE2C/38/44 (D844 writes them as `sb`/`sh`, D87C zeroes them as `sw`).
Use the proven alias trick: `extern s32 D_800BBE2C_w __asm__("D_800BBE2C");`.

---

# TIER S — write it, compile it, it should just match (very high confidence)

## S1–S6. The six empty functions (2 insns each)

`func_800394A4` (29CA4.s) · `func_80044934` (35134.s) · `func_80044AA8` (352A8.s) ·
`func_80047BD0` (383D0.s) · `func_8005C860` (4D060.s) · `func_8005D974` (4E174.s)

Target asm (all six identical):
```
jr $ra
 nop
```
m2c: `void func_X(void) {\n\n}`

**Cleaned C** — exact precedent already in the tree (`src/rock_neo/Code80043DF8.c`,
`Code8005D96C.c`):
```c
#include "common.h"

void func_800394A4(void) {
}
```
Confidence: **highest in the batch.** These are `copies=378` duplicates of an
already-matched shape. Zero risk; the only cost is the linker/splat carve.

---

## S7. func_8005C218 — 3 insns, 4C710.s

```
sb $a0, %gp_rel(D_80098830)($gp)
jr $ra
 nop
```
m2c:
```c
extern s8 D_80098830;
void func_8005C218(s8 arg0) { D_80098830 = arg0; }
```
**Cleaned C** (D_80098830 IS in the gp census ⇒ scalar extern is right):
```c
extern s8 D_80098830;

void func_8005C218(s8 arg0) {
    D_80098830 = arg0;
}
```
Note: an `s32` param would also emit a bare `sb $a0` (no `andi`), so the param width is
not load-bearing here. Keep `s8`.

## S8. func_8005C224 — 3 insns
```
sw $a0, %gp_rel(D_80098AF4)($gp) ; jr $ra ; nop
```
```c
extern s32 D_80098AF4;

void func_8005C224(s32 arg0) {
    D_80098AF4 = arg0;
}
```

## S9. func_8005C230 — 3 insns
```
sw $a0, %gp_rel(D_800989E0)($gp) ; jr $ra ; nop
```
```c
extern s32 D_800989E0;

void func_8005C230(s32 arg0) {
    D_800989E0 = arg0;
}
```

## S10. func_8005C24C — 3 insns
```
sb $a0, %gp_rel(D_800989D4)($gp) ; jr $ra ; nop
```
```c
extern s8 D_800989D4;

void func_8005C24C(s8 arg0) {
    D_800989D4 = arg0;
}
```
(`game.h:45` comments that `Game_work.x84_tbl` is "indexed by D_800989D4" — the real
name is probably a stage/slot index setter. Type is not load-bearing.)

## S11. func_8005C23C — 4 insns (NOT gp: uses lui/%lo)
```
lui $at, %hi(D_800989EC)
sw  $a0, %lo(D_800989EC)($at)
jr  $ra ; nop
```
```c
extern s32 D_800989EC;

void func_8005C23C(s32 arg0) {
    D_800989EC = arg0;
}
```
Confirmed: D_800989EC is **not** in the gp census, so the scalar extern will not be
gp-rewritten and GAS produces exactly these two words.
*This one is the useful canary:* it and its four neighbours land in one TU, and it proves
gprel.py is discriminating per-symbol rather than per-TU.

## S12. func_80020A40 — 3 insns, 10638.s
```
sw $zero, %gp_rel(D_80097C3C)($gp) ; jr $ra ; nop
```
```c
extern s32 D_80097C3C;

void func_80020A40(void) {
    D_80097C3C = 0;
}
```
⚠️ The asm file has `/* Handwritten function */` on the *next* function — that marker
belongs to the neighbour, not to func_80020A40. Carve carefully.

## S13. func_8005D4E8 / S14. func_8005D97C — 4 insns each
```
lui $at,%hi(SYM) ; sb $zero,%lo(SYM)($at) ; jr $ra ; nop
```
```c
extern s8 D_800BBD84;
void func_8005D4E8(void) { D_800BBD84 = 0; }

extern s8 D_800BBDAB;
void func_8005D97C(void) { D_800BBDAB = 0; }
```

## S15/S16. func_8005D4C0, func_8005D4D4 — 5 insns each
```
addiu $v0,$zero,1 ; lui $at,%hi(D_800BBD84) ; sb $v0,%lo(...)($at) ; jr $ra ; nop
```
```c
extern s8 D_800BBD84;
void func_8005D4C0(void) { D_800BBD84 = 1; }
void func_8005D4D4(void) { D_800BBD84 = 2; }
```
Same TU as func_8005D4E8 (contiguous 0x8005D4C0–0x8005D4F8).

## S17. func_8005D95C — 4 insns (getter)
```
lui $v0,%hi(D_800BBD87) ; lbu $v0,%lo(...)($v0) ; jr $ra ; nop
```
```c
extern u8 D_800BBD87;

u8 func_8005D95C(void) {
    return D_800BBD87;
}
```
`lbu` ⇒ the value is read unsigned; returning `u8` (or `s32` with a `u8` global) both give
`lbu` with no extra `andi` because the return is the loaded reg itself.

## S18/S19. func_8005EBA4, func_8005EBB4 — 4 insns (s16 setters)
```
lui $at,%hi(SYM) ; sh $a0,%lo(SYM)($at) ; jr $ra ; nop
```
```c
extern s16 D_800BBD94;
extern s16 D_800BBD96;

void func_8005EBA4(s16 arg0) { D_800BBD94 = arg0; }
void func_8005EBB4(s16 arg0) { D_800BBD96 = arg0; }
```

## S20/S21. func_8005EBC4, func_8005EBD4 — 4 insns (s16 getters)
```
lui $v0,%hi(SYM) ; lh $v0,%lo(SYM)($v0) ; jr $ra ; nop
```
```c
s16 func_8005EBC4(void) { return D_800BBD94; }
s16 func_8005EBD4(void) { return D_800BBD96; }
```
`lh` (signed) ⇒ the global is `s16`, consistent with the setters above. All four are one
contiguous TU at 0x8005EBA4.

## S22. func_8005DF3C — 4 insns ⚠️ RETURN TYPE IS PINNED
```
sh $zero, %gp_rel(D_800986F8)($gp)
sh $zero, %gp_rel(D_800986FC)($gp)
jr $ra ; nop
```
`include/rock_neo.h:25` already declares `unknown_t func_8005DF3C();` and `game.c:161`
calls it. A `void` definition is a **C89 conflicting-types error**. Define it non-void
with no `return` (cc1 emits the same epilogue; you get a warning, not an error):
```c
extern s16 D_800986F8;
extern s16 D_800986FC;

unknown_t func_8005DF3C(void) {
    D_800986F8 = 0;
    D_800986FC = 0;
}
```
Both symbols are in the gp census; scalar externs are correct.

## S23. func_80039BA4 — 6 insns, 29CA4.s
```
lui $at,%hi(D_800A395A) ; sb $a0,%lo(...)($at)
lui $at,%hi(D_800A395B) ; sb $a1,%lo(...)($at)
jr $ra ; nop
```
```c
extern s8 D_800A395A;
extern s8 D_800A395B;

void func_80039BA4(s8 arg0, s8 arg1) {
    D_800A395A = arg0;
    D_800A395B = arg1;
}
```

## S24. func_800398E0 — 6 insns
```
blez $a0, .L800398F0
 nop
lui $at,%hi(D_800A395F) ; sb $a0,%lo(...)($at)
.L800398F0: jr $ra ; nop
```
```c
extern s8 D_800A395F;

void func_800398E0(s32 arg0) {
    if (arg0 > 0) {
        D_800A395F = arg0;
    }
}
```
Param declared **`s32`, not `s8`** — the compare is on the raw register with no
sign-extension at entry (LESSONS: "when the asm compares a param raw, the param is
int-typed in the source"). m2c guessed `s8`; that would add an `sll/sra` pair.
Branch sense follows LESSONS "bgez/blez that jumps past the store ⇒ `if (x > 0) {…}`".

## S25. func_80050B68 — 3 insns, 3EC90.s
```
lh   $v0, 0x2($a0)
jr   $ra
 sltu $v0, $zero, $v0
```
m2c: `return arg0->unk2 != 0;`
```c
typedef struct { u8 pad0[2]; s16 x2; } OBJ50;   /* or use the real struct once typed */

s32 func_80050B68(OBJ50 *p) {
    return p->x2 != 0;
}
```
The `sltu $v0,$zero,$v0` tail is the clean setcc form (LESSONS func_8001DE84 says this
collapse happens when the diamond is clean — here it *is* the target, so write the
one-liner and do **not** use the goto-shared-return trick).

## S26. Pl00_shot_enable_on — 5 insns, 32A38.s ⚠️ RETURN TYPE PINNED
```
lbu $v0, 0x175($a0) ; nop ; ori $v0,$v0,1
jr $ra
 sb $v0, 0x175($a0)
```
`include/rock_neo/player.h:82-83` declares both Pl00_shot_enable_* as
`unknown_t (PL_WORK*)`. Keep that:
```c
unknown_t Pl00_shot_enable_on(PL_WORK *pl) {
    pl->x175[0] |= 1;
}
```
The `$v0` left holding the OR result is just the RMW temp — do not write `return`.

## S27. Pl00_shot_enable_off — 10 insns, 32A38.s
```
lbu $v0,0x175($a0) ; lbu $v1,0xAC($a0) ; lbu $a1,0xA4($a0)
sb  $zero,0x174($a0)
sb  $zero,0x177($a0)
andi $v0,$v0,0xFE
sb  $v0,0x175($a0)
sb  $v1,0x104($a0)
jr  $ra
 sb $a1,0x100($a0)
```
```c
unknown_t Pl00_shot_enable_off(PL_WORK *pl) {
    pl->x174 = 0;
    pl->x175[2] = 0;            /* 0x177 */
    pl->x175[0] &= 0xFE;
    pl->x101[3] = pl->x76[0x36];  /* 0x104 = 0xAC */
    pl->x100    = pl->x76[0x2E];  /* 0x100 = 0xA4 */
}
```
Offsets check out against `include/rock_neo/player.h` (x76[] spans 0x76..0xB3,
x101[] spans 0x101..0x107, x175[] spans 0x175..0x448).
**Store order in the source is exactly the store order in the asm**; the three `lbu`s
get hoisted by the scheduler (distinct constant offsets off one base ⇒ no alias).

## S28. func_80036478 — 5 insns, 26C78.s
```
srl $a2,$a2,4
sb  $a1,0x6($a0)
sb  $a2,0xF($a0)
jr  $ra
 sb $zero,0x7($a0)
```
```c
void func_80036478(u8 *p, s8 a, u32 b) {
    p[6] = a;
    p[0xF] = b >> 4;
    p[7] = 0;
}
```
`srl` (not `sra`) ⇒ the shifted value is **unsigned**; declare the 3rd param `u32`
(m2c agrees). Prefer a real struct if one is known for this object.

## S29. func_80066750 — 5 insns, 543A8.s
```
addiu $v0,$zero,1
sh    $zero,0x0($a0)
sw    $zero,0x4($a0)
jr    $ra
 sb   $zero,0x4($a0)
```
m2c mis-rendered the `sw`+`sb` as two `unk4 = 0`. Real shape: a halfword, a word and a
byte, and `return 1`:
```c
typedef struct { s16 x0; u8 pad2[2]; s32 x4; } OBJ66;

s32 func_80066750(OBJ66 *p) {
    p->x0 = 0;
    p->x4 = 0;
    *(u8 *)&p->x4 = 0;     /* the sb at +4 after the sw at +4 */
    return 1;
}
```
⚠️ **Uncertain about the last store.** `sw $zero,4` immediately followed by `sb $zero,4`
is a redundant byte store — which means +4 is really *two different declared members*
(e.g. `s32 x4;` and a `u8` union/first-byte field, or a `u8 x4; u8 x5; u16 x6;` group that
one statement zeroes as a word). If the naked form above doesn't match, try:
```c
p->x0 = 0;  p->x4_word = 0;  p->x4_byte = 0;
```
with two named members aliasing +4 (LESSONS "One symbol, two per-TU views" applies to
struct members too). Everything else about this function is certain.

## S30/S31. func_800459A0 and func_8004676C — 8 insns, 352A8.s (byte-identical shape)
```
addiu $v0,$zero,-0x28
sh    $v0,0x2E($a0)
lbu   $v0,0x8($a0)
addiu $v1,$zero,5
sh    $v1,0x34($a0)
addiu $v0,$v0,1
jr    $ra
 sb   $v0,0x8($a0)
```
```c
void func_800459A0(OBJ *o) {
    o->x2E = -0x28;
    o->x34 = 5;
    o->x8 = o->x8 + 1;
}
```
Statement order = store order; the `lbu 0x8` and the `li 5` interleave exactly as the
scheduler places them. Two functions for the price of one draft (`copies=2`).
Use `o->x8 = o->x8 + 1;` (not `o->x8++`) — both should emit `lbu/addiu/sb`, but the
explicit form is what matched in the moji `m->x72` family.

## S32/S33. func_80045918 and func_8004594C — 13 insns, 352A8.s
```
lbu $v0,0xB($a0) ; nop ; addiu $v0,$v0,-1 ; sb $v0,0xB($a0)
andi $v0,$v0,0xFF
bnez $v0, .Lend
 nop
lbu $v0,0x8($a0) ; nop ; addiu $v0,$v0,K ; sb $v0,0x8($a0)
.Lend: jr $ra ; nop
```
```c
void func_80045918(OBJ *o) {
    o->xB = o->xB - 1;
    if (o->xB == 0) {
        o->x8 = o->x8 + 2;      /* K = 1 for func_8004594C */
    }
}
```
The surviving `andi 0xFF` is the give-away that the **test re-reads the field through the
struct** rather than testing a local temp: writing `u8 t = o->xB - 1; o->xB = t; if (t == 0)`
would let combine drop the mask (LESSONS func_80041EF4 genus). Re-reading `o->xB` is
blocked from CSE by nothing here, so cc1 keeps the value *and* re-masks it. If the
re-read form instead emits a second `lbu`, fall back to the temp form and re-check
whether the `andi` survives.
Confidence: high on structure, **medium on the `andi`** — flagged.

## S34. func_8002F998 — 11 insns, 1DB20.s
```
lhu $v0,0x14($a0) ; lhu $v1,0x0($a1) ; nop ; addu $v0,$v0,$v1 ; sh $v0,0x14($a0)
lhu $v0,0x18($a0) ; lhu $v1,0x4($a1) ; nop ; addu $v0,$v0,$v1
jr $ra
 sh $v0,0x18($a0)
```
```c
void func_8002F998(OBJ *o, VEC *d) {
    o->x14 = o->x14 + d->x0;
    o->x18 = o->x18 + d->x4;
}
```
Two fully independent RMW pairs; nothing subtle. `lhu` on both sides ⇒ u16 fields
(the `d` side is a 32-bit-strided struct read as u16, i.e. the low half of a `s32` —
if `d` is a VECTOR, write `d->vx` and `d->vy` and cast, but `lhu 0x0/0x4` reading only
the low half means the source really is `(u16)` typed. Keep the u16 spelling.)

---

# TIER A — high confidence, one judgement call each

## A1. func_80063BA8 — 8 insns, 543A8.s ⚠️ RETURN TYPE PINNED
`rock_neo.h:32` has `unknown_t func_80063BA8();` (called from game.c:209).
```c
extern s32 D_800987B0;
extern s32 D_800987B4;
extern s32 D_800987B8;

unknown_t func_80063BA8(void) {
    D_800987B0 = 0;
    D_800987B4 = 0;
    D_800987B8 = 0;
}
```
Three separate `lui $at,%hi`/`sw %lo($at)` pairs — i.e. three *independent* symbols, not
one array (LESSONS Code800133D8: "constant-index global array → separate %hi/%lo per
element" also produces this, so either spelling works; separate scalars is simpler).
None are in the gp census. ✔

## A2. func_80067E18 — 13 insns, 543A8.s
`scene.c:126` already declares `void func_80067E18(void);` — keep `void`.
```c
extern s32 D_800BE428;
extern s32 D_800BE42C;
extern s32 D_800BE430;
extern s32 D_800BE434;

void func_80067E18(void) {
    D_800BE428 = 0x3F00;
    D_800BE42C = 0x18;
    D_800BE430 = 0x80;
    D_800BE434 = 0;
}
```

## A3. func_8005D87C — 18 insns, 4D1BC.s
```c
extern s32 D_800BBE2C, D_800BBE30, D_800BBE34, D_800BBE38;
extern s32 D_800BBE3C, D_800BBE40, D_800BBE44, D_800BBE48;

void func_8005D87C(void) {
    D_800BBE2C = 0;  D_800BBE30 = 0;  D_800BBE34 = 0;  D_800BBE38 = 0;
    D_800BBE3C = 0;  D_800BBE40 = 0;  D_800BBE44 = 0;  D_800BBE48 = 0;
}
```
Eight `lui $at,%hi` + `sw $zero,%lo($at)` pairs, no base-register CSE ⇒ eight
independent symbols (or eight constant-index elements of one array). **Do not** write it
as a loop or a `memset` — that would strength-reduce to a walking pointer.
⚠️ See the two-views warning if this shares a TU with func_8005D844.

## A4. func_8005D844 — 14 insns, 4D1BC.s
```
addiu $v0,$zero,3 ; lui $at,%hi(D_800BBE2C) ; sb $v0,%lo(...)($at)
addiu $v0,$zero,1                        <-- hoisted
lui $at,%hi(D_800BBE44) ; sh $a0,%lo(...)($at)
lui $at,%hi(D_800BBE46) ; sh $a1,%lo(...)($at)
lui $at,%hi(D_800BBE38) ; sb $a2,%lo(...)($at)
lui $at,%hi(D_800BBE39) ; sb $v0,%lo(...)($at)
jr $ra ; nop
```
```c
extern s8  D_800BBE2C;
extern s16 D_800BBE44;
extern s16 D_800BBE46;
extern s8  D_800BBE38;
extern s8  D_800BBE39;

void func_8005D844(s16 arg0, s16 arg1, s8 arg2) {
    D_800BBE2C = 3;
    D_800BBE44 = arg0;
    D_800BBE46 = arg1;
    D_800BBE38 = arg2;
    D_800BBE39 = 1;
}
```
The `li 1` sitting *before* the D_800BBE44 store is just scheduling of the last
statement's constant — do not reorder the source to chase it.

## A5. func_800395E8 — 18 insns, 29CA4.s
```c
extern s32 D_800A396C;
extern s32 D_800A3964;
extern s8  D_800A3962;
extern s8  D_800A3960;
extern s32 D_800A39B4;
extern s8  D_800A3959;

void func_800395E8(void) {
    D_800A396C = 0x30;
    D_800A3964 = 0x200;
    D_800A3962 = -1;
    D_800A3960 = 0;
    D_800A39B4 = 0x96;
    D_800A3959 = 0;
}
```
(`li 0x96` is hoisted one slot above the D_800A3960 store — scheduler, not source order.)

## A6. func_800397E8 — 14 insns, 29CA4.s (sibling of A5)
```c
#include "rock_neo/player.h"
extern s32 D_800A396C;
extern s32 D_80097D2C;    /* gp-accessed  */
extern s32 D_800A3964;
extern s8  D_800A3962;

void func_800397E8(void) {
    D_800A396C = 0x30;
    D_80097D2C = 0x200;
    D_800A3964 = 0x200;
    D_800A3962 = -1;
    Player_work.x44C[1] = 0;
}
```
Note the single `li 0x200` feeds **both** the gp store and the lui store — that is CSE of
the constant, so writing `0x200` twice is correct. `Player_work + 0x44D` = `x44C[1]`. ✔

## A7. func_80039DE8 — 12 insns, 29CA4.s
```
lw $v0,%gp_rel(D_80098674)($gp)
lw $a0,%gp_rel(D_80098670)($gp)
addiu $v1,$zero,0x200
sw $v1,%gp_rel(D_80097D2C)($gp)
lui $at,%hi(Player_work+0x44D) ; sb $zero,%lo(...)($at)
lui $at,%hi(Scene_work+1)      ; sb $v0,%lo(...)($at)
lui $at,%hi(Scene_work)        ; sb $a0,%lo(...)($at)
```
```c
#include "rock_neo/player.h"
#include "rock_neo/scene.h"
extern s32 D_80097D2C;
extern s32 D_80098670;
extern s32 D_80098674;

void func_80039DE8(void) {
    D_80097D2C = 0x200;
    Player_work.x44C[1] = 0;
    Scene_work.x1 = D_80098674;
    Scene_work.x0 = D_80098670;
}
```
The two `lw`s hoist to the top because the loads are side-effect-free and the stores are
to different symbols (LESSONS func_8001F6C4 "hoist an unconditionally-safe load"). Write
them in *use* order (x1 before x0) — that is what the store order says.

## A8. func_80039E18 — 14 insns, 29CA4.s (A7 minus the first two stores, plus a call)
```c
#include "rock_neo/scene.h"
extern s32 D_80098670;
extern s32 D_80098674;
void func_80039E50(void);

void func_80039E18(void) {
    Scene_work.x1 = D_80098674;
    Scene_work.x0 = D_80098670;
    func_80039E50();
}
```
⚠️ func_80039E50 is itself unsplit; declare it `void … (void)` here. If a later diff
shows a spurious nop after the call, re-type it non-void (LESSONS func_80057B70).

## A9. func_80039A38 — 10 insns, 29CA4.s
```c
extern s32 D_80097D6C;   /* gp */
extern s32 D_80097D70;   /* gp */
void func_800399EC(void);

void func_80039A38(void) {
    D_80097D6C = 0;
    D_80097D70 = 0;
    func_800399EC();
}
```

## A10. func_80033EBC — 18 insns, 22D38.s — *mixed gp and lui in one function*
```c
extern s32 D_800988E0;   /* lui */
extern s32 D_80098798;   /* lui */
extern s8  D_8009A306;   /* lui */
extern s8  D_8009A31A;
extern s8  D_8009A32E;
extern s8  D_8009A342;
extern s32 D_80098560;   /* gp  */
extern s32 D_80098564;   /* gp  */
extern s32 D_80098568;   /* gp  */

void func_80033EBC(void) {
    D_800988E0 = 0;
    D_80098798 = 0xFF;
    D_8009A306 = 0;
    D_8009A31A = 0;
    D_8009A32E = 0;
    D_8009A342 = 0;
    D_80098560 = 0;
    D_80098564 = 0;
    D_80098568 = 0;
}
```
This is the best **validation function in the whole batch**: 6 lui-form and 3 gp-form
stores in one body, all from plain scalar externs. If it matches, the gp census rule is
proven end-to-end and you can trust every other draft's addressing. **Land this early.**
(The `li 0xFF` is hoisted to the very first insn — scheduling, ignore.)
The four `D_8009A3xx` at 0x14 stride are almost certainly `field[i]` of a 0x14-stride
array; separate scalars produce the same bytes, so don't bother modelling it.

## A11. func_800333A0 — 5 insns, 22D38.s
```
lui $v0,%hi(D_800BC728) ; addiu $v0,$v0,%lo(D_800BC728)
sw $v0, %gp_rel(D_80098A80)($gp)
```
```c
extern u8 D_800BC728[];
extern u8 *D_80098A80;

void func_800333A0(void) {
    D_80098A80 = D_800BC728;
}
```
`&SYM` taken as a value ⇒ `lui/addiu`; the pointer global is written once ⇒ **plain
scalar pointer decl** (LESSONS: array-of-pointer spelling is only for single-read
pointers; a write wants the scalar form).

## A12. func_80051B40 — 10 insns, 3EC90.s (same idiom, two pointers, lui form)
```c
extern u8 D_800BC058[];
extern u8 D_800BC360[];
extern u8 *D_80098894;
extern u8 *D_80098898;

void func_80051B40(void) {
    D_80098894 = D_800BC058;
    D_80098898 = D_800BC360;
}
```

## A13. func_800665E8 — 5 insns, 543A8.s
```
sh $zero, %gp_rel(D_80098910)($gp)
lui $at,%hi(D_80098914) ; sw $zero,%lo(D_80098914)($at)
```
```c
extern s16 D_80098910;
extern s32 D_80098914;

void func_800665E8(void) {
    D_80098910 = 0;
    D_80098914 = 0;
}
```
⚠️ **D_80098910 is the known two-views symbol** — LESSONS records that game.c declares it
as an *unsized array* because game.c's uses are lui-form. Here it must be a **plain `s16`
scalar** to get the `%gp_rel`. That is legal (per-TU views) as long as this goes in a
**new TU**, not into game.c.

## A14. func_8005D938 — 9 insns, 4D1BC.s
```
lui $v0,%hi(D_800BBD94) ; lh $v0,%lo(...)($v0) ; nop
bgtz $v0, .Lend
 addu $v0,$zero,$zero
lui $v0,%hi(D_800BBD87) ; lbu $v0,%lo(...)($v0)
.Lend: jr $ra ; nop
```
```c
extern u8  D_800BBD87;
extern s16 D_800BBD94;

s32 func_8005D938(void) {
    s32 var = 0;
    if (D_800BBD94 <= 0) {
        var = D_800BBD87;
    }
    return var;
}
```
The `addu $v0,$zero,$zero` in the branch delay slot is the `= 0` initialiser
(LESSONS #6: `addu v0,zero,zero` = 0), and the `<= 0` sense (not `> 0`) is fixed by
"bgtz jumping to the tail ⇒ the *other* arm is the body".
Keep the single accumulator variable, not two `return`s — a shared `var` is what puts
everything in `$v0` (LESSONS func_80019AA4).

## A15. func_80039B80 — 9 insns, 29CA4.s (same genus as A14)
```c
extern u8  D_800A3960;
extern s32 D_800A396C;

s32 func_80039B80(void) {
    s32 var = 0;
    if (D_800A3960 != 0) {
        var = D_800A396C;
    }
    return var;
}
```

## A16. func_80052644 — 12 insns, 3EC90.s
```
lb $v0,0x76($a1) ; nop ; beqz $v0,.Lend ; nop
lw $v0,0xC0($a1) ; nop ; bne $a0,$v0,.Lend ; nop
sb $zero,0x76($a1)
sw $zero,0xC0($a1)
```
```c
void func_80052644(s32 arg0, OBJ *o) {
    if (o->x76 != 0 && arg0 == o->xC0) {
        o->x76 = 0;
        o->xC0 = 0;
    }
}
```
`lb` (signed) ⇒ x76 is `s8`. Two separate `beqz`/`bne` exits to a shared tail = a plain
`&&` (no cross-jumping to worry about here since both arms just fall to `jr`).

## A17. func_800354E0 — 10 insns, 22D38.s
```
lui  $v0,0x8000
srlv $v0,$v0,$a0
lui  $v1,%hi(D_80098798) ; lw $v1,%lo(...)($v1)
nor  $v0,$zero,$v0
and  $v1,$v1,$v0
lui  $at,%hi(D_80098798) ; sw $v1,%lo(...)($at)
```
```c
extern u32 D_80098798;

void func_800354E0(s32 arg0) {
    D_80098798 &= ~(0x80000000 >> arg0);
}
```
Two separate `lui %hi(D_80098798)` (one for the load, one for the store) ⇒ **plain scalar
extern**, NOT the held-pointer / array spelling (contrast func_8005D4F8 below, where the
address *is* materialised once). `srlv` (logical) ⇒ the shifted constant is unsigned:
write `0x80000000` (unsigned by default in C89 since it doesn't fit in `int`), and the
`nor` is `~`.
The load of D_80098798 is emitted *between* the shift and the `nor`, i.e. the scheduler
hoisted it — expected from a single expression. Confidence high.

## A19. func_8005D4F8 — 12 insns, 4D1BC.s ⚠️ **ARRAY EXTERN REQUIRED**
```
lui   $v1,%hi(D_800BBD85)
addiu $v1,$v1,%lo(D_800BBD85)      <-- address MATERIALISED
lbu   $v0,0x0($v1)
bnez  $v0,.Lend
 addiu $v0,$zero,2
sb    $v0,0x0($v1)                 <-- same held register
addiu $v0,$zero,0x1C
lui   $at,%hi(D_800BBD81) ; sb $v0,%lo(D_800BBD81)($at)   <-- NOT materialised
```
This is the exact func_8001CB7C idiom (read+write of one symbol through one materialised
address):
```c
extern u8 D_800BBD85[];    /* unsized ARRAY — makes cc1 materialise the address once */
extern s8 D_800BBD81;      /* plain scalar — two-insn lui/%lo form                    */

void func_8005D4F8(void) {
    if (D_800BBD85[0] == 0) {
        D_800BBD85[0] = 2;
        D_800BBD81 = 0x1C;
    }
}
```
The `li 2` in the `bnez` delay slot comes free (LESSONS: an assignment placed before a
branch lands in that branch's delay slot). **Confidence high**, and this one is worth
landing early because it re-validates the array-vs-scalar rule from a fresh site.

## A20. func_8005D98C — 14 insns, 4E174.s
```c
extern s8  D_800BBD87;
extern s16 D_800BBD94;
void func_8005D4F8(void);

void func_8005D98C(void) {
    if (D_800BBD94 <= 0) {
        D_800BBD87 = 1;
        func_8005D4F8();
    }
}
```
Note the frame setup (`addiu $sp,-0x18`) is *scheduled into* the compare/branch —
that is normal cc1 prologue scheduling, not a source artifact.

## A21. func_8005D9C4 — 10 insns, 4E174.s
```c
extern s8 D_800BBD87;
void func_8005D528(void);

void func_8005D9C4(void) {
    D_800BBD87 = 0;
    func_8005D528();
}
```

## A22. func_8005D9EC — 12 insns, 4E174.s
```
lui $v0,%hi(Player_work+0xB0) ; lhu $v0,%lo(...)($v0)
addiu $sp,-0x18 ; sw $ra,0x10($sp)
lui $at,%hi(Player_work+0x6E) ; sh $v0,%lo(...)($at)
jal func_8005D9C4
```
`Player_work + 0x6E` is `life` (player.h:23 `u16 life;` at 0x6E); `+0xB0` is inside
`x76[]` ⇒ `x76[0x3A]`, but it is read as a **halfword**, so the cleanest spelling is a
`u16` view. m2c invented `Player_work.unkB0`; add a named member or use a neighbour cast:
```c
#include "rock_neo/player.h"
void func_8005D9C4(void);

void func_8005D9EC(void) {
    Player_work.life = *(u16 *)&Player_work.x76[0x3A];   /* 0x76+0x3A = 0xB0 */
    func_8005D9C4();
}
```
Better: add `u16 xB0;` to PL_WORK by splitting `x76[]` — but that edits a shared header
and can disturb other TUs. **Flagged as the only real uncertainty here** (the cast form
should produce the same `lhu %lo(Player_work+0xB0)` because the address is a constant
folded at compile time; if cc1 instead materialises `&Player_work` into a register, split
the struct member instead).

## A23. func_8005DCB4 — 10 insns, 4E174.s
```c
void func_8005DCDC(void);
void func_8005DE28(void);

void func_8005DCB4(void) {
    func_8005DCDC();
    func_8005DE28();
}
```

## A24–A30. The jalr-dispatcher family (15 insns each) — **highest leverage in the pool**

Shape (identical in all of them):
```
addiu $sp,-0x18 ; sw $ra,0x10($sp)
lbu/lb $v0, OFF($a0) ; nop
sll   $v0,$v0,2
lui   $at,%hi(TBL) ; addu $at,$at,$v0 ; lw $v0,%lo(TBL)($at) ; nop
jalr  $v0 ; nop
lw $ra,0x10($sp) ; addiu $sp,0x18 ; jr $ra ; nop
```
Proven template already in the tree — `scene.c:174/181`
(`D_80088FA8[Game_work.stage_no]();`) and `game.c:26` (`Game_main_tbl[…](gp)`):

```c
extern void (*D_8008A0CC[])(void);

void func_800433C0(OBJ *o) {
    D_8008A0CC[o->x8]();
}
```

| function | asm | index | table | load |
|---|---|---|---|---|
| func_800433C0 | 32A38.s | `o->x8` | D_8008A0CC | lbu (u8) |
| func_80043F34 | 34600.s | `o->x8` | D_8008A208 | lbu |
| func_8004495C | 35134.s | `o->x8` | D_8008A220 | lbu |
| func_80044AB0 | 352A8.s | `o->x8` | D_8008A250 | lbu |
| func_80044C58 | 352A8.s | `o->x8` | D_8008A258 | lbu |
| func_80045074 | 352A8.s | `o->x8` | D_8008A288 | lbu |
| func_80045238 | 352A8.s | `o->x8` | D_8008A290 | lbu |
| func_8004569C | 352A8.s | `o->x8` | D_8008A358 | lbu |
| func_80047318 | 352A8.s | `o->x8` | D_8008A708 | lbu |
| func_80047D9C | 383D0.s | `o->x8` | D_8008A724 | lbu |
| func_80048354 | 383D0.s | `o->x8` | D_8008A76C | lbu |
| func_800485E4 | 383D0.s | `o->x4` | D_8008A778 | lbu |
| func_8005C720 | 4C710.s | `o->x4` | D_8008D7B4 | lbu |
| func_8005C868 | 4D060.s | `o->x4` | D_8008D7C0 | lbu |
| func_8005C978 | 4D060.s | `o->x4` | D_8008D7C4 | lbu |
| func_800426F8 | 32A38.s | `pl->x170` | D_8008A08C | **lb (s8)** |
| func_80042734 | 32A38.s | `pl->x171` | D_8008A0C4 | **lb (s8)** |

Variants that need one extra line:

* **func_800153EC** (52C0.s) — the callee takes an argument built in the jalr delay slot:
  ```c
  extern void (*D_80082104[])(u8 *);
  void func_800153EC(u8 *o) { D_80082104[o[8]](o + 0x348); }
  ```
* **func_80017E94** (77D4.s, but a normal compiler frame) — index from a global:
  ```c
  extern void (*D_80082140[])(void);
  extern s8 D_80098A68;                 /* lb ⇒ signed */
  void func_80017E94(void) { D_80082140[D_80098A68](); }
  ```
* **func_800666D8 / func_80066714** (543A8.s, `copies=2`) — return 0 after the call:
  ```c
  extern void (*D_8008FD20[])(void);
  s32 func_800666D8(OBJ *o) { D_8008FD20[o->x5](); return 0; }
  ```
  (`addu $v0,$zero,$zero` sits after the `lw $ra`, i.e. the `return 0` — LESSONS #6.)

All these tables use the **normal** `addu $at,$at,$v0` order, so no maspx change needed.
Confidence: **high** — the exact source template is already hash-proven twice.
`family=672` on most of these means the shape recurs across the whole game; getting one
right is a template for hundreds.

## A31–A37. Trivial tail-call wrappers (8 insns) — `copies=43`, `family=324`

```
addiu $sp,-0x18 ; sw $ra,0x10($sp) ; jal CALLEE ; nop
lw $ra,0x10($sp) ; addiu $sp,0x18 ; jr $ra ; nop
```
Pass-through form (no `move $a0` because the param pseudo already lives in `$a0`):
```c
void func_80031988(OBJ *);

void func_8004493C(OBJ *o) {
    func_80031988(o);
}
```
| function | asm | callee |
|---|---|---|
| func_8004493C | 35134.s | func_80031988 |
| func_8004557C | 352A8.s | func_80031988 |
| func_80045980 | 352A8.s | func_80031988 |
| func_8004674C | 352A8.s | func_80031988 |
| func_80047BD8 | 383D0.s | func_80031988 |
| func_800485C4 | 383D0.s | func_80031988 |
| func_80047C64 | 383D0.s | func_80047CD8 |
| func_800153CC | 52C0.s  | func_80031824 |

⚠️ **Two possible sources, same bytes** — `void f(OBJ *o) { g(o); }` and
`void f(void) { g(); }` (with `g` declared K&R-style, no args) both emit exactly this.
Prefer the pass-through: these are state-handler slots and the caller does pass a
pointer. If the callee's prototype forces an argument you don't have, use the K&R form.

**func_80044A80** (35134.s, 8 insns) is the same wrapper with a store in the delay slot:
```c
void func_80044A80(OBJ *o) {
    o->x0 = 0;
    func_80031988(o);
}
```
(`sb $zero,0x0($a0)` is IN the `jal` delay slot, and `$a0` is unchanged — so the store
precedes the call in source order and `o` is still the argument.)

## A38. func_8002F9EC / func_8002FA10 — 9/10 insns, 201EC.s
```
addu $a2,$a1,$zero
jal  func_8002FA38
 lui $a1,(0x80190000 >> 16)          <-- func_8002F9EC
 / lui $a1,0x801F ; ori $a1,$a1,0x2000   <-- func_8002FA10
```
```c
void func_8002FA38(void *, void *, s32);

void func_8002F9EC(void *arg0, s32 arg1) {
    func_8002FA38(arg0, (void *)0x80190000, arg1);
}

void func_8002FA10(void *arg0, s32 arg1) {
    func_8002FA38(arg0, (void *)0x801F2000, arg1);
}
```
LESSONS: "Raw-constant pointers use lui/ori, symbols use lui/%hi" — 0x801F2000 gives the
`lui`+`ori` pair, 0x80190000 gives a bare `lui` (low half is 0). Write them as raw
constants, **not** as externs. The `addu $a2,$a1,$zero` is the real 3rd-arg move
(a1 → a2), which is a genuine instruction, so the 2-arg → 3-arg shuffle is correct.

## A39. func_80049448 — 10 insns, 393EC.s
```
lw   $a1,0x78($a0) ; lbu $a2,0x1($a0)
jal  func_80049470
 addiu $a0,$a0,0x14
```
```c
void func_80049470(u8 *, s32, u8);

void func_80049448(u8 *o) {
    func_80049470(o + 0x14, *(s32 *)(o + 0x78), o[1]);
}
```
Args evaluate right-to-left, so the `$a0` adjust naturally lands last (in the delay slot).
Use the real struct if one exists for this object; the `u8 *` view reproduces the offsets.

## A40. func_80048E6C — 14 insns, 393EC.s ⚠️ RETURN TYPE `s32`
`player.c:485` declares `s32 func_80048E6C(void);` and uses the return value
(`func_80048E6C() - 0x20`) — the `(void)` there is a deliberate hack because the caller
leaves `$a0` set up. In the **new** TU, define it with the real parameter:
```c
s32 func_80048EA4(u8 *, s32, s32, u8);

s32 func_80048E6C(u8 *o) {
    return func_80048EA4(o + 0x14,
                         *(s32 *)(o + 0x78),
                         *(s16 *)(o + 0x16) - *(s16 *)(o + 0x2E),
                         o[1]);
}
```
The `addu $v0,$a0,$zero` at the top is cc1 preserving the base pointer because `$a0` is
about to be overwritten by the first argument — it comes for free, don't chase it.

## A41. func_80030604 / func_8003648C — 9 insns each ⚠️ m2c GOT THE SIGNATURE WRONG

m2c reported `func_80030604(void *arg0) { func_80030628((s32) arg0->unk56); }` — it did
not notice that `$a1/$a2/$a3` are **live pass-throughs** (never written, so m2c thinks
they're dead).
```
func_80030604:  lh $v0,0x56($a0) ; jal func_80030628 ; sw $v0,0x10($sp)
```
`0x10($sp)` is the **5th argument slot** (o32 ABI), so:
```c
void func_80030628(OBJ *, s32, s32, s32, s32);

void func_80030604(OBJ *o, s32 a1, s32 a2, s32 a3) {
    func_80030628(o, a1, a2, a3, *(s16 *)((u8 *)o + 0x56));
}
```
```
func_8003648C:  lh $a2,0x56($a1) ; jal func_800364B0 ; nop
```
```c
void func_800364B0(s32, OBJ *, s16);

void func_8003648C(s32 a0, OBJ *o) {
    func_800364B0(a0, o, *(s16 *)((u8 *)o + 0x56));
}
```
**Same trap in func_80030934 / func_80030CB8 / func_80030CF0** — check each for a
`sw $vN,0x10($sp)` in the delay slot (5th arg) and for untouched `$a1..$a3`.
For func_80030934 the two `sll/sra 16` pairs on `$a2/$a3` are s16 **parameter**
sign-extensions:
```c
void func_80030968(s32, s32, s16, s16, s32);

void func_80030934(s32 a0, s32 a1, s16 a2, s16 a3) {
    func_80030968(a0, a1, a2, a3, 0x100);
}
```
Confidence: **medium-high on structure, medium on the exact arity** — verify each
callee's true arity before landing (a wrong arity changes nothing in *this* function's
bytes as long as the extra params are pass-throughs, but a wrong *type* can add a
sign-extension).

## A42. func_8002037C — 11 insns, 10638.s ⚠️ ARRAY EXTERN
```
lui $v0,%hi(D_800BEAD8) ; addiu $v0,$v0,%lo(D_800BEAD8)    <-- materialised
sb  $zero,0x0($v0)
lui $at,%hi(D_8009893C) ; sw $v0,%lo(D_8009893C)($at)
lui $at,%hi(D_800987D4) ; sb $zero,%lo(...)($at)
lui $at,%hi(D_800987DC) ; sb $zero,%lo(...)($at)
```
```c
extern u8  D_800BEAD8[];
extern u8 *D_8009893C;
extern s8  D_800987D4;
extern s8  D_800987DC;

void func_8002037C(void) {
    D_800BEAD8[0] = 0;
    D_8009893C = D_800BEAD8;
    D_800987D4 = 0;
    D_800987DC = 0;
}
```
The address is materialised because it is *both* dereferenced and stored as a value —
cse merges the two into one `lui/addiu`. Array spelling required.

## A43. func_80036B00 — 16 insns, 26C78.s
```c
extern u8  D_8009A370;
extern u8  D_8009A371;
extern u8  D_8009A372;
extern u16 D_8009A374;

void func_80036B00(s16 *p) {
    p[0] = D_8009A370;
    p[1] = D_8009A371;
    p[2] = D_8009A372;
    p[3] = D_8009A374;
}
```
Three `lbu`→`sh` and one `lhu`→`sh`; each with its own `lui %hi` ⇒ independent scalars.
Load-delay `nop`s between each pair are automatic.

## A44. func_800178FC — 18 insns, 77D4.s (normal compiler frame — safe despite the file)
```
lui/addiu $a0 = &D_80097A28
sw $s0,0x10($sp) ; lui/addiu $s0 = &D_800B1168
jal LoadImage ; addu $a1,$s0,$zero
lui/addiu $a0 = &D_80097A30
jal LoadImage ; addiu $a1,$s0,0x2000
```
```c
#include "psxsdk/libgpu.h"
extern RECT D_80097A28;
extern RECT D_80097A30;
extern u_long D_800B1168[];

void func_800178FC(void) {
    u_long *p = D_800B1168;             /* held pointer: LESSONS func_8001D394 */
    LoadImage(&D_80097A28, p);
    LoadImage(&D_80097A30, p + 0x800);  /* +0x2000 bytes */
}
```
`LoadImage` is already prototyped in `include/psxsdk/libgpu.h:127`. The **held local
pointer is load-bearing** — writing `LoadImage(&D_80097A30, D_800B1168 + 0x800)` would
constant-fold to `lui/addiu %hi/%lo(SYM+0x2000)` and lose the `$s0` reuse.

## A45. func_80031268 — 12 insns, 201EC.s
```
lh $a2,0x0($a1) ; lh $v0,0x0($a0) ; lh $v1,0x4($a1) ; lh $a1,0x4($a0)
subu $a0,$a2,$v0
jal  ratan2
 subu $a1,$v1,$a1
addiu $v0,$v0,0x800
```
```c
s32 ratan2(s32, s32);

s32 func_80031268(s16 *a, s16 *b) {
    return ratan2(b[0] - a[0], b[2] - a[2]) + 0x800;
}
```
⚠️ m2c guessed `ratan2(y, x, temp_a2)` — 3 args — because `$a2` still holds `b[0]` at the
`jal`. It doesn't: `$a2` is a *temp with another use* (the first `subu`), so the LESSONS
rule "an arg-register load living across a jal with no other use is that call's argument"
does **not** fire. PSY-Q `ratan2` is 2-arg. Declare it locally (`ratan2` is not in
`include/psxsdk/`).
If the operands are a `SVECTOR`-ish struct, use `b->vx - a->vx` etc.; the `lh 0x0/0x4`
stride says 4 bytes between the two members.

## A46. func_80048CEC — 7 insns, 393EC.s
```
lui $v0,(0x1F800048>>16) ; lw $v0,(0x1F800048&0xFFFF)($v0)
sll $a0,$a0,1
addu $a0,$a0,$v0
lh  $v0,0x0($a0)
```
```c
s16 func_80048CEC(s32 i) {
    return (*(s16 **)0x1F800048)[i];
}
```
0x1F800048 is scratchpad; the raw-constant deref reproduces the `lui`+offset form.
`addu $a0,$a0,$v0` is index-first, which the bracket form `p[i]` gives (LESSONS
func_8001319C). Confidence high; if the addu mirrors, switch to `*(base + i)`.

## A47. func_80036A78 — 4 insns, 26C78.s
```c
void func_80036A78(s16 arg0) {
    *(s16 *)0x1F800076 = arg0;
}
```

---

# TIER B — plausible, but one thing I could not pin down (compile these second)

## B1. func_8004327C — 6 insns, 32A38.s
```
lui $v0,%hi(Game_work+0x1C) ; lw $v0,%lo(Game_work+0x1C)($v0)
negu $a0,$a0
sltu $v0,$v0,$a0
jr $ra
 negu $v0,$v0
```
`Game_work + 0x1C` is `zennyCount` (game.h:22).
```c
#include "rock_neo/game.h"

s32 func_8004327C(s32 arg0) {
    return -(Game_work.zennyCount < (u32)-arg0);
}
```
⚠️ **Uncertain about the source spelling.** The `sltu … ; negu` tail is what cc1 emits for
`-(a < b)`, `0 - (a < b)` and `(a < b) ? -1 : 0` alike, and I could not tell which the
original used. All three should produce identical RTL after cse; if the first fails, try
the ternary. Semantically this looks like "would `zenny + arg0` wrap" ⇒ an affordability
/ overflow test returning 0 / -1.

## B2. func_80045FE4 / func_80046008 — 9 insns, 352A8.s (`copies=2`)
```
addiu $sp,-0x18 ; sw $ra,0x10($sp)
lbu $a0,0x1($a0)
jal func_8002FE74 ; nop
lw  $ra,0x10($sp)
addu $v1,$v0,$zero          <-- ???
jr  $ra
 addiu $sp,$sp,0x18
```
Naive draft:
```c
s32 func_8002FE74(u8);

void func_80045FE4(OBJ *o) {
    func_8002FE74(o->x1);
}
```
⚠️ **This will be one instruction SHORT.** The `addu $v1,$v0,$zero` is a live
register-to-register copy of the return value into `$v1` that is then never used — and
per LESSONS ("When does a register-to-register copy survive?") a plain `b = a;` in C is
always deleted by cse+combine_regs, so it cannot come from an obvious source. Two things
to try, in order:
1. the NFSHS "void→non-void discriminator": declare the function non-void and
   `return`-something that dies (`s32 func_80045FE4(OBJ *o) { s32 r = func_8002FE74(o->x1); }`);
2. a reload live-range split — i.e. this may need the permuter.
**Do not spend a build cycle on this one before the Tier S/A queue is exhausted.**

## B3. func_8004324C — 12 insns, 32A38.s
```
lb $v0,0x174($a0) ; nop ; andi $v0,$v0,0xC0
bnez $v0,.Lend
 addiu $v0,$zero,1
lb $v0,0x177($a0) ; nop
bnez $v0,.Lend
 addiu $v0,$zero,1
addu $v0,$zero,$zero
.Lend: jr $ra ; nop
```
`player.c:21` already declares `s32 func_8004324C(PL_WORK*)` — good.
```c
s32 func_8004324C(PL_WORK *pl) {
    if (pl->x174 & 0xC0) {
        return 1;
    }
    if (pl->x175[2] != 0) {          /* 0x177 */
        return 1;
    }
    return 0;
}
```
⚠️ **Uncertain**: the target has **two separate `li 1`s** (one per delay slot) and does
NOT cross-jump them. LESSONS records both directions of this lever, and the NFSHS note
says "do NOT let gcc tail-merge duplicate epilogues — keep TWO separate `return 1`
blocks". Separate `return 1;` statements is therefore the right first try; if cc1
cross-jumps them into one, try the goto-shared-return trick on the `return 0` instead.
Note `lb` on x175[2] means it is read as **signed** here, so cast: `(s8)pl->x175[2]`.

## B4. func_8002FE74 — 12 insns, 201EC.s
```c
s32 func_8002FE74(u32 arg0) {
    s32 v;
    if (arg0 < 0x20) {
        v = arg0 & 0x10;
    } else {
        v = arg0 & 0xE0;
        if (v == 0xA0) {
            v = 0x80;
        }
    }
    return v;
}
```
⚠️ The `v == 0xA0` equality leg is exactly the **jump-canonicalization genus** that cost
several sessions (LESSONS func_8001FCA4). Per the closing entry, try a `switch` on the
masked value FIRST if the if/else form inverts the branch. Structure itself is certain.

## B5. func_80050BF4 / func_80050B74 / func_80050BB4 — 14/16 insns, 3EC90.s
All three poke the scratchpad byte at 0x1F8000FF. m2c is clean but mixes types
(`*(void*)0x1F8000FF = 0x10`). Cleaned:
```c
s32 func_80050BF4(s16 *p) {
    if (*p <= 0) {
        *(u8 *)0x1F8000FF = 0x20;
        return 1;
    }
    *(u8 *)0x1F8000FF = 0x10;
    return 0;
}

void func_80050B74(s16 *p) {
    u8 v;
    if (*(u8 *)0x1F8000FF & 3) {
        *(s16 *)0x1F800106 = 0;
    }
    v = 1;
    if (*p <= 0) {
        v = 2;
    }
    *(u8 *)0x1F8000FF = v;
}
```
(func_80050BB4 is the same with 0xC / 0x1F80010A / 4 / 8 and the test on `p->x4`.)
⚠️ Medium confidence — the two-`return` vs single-accumulator choice in func_80050BF4
and the `v` accumulator in func_80050B74 are exactly the levers that flip `$v0`/`$v1`.
Diff before believing.

## B6. func_8005D4F8's siblings func_80045038 / func_80045608 / func_80046848 /
func_80045A7C / func_80048268 — 15–18 insns, 352A8.s / 383D0.s
m2c output is clean for all five; I did not hand-verify their asm. Raw drafts are in
`/tmp/lane_b/review.txt`. Treat as Tier B until someone reads the asm.

---

# TIER C — draft is right but the callee/table type is unknown

* **func_800435BC** (15 insns) — `func_80036294(o->xC)->x198 = 0; func_80031930(o);`
  Needs func_80036294's real return type (a struct pointer). LESSONS: "a called
  function's declared RETURN TYPE steers the caller's register allocation" — so this
  cannot be landed on a guess.
* **func_80032304** (18 insns, 22B04.s) — clean m2c, needs the object struct.
* **func_80051B68 / func_80051BAC** (17/14 insns, 3EC90.s) — a 0xC-stride display-list
  writer through `D_80098894`/`D_80098898`. Needs the entry struct; `D_80098894` is
  read-and-written, so per LESSONS it wants a **plain scalar pointer** decl (not the
  array-of-pointer form). Good candidate once the struct is named.
* **func_80063EF0** (14 insns) — a 4-iteration 0xC-stride clear loop over `D_800C3528`
  with **two cursors** ($a0 at +0 and $v1 at +8, writing -4 and 0). LESSONS
  func_8001CAAC says: write the fields *individually*, not as a struct copy, to keep the
  second cursor. Returns the loop's `slt` result (`temp_v0`), i.e. `return 0;`.
* **func_8002DE04 / func_8002DE20 / func_8002DE34** (5–10 insns, 1DB20.s) — indexed
  0x8/0x10-stride copies. Trivial once the element type is known.
* **func_8002DD68** (14 insns) — contains `lwl/lwr`. ⚠️ Unaligned access ⇒ needs `-mel`
  (already in the flags per the 2026-07-26 lesson) AND m2c can't decompile it
  ("Unable to handle lwr"). Do this one by hand from the asm.
* **func_80037594** (16 insns) — `Player_work.weapon_data[...]` + `D_80089EAC` table;
  m2c produced a monster one-liner. Structurally understood, needs the 8-byte table
  element type.
* **func_800375D4 / 604 / 634 / 664** (12 insns each, `family=4`) — same `D_80089EAC`
  table, 4 near-identical accessors. High leverage once one is solved.
* **func_8003697C** (17 insns) — scratchpad pokes + `SetFogNear(D_800BC81C, 0x180)`.
  Check the PSY-Q signature in `notes/reference/` before landing (the CdIntToPos lesson).

---

# TIER D — BLOCKED (do not assign until the blocker is cleared)

| function | insns | blocker |
|---|---|---|
| func_8003ACF4 | 8 | `addu $at,$v0,$at` reg-first on **D_8015C008** — needs `"D_8015C"` in `ASPSX_REGFIRST_SYM_PREFIXES` (`tools/maspx/maspsx/__init__.py:21`) |
| func_8003786C | 9 | same, D_8015C006 |
| func_80037C14 | 15 | same, D_8015C006 |
| func_80037738 | 17 | calls into the same D_8015C family |
| func_800175D8, 800175FC, 80017668, 80017684, 800176A4, 800177CC, 800177E0, 800177F8, 80017820, 8001782C, 80017860 | 3–14 | **hand-written asm** in 77D4.s (manipulate `$sp` directly / branch to their own `glabel`). Not C-reachable. Should probably be re-tagged in the function map so they stop showing up in the "easy" query. |
| any function whose asm mentions `jtbl_` | — | needs a rodata carve first (LESSONS func_8005ECE0) — **grep before assigning**, none of the Tier S/A drafts above contain one |

Once `"D_8015C"` lands, func_8003ACF4 is a Tier-S-grade one-liner:
```c
extern s32 D_8015C008[];
void func_8003ACF4(s32 i, s32 v) { D_8015C008[i * 3] = v; }
```
(`i*3` on an `s32[]` = the `sll 1 / addu / sll 2` ×12 index, and the symbol-indexed
`%lo(SYM)($at)` store is the array-indexing form from LESSONS.)

---

# Suggested build order

1. **func_80033EBC** (A10) — proves gp-vs-lui end to end in one function.
2. The six empty functions (S1–S6) — free, and they validate the carve pipeline.
3. The `Code8005C218.c` group (S7–S11) — 5 functions, one carve, includes the
   lui-vs-gp canary.
4. `Code8005EBA4.c` (S18–S21) and `Code8005D4C0.c` (S13, S15, S16, A19).
5. S22–S34 (the remaining single-body accessors).
6. **A24–A30, the jalr dispatcher family** — 17+ functions from one proven template;
   this is the biggest single win in the pool.
7. A31–A37, the tail-call wrappers (8 more from one template).
8. Everything else in Tier A, then Tier B.
9. Fix `ASPSX_REGFIRST_SYM_PREFIXES`, then Tier D's D_8015C group.

# Things I am explicitly NOT confident about

* **func_80045FE4 / func_80046008** (B2) — the surviving `addu $v1,$v0,$zero`. I have no
  source shape for it; LESSONS says a plain copy is always deleted.
* **func_80066750** (S29) — the `sw` at +4 immediately followed by `sb` at +4.
* **func_8005D9EC** (A22) — whether the `Player_work+0xB0` halfword read survives as a
  folded `%lo(Player_work+0xB0)` through a member cast.
* **func_8004327C** (B1) — which of three equivalent C spellings produced `sltu`+`negu`.
* **func_80045918 / func_8004594C** (S32/S33) — whether the `andi 0xFF` survives from the
  struct re-read.
* **func_80030604 / 8003648C / 80030934** (A41) — the *arity* of the callees; I inferred
  pass-through registers from "never written, still live at the jal", which is sound for
  the caller's bytes but should be sanity-checked against the callee's own asm.
* **All struct field names** — I used offset-named members (`o->x2E`) and `u8 *`
  arithmetic where no typed struct exists. Where a real struct is already in
  `include/rock_neo/`, use it; the byte output should be identical either way, but a
  wrong *width* (u8 vs u16) is not neutral.
