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
- Harvest cycle: on a new `output-<score>-*`, apply the POSITIONAL GATE. If it
  passes, semantic-diff its source.c vs base.c (whitespace-normalized difflib),
  re-express the mutation cleanly, verify again, `cp` to base.c, `rm -rf output-*`,
  relaunch, commit the banked copy to notes/wip/53b40_draft_permbest.c.
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
