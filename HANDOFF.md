# HANDOFF — MML Decomp

<!-- BEGIN GENERATED COUNTS -->
| metric | value |
|---|---|
| matched functions | **284** |
| C-mapped slice | 284 / **488** = 58.2% |
| main executable | 284 / **1119** = **25.4%** |
| **whole game** | 284 / **~8129** = **~3.5%** |
| active INCLUDE_ASM stubs | **204** |
| still unsplit raw asm | **631** functions in `asm/rock_neo/*.s` |
<!-- END GENERATED COUNTS -->


**Authoritative state (2026-07-26, `tools/check_docs.sh` clean; the +1 match & first split
were clean-rebuild verified — see caveat):**

| metric | value |
|---|---|
| matched functions | **282** |
| C-mapped slice | 282 / **488** = 57.8% |
| main executable | 282 / **1119** = **25.2%** by function |
| **WHOLE GAME** | 282 / **~8,000** = **~3.5%** (main exe + ~7,010 unique overlay funcs) |
| active INCLUDE_ASM stubs | 206 |
| still unsplit raw asm | **631 functions** in `asm/rock_neo/*.s` (was 635; 4 split out this session) |

> **Always quote WHICH denominator.** `/488` = C-slice; `/1119` = main exe; `/~8,000` =
> whole game. A "split" MOVES a fn between buckets (C-slice up, unsplit down) — total 1119
> is invariant. `tools/check_docs.sh` (SessionStart + pre-commit hook) blocks count drift.

> ## ⭐ NEXT SESSION — the explicit goal is MATCHES, skip the meta
> Three tiny, high-leverage functions are **already mapped as stubs and ready to decompile**
> (each called by ~36 stage overlays — highest leverage on the board):
> - `func_800322A8` (23 insn, 37 stages) → `src/rock_neo/Code800322A8.c`
> - `func_80031824` (22 insn, 35 stages) → `src/rock_neo/Code80031824.c`
> - `func_80032488` (44 insn, 36 stages) → `src/rock_neo/Code80032488.c`
> **Do this:** for each, read its `asm/rock_neo/nonmatchings/CodeXXXX/*.s`, write real C,
> then verify with the FULL ritual (`make CPP=cpp check_rock_neo_only` = OK → mutation test
> → `tools/audit_count.sh` count must rise). Template match this session: `func_8002F9C4`.
> More targets: `notes/wip/DEP_PRIORITY.md` (142 raw funcs ranked by leverage).
> **⚠️ FIRST: verify the 3 maps hold under a clean rebuild** (`tools/audit_count.sh` should
> print 282 / 488). They were worker-build-gated but not clean-audited by me last turn.

> **Key docs (this session, read once):** `notes/WORK_MAP.md` (measured scope + evidence),
> `notes/STRATEGY.md` (leverage-ordered plan; main exe = shared runtime, 681 overlay
> call-sites), `notes/META_LESSONS.md` (the denominator saga, teachable),
> `notes/wip/PHASE0_SPLIT_TEMPLATE.md` (how to move a raw fn into the C queue).
> New tools: `overlay_scope.py`, `leverage_analysis.py`, `analyze_raw_asm.py`, `dep_priority.py`.

> **What the 2026-07-26 session did (honest):** measured the true whole-game scope
> (58%→3.5%), built the strategy + priority queue, proved the Phase 0 split template,
> matched +1 (`func_8002F9C4`, 281→282), split 3 more raw fns into the ready-to-match
> queue, and hardened `check_docs.sh` (2 latent bugs fixed + negative-tested). Thin on raw
> match output (one), heavy on the infrastructure that makes the next batch fast.

---

## READ THIS FIRST — what the 2026-07-26 audit found

An adversarial audit was run after the user challenged a count discrepancy. It found four
things. Three had been silently true for weeks or months, and **every existing safeguard
reported success the whole time.**

### 1. The denominator excluded 64.6% of the game

`tools/census.py` computes completion by reading `build/src/rock_neo/*.o`. Anything not
compiled into a C translation unit is **structurally invisible to it** — it cannot appear
in the numerator *or* the denominator.

`rock_neo.ld` (lines 32–42) links seven address-named raw-asm chunks from
`asm/rock_neo/*.s`. Those hold **635 game functions, 56,999 instructions, 227,996 bytes**
that were never split into per-function files and had never been counted by anything, ever.

So "58.1% complete" was really **25.1%**. Not a regression and not lost work — the project
had been measuring the carved-out third and calling it the whole. By *volume* (the honest
measure, since big functions are the hard ones) it is **~7%**.

**The generalizable failure:** *a metric whose denominator is defined by what a script
happens to parse, rather than by the goal.* Ask of every metric: is this defined by the
TARGET, or by a tool's input set?

### 2. Both hand-built "safeguards" exited 0 on failure

- `tools/audit_count.sh` captured `$HASH` and `$CMP` into variables and **never tested
  them**. It printed "DIFFERS" and exited 0.
- `tools/check_docs.sh` piped two of five checks into `while read` — a **subshell** — so
  their `fail=1` could not propagate. It exited 0 with six stale `276`s in HANDOFF.md.

Worse: one had been *demonstrated working*. It caught the three checks that weren't in
subshells and silently passed the rest. **A partial demonstration reads exactly like a full
one.**

**Rule now enforced:** every gate needs a committed negative test — an injected failure that
must produce a nonzero exit. A gate you have never watched FAIL is a confidence generator,
not a check.

### 3. The error is not random — it has a precise shape

Measured across the session:

- **20 / 20** claims about the binary or compiler were self-caught before reaching the user.
- **0 / 6** claims about *progress, scope, or process liveness* were self-caught.
- In **three** of those six the user probed, and the wrong answer was **re-asserted with
  confidence** by re-running an adjacent, easier check ("the binary matches") instead of the
  actual claim ("my change added a match").

Binary claims have a mechanical oracle: the sha1 either matches or it doesn't. Progress
claims had none. **Errors survive exactly where no script adjudicates.** This is the most
predictive finding in this document — when you notice a claim with no oracle behind it,
that is where the next undetected error will be.

### 4. Stale docs nobody caught

`notes/COUNTS.md` was **20 days** stale (said 223/261, real 281/203). A HANDOFF section was
5 counts stale *underneath a "verified, not aspirational" banner*. The dated 2026-07-14
HANDOFF block had been **silently overwritten by four later sessions**, partially destroying
the audit trail of the earlier count-inflation event itself.

**Rule now enforced:** never retro-edit a dated historical entry. Mark it `[SUPERSEDED]`
(the gate honours that marker) and append. The pre-audit HANDOFF is preserved verbatim at
`notes/HANDOFF_ARCHIVE_pre-2026-07-26.md`.

---

## What is now mechanically enforced (all negative-tested)

Prose enforcement — "Claude should remember to run the audit" — is the exact thing that
failed. These fire regardless of who or what is driving.

| trigger | runs | verified how |
|---|---|---|
| **session start** | `.claude/settings.json` SessionStart → `tools/adversarial_audit.sh` | fired live 2026-07-26, output observed |
| **commit** touching `progress.md` / `HANDOFF.md` / `activity.md` / `notes/COUNTS.md` / `src/rock_neo/` | `tools/git-hooks/pre-commit` → `tools/check_docs.sh` | injected stale count → exit 1, **HEAD unchanged**; re-verified after symlinking |
| **push** | `tools/git-hooks/pre-push` → hash + 205 overlay check | pre-existing |

Fresh clone: `make install_hooks` (`.git/hooks/` is not tracked).

`tools/adversarial_audit.sh` does three things the others don't:
- **A.** self-tests the other two gates by injecting failures and requiring nonzero exit
- **B.** attributes every `.text` byte in the target, printing **both** denominators
- **C.** asks five standing "what is not being measured" questions — answer them out loud

---

## Where the project actually stands

### The 120-instruction ceiling (independently re-verified 2026-07-26)

Measured correctly this session, separating matched functions from INCLUDE_ASM stubs
assembled into the same `.o` (an earlier attempt got this wrong and reported an 809-instr
"match" that was actually a stub — same class of error as the denominator bug):

- **Largest function ever matched: `MojiTaskExec`, 119 instructions.** Nothing above 120 has
  ever matched, across the project's entire history.
- Median matched function: **20** instructions; 90th percentile 45.
- Of the remaining stubs: median **90** instructions; **71 over 120**; 28 over 200; 11 over 300.
  Only **55** sit at ≤60. [SUPERSEDED counts — regenerate with `tools/analyze_raw_asm.py`;
  live totals in `notes/COUNTS.md`.]

This is a real wall, not coincidence. Small functions have few register-allocation degrees
of freedom; past ~120 instructions gcc 2.7.2's reload pass makes choices no source-level
spelling reproduces. `func_80053B40` (495 instr) is the documented proof case: ~20 distinct
levers tried and closed, best 70 mismatched rows of 495, residual proven to be a **reload
live-range split** (see `notes/wip/GCC_SOURCE_PROGRESS.md` teeth 10–15 — **do not retry any
of them**).

### Honest projection

- Realistic ceiling on the C-mapped slice: **~71–75%** — roughly the
  ≤120-instruction population plus a few lucky mid-size wins.
- Against the real executable: **~31–33%**.
- **100% is not reachable** with current toolchain understanding. The remainder ships as
  assembly — a normal, legitimate outcome for a PSX decomp.

State that ceiling up front. Do not let it drift back into "58% and climbing."

---

## Matching facts worth carrying forward

- **decomp-permuter is real but weak.** True rate **3 wins / 11 fair trials = 27%**, not the
  "3 for 3" survivorship illusion. Two refuted beliefs: *"low score means close"* (DDE4 sat
  at score 20 for 6,903 iterations with zero improvement; E4C4 started at 80 and zeroed) and
  *"it can't converge from a high score"* (SCEOFF went 100,430 → 260 in ~60s).
- **Permuter score 0 ≠ byte match.** It normalizes jump targets. `func_80009AE6C` scored 0
  with a `j` to the wrong label. Always full-audit; revert false zeros.
- **A workdir zero can fail to transfer in-tree.** `func_8001F828` was a genuine zero,
  faithfully transcribed, still 5 rows off — the workdir used a stub `GAME_WORK` and an
  only-declared `Sce_flag_test` vs the full struct and a same-TU definition.
- **`ACCEPT_REORDERING_BULLSHIT`** in `game.c` and `sub_scrn.c` makes their `#ifndef`-guarded
  INCLUDE_ASM stubs compile the `#else` body. Raw grep sees 209 stubs; only 203 are real.
  **Un-gating one of those 6 is a NO-OP, not a match** — precisely how the earlier inflation
  happened.
- **cc1 segfaults** (bare `Error 139`, no diagnostic) on `break` inside a `do{}while` inside
  an `if`. Rewrite with `goto`.
- Stack slots follow declaration order; constants are rematerialized not spilled (REG_EQUIV);
  `local-alloc.c:combine_regs` deletes a reg-to-reg copy iff the source pseudo is dead, and
  `cse` always makes it dead for a plain `b = a`. Full detail in `notes/LESSONS.md`.

---

## Build & operational gotchas (cost real time; do not rediscover)

- Activate the venv first: `source .venv/bin/activate` (rabbitizer errors = venv not active).
- `CPP=cpp` on **every** make invocation.
- NEVER `make clean` (deletes `asm/` + `assets/` → full re-extract). `rm -rf build` only.
- `.c.o` files are make *intermediate* files: after editing pipeline tools (patchasm.py etc.)
  run `touch src/rock_neo/*.c && rm -f build/rock_neo.elf`, else make reuses stale output.
- Extern declarations: read the target asm FIRST — `($gp)` access ⇒ plain scalar extern;
  `lui/%lo` ⇒ unsized array extern (`extern u8 foo[];`). Wrong choice flips addressing mode
  and mismatches. (LESSONS.md §2.)
- `moji.h`'s `Moji_flag` is `u8[8]` (matches moji.c) — don't "fix" it back to u32.
- Avoid `static` — those functions get no `.globl` marker and patchasm.py's reorder pass
  leaves them at end of file. Keep them last if unavoidable.
- `tools/asm-differ` carries a LOCAL uncommitted Python 3.14 patch; `git submodule update
  --checkout` wipes it. Symptom: diff.py "Failed to parse --base-shift". See SETUP.md.

---

## Next actions

1. **Work the ≤60-instruction lane** — 55 stubs sit there, and it is the only lane where
   both the 27% permuter rate and hand-decomp levers still have good expected value.
2. **Do not resume `func_80053B40`** source-form work. Verdict reached and documented.
3. **`tools/gen_counts.sh`** — generate `notes/COUNTS.md` instead of hand-writing it. Every
   hand-written number in this repo has rotted at least once.
4. **Transfer-gated permuter pipeline** — no permuter result counts as a win until
   `tools/bytecmp.sh` confirms it in-tree, because workdir zeros demonstrably fail to
   transfer.
5. **Answer the five questions** in section C of `adversarial_audit.sh` each session — out
   loud in the response, not silently.

## Delegation & token hygiene

- `.claude/agents/build-runner.md` + `function-mapper.md` (Haiku) exist — route builds and
  greps there when output would bloat context; they return summaries. Keep C-matching
  reasoning in the main thread.
- Don't paste whole asm files for big functions; read the span.
- The user clears context between sessions deliberately — everything worth knowing must land
  in this file, LESSONS.md, progress.md, or a commit message.

## Verification contract with the user (do not weaken it)

- The only authoritative count is `tools/audit_count.sh`. Never a source grep.
- Record the baseline at session start. A "+N" claim requires the tool's number to *rise by
  N*. "Check prints OK" only proves the binary matches now, not that you caused it.
- When challenged on a number, **find the falsifying question**; don't re-run the comfortable
  proxy. For a match claim that question is *"would it match without my change?"*
- Before asserting anything works — a match, a script, a hook, a watcher — **trigger it and
  show the output.** "I set it up" is not "I verified it fires."
- The user can independently verify: `make CPP=cpp check_rock_neo_only` and
  `cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe`.
- Fake progress is worse than no progress. It hides where you actually are.

Prior history: `notes/HANDOFF_ARCHIVE_pre-2026-07-26.md` (verbatim, unedited).
