# HANDOFF — MML Decomp

<!-- BEGIN GENERATED COUNTS -->
| metric | value |
|---|---|
| matched functions | **451** |
| C-mapped slice | 451 / **622** = 72.5% |
| main executable | 451 / **1122** = **40.2%** |
| **stage overlays** | 112 / **~7064** |
| **whole game** | 563 / **~8632** = **~6.5%** |
| active INCLUDE_ASM stubs | **171** |
| still unsplit raw asm | **500** functions in `asm/rock_neo/*.s` |
<!-- END GENERATED COUNTS -->


**Live numbers are the generated block above** — produced by `tools/gen_counts.sh` /
`tools/sync_docs.sh` and byte-diffed by `tools/check_docs.sh`. Never hand-type a count
into this file; the stale table that used to sit here said 282 long after it was 301.

> **Always name WHICH denominator.** `/566` = C-slice · `/1119` = main exe (invariant) ·
> `/~8629` = whole game. A *split* MOVES a function between buckets — it is bookkeeping,
> not progress. Only a rise in **matched** counts.
>
> **Stage counting has TWO honest numbers and they are both real:** *unique bodies* (the
> headline, 106) and *function instances* (449). Transplanting a solved body into a new
> archive adds instances but **zero** unique bodies — say which one you mean, and do not
> present instance growth as headline growth.

> ## ⏸ LOOP STOPPED 2026-07-29 ~00:10 — usage limit (resets 4:30am CT)
> Stopped cleanly. Tree is GREEN and pushed: hash OK, 205/205 overlays, nothing half-edited.
>
> **DRAFTED AND VERIFIED BUT NOT LANDED — do this first, it is nearly free.**
> `/tmp/st11_new1.md` holds 5 finished bodies for ST11/ST11B that an agent verified but I
> ran out of budget before landing. **/tmp may be cleared on reboot — rescue it early.**
> Landing needs three things applied to BOTH `src/ST11/*/Code801000B4.c` and `src/ST11B/...`:
> 1. `BOSS_WORK` needs a byte at 0x0B — split `u8 padB[0x14 - 0xB];` into
>    `u8 xB;` + `u8 padC[0x14 - 0xC];`
> 2. add `extern volatile u16 System_timer;` and `extern void (*D_8010A1BC[])(void);`
> 3. `tools/land_stage_batch.py /tmp/st11_new1.md Code801000B4 ST11 ST11B`
> Then chunks (check exit code) + cmp both BINs + checksizes + check_overlays.
>
> ⚠️ **`func_80109034` in that batch reports 2 HARD mismatches in bytecmp and is still
> correct.** bytecmp decides "is this a relocation slot?" by regex-matching the reference
> asm text for `%hi`/`%lo`; splat never attached the symbol there, so the line reads as
> literal arithmetic `lui $v0, (0x1F800002 >> 16)` and two real reloc slots get graded as
> failures. The agent linked and disassembled it to prove the 7 words match. **Gate it on
> the full build, not on bytecmp.** This is a harness blind spot worth fixing.
>
> **New idiom, worth a LESSONS entry:** `volatile` on `System_timer` is load-bearing —
> without it gcc sees `(x << 3) & 0x60` keeps only bits 2-3 and narrows the halfword load
> to `lbu`, breaking the match. Reusable for any hardware/scratchpad read.
>
> **Also queued:** 7 more dispatchers in ST11 (`func_801015F4` D_8010A1BC 0x8;
> `func_80103524` D_8010A3BC 0x34B; `func_80103768` D_8010A3D4 0x34B; `func_80103B70`
> D_8010A3E8 0x34B; `func_80107EBC` D_8010E138 0x8; `func_80108394` D_8010E14C 0x8;
> `func_80109050` D_8010E26C 0x8). Template is the landed `func_801019CC`. An agent was
> mid-sweep of the other seven open archives for this same family when usage ran out —
> **that sweep is the highest-value unfinished question.**
>
> **Note:** `func_801015F4` was accidentally assigned to two agents at once. Dedupe batches.
>
> **Model note:** sonnet went 28/28 on transplants and 6/6 on the template family, and
> correctly flagged its own tool's false failure rather than shipping a bad pass. Good fit
> for both lanes.

> ## ⭐ NEXT SESSION — the stage duplicate lane is the live one
> **Highest leverage now: keep opening stage archives and transplanting solved bodies.**
> Six archives are wired (ST03/ST03B, ST0C/ST0CB/ST0CC, ST10/ST10B, plus ST1A/eve19).
> `tools/dups.py` ranks the rest; the "already" column is free work waiting.
>
> The compounding is real and measured: ST0C's free-win count went **8 → 69** purely
> because of ST03 work, with no new matching effort. Rank, open, transplant, repeat.
>
> **Recipe for opening an archive** (proven 4x this session):
> ```
> # 1. confirm twins: sha1 the code chunk of each candidate (ST03==ST03B, ST0C==ST0CB==ST0CC,
> #    ST10==ST10B, ST04==ST04B, ST11==ST11B) -- one C file then serves 2-3 archives
> # 2. edit the yaml: turn the bulk `- [OFF, asm]` text subsegment into
> #      - [OFF, c, Code<VRAM>]
> #    for the WHOLE text. rom = vram - 0x800FF800 (overlay constant; main exe is 0x8000F800)
> # 3. mirror the SAME edit into every twin's yaml, refresh generated syms, re-split each
> # 4. DELETE the stale bulk OFF.s splat leaves behind -- gen_map now FAILS if you forget
> # 5. build stubs only and prove byte-identical BEFORE writing any C:
> #      make CPP=cpp chunks ; echo $?     # READ THE EXIT CODE
> #      cmp build/<A>.BIN disks/us/CDDATA/DAT/<A>.BIN
> # 6. transplant: for each function whose body hash is already MATCHED, paste the solved
> #    body renamed, and RE-VERIFY each against THIS archive's own asm with bytecmp
> # 7. after landing: tools/checksizes.py, then chunks + cmp + check_overlays
> ```
>
> **Landing checklist (each step caught a real bug this session):**
> - `bytecmp` per function BEFORE landing — and AGAIN after, if you adapted anything.
> - `tools/checksizes.py <built.o> <nonmatchings-dir>` — catches dropped code instantly.
> - `make CPP=cpp chunks` **exit code**, not just `check_overlays` (a failed chunks leaves
>   stale BINs and check_overlays then passes on them).
> - `cmp` each twin against the disc.
>
> **Also ready:** ~20 Tier A engine drafts remain in `notes/wip/lane_b_drafts.md`
> (Tier S spent, most of Tier A landed 2026-07-29). The four-parallel-read-only-agent
> pattern worked well: agents draft+verify in /tmp with a unique `BYTECMP_OBJ`, the main
> thread lands serially behind the build gate. 21/21 and 10/10 on two waves.

> ## ⚠️ LANDMINES — read before touching the build
> **1. splat offsets.** `rom_offset = vram - 0x8000F800`, NOT `- 0x80010000` (a 0x800
> header sits between). Use `tools/phase0_split.py`, which has the correct constant.
> Splat IS safe to re-run now; it was not before.
>
> **2. PHANTOM MATCHES — a green hash cannot detect them.** A `.c` can compile to a `.o`
> that `rock_neo.ld` never references: census counts it, the binary links the raw asm, and
> the full hash stays **OK** because the original bytes shipped. Guards are in `census.py`
> and `gen_map.py`, both negative-tested. **Counting objects is not counting the binary.**
>
> **3. WORKTREE PARALLELISM DOES NOT WORK — unresolved.** `-gcoff` embeds the absolute
> source path in every C object, so a worktree at any other path builds a different binary
> and can never match. Equalising path LENGTH did not fix it. See LESSONS.md. Consequence:
> **build-gated work must stay in the main tree.** Read-only agents in parallel are fine and
> productive — that is how the three analysis lanes were produced.
>
> **5. cc1-27 ACCEPTS AN UNKNOWN STRUCT MEMBER.** `-lang-c` keeps the pre-ANSI global
> member namespace, so `work->x328` compiles even when `WORK` has no such field — it
> resolves against some other struct and the body can be **eliminated entirely**. Two
> functions silently became 2 words (an empty body) while `make chunks` exited 0. Never
> dedupe typedefs by NAME across files; use distinct names or assert the field exists.
> **A function that builds to 2 words had its body eliminated** — that is the fingerprint.
>
> **6. A STALE BULK `.s` DOUBLE-COUNTS EVERY FUNCTION.** Turning `- [OFF, asm]` into
> `- [OFF, c, Name]` makes splat write the new `nonmatchings/*.s` but NOT delete the old
> `OFF.s`. `gen_map`'s recursive glob then sees everything twice (645 phantom rows on the
> ST0C triplet; 34 real C functions reported as 68). The headline unique-BODY count was
> unaffected, so no gate saw it. **`gen_map` now FAILS LOUDLY** on duplicate
> `(archive, function)` keys — delete the stale file it names.
>
> **7. SCRATCH HARNESSES MUST BE FLAG-IDENTICAL TO THE MAKEFILE.** `tools/tryfn.sh` (used
> by `bytecmp.sh`) was missing `-mel`, so it disagreed with the real build on any unaligned
> struct copy — 8 phantom mismatches on correct C. Also fixed: `bytecmp`'s reloc regex
> omitted `%gp_rel`, scoring every gp-relative store as a hard mismatch. **When the harness
> and the build disagree, the harness lies in BOTH directions.**
>
> **4. A DENOMINATOR BUG SURVIVED UNTIL 2026-07-28.** `gen_map` globbed `asm/rock_neo/*.s`,
> which does not descend into subdirectories, so `asm/rock_neo/psxsdk/code.s` — **446
> functions, genuinely linked** — was invisible to every count this project ever produced.
> Whole game 8,183 → **8,629**. Same class as the 484-vs-1119 scandal, found by an agent
> rather than by any gate. When a number looks stable, ask what the glob cannot see.

> ## ✅ WHAT 2026-07-29 DID (all hash-gated, 20 commits pushed)
> **Engine 363 → 407** (+44): the 20-function jalr-dispatcher family on a template already
> proven twice in the tree, 21 Tier A singles via four parallel read-only agents, and 3
> functions that were **unmatchable from any C source** until the ASPSX prefix table was
> completed from a census (52 reg-first sites / 21 symbols; the tuple had 5).
> **Stage 12 → 106 unique bodies / 449 real function instances across 8 archives**: opened
> ST03+twin, the ST0C triplet, and ST10+twin, chose work by COPY COUNT rather than address
> order, and transplanted solved bodies between archives.
>
> **Four tool bugs fixed, every one of which had been silently distorting results** — see
> landmines 5-7 plus the ASPSX table. New tools: `tools/checksizes.py` (per-function word
> count vs reference; localised two transcription bugs no other gate caught) and
> `tools/structdiff.py`. New LESSONS entries: ~20, all measured not guessed, including a
> four-way decision table for conditional-return shapes.
>
> **Method note worth keeping:** auto-deriving template parameters from each function's own
> asm beat transcribing them from write-ups — 17/18 and 25/32 first-pass, and every miss was
> an error in the notes, not the code.

> ## 📋 THE PLAN → `notes/NEXT_STEPS.md`
> Template leverage lives in the STAGE files (1,331 functions) not the engine (32), so
> converting archives beats grinding the engine. m2c is installed and proven. Library code
> is downloadable via `tools/fetch_vendor.sh` — but note Lane A's finding: **zero of the
> 614 unsplit engine functions are library code.** All 614 are Capcom; the real SDK is the
> separate psxsdk segment. `notes/wip/SDK_CANDIDATES.md` is 100% false positives and should
> not be trusted.

> ## ✅ RESEARCH ANSWERED 2026-07-26 → `notes/RESEARCH_FINDINGS_scaling.md`
> CANDIDATE / not hash-gated — verify before trusting paths and flags.
> - **The >120 ceiling is OURS, not the toolchain's.** MGS's ~626 KB main exe is 100%
>   matched on PSY-Q GCC 2.x. Corroborates what we proved ourselves at 137 insns. The
>   engine target moves from ~908 toward **1,119**; 97 unmatched sit in 120-200, 63 in
>   200-300, 50 above 300. The lever is anything that supplies **source shape**, not more
>   hand-iteration.
> - **Install m2c** (`--target mips-gcc-c`) against the existing `tools/m2ctx.py`; aim it
>   at the 354 unmatched functions ≤50 insns. Draft only — rewrite expressions. Keep it if
>   time-to-first-draft drops ≥30%.
> - **Stage dedup is a solved pattern:** one shared `.c` compiled per overlay; port SOTN's
>   `dups` / `mipsmatch` / `function_finder_psx.py`. Do this BEFORE converting archive #2.
> - **PSY-Q/libgcc can be obtained, not RE'd:** `psyq-obj-parser` (PCSX-Redux) converts
>   Sony `.OBJ`/`.LIB` to ELF; `ghidra_psx_ldr` has FLIRT signatures; `sozud/psy-q-decomp`
>   is reference C. No public per-function hash DB exists.
> - **Permuter policy:** late-stage regalloc only, always `-j`, and **never accept score 0
>   without an in-tree rebuild + full-binary SHA-1** (score 0 ≠ byte match by design).

> ## Research farmed out
> `notes/RESEARCH_QUESTIONS.md` — five scoped questions with the numbers that make each one
> matter. A later session should expand these into a full deep-research prompt (house style:
> `notes/DEEP_RESEARCH_PROMPT_reload.md`). The highest-value one: **is the >120-instruction
> wall ours or everyone's?** It moves the realistic engine target between ~908 and 1,119.
> The reload prompt itself was launched in a separate session on 2026-07-26; result pending.

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
