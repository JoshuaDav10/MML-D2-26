# func_80053B40 — reconcile + push-to-0 progress (bg/53b40-reconcile)

Resume bytecmp command (run from MAIN checkout, venv active):
```
cd /home/X/Documents/MML-D2-26 && source .venv/bin/activate
CPP=cpp tools/bytecmp.sh func_80053B40 .claude/worktrees/agent-a6786f513ddba61bd/notes/wip/53b40_draft.c 2>&1 | tail -8
```
(This worktree is tool-poor — asm/ and tools/maspx live only in the main
checkout. bytecmp auto-finds asm/rock_neo/nonmatchings/moji/ from main; point
it at the WORKTREE draft path above.)

## STEP 1 — RECONCILE VERDICT: **CASCADE, not regression** (2026-07-12)

Both drafts in 53b40_draft.c history are salvage; there is no pre-salvage dev
draft (file was born at commit d9982f6). The two salvage points measured:

| draft | commit | Moji_flag idiom | hard mismatches |
|-------|--------|-----------------|-----------------|
| prior salvage | d9982f6 | `extern u32 Moji_flag[]; Moji_flag[0]` | **455** |
| current (HEAD) | 4d77f3e (eba6103) | `extern u8 Moji_flag[8]; *(u32*)Moji_flag` | **426** |

The eba6103 salvage is an IMPROVEMENT (-29), NOT a regression. The commit's
"movable set now 5; wrong by 0x1F800070<->0x40000000 swap ONLY" framing was
describing the remaining *conceptual* blocker, NOT claiming a low count. 426 is
the correct current baseline; keep the current draft.

### Why 426 despite "one swap": it CASCADES the whole prologue/tail
The 0x1F800070<->0x40000000 saved-reg swap forces a 6th callee-saved reg
($s8/$fp) to be saved+restored for the hoisted 0x1F800070 address. That:
- changes the frame from the required 0x50 to 0x40 in the draft, AND
- draft emits MORE words than the reference (500 vs 495 — the extra $fp
  save/restore pair + inline `li 0x40000000` at the two blocks that lost $s7).

Consequence: EVERY stack-relative store/load offset shifts, so all prologue
saves ([3]-[13]), all epilogue restores, and every spill offset in the 522-insn
body mismatch. That single structural fault is what inflates the count to 426.

Diagnostic proof (bytecmp indices, expected vs actual):
- `[0]`  expected `B0FFBD27` (addiu $sp,-0x50) vs actual `C0FFBD27` (-0x40).
- `[3]`  expected `4800BFAF` ($ra@0x48) vs actual `3800BFAF` ($ra@0x38).
- `[28]` expected `71005026` (addiu $s0,$s2,0x71 = the +0x71 2nd base ptr) vs
         actual `801F033C` (lui $3,0x1F80 - the hoisted OTPTR address).
- `[30]` expected `0040173C` (lui $s7,0x4000 -> 0x40000000) vs
         actual `801F143C` (lui $s4,0x1F80 -> 0x1F800070). <- THE SWAP, verbatim.

=> Fixing the single movable-ranking issue (keep 0x1F800070 inline so
0x40000000 takes $s7) should collapse the bulk of the 426 at once.

## Current best count: **426** (baseline, current committed draft) — plateau

## STEP 2 — push toward 0  (CORRECTED root cause + blocker)

### CORRECTION to the prior analysis: there is NO 6th saved reg / $fp
Verified via `-dg` (gccdump.greg) + reading the actual emitted prologue.
BOTH the draft and the reference save exactly **$s0-$s7 + $ra (8 s-regs)**.
There is no $fp/$s8. The earlier "hoisted into a 6th callee-saved reg" theory
was WRONG. The only real difference is a **1-for-1 SWAP** of which constant
occupies $s7 (and a downstream spill-slot count diff):

Draft greg dispositions (regno -> hard reg):
  106 (0x40000)     -> 22 ($s6)
  182 (0x1F800070)  -> 20 ($s4)   <- WRONG: original leaves this INLINE
  194 (0xFF000000)  -> 21 ($s5)
  204 (0xFFFFFF)    -> 19 ($s3)
  267 (0x86186187)  -> 23 ($s7)
  170 (0x40000000)  -> 3  ($v1)   <- inline `li` at 2 sites (WRONG: orig hoists)
Reference: 0x40000000 -> $s7 (set once @80053BB8, used or'd at D84 & DA4);
  0x1F800070 -> fresh `lui/ori` at ALL 4 sites (80053DF4/EEC, 800540BC, 80054240).

Frame: draft 0x40 ($ra@0x38, $s0@0x24) vs reference 0x50 ($ra@0x48, $s0@0x28).
The 0x10 delta is SPILL SLOTS, a downstream consequence of the swap (the two
inline `li 0x40000000` sites spill differently than the reference layout) — NOT
an extra saved reg. Fixing the swap should collapse frame + the whole cascade.

### Why the swap happens (loop.c move_movables, from -dL dump)
Outer loop (insns 61-1036). Movable ranking:
  0x40000000 (regno 170): savings 2  -> "not desirable" (below threshold)
  0x1F800070 (regno 182): savings 3  -> MOVED to preheader
Threshold sits exactly at 3. 0x1F800070's 3 in-loop uses CSE-merge into ONE
savings-3 movable that clears threshold; 0x40000000 (2 uses, savings 2) does
not. The other 4 real constants (0x40000/0xFF000000/0xFFFFFF/0x86186187) all
hoist correctly.

In the ORIGINAL, 0x40000000 (savings 2) WAS hoisted and 0x1F800070 was not.
The only self-consistent explanation: in the original **0x1F800070 is not a
loop movable at all** (its 3 sites did not merge into one invariant), so the
threshold stays at 2 and the 2-use 0x40000000 hoists. The reference asm
confirms 0x1F800070 is a fresh lui/ori at every site (never a saved reg).

### Attempts to break the CSE merge of the 3 OTPTR address computations
Goal: stop 0x1F800070 from forming a single savings-3 loop movable, so the
threshold drops to 2 and 0x40000000 hoists into $s7 (matching the original).

| # | idea | result |
|---|------|--------|
| A | plain inline-asm `lui/ori` per site (non-volatile) | CSE merged all 3 -> 1 lui, still hoisted |
| B | `__volatile__` inline-asm per site | loop.c hoisted the single volatile asm out; 1 lui |
| C | `__volatile__` asm + `"memory"` clobber per site | same, 1 lui hoisted |
| D | distinct pointer TYPES per site (A/B/C structs) | only avoided hoist when ADDRESSES also differed (0x70/74/78) — invalid, addr must stay 0x1F800070 |
| E | per-site `volatile` LOCAL pointer vars = (u32*)0x1F800070 | became stack loads (wrong bytes), 0x40000000 still inline |
| F | SAME address 0x1F800070 but different struct MODES (u32/u16/u8) per site | address still merged into ONE pseudo (single `li/ori` reused via `lw 0($reg)`), NOT 3 fresh lui/ori. Mode diff does not split the address movable. |

KEY MECHANISM FINDING (probe F): gcc-2.7 merges the constant ADDRESS into a
single pseudo regardless of access mode; the 3 sites share one `li/ori` +
per-site `lw 0($reg)`. To get the original's THREE independent `lui/ori`
sequences, the address pseudo must be prevented from existing across sites at
all — i.e. loop.c must decline to hoist AND cse must not keep a live pseudo.
No C source form achieves this for an identical constant address under -O2.

Prior-analysis attempts that also failed: `*(u32* volatile*)` deref (current
draft form), `__asm__ __volatile__("":::"memory")` barrier between sites,
genuine nested inner loop around a site.

Root difficulty: gcc-2.7 CSE is VALUE-based — three accesses to the *same*
constant address always hash-merge into one movable; any construct that makes
them syntactically distinct either (a) still folds to the same value pre-loop.c
and merges, or (b) changes the emitted access (stack load / different addr) and
breaks the bytes. No C form found that yields "3 independent fresh lui/ori to
the identical constant address" the way the original asm has.

### BLOCKER (plateau after 5 distinct attempts)
The 426 count is dominated by a SINGLE register-allocation swap
(0x1F800070 <-> 0x40000000 for $s7) that cascades stack offsets across the
522-insn body. Body LOGIC is byte-faithful (confirmed earlier: the /0x15
glyph-cell divide-magic + *0xC UV block generate exactly). The swap is caused
by cc1's loop.c hoisting the CSE-merged 3-use 0x1F800070 address as a
savings-3 movable; the original never forms that movable.

NEXT HYPOTHESES for a fresh session (untried):
1. `-dj`/RTL after cse but before loop: confirm at which pass the 3 addresses
   merge (cse1 vs cse2). If cse2 (post-loop), a form that keeps them split
   through loop.c might exist.
2. Try threading the pointer as `prim = OTPTR; ... OTPTR = prim + 3;` where
   later sites derive from a RELOADED `OTPTR` but through a helper that the
   scheduler still lowers to fresh lui — verify against the asm's per-site
   memory round-trip (asm DOES re-read 0x1F800070 then lw 0($reg) each site).
3. `-dg` allocno-priority arithmetic (LESSONS MojiTaskExec recipe): even if
   0x1F800070 stays a movable, perturb 0x40000000's live_length or add a
   3rd genuine use to raise its priority so BOTH hoist — but that needs a 6th
   saved reg the original lacks, so this only helps if it simultaneously
   forces 0x1F800070 to spill inline. Low confidence.
4. Investigate whether the ORIGINAL source routed OTPTR through the
   `PRIM_PTR`/`PRIM_PTR_INC` macros (rock_neo.h) AND whether a different prim
   struct type per site (POLY_FT4 vs SPRT vs the SetDrawArea env prim) makes
   the three accesses non-mergeable while all still assembling to lui/ori 70.
   (Attempt D used same addr diff types and DIDN'T merge — retest with the
   SAME address 0x1F800070 but genuinely different struct modes at the deref.)

### 2026-07-12 (Opus) — re-confirmed, no novel crack
Re-ran bytecmp on the main checkout: **426 baseline holds** (500 words vs 495).
Evaluated this session's new preheader-scheduling knob (from func_80057C2C):
it controls preheader instruction ORDER, not hoist SUPPRESSION, so it does NOT
apply to this movable-formation blocker. Research-doc Q1 idioms substantially
overlap the failed attempts A–F (inline-asm forms = B/C; volatile-offset var
would add an lw/addu per site that the clean reference does not have). No
high-probability untried form identified. Per the GIANTS_PLAN timebox rule,
parking 53B40 again rather than low-yield grinding; the crack likely needs the
research still in flight (RESEARCH_FINDINGS_gcc272_idioms.md) or an RTL-pass
(-dj cse1-vs-cse2) investigation, not another blind C permutation.

### Resume bytecmp (current best = 426)
```
cd /home/X/Documents/MML-D2-26 && source .venv/bin/activate
CPP=cpp tools/bytecmp.sh func_80053B40 .claude/worktrees/agent-a6786f513ddba61bd/notes/wip/53b40_draft.c 2>&1 | tail -8
```
-dL loop dump: cpp ... | ./bin/cc1-27 <build flags> -dL <file.i> ; reads
`/tmp/<file>.i.loop` (movable ranking) and `.greg` (dispositions).
