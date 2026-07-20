# 53B40 — Brief: crack the register-pressure fossil (cluster #2, "the boss")

**For a fresh strong-model session.** Read this + `notes/wip/GCC_SOURCE_PROGRESS.md`
teeth 6–9 first. Do NOT re-derive what's below — it cost two Fable sessions.
Work ONLY this problem unless the permuter has already hit 0 (check first! see
"Before you start").

## Before you start
1. `ls tools/decomp-permuter/mml_53B40/output-*` — if an `output-0-*` exists,
   SKIP this brief: verify that candidate with the positional gate below, land it
   via `notes/wip/53b40_landing_tu.c` (tooth 9 recipe), run the full ritual,
   `audit_count.sh` must print 275. Done.
2. Otherwise record the baseline: `tools/audit_count.sh` (expect 274) and
   `CPP=cpp tools/bytecmp.sh func_80053B40 notes/wip/53b40_draft_permbest.c`
   (expect: words 495/495, ~193 hard mismatches — the banked parity base).

## The positional gate (LEARNED THE HARD WAY — the permuter's score lies near the end)
A candidate counts as progress ONLY if `tools/bytecmp.sh` shows:
- `words: expected=495 actual=495` (length parity), AND
- hard mismatch count strictly below the baseline 193.
The permuter's weighted score is alignment-based and will happily drop while the
POSITIONAL state worsens (e.g. its "1030"/"1020" candidates were 494 words / 316+
hard — traps). Use `tools/structdiff.py` (after a bytecmp run) to see the diff as
aligned blocks. ~16 of the 193 are the frame-offset echo of THIS cluster; ~12 are
gp_rel-offset artifacts of the scratch build (ignore those — they resolve in-tree).

## The problem (all evidence already verified)
The target frame is `addiu $sp,-0x50`; ours is `-0x48`. The target's extra 8 bytes
(0x20–0x27($sp)) are NEVER TOUCHED — they are reload SPILL SLOTS allocated for two
pseudos during the ORIGINAL compile and then never used (reload inheritance
optimized the accesses away, the reservation stayed). Consequences visible in the
bytes:
1. Every callee-save/restore offset differs by 8 (prologue/epilogue block).
2. At the RENDER site the target has `lw $v0,0($v1)` then `addu $s1,$v0,$zero`
   (load into a temp, copy to the cross-call var). Our build folds the copy —
   `lw $s1` direct. The POST_RENDER twin of this copy was achieved (statement
   interleave `pr = *X; rect.x = m->x8; prim = pr;`), but the RENDER one resists
   every source spelling tried (list in tooth 8/9 + session transcript):
   plain re-read, multi-set pr, store-from-pr, embedded assignment, block temps.
   cse's copy machinery folds them all (cse.c make_regs_eqv canonicalization +
   local-alloc/global copy preference tying).
3. The surviving-copy + spill-slot evidence together say: the ORIGINAL compile had
   ~2 more simultaneously-live pseudos in the render/post_render region than we
   do. The register mirrors elsewhere (dc in a0-vs-a2, tail masks a1/a2) are
   plausibly downstream of the same traffic difference.

## The method (dump forensics, NOT blind permutation)
1. Regenerate the allocation dumps on the current base:
   ```
   cd <scratch>; REPO=~/Documents/MML-D2-26
   CPPF="-I$REPO/include -undef -Wall -lang-c -fno-builtin -Dmips -D__GNUC__=2
     -D__OPTIMIZE__ -D__mips__ -D__mips -Dpsx -D__psx__ -D__psx -D_PSYQ
     -D__EXTENSIONS__ -D_MIPSEL -D_LANGUAGE_C -DLANGUAGE_C -DHACKS"
   CCF="-mcpu=3000 -quiet -w -O2 -funsigned-char -fpeephole -ffunction-cse
     -fpcc-struct-return -fcommon -fverbose-asm -fgnu-linker -mgas -msoft-float
     -G8 -gcoff"
   cpp $CPPF $REPO/notes/wip/53b40_draft_permbest.c > b.i
   $REPO/bin/cc1-27 $CCF -dL -dg -dl -dumpbase b.c b.i -o b.s
   ```
2. In `b.c.greg`: find the "Spilling reg N" lines and the "Register dispositions"
   list. In OUR compile the reload spills (reg 9=$t1, reg 64=HI) displace pseudos
   that all still land in HARD regs → no stack slots → frame 0x48. The goal state:
   TWO pseudos end up with NO hard reg (memory) → assign_stack_local gives them
   0x20/0x24 → frame 0x50 → prologue/epilogue block snaps to match.
3. Which pseudos spilled in the ORIGINAL? Unknown directly — but the fossil sits
   BELOW the rect local (0x18–0x1F) and the render-site copy says the loaded-value
   temp ($v0) and the cross-call var ($s1) were SEPARATE pseudos there. Leading
   hypotheses to test IN THIS ORDER (each: edit → cc1 → check greg dispositions
   for memory pseudos + frame size in b.s → then the positional gate):
   a. An extra pointer/temp variable live across BOTH SetDrawArea calls (render
      AND post_render) but otherwise cheap — forces one more callee-saved
      candidate than there are s-regs (we currently use exactly $s0–$s7+none
      spare; the 9th loser would spill). E.g. a variable holding `&D_80097F50[…]`
      or dc hoisted to function scope AND kept live — but verify it doesn't
      change emitted code elsewhere (the gate catches it).
   b. Reload-forced: make the two mult/div regions' HI/LO reload displace a
      pseudo to memory by increasing t-reg pressure inside the glyph loop
      (an extra named temp in the %/÷ 0x15 cluster).
   c. If (a)/(b) shift OTHER registers before reaching frame 0x50: the mirrors
      that move are DIAGNOSTIC — compare which pseudo numbers swapped in .greg
      vs the target's known assignment (target map in tooth 6: m=$s2, prim=$s1,
      giv=$s0, tst=$s5, setflag=$s7, fff=$s3, 0xFF000000=$s4, magic=$s6).
4. When frame hits 0x50 AND the copy appears: run the full positional gate, then
   hand the residue (if any) back to the permuter (protocol below).

## Permuter protocol (fixed infra — do not rediscover)
- Workdir `tools/decomp-permuter/mml_53B40/`; compile.sh MUST cd to repo root
  (already patched — see PERMUTER_GUIDE.md gotcha).
- Launch: `cd tools/decomp-permuter && nohup python3 permuter.py mml_53B40 -j8
  --best-only --stop-on-zero > mml_53B40/permuter.log 2>&1 &` (venv active).
- Harvest cycle — ORDER MATTERS (a race once cost a sub-900 candidate):
  (1) `pkill -f "permuter.py mml_53B40"` FIRST, (2) gate-check EVERY remaining
  `output-*` (new ones can land between your check and any cleanup), (3) adopt
  the best passer: `cp` to base.c + to notes/wip/53b40_draft_permbest.c, commit,
  (4) `rm -rf output-*`, (5) relaunch. Never `rm` while the permuter is running.
- Adoption rule: adopt candidates that IMPROVE the positional count, and also
  ones that are positionally NEUTRAL (495/495, hard count equal) but improve the
  weighted score — the latter reposition the search for free. Two proven device
  families to recognize in diffs: added always-true folded-ifs with an identical
  else copy (pseudo-numbering nudge, folds away entirely) and `x = m;` alias
  temps. REJECT anything at 494 words regardless of score (alignment traps).
- Its useful finds so far came as: added constant locals (tst/setflag family),
  statement moves, and alias temps (`mv = m`) — expect more of the same.

## Landing ritual (unchanged, MANDATORY)
0 hard mismatches in bytecmp → adapt via `notes/wip/53b40_landing_tu.c` (already
byte-parity-verified against the tree headers; needs the (u8)x3F/(s8)x71 casts +
DRAWCTX typedef + externs pasted into moji.c, INCLUDE_ASM line replaced) → clean
`rm -rf build` rebuild → `make CPP=cpp check_rock_neo_only` OK → mutation test
(break a constant, hash MUST fail, restore, OK) → `tools/audit_count.sh` must
print **275** → update progress.md/activity.md/HANDOFF.md/LESSONS.md → commit dev.
NEVER claim the match from bytecmp alone.

## Forensics session notes (2026-07-20, appended live — read before starting)

**Crunch points identified** (from `-dg` on the 192-state base; "Need reg" lines):
- insn 91 = the HEAD division's `reg112 = LO >> 2` (REG_EQUAL div 196): MIPS can't
  shift from LO, reload needs a GR scratch → **spills hard reg 8 ($t0)**. In the
  target this is `mflo $t0; sra $a0,$t0,2`.
- insn 716 = the glyph `umulsi3_highpart` (multu, %/÷ 0x15): needs HI/MD scratch
  → **spills hard reg 64 (HI)**.
- These spills are intrinsic to the instruction shapes — the ORIGINAL had the
  same ones. The fossil = at those moments the original had pseudo(s) LIVING in
  $t0/HI whose retry found no free reg → memory slots 0x20/0x24. In our compile
  nobody occupies $t0/HI (pressure below threshold) → no slots → frame 0x48.
- Observed unified-theory candidate: in the target, $a0 holds 0x80000000 ACROSS
  the head division (born in the loop-top bgez slot — this is also the [39-50]
  slot cluster). Probe VE (named `hb = 0x80000000;` local at arm top) was
  NEUTRAL: 495/192, frame unchanged, slot unchanged.

**Probes already tried on the 192-state — all NEUTRAL or WORSE (do not repeat):**
- VA: `t = (s16)m->x6;` at the head cond → 494/326 ✗
- VB: render-site `rp = &D_80097F50[…]` interleave between pr-load and prim-copy
  → 495/192 neutral (was 37-worse three bases ago — base evolution changes probe
  outcomes; re-testing old failures is legitimate)
- VC: named `t = m->script[0]` in the glyph %-/÷ pair → 493/360 ✗
- VD: re-split tail `t = m - mb` → 495/196 ✗
- VE: `hb = 0x80000000` named local → 495/192 neutral, frame still 0x48

**Next moves (in order):**
1. Read the ORIGINAL's live-set at the crunch directly from the target asm
   (44 3D0-44 414): enumerate every register carrying a value across the head
   div and match each to our dump's pseudo at that point; the VALUE with no
   home in our compile is the fossil variable. Do the same at the glyph multu
   (44 780-44 7C8).
2. Combination probes: single additions were neutral — try hb + sh6 + (a
   glyph-region extra) TOGETHER; the crunch needs ~2 more live values, not 1.
3. If frame flips to 0x50 but hard count rises: keep it anyway if the RISE is
   the prologue/epilogue offsets snapping (they'd now MATCH — check indices
   0-13 first) — the net after the cascade is what matters.
