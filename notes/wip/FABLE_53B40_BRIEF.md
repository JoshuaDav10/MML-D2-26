# Fable session brief — finish func_80053B40 (53B40), the moji text/font renderer

**You are Claude Fable in a fresh Claude Code session on the MML (Mega Man Legends
PSX) matching-decompilation at this repo.** This is a max-effort, single-target
session. Work ONLY from this brief + the files it names — do NOT load unrelated
context. One job: land func_80053B40 to a byte-for-byte match (or prove the exact
remaining lever and advance it).

## Session start — read these first (in order), then stop reading and work
1. `CLAUDE.md` (auto-loaded) — build commands, dev-branch-only rule, and the
   **"Progress-counting integrity"** section (MANDATORY — read it, it is why this
   project exists as a trustworthy artifact).
2. `notes/LESSONS.md` — esp. "Counting matched functions — GOTCHA" and the
   register-alloc / `-dl`/`-dg` entries.
3. `notes/wip/GCC_SOURCE_PROGRESS.md` — teeth 1–5: the FULL 53B40 mechanism,
   already cracked. This is your primary source.
4. `notes/wip/53b40_draft.c` — the current lever'd C draft (compiles, near-match).
5. `notes/COMPILER_IDIOMS.md` §2 (allocno priority) + §4 (move_movables formula).
6. Target asm: `asm/rock_neo/nonmatchings/moji/func_80053B40.s` (read spans, not walls).

## What is already established (do NOT re-derive — it cost a prior Fable session)
- The 53B40/BB4C loop-hoist wall is **C-REACHABLE, not a toolchain wall.** Proven by
  reading FSF gcc-2.7.2 `move_movables` and cross-checking the real `cc1-27 -dL` dump.
- `cc1-27` is SN-patched `GNU C 2.7.2.SN32.3.7` — FSF source is the mechanism baseline;
  **ground truth is always the real `-dL`/`-dg` dump from `./bin/cc1-27`.**
- Three C levers are compile-verified (in 53b40_draft.c): (1) reused `pp` local for
  `0x1F800070` (keeps it inline, 4 sites); (2) pre-loop `setflag=0x40000000` local
  (hoists it to a callee-saved reg); (3) extern symbol `D_800BB9C8` (=Moji_work+0x310)
  for the tail entry-check / loop-end.

## THE remaining problem (the whole job)
The sole residual is an **allocno register assignment**: `0x40000000` lands in `$fp`,
but the target puts it in `$s7` (and Moji_work base in `$s2`). That one swap cascades
through the frame size and every downstream offset. Fix the register choice → the
cascade collapses → match.

**Approach:** this is the same allocno-priority problem that cracked MojiTaskExec.
Generate the real `-dg` (global reg alloc) dump from `cc1-27` on 53b40_draft.c
(recipe is in GCC_SOURCE_PROGRESS.md tooth 1 — the exact cpp|cc1 flags with `-dg`),
read which pseudo wins `$s7` and why (priority = floor_log2(refs)*refs/live_len; ties
→ lower pseudo). Then find the C form that raises the `0x40000000` pseudo's priority
(or lowers the competitor's) so greg assigns it `$s7`. Test each hypothesis against
the dump + `tools/bytecmp.sh func_80053B40 notes/wip/53b40_draft.c` before an in-tree build.

## Verification gate — NON-NEGOTIABLE (this project was burned by fake progress)
- A match counts ONLY when `tools/audit_count.sh` shows the count RISE and prints
  `hash check : OK`. Never claim a match from "bytecmp looks close" or "check OK"
  alone. Run `audit_count.sh` at the start to record the baseline (currently 274).
- Land into moji.c ONLY at 0 hard mismatches + clean `rm -rf build` rebuild + full
  hash OK + mutation test (break a constant → hash MUST fail → restore → OK).
- Gate on the FULL hash, not `census.py` alone (census reads per-.o and misses
  register/link nits — see LESSONS "Counting matched functions — GOTCHA").

## Checkpoint / save protocol (a session can die at any time)
- ALL commits on `dev` (verify `git branch --show-current` first; NEVER main).
- Append findings to `notes/wip/GCC_SOURCE_PROGRESS.md` as you go — never leave them
  only in chat. Commit small and often.
- On a match: run the full ritual above, update `progress.md` + `activity.md` +
  `HANDOFF.md` (authoritative count from audit_count.sh), record the winning lever in
  `LESSONS.md`, commit.
- On pausing/handing off: update GCC_SOURCE_PROGRESS.md with the exact next step.

## Honest expectation
The permuter stalled on this and a prior Fable session got the mechanism but not the
finish. This IS the best-positioned target (most groundwork done, tightest scope,
pure Fable-lane problem) — but landing it is not guaranteed. A definitive
"here is the exact allocno lever, here is why cc1 resists it, here is the C that fixes
it" verdict is itself a win even if the byte-match needs one more pass.

## If 53B40 lands and budget remains
BB4C (`func_8001BB4C`, cd.c, ~809 insns) is the next giant — same addressing/register
genus. Start from `notes/wip/BB4C_PROGRESS.md` + `bb4c_draft_v2.c`, and try the
**held-pointer idiom** (`u8 *p = SYM; use(p+off)` keeps the base in a reg vs folding to
absolute — see LESSONS 2026-07-14) on its addressing-crux.
