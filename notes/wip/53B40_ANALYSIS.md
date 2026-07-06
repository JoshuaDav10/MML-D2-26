# func_80053B40 — analysis & field map (the 522-insn moji text/font renderer)

Status: **sized + field-mapped; not yet matched.** m2c draft in scratch.
Signature: `void func_80053B40(? *arg1)` (arg1 = a param passed to func_8001D878;
likely a draw/OT context pointer). Frame 0x50, saves $ra + **$s0..$s7 (EIGHT
callee-saved regs)**.

## What it does
Per-frame text/font renderer. Guards on `(Moji_flag & 0x400000)==0 && D_80098824==0`,
then loops every Moji_work slot (MOJI_TASK, stride 0xC4, 0x71..0x3D4 range = the
whole array). For active slots (flags<0, i.e. MOJI_TASK0_ON bit set) it builds GPU
primitives into the scratchpad primitive buffer at 0x1F800070, links them into the
OT via draw-tag manipulation (`tag = (tag & 0xFF000000) | (ptr & 0xFFFFFF)`), and
uses SetDrawArea/SetDrawMode/GetTPage + Sound_call. Dispatches per-char via two
function tables D_8008A91C[c] and D_8008AAC4[c] (char < 0x84 = glyph, else control
opcode) and a post table D_8008AAE4[x7C].

## ⚠ Why it's a second BB4C-scale giant
- 8 saved registers. FIVE hold hoisted loop-invariant CONSTANTS: $s5=0x40000,
  $s7=0x40000000, $s3=0xFFFFFF, $s4=0xFF000000, $s6=0x86186187 (the divide-by-0xC4
  magic / a *0x15 char-cell divide). Reproducing "cc1 hoists exactly these five
  constants into callee-saved regs across the loop" is the hard matching problem —
  they must be referenced enough inside the loop for the allocator to hoist them.
- Two induction pointers: m2c splits ONE MOJI_TASK* into var_s2 (base, flags@0x00)
  and var_s0 (base+0x71, negative offsets). Source almost certainly uses one
  MOJI_TASK* `m`; cc1 creates the +0x71 second base. Match will need to reproduce
  that (or coax it) — same genus as prior register-birth parks.

## MOJI_TASK FIELD MAP (verified widths; $s0 = &m->x71, offset = 0x71+disp)
  disp    field   width  role
  -0x6D   x04     u16    lh/sh/lhu  per-slot countdown/counter
  -0x6B   x06     s16    lh (2x)    slot's flag-bit shift amount (srav)
  -0x69   x08     u16    lhu (2x)   x origin base
  -0x67   x0A     u16    lhu (2x)   y origin base
  -0x61   x10     u16    lhu/sh     running x cursor
  -0x5F   x12     s16    lhu/sh     running y (uses D_80097F30[x3C*2])
  -0x5D   x14     u8*    lw/sw      script (glyph stream) ptr  [MOJI_TASK.script]
  -0x39   x38     u16    lhu        y advance addend
  -0x35   x3C     u8     lbu        row/palette index (D_80097F30 index)
  -0x34   x3D     s8     lb (6x)    draw-context index into D_80098934[]
  -0x33   x3E     u8     lbu/sb     copy of x3C used per-line
  -0x32   x3F     u8/s8  lbu (3x)   line height/step (>>1 in condensed mode)
  -0x29   x48     u8*    lw         (MOJI_TASK.x48 script entry ptr) reused here
  -0x05   x6C     u8*    lw/sw      script2 ptr — the ACTIVE cursor in this fn
   0x00   x71     s8/w   lb + lw/sw x71 low byte packs into D_80098B2C; word too
   0x02   x73     s8     sb         [MOJI_TASK.x73]
   0x07   x78     u8     lbu/sb     color/intensity byte (0x80)
   0x0B   x7C     u8     lbu        post-dispatch table index (D_8008AAE4)
   0x0D   x7E     u8     lbu        cell width *0xC
   0x0E   x7F     u8     lbu        cell height *0xC
   0x47   xB8     u16    sh/lhu     ⭐NEW — TPage/draw-mode word (D_800C0C26 src)
   0x49   xBA     u16    sh         ⭐NEW — second draw-mode word (D_800C0C2A src)
   0x4D   xBE     u16    sh         [MOJI_TASK.xBE] written here as u16
  (flags@0x00 via var_s2: RMW with masks 0x08000000/0x40000/0x10000000/0x04000000/
   0x40000000/0x100000/0x60000000; cleared with &0xAF7FFFFF at loop end.)

  ⭐ xB8/xBA are currently inside `u8 x80[0xBC-0x80]` in moji.h. To name them,
  split: `u8 x80[0xB8-0x80]; u16 xB8; u16 xBA; u16 xBC;` (layout-neutral —
  verify with a full moji rebuild before trusting). Sourced from D_800C0C26/2A.

## Other externs to type
  D_80098934 : pointer to a draw-context array; element has ->x70 (OT draw tag)
               and ->x7C (a second tag); indexed by m->x3D. (Shared w/ other fns.)
  D_8008A91C[c], D_8008AAC4[c], D_8008AAE4[i] : function-pointer tables (glyph
     handlers / control-opcode handlers / post handlers). Call as (*tbl[c])(m).
  D_80097F30[x3C*2] (u16), D_80097F50 (SetDrawArea rect base), Moji_flag (sdata,
     u32 RMW — the standard idiom), D_80098824, D_80098B2C (sdata), D_80098960.
  func_8001D494(0,1,0), func_8001D878(flags, arg1) -> s32.
  Primitive struct at 0x1F800070 (the scratchpad OT ptr): SetDrawMode/POLY-like
  fields tag/unk3..unk12 — use the PSYQ primitive structs (check reference corpus).

## Matching plan
1. Add xB8/xBA to moji.h (layout-neutral split of x80[]); full moji rebuild must
   still hash OK before proceeding.
2. Translate m2c draft using ONE `MOJI_TASK *m` (not two pointers); write the loop
   with the constants used inline and see if cc1 hoists them into $s3..$s7.
3. bytecmp-iterate; expect heavy register-allocation work (8 saved regs + 5 hoisted
   constants). Use the -dl/-dg allocno-priority dumps (LESSONS) rather than blind
   permutation. This is a dedicated-session effort like BB4C.

## Session (worktree agent, opus) — draft + 5-constant hoist analysis

Draft lives in `notes/wip/53b40_draft.c` (scratch TU, mirrors moji.h layout).
Runs via: `CPP=cpp tools/bytecmp.sh func_80053B40 <draft> <REF.s>` — BUT the
worktree is GITIGNORED-tool-poor: `asm/`, `tools/maspx/`, are only in the MAIN
checkout. Run bytecmp FROM the main checkout pointing at the worktree draft:
  `cd /home/X/Documents/MML-D2-26 && source .venv/bin/activate`
  `CPP=cpp tools/bytecmp.sh func_80053B40 .claude/worktrees/<wt>/notes/wip/53b40_draft.c`
(no REF needed from main tree — it auto-finds asm/rock_neo/nonmatchings/moji/).

### Field-map corrections (verified against $s0-relative disps, base 0x71)
- -0x5D (0x14) = **m->script** (NOT script2). LOOP 2 (render, .L80053EC0)
  iterates m->script (0x14); LOOP 1 (timing, .L80053CC4) iterates m->script2
  (0x6C, disp -0x5). Loop-2 exit compares script(0x14) != script2(0x6C).
- 0x02 disp = m->x73 (sb 0 in render setup). 0x4D disp = m->xBE (sh 0).
- The active-slot flag shift is `0x8000000 >> ((m-Moji_work)/0xC4)` — the
  `sra $a0,$t0,2` after the *0x1A1F58D1 mult IS the /0xC4 divide (index), NOT
  an extra <<2. (The D_80098B2C pack at .L800541D4 adds `sll $a0,3` = index*8.)
- Active test is `if ((s32)m->flags < 0)` → **bgez** (NOT `& 0x80000000`;
  that materialized a spurious 0x80000000 constant).
- pb[0xC]=(ch%0x15)*0xC, pb[0xD]=(ch/0x15)*0xC — ch = m->script[0] reloaded
  each time (two lbu of -0x5D). 0x86186187 is the /0x15 (21) magic → $s6.
- Loop over slots: `for (m=Moji_work; m < &Moji_work[5]; m++)` (5 slots).
  D_800BB9C8 = &Moji_work[4]. The x71→D_80098B2C pack is SKIPPED for slot 4
  (`if (m != &Moji_work[4])`).

### THE 5-CONSTANT HOIST — root cause identified (not yet solved)
Original hoists into callee-saved: $s3=0xFFFFFF, $s4=0xFF000000, $s5=0x40000,
$s6=0x86186187, $s7=0x40000000. It does **NOT** hoist 0x1F800070 (the
Map_prim_ptr scratchpad addr) — every one of its 4 sites is inline
`lui/ori 0x1F800070` (80053DF4/EEC, 800540BC, 80054240).

My draft's cc1 output hoists 0x40000, 0xFFFFFF, 0xFF000000, 0x86186187 CORRECTLY
but ALSO hoists **0x1F800070** (into a 6th callee-saved reg, $s8/$fp), which
STEALS the slot that should hold **0x40000000**. 0x40000000 then falls to two
inline `li $3,0x40000000` (blocks $L22/$L38). Net: frame 0x40 (wrong, want
0x50), $fp saved (original saves only s0-s7), and the whole tail mis-registers.

Diagnosis (via `cc1 ... -dl`, gccdump.lreg): 0x1F800070 forms a long-lived
invariant pseudo (crosses 8 calls, 4 uses); 0x40000000 has only 2 uses. gcc-2.7
loop.c hoists invariants roughly by use-count under register pressure; with 6
invariants for 5 slots, the 2-use 0x40000000 loses to the 4-use 0x1F800070.
In the ORIGINAL, 0x1F800070 is never a hoist candidate (each site is a fresh
independent `lui/ori`), so only the 5 real constants compete and all fit.

**OPEN PROBLEM**: make the raw-constant address `0x1F800070` stay inline
(un-hoisted) so exactly 5 invariants remain. Tried: `volatile`-qualified
pointer deref (`*(u32 * volatile *)0x1F800070`) — did NOT stop the LICM hoist
(it's the ADDRESS const being hoisted, not the load). Probes show that with few
competing invariants BOTH 0x1F800070 and 0x40000000 hoist; only the full
function's slot pressure drops one. Next avenues to try:
  - Reduce OTPTR to <2 uses so it's below the hoist threshold (hard: 4 real
    sites — render, glyph-loop, post-render, tail). The tail site is OUTSIDE
    the slot loop already; the 3 in-loop sites are the invariant.
  - Find a source form where the 3 in-loop map-prim-ptr accesses do NOT merge
    into ONE invariant pseudo (original keeps them as independent fresh
    lui/ori). Possibly: don't route through a shared macro; write each as a
    distinct-looking expression; or an `asm` barrier. Investigate whether the
    accesses being in nested/conditional blocks that don't all execute stops
    the merge.
  - Check whether the extern-symbol `Map_prim_ptr` (= 0x1F800070 absolute)
    assembles to lui/ori vs %hi/%lo — it produces the SHORTER `lw sym` (2-insn
    %hi/%lo) form, NOT the 3-insn lui/ori/lw the original has. So the symbol
    form does NOT byte-match; must stay raw-constant.

Mismatch trajectory so far: first bytecmp 449 hard (frame + full tail drift).
After `bgez` + `>>c` (not <<2) fixes + volatile ptr: constants now hoist
4-of-5 correctly but 0x1F800070 vs 0x40000000 slot theft persists → still
heavy tail drift. NOT yet re-counted post-fix; the 5-constant hoist must be
solved before instruction-level matching is meaningful.

### Update: logic verified; hoist is REGISTER-PRESSURE-driven (leading theory)
Fixed loop-1 tail sense: on the reset path `if ((s16)m->x4 == 0) { flags |=
0x40000000; goto loop1; }` (D8C: x4==0 loops back, x4!=0 exits to render-
decision DD0). x3F changed to u8 (lbu everywhere). The /0x15 div-magic + *0xC
SPRT-uv block now generates BYTE-EXACT (multu $s6/mfhi/subu/srl1/addu/srl4/
mul-by-0x15/subu/andi ff/*0xC) — confirms `pb[0xC]=(ch%0x15)*0xC`,
`pb[0xD]=(ch/0x15)*0xC` with ch=m->script[0] reloaded. So the BODY logic is
right; only the callee-saved constant allocation is wrong.

Proven with probes (/tmp/p6-p9): gcc-2.7 loop.c ALWAYS hoists a repeated large
constant ADDRESS (0x1F800070) as a loop invariant — volatile-slot, volatile-
ptr, const-local-ptr, and distinct-but-folding address expressions all still
hoist it (constant folding precedes loop.c; CSE merges to one movable). With
FEW competing invariants BOTH 0x1F800070 and 0x40000000 hoist; only under
enough register pressure does loop.c drop one, and it drops the lower-use one
(0x40000000, 2 uses) rather than 0x1F800070 (4 uses).

The ORIGINAL has NO free 6th callee-saved reg (epilogue restores only s0-s7),
so it too faced the choice and kept 0x40000000 (inline-drop 0x1F800070). The
delta is the PRESSURE PROFILE of the loop body: my draft's body evidently has
slightly LOWER register pressure at the hoist-decision point, so loop.c can
still afford to hoist 0x1F800070 into a 6th reg ($s8/$fp). The original's body
pressure is high enough that hoisting 0x1F800070 would spill, so loop.c leaves
it inline and 0x1F800070 never competes → 5 real constants fill s3-s7 exactly.

NEXT-SESSION PLAN (in priority order):
1. Get the FULL body byte-exact modulo the constant regs — the pressure profile
   is emergent from the exact set/liveness of temps. Likely the hoist self-
   corrects once loop-2 (the nested glyph loop) has the right live temps
   (m, prim, the tag/DRAWCTX temps, the SetDrawArea RECT). loop.c hoists out of
   the INNER loop first; a correctly-pressured inner loop is the lever.
2. If it doesn't self-correct, use `-dg` (gccdump.greg) to read the greg
   allocation order + conflicts of the 0x40000000 pseudo vs the 0x1F800070
   pseudo, and the MojiTaskExec allocno-priority formula
   (floor_log2(n_refs)*n_refs/live_length) to see the exact tie. Then perturb
   0x40000000's live_length (e.g. its two uses' spacing) to raise its priority
   above 0x1F800070, OR shorten 0x1F800070's effective life so loop.c declines.
3. Worst case: accept that 0x1F800070 must be un-hoisted and find the source
   idiom (an `__asm__ __volatile__("" ::: "memory")` barrier between OTPTR
   accesses was NOT yet tried — could break the single-movable merge).

STATUS: draft logic complete & largely byte-faithful; blocked solely on the
5-constant callee-saved allocation (0x1F800070 vs 0x40000000). NOT ready to
port. Do NOT move into moji.c until bytecmp hits 0 hard mismatches.

### CORRECTION: hoist is loop.c THRESHOLD-driven, NOT register-pressure driven
Falsified the pressure theory with probes: adding many live locals + spills
(frame 96) did NOT stop 0x1F800070 from hoisting, and a genuine NESTED inner
loop around one OTPTR site did NOT either (loop.c propagates a doubly-invariant
constant out to the OUTERMOST preheader). So nesting/pressure are red herrings.

The real mechanism (gcc-2.7 loop.c move_movables): it hoists movables ranked by
"savings" (~ use-count); as it consumes registers the acceptance THRESHOLD
rises. With MANY invariants, the 2-use 0x40000000 falls below threshold and is
NOT hoisted, while the 4-use 0x1F800070 stays above and IS. In a small probe
(few invariants, low threshold) 0x40000000 DOES hoist — confirming it's the
threshold, not pressure. Neither volatile, const-local-ptr, distinct-folding
addresses, an `__asm__ __volatile__("":::"memory")` barrier, nor nesting moved
0x1F800070 off the hoist list.

=> The ONLY way to reproduce the original is to make loop.c NOT rank
0x1F800070 as a hoistable movable AT ALL (so 0x40000000 rises above threshold),
i.e. the original C accesses Map_prim_ptr in a form whose ADDRESS constant is
not a single CSE'd loop-invariant movable. UNTRIED / next avenues:
  - Make the 3 in-loop Map_prim_ptr accesses genuinely NON-mergeable by CSE so
    no single movable forms: e.g. read the pointer ONCE into a local at loop
    top and thread it, writing back once — BUT the asm re-reads memory at each
    site, so that changes bytes. Need to confirm whether the original truly
    re-reads or threads a local.  <-- verify against asm memory round-trips.
  - Try `-dg` greg dump on the real draft: read whether 0x1F800070's pseudo and
    0x40000000's pseudo even coexist post-loop.c; if 0x40000000 never becomes a
    movable, the fix is purely "demote 0x1F800070 below it in loop.c ranking".
  - Investigate whether the original's Map_prim_ptr type/decl (game.c uses
    `(*(UNK_PRIM_1**)0x1F800070)`) combined with a DIFFERENT prim advance
    idiom changes the movable count.

This is a genuine BB4C-scale register grind; body logic is done, this is the
sole blocker. Draft compiles clean; NOT ready to port.
