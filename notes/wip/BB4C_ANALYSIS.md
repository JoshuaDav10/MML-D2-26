# func_8001BB4C — analysis & matching plan (the 845-insn CD loader state machine)

## 2026-07-11 (Fable, worktree agent) — xm-diff VERDICT + real blocker re-diagnosed
VERDICT on notes/wip/bb4c_inflight_xm.diff: **REVERTED, both variants.**
  - full inflight diff (u32 xm + D_800C5608 extern replacing p2[-1]):
    817 words / **479 hard** (baseline: 809 expected / 806 / 461). The
    D_800C5608 absolute-name form for the countdown adds lui+sw pairs
    (p2[-1] base-relative is CORRECT per the original's base-off-$s2
    addressing) — that half is permanently wrong, discard.
  - partial variant (u32 xm for the & 0x100 masks ONLY, p2[-1] kept):
    808 words / **472 hard**. Numerically worse BUT the shift fix itself is
    PROVEN REQUIRED (below); the count rise is positional-cascade noise.
KEY FINDING 1 — srl/sra proof: the original emits **srl** for every
  `(x10 & 0x100) >> 4` texpage shift (ref .s lines 205/216/222/286/297/303)
  and the draft's `s32 x` emits **sra**. sra can NEVER byte-match srl, so the
  mask local MUST be unsigned in the final code. With u32 xm the scratch srl
  count == ref (9/9). Re-introduce the u32-mask form once word parity (809)
  is reached; only then is the hard count comparable again.
KEY FINDING 2 — the "$s5/$s6 rematerialized" blocker is STALE: cc1 -dg/-dl
  dumps of the 461 baseline show dispositions 77→16 76→17 73→18 72→19 74→20
  78→21 75→22 — all SEVEN pseudos hold $s0..$s6; prologue/epilogue match the
  original exactly (frame 0x38, 7 saves). Global regalloc is NOT the problem.
KEY FINDING 3 — the REAL remaining blocker: a 3-word deficit (806 vs 809)
  plus a catalog of true per-site codegen-shape diffs found by resynced
  (difflib/opcode-normalized) alignment, NOT a regalloc cascade:
   - states 8/9 gate reads: ref does **lbu** where draft's `w->x14 != 0` /
     `w->x18 != 0` emit lw (4 sites, ref norm-lines ~687/704/729/746).
     Byte-read the gates ((u8) member alias or cast).
   - branch-sense inversions: ref bne vs scratch beq (~norm 589), ref bnez
     vs scratch beqz (~510, ~600) in states 6/8 — leg-order/shape knobs.
   - store-order swaps in the type7/type8 setup (sw/sb pairs emitted in
     opposite order, norm 412-417/441-445) — LESSONS store-order rules.
   - sound path raw consts: ref materializes 0x8014B000/0x8013B000 as
     lui/ori pairs positioned differently than scratch (norm 337-341,
     403-408, 432-437) — see LESSONS "Raw-constant pointers use lui/ori".
   - case-6 sign-fixup shape differs (norm 623-635): ref = nop/sll/bgez/
     addu/addiu then sra later; scratch = addiu/bgez/nop/sra earlier.
     Re-derive from ref C530-C540 (looks like (x+3)>>2 conditional form).
   - outer state-3 `D_8009896C |= 2` region: scratch emits an extra
     lui/sw/j (norm 87-91) — one redundant store + jump; check the case-3
     early-return shape.
  Method note: normalize both streams (regs→R, imm→#, %hi/%lo→#, branch
  targets→L, li≡addiu) and difflib-align; the positional bytecmp count is
  DOMINATED by the 3-word shift — fix word count first, then the hard count
  becomes meaningful again.

Status: **STRUCTURE MATCHED, register allocation NOT converged; not yet
matched.** Signature: `void func_8001BB4C(void)`. Frame 0x38, saves $ra +
$s0..$s6 (7 callee-saved). Full compilable draft: `notes/wip/bb4c_draft_v2.c`
(419 lines, self-contained scratch TU). Verify with:
`source .venv/bin/activate && CPP=cpp tools/bytecmp.sh func_8001BB4C notes/wip/bb4c_draft_v2.c`
Current: **7/7 saved regs NOW CORRECT; 461 hard mismatches (was 510).** Tree
byte-identical (cd.c still INCLUDE_ASM). Do NOT land until 0 hard mismatches +
clean-rebuild OK.

### 2026-07-06 (Opus, MAIN thread) — advanced the near-match; START HERE
The prior agent's "cbase/1 not held" diagnosis was STALE. Verified via cc1 -dg
dump of bb4c_draft_v2.c: cbase(&D_800B5DB0)→$s5 and 1→$s6 ARE held correctly.
The REAL blocker was an EXTRA 8th saved reg: cc1 hoisted `&Game_work+0x1B8`
into $s7 (reg 245 in the greg) because the `((s16*)&Game_work)[...]` macro form
makes a hoistable base pseudo. FIX APPLIED (already in bb4c_draft_v2.c): declare
Game_work as a struct with the per-type slot array at 0x1B8 and access by member
(`Game_work.s[idx].v/.b/.w`) — this forces per-site absolute %hi/%lo, no hoist.
Result: $s7 gone, hard regs now exactly 16-22 ($s0-$s6, 7 regs), 510→461.
  ⚠ TREE-SAFE NOTE: the scratch draft defines `struct GAMEWORK` locally. For the
  final cd.c, do NOT edit the shared game.h — reproduce the same member-access
  codegen via a local typedef + `(*(struct GAMEWORK*)&Game_work).s[idx].v` and
  confirm it still emits per-site absolute (bytecmp) before landing.
REMAINING (the 461): now mostly (a) preamble SETUP ORDER — original emits
  s6=1, s5=&D_800B5DB0, s3=&D_800C5604, s2=s3+8, s4=s3+0x1C (s5 BEFORE s3);
  the draft hoists cbase LAST (s6,s3,s2,s4,s5). Introduce an explicit
  `CMD_ENT *cbase = D_800B5DB0;` set in the right source position so cc1 orders
  it between s6 and s3; and (b) branch-displacement + register-number cascades
  that should collapse once the preamble order + any remaining insn-count diff
  is fixed. Re-run `CPP=cpp tools/bytecmp.sh func_8001BB4C notes/wip/bb4c_draft_v2.c`
  and diff from index ~95 (loop preamble .L8001BCE4) onward.

## 2026-07-06 (Opus, worktree agent) — SESSION SUMMARY: what's SOLVED vs LEFT
The whole control-flow + field-layout structure is CORRECT and verified
block-by-block. Spot-checked state 1 (DMA copy, C2AC): my output matches the
original instruction-for-instruction, MODULO REGISTER NUMBERS. So the remaining
~510 "hard mismatches" are almost ENTIRELY register-number diffs (right opcode,
right offset, wrong $reg) cascading from register allocation.

REGISTER ROLES (original): $s0=e (cmd entry), $s1=&D_8009BE48 (RECT, set LAZILY
in type1/type4-else/state2/state4), $s2=p2 (&work+8), $s3=w (&D_800C5604),
$s4=q4 (&work+0x1C), $s5=&D_800B5DB0 (cmd array base), $s6=1 (the constant).

WHAT'S SOLVED (in bb4c_draft_v2.c — reuse these):
 - Three switches as real switches (outer D_800989C4 fallthrough 0-4; inner
   while(1) switch D_800989C8 0-9; innermost switch type 0-10). jtbl carve NOT
   yet done (tables still in 800.rodata.s; carve after code matches).
 - `type==-1` -> `D_800989C4 = 4; return;` (shares C7AC epilogue).
 - CMD_ENT: all s32 x0..x28 + data at 0x100, stride 0x800; e = &D_800B5DB0[idx].
 - WORK block CD_WORK @ D_800C5604 + THREE pointer views held across the loop:
   `w=&D_800C5604` ($s3), `p2=&w->x8` ($s2), `q4=&w->x1C` ($s4). Set up right
   before while(1). Setup/type0-1-3-5/state4 use p2 (base+8; p2[-1]=x4,
   p2[10]=x30, p2[11]=x34); state6 uses q4 NEGATIVE (q4[-6]=x4,q4[-5]=x8,
   q4[-3]=x10,q4[-2]=x14,q4[-1]=x18,q4[0]=gate); state1/8/9 use w by name.
 - ADDRESSING CRUX resolved: state2 AND type7/type8 access the work block by
   ABSOLUTE NAME (extern D_800C5614/5618/561C/5620/5634/5638), NOT the pointers.
   Declare union offsets by BYTE nature (5618/561C s8, 5620 u8) + cast (s32*) at
   word sites -> reproduces both sb and lw/sw. This ALSO flipped p2/w priority so
   w->$s3, p2->$s2 (moving type7/8 off `w->` shed refs from w).
 - Game_work WITHOUT hoisting +0x1B8 base: symbol-indexed idiom
   `((s16*)&Game_work)[0xDC + idx*4]` (see GW_VAB/GW_1BA/GW_1BC/GW_1DA macros).
 - D_80098AD0/AD1/AD4 = one 8-byte AD_ENT{u8 x0;u8 x1;u8 pad[2];s32 x4;}[type].

THE REMAINING BLOCKER (next session START HERE): cbase ($s5) and the const 1
   ($s6) are NOT held in callee-saved regs — cc1 rematerializes `&D_800B5DB0`
   (lui/addiu) and folds `one=1` to `li` at each site, so the real pipeline uses
   only $s0..$s4 (5 saved) vs the original's 7. Tried: cbase as var AND direct
   `&D_800B5DB0[idx]` array indexing; `s32 one=1;` — none held. NEXT MOVES:
   (a) -dl/-dg the draft, read .lreg for the base/const pseudos.
   (b) $s6=1 is used as sllv operand (MIPS sllv needs a REG, cannot fold to imm),
       sb value, and beq/bne compare — route the `1<<n` shift through a NAMED var
       used FIRST in a sllv so the pseudo survives cprop and pins to $s6.
   (c) For $s5: find the construct that keeps &D_800B5DB0 live (single advanced
       pointer / aliased so cprop can't fold). The disposition already puts
       e/$s0, r/$s1, w/$s3, p2/$s2, q4/$s4 right, so fixing $s5/$s6 should
       cascade-fix most register diffs.
   Then: jtbl carve (jtbl_80010244/25C/284 contiguous 0x244..0x2AC in
   800.rodata.s -> cd.c.o(.rodata), mirror scene recipe), clean rebuild +
   mutation test, land in src/rock_neo/cd.c @ line 41.

## (historical) prior status — full structure map, field widths, union below
Was: **draft + full structure mapped + field widths VERIFIED.** m2c draft in
`notes/wip/bb4c_m2c_draft.c`; combined asm+tables in `notes/wip/bb4c_combined.s`.
2026-07-06: all load/store widths cross-checked (see "VERIFIED FIELD WIDTHS").
Union arms fully enumerated per inner state (see "UNION ARM ENUMERATION").

## Why it's a multi-session job
Three nested jump-table switches, two complex work structs to type exactly,
PSX VRAM/texture-page addressing math, LoadImage/TIM + SsVab* sound. Every
lb/lh/lw signedness is byte-load-bearing. Build in the scratch TU with
`tools/bytecmp.sh func_8001BB4C <draft.c> notes/wip/bb4c_combined.s` and only
land in cd.c at 0 hard mismatches.

## Control flow (three switches)
1. **Outer: `switch (D_800989C4)` — jtbl_80010244, states 0..4, FALL-THROUGH.**
   Classic CD state machine; each state does work and falls into the next.
   - state 0 (.L8001BB9C): if (D_80098964==0) func_8001CB7C(); D_800989C4++;
   - state 1 (.L8001BBD0): if (D_80098964==0) return; func_8001D324(0xA0);
     D_800989C4++;
   - state 2 (.L8001BC04): if (D_80098828!=0){D_80098828--; return;}
     func_8001CF98(D_80098A84->x4); D_800988D0=0; D_800987A8=0;
     D_80098828=0; D_800989C4++;
   - state 3 (.L8001BC6C): D_80098828++; if(==0x96){D_8009896C|=2; return;}
     if (D_80098B38[D_800987A8] != 1) return;  else enter the INNER loop.
   - state 4 (.L8001C7BC): func_8001CAAC();  (default >=5: return)
   Default target for the outer sltiu-0x5 miss is .L8001C7C4 (the shared
   epilogue / `return`). NOTE the fall-through braces in the m2c draft: state
   1's body opens the `if (D_80098964!=0){...}` that ENCLOSES states 2/3 as
   cascading cases — that is m2c's rendering of the fall-through, will need
   restructuring into clean C (likely a real fall-through switch).

2. **Inner: `while(1) switch (D_800989C8)` — jtbl_8001025C, states 0..9.**
   Entered from outer state 3. loop label .L8001BCE4. Setup before loop:
   s6=1; s5=&D_800B5DB0; s3=&D_800C5604; s2=s3+8 (&D_800C560C);
   s4=s3+0x1C (&D_800C5620). These are HELD across the whole inner loop
   (callee-saved) — the source almost certainly uses three struct pointers
   into the D_800C5604 work area (base, base+8, base+0x1C) plus the
   D_800B5DB0 command-buffer base.
   - state 0 (.L8001BD14): load cmd entry, dispatch switch 3 (below).
   - states 1,2,4,6,8,9: DMA/copy sub-states (0x800-byte chunked transfers
     via func_800176DC, or word copy loops, or LoadImage tiling).
   - default (.L8001C740 region, block_87): D_80098B38[D_800987A8]=0;
     D_800987A8=(D_800987A8+1)%0xA; if (D_80098B38[D_800987A8]!=1) return;
     else continue the loop. After the loop path: D_800989C4=4; return.

3. **Innermost: `switch (cmd->type)` — jtbl_80010284, types 0..0xA.**
   Inside inner state 0. cmd = &D_800B5DB0[D_800987A8] (stride 0x800, i.e.
   D_800987A8<<11). cmd->x0 = type (stored to D_80098A54; ==-1 → .L8001C7AC
   skip). Copies cmd->x4/x8 into work[0]/work[4]. Types:
   - 0: work[8]=cmd->xC; D_800C5608--; D_800989C8=type+1.
   - 1: big TIM-load branch (width cmd->xC <0x140 → func_800176A4 VRAM
     upload + D_80098868 texpage bit; else LoadImage). Sets many work fields.
   - 3: partial field copy + D_8009BE4C/4E=0x20.
   - 4/9/10: TIM load (width <0x100 → func_800176A4 else LoadImage; special
     D_80098A54==0xA half-height case). D_800989C8=0.
   - 5: sound/VAB path — cmd->xC*0x1820+0x8014B000; Game_work.x1DA compare;
     D_80098958&0x800 → func_8001B33C(0x8014B000); D_800C5620==1 →
     Game_work[0x1BA+type*8] writes + SsVabClose.
   - 7: D_80098AD4[type*8]=cmd->x14+0x8013B000; D_800C5614=..; compare
     D_80098AD0[type*8].
   - 8: like 7 with cmd->x18 and D_80098AD1[type*8].

## VERIFIED FIELD WIDTHS (2026-07-06 Opus — cross-checked against every
## load/store in func_8001BB4C.s; supersedes the "VERIFY signedness" draft below)

Base regs held across the whole inner loop (all callee-saved):
  $s5 = &D_800B5DB0 (command buffer base)
  $s3 = &D_800C5604 (work base)   $s2 = $s3+8 (=0x560C)   $s4 = $s3+0x1C (=0x5620)
  $s0 = $a2 = &D_800B5DB0[slot] (current command entry; slot = D_800987A8<<11)
  $s6 = 1 (a held constant, reused as the "1" immediate everywhere)
  $s1 = &D_8009BE48 (the RECT) — set up lazily inside the type-1/3 blocks

### Command entry @ D_800B5DB0, stride 0x800 — ALL s32 WORDS.
  Every access is lw/sw. Offsets touched: 0x00 (type; -1 sentinel),
  0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28; payload at 0x100
  (passed as `entry+0x100` to the loaders/DMA). No sub-word reads of the entry.
  => `typedef struct { s32 x0..x28; u8 data[...]; } CMD_ENT;` (0x800 stride).

### Work area @ D_800C5604 — COMMON HEADER + PER-COMMAND-TYPE UNION.
  KEY DISCOVERY: the region from +0x14 (D_800C5618) onward is NOT one fixed
  layout. The SAME address is written at different WIDTHS by different command
  types, so the source is almost certainly a common header followed by a
  `union` of per-type sub-structs. Evidence (asm line #s):
   - D_800C5618 (+0x14): `sb $zero`/`sb $s6` in the type-7 handler (C1C4/C1FC,
     a 0/1 flag) BUT `sw $v0` in the type-8 handler (C230, a 0x8013B000 ptr).
   - D_800C561C (+0x18): `sb $s6` in type-8 (C288, flag) BUT written as a WORD
     pointer in the type-5/sound path (temp_s2->unk10 store).
   - D_800C5620 (+0x1C = $s4+0): u8 everywhere (lbu C10C/C48C, sb 0x14($s2)).
  Confirmed-width fields:
    +0x00 (5604) u32   rem / copy-loop counter (lw/sw; unsigned compares >=0x800)
    +0x04 (5608) s32   countdown (lw/sw)
    +0x08 (560C) s32   ($s2+0)   dest ptr / field (lw/sw)
    +0x0C (5610) s32   ($s2+4)   (lw/sw)
    +0x10 (5614) s32   ($s2+8)   (lw/sw; type-7 value word at C1BC)
    +0x14 (5618) UNION s8 flag (type7) | s32 word (type8)   <-- union boundary
    +0x18 (561C) UNION s8 flag (type8) | s32 word (type5)
    +0x1C (5620) u8    ($s4+0)   the ==1 gate (lbu/sb)
    +0x20 (5624) u32   ($s4+4)   width param (>>5 tile count)
    +0x24 (5628) u32   ($s4+8)   height param (>>5)
    +0x30 (5634) s32   ($s2+0x28) tile-x counter (lw/sw)
    +0x34 (5638) s32   ($s2+0x2C) tile-y counter (lw/sw)
  Next-session task: enumerate every READ of +0x14/+0x18 in inner states
  6/8/9 to fix each union arm's member types, then declare
  `struct { <header>; union { struct type1{}; struct type7{}; ... } u; }`.

### RECT @ D_8009BE48 — all s16 (every access is sh/lh).
  0x00 s16 x, 0x02 s16 y, 0x04 s16 w, 0x06 s16 h. (D_8009BE4A/4C/4E are y/w/h.)

### Per-type 8-byte arrays (indexed by cmd->xC * 8, i.e. `type<<3`):
  D_80098AD0[] (lbu/sb — u8 per type), D_80098AD1[] (sb — u8),
  D_80098AD4[] (sw — s32/ptr per type). Game_work @ 0x1B8+type*8:
  0x1B8 s16 (lh/sh, a SsVab handle), 0x1BA s8 (sb), 0x1BC (sb). 0x1DA u8 (lbu).

## UNION ARM ENUMERATION (2026-07-06 Opus) — every read of the work area,
## by inner-loop state. Offsets are relative to base D_800C5604.
## Access via: $s3=base, $s2=base+8, $s4=base+0x1C. Width = the actual insn.

COMMON HEADER (same meaning across states):
  +0x00 (5604) u32   rem byte-count. states 1/8/9 unsigned-compare >=0x800.
  +0x04 (5608) s32   countdown. state1 dec (C334); state6 via -0x18($s4) (C49C).

UNION region — offset : {state, width, role} (⚠ = same offset, differing width):
  +0x08 (560C):
     state1  0x8($s3)  lw   dest ptr (copy loop C308-328)
     state4  0x0($s2)  lw   x base coord (C424)
     state6 -0x14($s4) lw   vab index (Game_work 0x1B8+idx*8; SsVab args, C53C..)
  +0x0C (5610):
     state4  0x4($s2)  lw   y base coord (C43C)
  +0x10 (5614):
     type7-setup sw    (C1BC) value word;  state8/9 0x10($s3) lw ptr (C614/C630)
     state4  0x8($s2)  lw   tile-count-x (C45C);  state6 -0xC($s4) lw SsVab param (C538)
  ⚠+0x14 (5618):
     type7-setup  sb   0/1 flag (C1C4/C1FC)
     type8-setup  sw   0x8013B000 ptr (C230)
     state4  0xC($s2)  lw   tile-count-y (C470)
     state6 -0x8($s4)  lw + slti 0x800  => SIGNED s32 remaining-bytes (C4B8/C4C0)
     state8/9 0x14($s3) lbu => u8 flag (C604/C648)
  ⚠+0x18 (561C):
     type8-setup  sb   flag (C288);  type5-setup sw ptr (temp_s2->unk10)
     state2  lw D_800C561C  word x-origin (C370)
     state6 -0x4($s4)  lw   src ptr (C4D8/C4E8)
  ⚠+0x1C (5620 = $s4+0):
     gate: lbu/sb u8 (C10C, C48C, 0x14($s2) sb C09C) — the ==1 test
     state2  lw D_800C5620  word y-origin (C390)   <-- read wide here
  +0x20 (5624) u32  state2 tile-count-x (>>5, C3B8)
  +0x24 (5628) u32  state2 tile-count-y (>>5, C3E0)
  +0x30 (5634) s32  state2/4 tile-x counter (lw/sw)
  +0x34 (5638) s32  state2/4 tile-y counter (lw/sw)

CONCLUSION: the work area is a per-command-type UNION over a 2-word common head
(rem@0x00, countdown@0x04). Each command type's switch-3 setup writes its arm and
the matching inner state reads it back at the same offsets/widths. The wide-vs-byte
reads at 5618/5620 mean the winning C is EITHER a `union { struct dma; struct tile;
struct sound; struct texload; }` with per-arm member types, OR one flat s32 struct
with `(u8)`/`(u8*)` casts at the byte sites (lbu 5618/5620) — try the flat+cast form
FIRST (fewer decls, m2c's temp_s2/temp_s4 offsets map straight onto it), fall back
to the formal union if a width won't reproduce. Signedness that IS load-bearing:
5618-as-remaining is SIGNED (slti, and the `if (<0) +=3` in state6 C51C-524).

## ⚠⚠ FIRST CODING TASK — resolve the width contradictions (2026-07-06 Opus)
Before any translation compiles meaningfully, decide the C shape for the three
offsets that are accessed at TWO widths (same address, byte AND word):
  0x14 (D_800C5618): sb flag (type7 setup C1C4/C1FC) | sw ptr (type8 C230) |
                     lw word (state4 tile-count-y C470) | lw SIGNED (state6
                     remaining, slti C4C0) | lbu u8 flag (state8/9 C604/C648)
  0x18 (D_800C561C): sb flag (type8 C288) | sw ptr (type5) | lw word x-origin
                     (state2 C370) | lw src ptr (state6 C4D8)
  0x1C (D_800C5620): lbu/sb u8 ==1 GATE (C10C/C09C/C48C) | lw WORD y-origin
                     (state2 C390)
A plain scalar/flat-struct field CANNOT emit both lbu and lw — so this region
is a genuine per-command-type UNION. Options, try in order:
  (A) one struct with the region typed s32 + read the byte sites via `(u8)x`
      / write via a `u8`-typed member alias — risk: cc1 may not reproduce sb.
  (B) formal `union { struct dma; struct tile; struct sound; struct tex; }`
      after the 2-word common head — most faithful, most decls.
  (C) separate globals where each is single-width, and the CROSS-width site
      uses an explicit cast `*(s32*)&D_800C5620` — matches the "globals +
      pointer view" addressing crux; likely the real source form.
Recommend starting with (C): declare the block as individual globals at their
natural single width (5620=u8 gate, 5618/561C per their dominant use), and at
the 2 or 3 cross-width read sites write the pointer-cast the asm implies. This
also lines up with the absolute-vs-base addressing crux below.

## JUMP TABLES (read 2026-07-06; carve as one contiguous block 0x244..0x2AC)
jtbl_80010244 (outer, D_800989C4, sltiu 0x5): 0=.L8001BB9C 1=.L8001BBD0
  2=.L8001BC04 3=.L8001BC6C 4=.L8001C7BC + one .word 0 pad.
jtbl_8001025C (inner, D_800989C8, sltiu 0xA): 0=BD14 1=C2AC 2=C350 3=C740
  4=C40C 5=C740 6=C48C 7=C740 8=C5F0 9=C698. (cases 3,5,7 → C740 = the
  block_87 default no-op; so switch(state){0,1,2,4,6,8,9 real; default: fall}.)
jtbl_80010284 (cmd type, sltiu 0xB): 0=BD80 1=BD98 2=C28C 3=BEF8 4=BF44
  5=C064 6=C28C 7=C194 8=C208 9=BF44 10=BF44. (cases 9,10 SHARE case 4's
  target BF44 → `case 4: case 9: case 10:`; cases 2,6 → C28C = default
  `D_800989C8 = D_80098A54 + 1` tail.)

## ⚠ ADDRESSING CRUX (the thing the match hinges on) — 2026-07-06 Opus
The work block is accessed TWO ways in the SAME function:
 - DMA/copy/sound states (1,4,6,8,9) use base+offset off the callee-saved
   pointers $s3=&D_800C5604, $s2=$s3+8, $s4=$s3+0x1C (set up ONCE in the inner-
   loop preamble .L8001BCCC..E0, held across all calls). These states walk the
   block pointer-style (copy loops increment $a2/$v1).
 - The TILING state 2 (.L8001C350) instead references D_800C5634/561C/5638/
   5620/5624/5628 by ABSOLUTE lui/%lo — even though $s3 is live. State 4
   (.L8001C40C) does the SAME tiling logic but via 0x28($s2)/0x0($s2)/etc.
 => The source almost certainly declares the block as SEPARATE contiguous
    globals (D_800C5604, _5608, _560C, ... _5638) AND, in the copy/tiling
    inner states, takes their address into a local pointer for pointer-walk
    access. cc1 then hoists &D_800C5604 / +8 / +0x1C into $s3/$s2/$s4 as loop
    invariants. Matching plan: declare the individual globals; in states that
    the asm addresses base-relative, write `p = &D_800C5604;` (or a small
    struct pointer) and index; in state 2 write the globals by name. Getting
    which-state-uses-which right is what reproduces the lui/%lo-vs-offset mix.
    This supersedes the "one flat struct" idea — it's globals + a pointer view.

## Struct layouts (derived from the asm; VERIFY signedness per field)
### Command entry @ D_800B5DB0, stride 0x800 (indexed by D_800987A8)
    0x00 s32 type          // -> D_80098A54; -1 sentinel = skip
    0x04 s32               // -> work.x0
    0x08 s32               // -> work.x4
    0x0C s32 w/size        // width; compared <0x140 (type1) / <0x100 (type4)
    0x10 s32 x/clut        // texpage math: (x + (x&0x100)>>4 - 0xF0) - (x&0x100)
    0x14 s32 h
    0x18 s32 w2
    0x1C s32
    0x20 s32
    0x24 s32
    0x28 s32
    0x100 u8 data[]        // image/DMA payload (cmd+0x100 passed to loaders)
### Work area @ D_800C5604 (s3 base; s2=+8; s4=+0x1C)
    0x00 (5604) s32 rem    // remaining byte count (copy-loop counter)
    0x04 (5608) s32        // (s4->x-18) a countdown
    0x08 (560C) s32        // s2+0 : dest ptr / field
    0x0C (5610) s32        // s2+4
    0x10 (5614) s32        // s2+8
    0x14 (5618) s32/s8     // s2+C ; also D_800C5618 s8
    0x18 (561C) s8         // s2+0x10 ; D_800C561C
    0x1C (5620) u8         // s4+0 ; D_800C5620 (==1 gate)
    0x20 (5624) u32        // s4+4 ; width param (>>5 tile count)
    0x24 (5628) u32        // s4+8 ; height param (>>5)
    0x28 (562C) ...        // s2+0x24? re-derive
    0x30 (5634) s32        // tile x counter
    0x34 (5638) s32        // tile y counter
  NB: m2c shows s2->x28 / s2->x2C (=5634/5638) and s4 negative offsets —
  reconcile all three pointers to ONE struct base when writing the C.
### LoadImage RECT/GsIMAGE @ D_8009BE48
    0x00 s16 x (D_8009BE48)   0x02 s16 y
    0x04 s16 w (=0x20 often)  0x06 s16 h (=0x20)
    D_8009BE4A/4C/4E are the y/w/h halves at +2/+4/+6.

## Externs / signatures confirmed
- func_8001CB7C/CF98(s32)/CAAC/D324 — all matched or signature-known this session.
- func_800176A4(void*, void*, s32, u32)  — VRAM upload (dst, src, len, w?)
- func_800176DC(void*, u32, s32)          — 0x800-chunk DMA
- func_80017684(void*, u32, s32)          — word transfer
- func_8001B33C(s32)                       — VAB helper
- LoadImage(RECT*, void*)                  — PSYQ
- SsVabOpenHeadSticky(s32, s16, s32), SsVabClose(s16), SsVabTransBodyPartly(
  void*, s32, s16), SsVabTransCompleted(s32) — PSYQ SPU (check exact protos
  in notes/reference PSYQ docs before finalizing).
- Game_work.x1B8/x1BA/x1BC/x1DA (per-type 8-byte slots at 0x1B8 + type*8).

## Matching plan (next session)
1. Define the two work structs in cd.h (CMD_ENT @ D_800B5DB0, and the
   D_800C5604 work struct). Start from the layout above; fix signedness by
   watching lb/lbu/lh/lhu/lw in the asm.
2. Convert bb4c_m2c_draft.c into compilable C using those structs + the three
   pointers (s2/s3/s4 as one base + members). Keep the three switches as
   real switches (jump-table infra already proven; these tables carve like
   scene's — jtbl_80010244/25C/284 are contiguous in 800.rodata.s at
   0x244/0x25C/0x284, so ONE carve of cd.c.o(.rodata) covers all three).
3. bytecmp-iterate. Expect register-allocation fights on s0..s6 (7 saved
   regs) — use the -dg dump technique (see the F9AC/MojiTaskExec lessons).
4. Rodata carve: split 800.rodata.s around 0x244..end-of-284, place
   cd.c.o(.rodata) there (mirror the scene 800/1140 recipe; update splat
   yamls + rock_neo.ld). All three BB4C tables are one contiguous block.
5. Verify: clean rebuild + hash + cmp + mutation test (a case value AND a
   table remap, like the scene multi-table proof).
