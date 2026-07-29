# wave_b3_family — full thin-wrapper family census + solves

**Generated:** 2026-07-29. Repo read-only; all work in /tmp. `BYTECMP_OBJ=/tmp/bc_b3f_*.o`.

---

## HEADLINE

**The engine 8-insn family is exhausted.** All 10 engine members of the strict
8-insn shape are now landed in `src/rock_neo/` — including the 3 from wave_b3,
which another agent's landing script already wrote out (they are untracked `??`
files, byte-identical to my wave_b3 blocks). I did not re-solve those.

Remaining in scope after this pass:

| bucket | count | status |
|---|---|---|
| engine wrappers already MATCHED | 21 | done |
| **engine solved THIS pass** | **2** | verified below |
| engine parked (one unexplained artifact) | 2 | flagged below |
| psxsdk wrappers | 28 | **BLOCKED — segment is raw asm, see WARNING** |
| overlay wrappers (out of brief scope) | 309 unique | the real remaining leverage |

---

## 1. TOTAL FAMILY SIZE (this is the number you asked for)

Census across all of `asm/` (3,920 `.s` files), thin wrapper = 5–12 insns,
stack frame + `sw $ra`, exactly one `jal`, no branches, `77D4.s` excluded:

```
 8 insns : 366 occurrences   <-- the family
 9 insns :  33
10 insns :  24
11 insns :   3
12 insns :   4
```

Of the 366, **334 are the single pure pass-through shape**. That is the real
source of the "family=324" figure in `notes/wip/lane_b_drafts.md` — it is a
**whole-game** number, not an engine number. Split by location:

* `asm/rock_neo/` (engine): **53 unique names** (5–12 insns), 22 of them the strict 8-insn shape
* overlays: **309 unique names**

So the engine slice of the family was always small; the 300+ leverage is in the
stage overlays, which your brief scoped out. If you want mass production, that
is where to point it — the templates below transfer unchanged.

---

## 2. VARIANTS

Six distinct shapes appear in the engine. Templates for each:

**V1 — pure pass-through** (334 occurrences corpus-wide; the dominant one)
```
addiu $sp,-0x18 ; sw $ra,0x10($sp) ; jal T ; nop
lw $ra,0x10($sp) ; addiu $sp,0x18 ; jr $ra ; nop
```
→ `void f(void *o) { T(o); }`

**V2 — store-then-call** (`sb $zero,0x0($a0)` in the delay slot)
→ `void f(u8 *o) { o[0] = 0; T(o); }` — store FIRST in C.

**V3 — constant argument** (`addiu $a0,$zero,K` or `addu $a0,$zero,$zero` in delay slot)
→ `void f(void) { T(K); }`

**V4 — call then return 1** (`addiu $v0,$zero,0x1` after `lw $ra`)
→ `s32 f(void *p) { T(p); return 1; }`

**V5 — call then boolean-negate** (`sltiu $v0,$v0,0x1` after `lw $ra`)
→ `s32 f(a, b) { return T(a, b) == 0; }`

**V6 — call then `addu $v1,$v0,$zero`** — **NOT SOLVED**, see §4.

The V4/V5 tell: when the post-call instruction is real work, it fills the
load-delay slot after `lw $ra` and `addiu $sp` slides into the `jr` delay slot.
When there is no post-call work, `addiu $sp` takes the load-delay slot and the
`jr` delay slot is `nop`. That schedule flip is a reliable variant fingerprint.

---

## 3. SOLVED AND VERIFIED THIS PASS

### func_8005D9C4 — `asm/rock_neo/4E18C.s`, 10 insns
Bytecmp: **EXIT 0** — 0 hard, 3 reloc (lui/%lo pair + jal, all expected).
`lui/%lo` on `D_800BBD87` ⇒ unsized-array extern per the LESSONS rule.

```c
extern u8 D_800BBD87[];
void func_8005D528(void);

void func_8005D9C4(void) {
    D_800BBD87[0] = 0;
    func_8005D528();
}
```

### func_8002F9EC — `asm/rock_neo/201EC.s`, 9 insns
Bytecmp: **EXIT 0** — 0 hard, 1 reloc (jal).
`lui $a1,(0x80190000>>16)` is a bare constant in the splat output, not a symbol,
so it is a plain integer literal — no extern needed.

```c
void func_8002FA38(void *, void *, void *);

void func_8002F9EC(void *a, void *b) {
    func_8002FA38(a, (void *)0x80190000, b);
}
```

**Carving note:** `0x201EC` is *exactly* the start of the `- [0x201EC, asm]`
segment, so this is a clean one-line carve to `- [0x201EC, c, Code8002F9EC]`.
`func_8005D9C4` is at rom `0x4E1C4` but its asm segment starts at `0x4E18C`, so
that one needs the segment split, not just a retype.

---

## 4. PARKED — func_80045FE4 and func_80046008 (V6)

Both in `asm/rock_neo/361C0.s`, 9 insns, **byte-identical bodies** (same `jal
func_8002FE74`), so one solution lands both. They are the **only two copies of
this shape in the entire 3,920-file corpus** — there is no solved precedent
anywhere to copy.

```
addiu $sp,-0x18 ; sw $ra,0x10($sp) ; lbu $a0,0x1($a0)
jal func_8002FE74 ; nop
lw $ra,0x10($sp) ; addu $v1,$v0,$zero ; jr $ra ; addiu $sp,0x18
```

The blocker is `addu $v1, $v0, $zero` — a copy of the return value into `$v1`
that is then never read. m2c calls it dead and emits a plain `void` body.

I tested 12 candidate sources. Best result, **1 hard mismatch of 9 words**, with
the correct instruction *schedule* (the extra instruction lands in the right
slot and pushes `addiu $sp` into the `jr` delay slot):

```c
u8 func_8002FE74(s32);
s32 func_80045FE4(u8 *o) { return func_8002FE74(o[1]); }
```
This emits `andi $v0,$v0,0xFF` where the target has `addu $v1,$v0,$zero`. So the
frame is right and only the one slot instruction is wrong — whatever the real
source is, it performs *some* one-instruction operation on the call result.

Ruled out (all worse or equal): `void` return, K&R callee proto, named temp,
`s64`/`u64` return in four combinations, struct-by-value return, `(u8)` cast on
the return. An explicit `register s32 v asm("$v1")` is rejected by cc1-27.

This smells like the parked "redundant `andi`" combine artifact already recorded
in `notes/LESSONS.md` (a reaching def that combine cannot trace). **Recommend
leaving both stubbed** rather than shipping a guess.

---

## 5. ⚠️ WARNING — the 28 psxsdk wrappers are NOT landable as-is

28 of the 32 remaining engine wrappers live in `asm/rock_neo/psxsdk/code.s`:

```
CdGetSector  CdMix  CdSync  SpuSetNoiseVoice  SpuSetReverbVoice  SsSepClose
SsSeqClose  SsStart  SsUtReverbOff  SsVabOpenHeadSticky  SsVabTransCompleted
_SpuDataCallback  _SsSndPlay  func_8006EC8C  func_80070114  func_8007013C
func_800721A4  func_800721C4  func_80075F00  func_80075F60  func_80077808
func_80077830  func_80077858  func_8007792C  func_800779EC  func_80077E24
func_80077E44  func_80077E68
```

**This is exactly the phantom-match trap CLAUDE.md warns about.** In
`config/splat.us.rock_neo.yaml:228` the whole region is ONE named asm segment:

```
- [0x587F8, asm, psxsdk/code]      # runs to 0x70EC4
```

`rock_neo.ld` links that as a single object. There is no `src/rock_neo/psxsdk/`
directory and **no psxsdk function has ever been matched to C** in this repo. A
`.c` dropped here would compile, census would count it, and the linker would
still ship the original assembly — full hash stays green while the C is dead.

Also worth noting these are PSYQ *library* objects, not game code; they were
built with different flags than `cc1-27 -O2`, so byte-matching them from C may
not be reproducible at all even after a segment split.

**I am not asserting these will link. They currently will not.** Landing them is
a config decision (split the segment) plus a toolchain question, and it should be
your call, not mine.

That said, I verified four of them against their bytes so the templates are ready
if you do decide to carve the segment. All four: **EXIT 0, 0 hard mismatches,
1 reloc (jal)**.

```c
void CD_vol(void *);

s32 CdMix(void *p) {
    CD_vol(p);
    return 1;
}
```

```c
s32 CD_getsector(void *, s32);

s32 CdGetSector(void *madr, s32 size) {
    return CD_getsector(madr, size) == 0;
}
```

```c
void SpuSetReverb(s32);

void func_800721C4(void) {
    SpuSetReverb(1);
}
```

```c
s32 CD_sync(s32, u8 *);

s32 CdSync(s32 mode, u8 *result) {
    return CD_sync(mode, result);
}
```

Caveat on these four: `CdSync`, `CdMix`, `CdGetSector` are already **declared**
in `src/rock_neo/cd.c` (lines 317, 349, and nearby). If you land the definitions,
do not introduce a second conflicting view — reuse cd.c's spellings
(`s32 CdSync(s32, u8 *)`, `void CdMix(u8*)`; note cd.c declares `CdMix` as
returning **void**, which conflicts with the `return 1` the bytes require).

---

## 6. SYMBOL RESOLUTION CHECK

Every callee/data symbol used by a block above is defined in the asm corpus:

| symbol | defined in |
|---|---|
| `func_8005D528` | `asm/rock_neo/4DCF8.s` |
| `func_8002FA38` | `asm/rock_neo/20238.s` |
| `func_8002FE74` | `asm/rock_neo/20238.s` |
| `D_800BBD87` | `asm/rock_neo/data/8B648.bss.s` |
| `CD_vol`, `CD_getsector`, `CD_sync`, `SpuSetReverb` | `asm/rock_neo/psxsdk/code.s` |

The four psxsdk callees resolve only *within* the psxsdk asm object — that part
is fine, but it does not rescue the segment problem in §5.

---

## 7. COUNT

* Verified by me this pass: **6** (2 engine landable + 4 psxsdk template proofs)
* Genuinely new engine matches available to land right now: **2**
  (`func_8005D9C4`, `func_8002F9EC`)
* Parked with diagnosis: **2** (`func_80045FE4`, `func_80046008`)
* Enumerated but blocked on your segment decision: **28**
* Engine family otherwise exhausted; **309 unique overlay wrappers** remain
  untouched and are where mass production actually pays.

No count claim here is an `audit_count.sh` delta — these are bytecmp verdicts on
scratch objects only, and nothing was built or committed.
