# MOJI HARVEST PROGRESS (bg/moji-harvest)

Background agent harvesting small moji.c stubs into matches.
Branch: `bg/moji-harvest` (off dev HEAD 3fe98cf). NEVER push; foreground re-gates.

## Result summary (this session)
8 functions MATCHED + committed (clean-rebuild full-binary sha1 OK each).
2 attempted + reverted to INCLUDE_ASM (parked, register/loop-scheduling).
Stub count: 50 -> 42.

## Worktree setup notes (for resume)
Isolated worktree. Build inputs are UNTRACKED in git; copied from main repo
`/home/X/Documents/MML-D2-26`:
- submodules tools/maspx, tools/splat, tools/asm-differ (git submodule init
  fails on the pinned asm-differ commit; I copied the checked-out dirs from
  main instead — `cp -a`). NOTE: this left tools/asm-differ as a plain dir, so
  `git status` at repo root errors on the asm-differ submodule gitlink; scope
  git commands to paths (`git status src/ notes/`) or ignore that submodule.
- disks/us, assets/, asm/, rock_neo.ld (all copied from main).
Baseline `make CPP=cpp check_rock_neo_only` = OK before any edits.
Activate venv: `source /home/X/Documents/MML-D2-26/.venv/bin/activate`

## Verify recipe (every function)
```
touch src/rock_neo/*.c && rm -f build/rock_neo.elf && make CPP=cpp build_rock_neo_only 2>&1 | grep -iE 'error:|Error [0-9]'
make CPP=cpp check_rock_neo_only   # must print OK
```
GOTCHA: `./diff.py <func>` normalizes register renames AND commutes some
independent instruction reorders, so it can show a CLEAN function while the
full-binary hash still FAILS (a 4-byte or reg-swap size/order delta shifts the
downstream data pointer table at file offset 0x800). ALWAYS gate on
`check_rock_neo_only` printing OK, never on diff.py alone. To find the real
delta when diff.py looks clean: `diff build/rock_neo.bin.xxd build/rock_neo.bin.good.xxd | head`.

## MATCHED (committed to bg/moji-harvest)
- func_8005A598 — script2 range-select + x44 offset-table jump. Write the
  in-range test as the POSITIVE `if` body to get the arm order:
  `if (s2[1] <= lim && lim <= s2[2]) b=s2[3]; else b=s2[4];`. lim is a SIGNED
  s16 at raw offset: `*(s16*)((u8*)&Game_work + 0x60)` (game.h has x60 at the
  wrong struct offset + as u16 -> use raw cast for signed lh+slt). Table arm:
  `base=m->x44; m->xC2=b; m->x48=m->script2=base+((u16*)base)[b];`.
- func_80058C28 — script2 CALL push + Sce_flag_test select:
  `m->stack2[sp]=m->script2+3` push; then
  `if (Sce_flag_test((u16)func_80054410(m->script2+1))) script2=D_8008BAA4[(u16)func_80054410(...)]; else script2=D_8008CE58;`.
- func_80057744 — CALL-opcode w/ x44 offset-table + D_8008CACC fallback.
  Model of func_80055438 but with `if (base != 0) p = base+((u16*)base)[b];
  else p = D_8008CACC[b];` then `m->x48=p; m->script2=p; m->x3E=0; m->x3C=0;
  m->x72=0;`. Condition: `Game_work.x7C/x7D` via raw cast; op index 1 vs 2;
  `script2 += 3` on 0xFF.
- func_8005958C + func_80059660 (twins) — CALL push (stack2/xC0 vs stack/xBE)
  + branch on script[1]!=0 selecting Player_work.remap454[idx+D_8009899C] vs
  remap450[idx]; middle arm writes `script[-1]=0` and conditionally `=3` if
  `*(u32*)Moji_flag & 0x20000 && script[k]==(s8)m->x71`; final
  `script = D_8008CBA0[idx]` (remap entries read SIGNED: `(s8)Player_work.remapNNN[...]`).
  9660 is `void` (no return-1); 958C is `s32` returning 1. Added
  `extern u8 *D_8008CBA0[];`.
- func_80058DEC — D_800BE2F8 bit-scan compaction. KEY: hoist the bitmap base
  into a local pointer `u8 *t = D_800BE420;` and index `t[i>>3]` — otherwise
  cc1 emits the base load inside the loop (2 insns short -> whole binary
  shifts). Loop `i<0x40` sets D_800BE2F8[count++]=i for set bits, then fills
  D_800BE2F8[count..0x80) with 0xFF, then `m->script += 1;` (void).
  Added `extern s32 D_800989A8; extern u8 D_800BE420[];`.
- func_800563A8 — D_80098830 prelude + `if (m->flags & 0x2000)`-gated
  MojiTaskExec CALL-opcode (clone of func_80056558 inner block). LESSON: the
  D_800BE2F8 index MUST be written `D_8009899C + (s8)m->x71` (gp operand FIRST,
  x71 second) to match the target load order — the reverse order compiles the
  two byte-loads swapped, and diff.py hides it (commutes) while the hash fails.
  Prelude: `D_80098830 = D_800BE2F8[D_8009899C+(s8)m->x71] (+0x20 if D_80098850)`.
  D_800BE2F8 read UNSIGNED here (no (s8) cast, unlike func_80056558's null arm).
  Needs a forward `extern u8 D_80098830;` before the function (the existing one
  at ~L757 is after it). void, `m->script += 3`.
- func_800548C4 — flags state machine. `if(!(f&0x1000000)){flags|=0x1000000;
  func_8001D58C(func_80054424(m->script2+1));} else if(func_8001D878()){
  m->x4=5; *(u32*)Moji_flag|=0x100000; flags=(flags|0x40000)&0xFEFFFFFF;
  script2+=5;} flags|=0x10000000; return 0;`. Added
  `s32 func_8001D58C(s32); s32 func_8001D878(void);`.

## ATTEMPTED + REVERTED (parked)
- func_80057924 — BYTE-CLOSE. C body is correct (verified: struct members
  `Game_work.x7D/.x7C/.zennyCount` resolve right):
  ```
  if (m->script2[1] == 0) func_80043294(-((Game_work.x7D - Game_work.x7C)*10),0,0);
  else                    func_80043294(-Game_work.zennyCount,0,0);
  Game_work.x7C = Game_work.x7D; m->script2 += 2; return 1;
  ```
  diff.py shows CLEAN, but hash FAILS: the target frame is -0x20 saving s0 AND
  s1 (holds `&Game_work.x7D` in s0 and `m` in s1 ACROSS the func_80043294 call,
  then reloads x7D via s0 for the final store as `sb v0,-1(s0)`). My straight
  code uses a -0x18 frame (only s0), 7 insns shorter -> shifts everything.
  Needs allocno/register-persistence forensics (cc1 -dl) to force s0/s1 to hold
  &x7D and m live across the call. PARKED.
- func_80057C2C — BYTE-CLOSE (only 2 setup instr-pairs swapped). Bit-scan over
  a single byte D_800BE3DB (mask 0x80>>i, i=0..7) filling D_800BE2F8[cnt++]=i,
  then sentinel D_800BE2F8[cnt]=8, then the func_80057744-style CALL-opcode tail
  (op index = cnt, `script2 += 9` on 0xFF). Hoisting `u8 *bp = &D_800BE3DB;`
  fixed the inner bitmap load, but cc1 schedules the loop-invariant
  `p = D_800BE2F8` (a2) TWO slots too early vs target (target order:
  cnt, i, 0x80-const, THEN p). diff.py hides it (commutes the independent
  moves) but hash fails on the reordered `0C80063C F8E2C624` pair at 0x48438.
  Tried: separate `p=...` statement, `p[cnt++]` indexing — no change. Same
  const/loop-invariant-scheduling genus as the parked giants. PARKED.

## REMAINING INCLUDE_ASM STUBS (43) — smallest first
46 func_80053AA4, 48 func_80053788, 50 func_80056778(parked-giant genus),
53 func_80057924(PARKED above), 65 func_80057C2C(PARKED above),
73 func_80059D20(attempted; func_8005BF10 arg-birth scheduling, see note*),
76 func_8005851C, 77 func_800554E4, 84 func_8005861C,
84 func_80058AEC, 85 func_8005AB78, 85 func_8005B85C, 86 func_80055688,
87 func_80054450, 87 func_800577FC, 88 func_80056280, 91 func_800572C8,
92 func_80057408, 94 func_80057574, 108 func_8005B454, 115 func_80059728,
119 func_80057E50, 131 func_80059B44, 132 func_800592FC, 142 func_80055868,
146 func_8005BCE4, 154 func_8005A634, 167 func_8005B5D8, 174 func_80054BEC,
175 func_800598C8, 204 func_8005A874, 204 func_8005B98C, 209 func_800587F8,
257 func_80056D10, 266 func_80055CFC, 271 func_80058F18.
(SKIP per prompt: 80053B40, 80056820, 80057FF8, 80059E74, 8005ACA8,
 80054E54 — giants/owned by others.)

*func_80059D20 note: C body was logically correct (bit-scan idx via D_800BE2F8
/ D_8008AF00; magic-mult path `byte*v - Game_work[0x4C-idx*4]`; then CALL push +
`script2 = D_8008CE5C[D_800BE2F8[s[1]]]`). diff showed only register-allocation
+ the `func_8005BF10(0, arg, script2+7)` third-arg (a2) being computed
per-branch in target vs at-call in mine, and `a0=0` born in the beqz delay slot
vs at the end. Same arg-birth-order forensics as func_80057924. Left as a stub.
Next-easiest untried: func_80056778 is a known PARKED near-match (see HANDOFF
2026-07-06). The plain field-copy / small opcode handlers (func_800554E4,
func_800563A8, func_8005861C, func_80055688) are better next targets than the
call-heavy ones.
