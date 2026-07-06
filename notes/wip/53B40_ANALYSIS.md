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
