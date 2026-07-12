# BB4C_PROGRESS.md — func_8001BB4C resume doc (bg/bb4c-endgame)

## Resume command (run from a checkout with asm/ + build/ + tools submodules)
```
source .venv/bin/activate
CPP=cpp tools/bytecmp.sh func_8001BB4C notes/wip/bb4c_draft_v2.c 2>&1 | tail -25
```
Read the "summary:" line. Baseline this stream inherited: **319 hard mismatches, 811 words (expected 809)**.

## Current count
- **319 hard mismatches, 811 words** (expected 809). NO reduction achieved this
  session — see "Why plateau" below. The edits made are FIDELITY-NEUTRAL on the
  positional count (they fix real per-site shape diffs but the count is masked by
  the 2-word surplus that cascades every branch displacement + register number).

## WORKTREE SETUP GOTCHA (cost real time — do this FIRST on resume)
A fresh worktree off dev does NOT contain: `asm/` (splat-generated, gitignored),
`build/`, `assets/`, and the git SUBMODULES `tools/maspx`, `tools/splat`,
`tools/asm-differ`. bytecmp.sh needs all of them. Symlink from the main checkout:
```
MAIN=/home/X/Documents/MML-D2-26; WT=$(pwd)
for d in asm build assets; do ln -sf $MAIN/$d $WT/$d; done
for s in maspx splat asm-differ; do rm -rf tools/$s; ln -sf $MAIN/tools/$s tools/$s; done
```
(bin/ and disks/ ARE committed, so they're already present.)

## THE STRUCTURE OF THE PROBLEM (verified this session with -dg/-dl dumps)
- Global register allocation is **CORRECT and not the blocker**. The -dg dump
  (`bb4c.i.greg`, "Register dispositions") shows pseudos 72..78 → hard regs
  16..22 = $s0..$s6, all 7 callee-saved held exactly as the original. Prologue/
  epilogue match (frame 0x38, 7 saves). Do NOT chase a regalloc cascade — it's
  not there. The remaining 319 are (a) a 2-word instruction-count surplus that
  shifts every downstream branch displacement + register number, plus (b) a
  handful of LOCAL codegen-shape diffs (peephole/scheduling/jump-canon), NOT a
  global allocation fight.

## THE 2-WORD SURPLUS (this is the lever — fix these two and the count collapses)
Found by opcode-normalized difflib alignment (scratchpad/align.py). Running the
aligner and watching the cumulative net-delta, only TWO inserts never cancel:

1. **State 4 (case 4, .L8001C40C): `r->x` uses ABSOLUTE store, ref uses the
   held POINTER.** Ref (C40C..C454): `lui/addiu $s1,&D_8009BE48`;
   `addu $a0,$s1,$zero` (copy r into $a0 for the LoadImage 1st arg);
   `sh $v1,0x0($a0)` — stores r->x THROUGH $a0 (=$s1=r); then r->y via ABSOLUTE
   `sh %lo(D_8009BE4A)($at)`; then `jal LoadImage` with $a0=r.
   Draft emits `lui $at; sh $v1,0($at)` (absolute) for r->x → one EXTRA `lui`.
   The r pointer IS in a saved reg (greg confirms) but cc1 won't reuse it for
   the store. This is the "addressing crux" (absolute-vs-pointer for a global)
   from BB4C_ANALYSIS.md. State 2 (.L8001C350) has the SAME shape (ref C358
   `addu $a0,$s1,$zero` then pointer store) — fixing one likely fixes both.
   NOT YET CRACKED. Ideas to try next: (i) store r->x via an explicit local
   `RECT *r` where r is ALSO the LoadImage arg AND the store precedes the call
   with no intervening absolute ref to D_8009BE48 (currently `r=&D_8009BE48`
   const-folds); (ii) alias r->y through the pointer too and see if that flips
   BOTH to pointer form (then only D_8009BE4A-as-absolute is wrong, a smaller
   diff); (iii) -dl the state-4 RTL for the &D_8009BE48 pseudo and see why the
   store insn picks the constant address over the reg.

2. **State 1 tail (case 1): decrement-test-store + jump-canonicalization.**
   Ref (C334..C348): `lw $v0,D_800C5608`; `addiu $v0,$v0,-1`;
   `bnez $v0,.L8001C740(tail)`; `sw $v0,D_800C5608` IN THE DELAY SLOT;
   `j .L8001C738(tail0)`. I.e. test-BEFORE-store, store in branch delay,
   fall through to `j tail0`.
   Draft emits: `addiu -1`; `lui $at; sw $v0` (store BEFORE branch);
   `beqz $v0,tail0`; `j tail`. → store NOT in delay slot (needs a separate insn
   position) AND branch inverted (beqz→tail0 vs bnez→tail) AND an extra `j`.
   Tried `if ((D_800C5608 -= 1) != 0) goto tail; goto tail0;` — NO CHANGE; cc1
   still stores first and inverts. This is the PARKED jump-canonicalization
   genus (cc1 picks the branch sense + delay-slot fill from basic-block LAYOUT
   order; tail0 code is laid out closer). Same family as scene FC50/FCA4.
   NOT YET CRACKED. Ideas: reorder the case bodies so tail (C740) is the
   fall-through in cc1's block order, or find a source form where the store
   depends on nothing so it sinks into the delay slot and the branch keeps its
   sense. Revisit alongside the other jump-canon parked functions.

## Edits made this session (all in notes/wip/bb4c_draft_v2.c, count-NEUTRAL)
Kept because they make the draft match the reference's per-site SHAPE (they fix
real diffs the analysis cataloged; the count just can't reflect it until the
2-word surplus is gone):
- **case 4: `r->y` → `D_8009BE4A = ...`** (absolute for y, matching ref C448/C44C
  `sh %lo(D_8009BE4A)($at)`). Was `r->y = ...`.
- **case 2 branch sense**: first check rewritten to goto-out bne form
  `if (D_800C5634 != (D_800C5624>>5)) goto tail;` (ref C3CC `bne ...tail`);
  second check kept `if (== (D_800C5628>>5)) goto tail0;` (ref C3FC `beq ...tail0`).
  This RESOLVED the ~norm-589 branch inversion the analysis flagged.
- **case 4 branch sense**: both checks rewritten to goto-out bne form
  (ref C464 + C47C both `bne ...tail`, then `j tail0`). Also resolved an inversion.
- **case 1 tail**: folded to `if ((D_800C5608 -= 1) != 0) goto tail; goto tail0;`
  (no codegen change yet — blocked on #2 jump-canon).

## PROVEN-REQUIRED but DEFERRED (do AFTER word parity = 809)
- **Unsigned mask for the texpage `(x10 & 0x100) >> 4` shift** (BB4C_ANALYSIS
  KEY FINDING 1). The ref emits **srl** (unsigned) at 9 sites; the draft's `s32 x`
  emits **sra**. sra can NEVER byte-match srl, so the mask MUST be unsigned. I
  prepared this as `u32 xm` (declare `u32 xm;`, use `xm = e->x10 & 0x100;` and
  `(xm >> 4)` at the 2 case-1 and 2 case-4 sites) — but applying it while still
  at 811 words RAISED the count to 419 (positional cascade). REAPPLY IT ONLY
  once the 2 surplus words above are fixed and word count == 809; then it is
  net-positive. (This session reverted it to keep the baseline clean.)

## Method notes
- align.py (in scratchpad, recreate if gone): normalizes both instruction streams
  to mnemonic classes (move-equivalents unified), difflib-aligns, prints each
  divergent chunk with a per-chunk net-delta and a CUMULATIVE running delta.
  The chunks where cum-delta never returns to 0 are the true surplus. This is
  how the 2 surplus words above were isolated from ~40 net-zero scheduling swaps.
- The bytecmp positional count is DOMINATED by the 2-word surplus. Until it's
  809 words, the hard-mismatch number is a poor optimization signal — use the
  aligner's structural chunk list instead, and target the two non-cancelling
  inserts.

## Stop reason
Plateaued: 5 distinct genuine attempts (unsigned-mask, case-4 absolute r->y,
r-pointer/$s1 -dl forensics, states-2/4 branch-sense, state-1 decrement-fold),
no reduction below 319 because both remaining surplus words are in the parked
addressing-crux (absolute-vs-pointer) and jump-canonicalization genera. Handing
off with the two precise blockers above. Branch: bg/bb4c-endgame.
