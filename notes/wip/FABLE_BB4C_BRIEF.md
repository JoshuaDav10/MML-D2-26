# Fable session brief — advance/land func_8001BB4C (BB4C), the CD loader

**You are Claude Fable in a fresh Claude Code session on the MML (Mega Man Legends
PSX) matching-decompilation at this repo.** Max-effort, single-target session on the
BIGGEST function in the game (~809 insns, the CD streaming/loader state machine).
Work ONLY from this brief + the files it names.

## Why this target
53B40's reasoning path is exhausted (its own verdict — permuter-lottery only now; do
NOT touch it). BB4C is the same register/addressing genus but FAR less explored and is
the largest single prize (~809 insns ≈ 2.6% of total volume). A concrete lever banked
2026-07-14 (the held-pointer idiom, below) has NOT yet been tried on it.

## Session start — read these first, then work
1. `CLAUDE.md` — build, dev-branch rule, and the **"Progress-counting integrity"**
   section (MANDATORY). `tools/audit_count.sh` is the only trusted count (currently 274).
2. `notes/LESSONS.md` — esp. the 2026-07-14 **held-pointer idiom** entry and the
   register-alloc / `-dg` entries and "Counting matched functions — GOTCHA".
3. `notes/wip/BB4C_PROGRESS.md` + `notes/wip/BB4C_ANALYSIS.md` — the full structural
   solve. Do NOT re-derive; it cost multiple sessions.
4. Draft: `notes/wip/bb4c_draft_v2.c`. Branch with prior endgame work: `bg/bb4c-endgame`.
5. Target asm: `asm/rock_neo/nonmatchings/cd/func_8001BB4C.s` (read spans).

## State already established (verified — do not re-derive)
- Structure fully solved: ~805–806/809 words, CD_WORK union + three-pointer addressing
  correct. Blocked at ~**319 hard mismatches**, which is a **2-word instruction surplus**
  cascading every downstream branch-displacement + register number. NOT a register-count
  problem (bg/bb4c-endgame disproved that via `-dg`: all 7 callee-saved regs held exactly).
- The 2 surplus words are the project's two PARKED GENERA, both understood:
  1. **ADDRESSING-CRUX (states 2 & 4):** draft emits `r->x`/`r->y` as an ABSOLUTE store
     (`sh $v1, D_800...`); the reference reuses a held `$s1`=r pointer
     (`addu $a0,$s1,$zero; sh $v1,0($a0)`). cc1 folds the draft's pointer form to absolute.
  2. **JUMP-CANONICALIZATION (state 1 tail):** reference gets a delay-slot store by holding
     `&D_800C5608` in a base reg (`bnez tail; sw` in slot; `j tail0`); draft stores then
     `beqz`+extra `j` — cc1 constant-folds the pointer back to absolute.
- A proven-required unsigned-mask `srl` fix is DEFERRED (it spikes the count to ~419 until
  word-parity is reached) — apply only AFTER the 2-word surplus closes.

## First thing to try (the untested lever)
The **held-pointer idiom** (LESSONS 2026-07-14, cracked func_8001D394): assigning a symbol
address to a LOCAL pointer and offsetting THAT forces cc1 to keep the base in a register
instead of constant-folding `SYM+off` to an absolute address —
`u8 *p = &D_800C5608; ... *p = v;` / hold `r` in a local pointer and store `p->x`.
This is exactly the ADDRESSING-CRUX shape. Try it on the state-2/4 `r->x`/`r->y` stores
FIRST — it may close one or both surplus words.

## Verification gate — NON-NEGOTIABLE (this project was burned by fake progress)
- Iterate with `CPP=cpp tools/bytecmp.sh func_8001BB4C notes/wip/bb4c_draft_v2.c`
  (0 hard mismatches + sane relocs = landable). Watch WORD PARITY (809/809) — a lower
  hard-count at wrong length is a trap.
- A match counts ONLY when `tools/audit_count.sh` shows the count RISE to 275 AND prints
  `hash check : OK`. Gate on the FULL hash, never census-alone (census misses reg/link nits).
- Land into cd.c ONLY at 0 hard mismatches + clean `rm -rf build` + full hash OK + mutation.

## Checkpoint / save protocol (a session can die anytime)
- ALL commits on `dev` (verify branch first; NEVER main). Small, frequent commits.
- Append findings to `notes/wip/BB4C_PROGRESS.md` as you go. On a match: full ritual +
  update progress.md/activity.md/HANDOFF.md (count from audit_count.sh) + LESSONS, commit.
- On pause: write the exact next step into BB4C_PROGRESS.md.

## Honest expectation
This is the hardest single function in the project. "Land it" is the stretch goal;
"close the 2-word surplus with the held-pointer lever and get a clean partial" is a real
win. If the same fossil/canonicalization wall proves C-unreachable after a genuine
attempt, document that verdict and STOP — do not spiral into a multi-session grind
(that is the 53B40 mistake; don't repeat it).
