# Expedition: Read the GCC 2.7.2 source to crack the compiler walls

**Status:** PLANNED, not started. Pick this up in a fresh session.
**Model:** run as a **Fable** worker (toolchain forensics — the project's Fable
lane). Start from a FOCUSED brief (this doc + the files listed), NOT a long chat
context — carrying a bloated conversation in wastes tokens for no benefit.
**Created:** 2026-07-12 (Opus session, at usage limit — handing off).

## Goal

The parked compiler "walls" (53B40, F798, and their genera) resist every
trial-and-error C shape because the decisions are made by internal GCC passes we
currently treat as a black box. GCC 2.7.2 is **open source**. Read the actual
pass source to turn guessing into knowing.

## Honest expected outcome (do NOT oversell)

Reading the source yields ONE of two results per wall, and **both are wins**:
1. **A C-level trigger** — we learn the exact rule and construct the C that makes
   cc1 make the original's choice → the function (and its genus siblings) match.
2. **Proof it's unreachable from C** — the rule shows no C form can trigger it →
   we STOP guessing forever and route around it (toolchain/maspsx patch, a
   permuter heuristic, or accept-as-asm).

`notes/wip/53B40_ANALYSIS.md` already suspects 53B40 is a **value-based CSE**
merge that may be C-unreachable. So outcome (2) is a real possibility for 53B40 —
do not assume outcome (1). The point is to get a DEFINITIVE answer, not to
promise a match.

## Why this is worth premium (Fable) tokens

It's reasoning only a strong model can do, and it resolves a CLASS, not one
function. Grinding easy functions is the WRONG use of premium tokens (permuter +
batch do those cheaply/free). Cracking or definitively closing a wall is the
RIGHT use.

## Prerequisites / inputs for the worker

Read ONLY these (keep context small):
- This doc.
- `notes/wip/53B40_ANALYSIS.md` and `53B40_PROGRESS.md` (the diagnosis + attempts).
- `notes/COMPILER_IDIOMS.md` §2 (register alloc), §4 (loop), §11 (the -d dumps).
- The target asm: `asm/rock_neo/nonmatchings/moji/func_80053B40.s`.
- The permuter base: `tools/decomp-permuter/mml_53B40/base.c` (a compiling near-match).

## Phase 0 — Verify & acquire (cheap, do first)

1. Confirm `bin/cc1-27` is **stock GCC 2.7.2** (version/strings). If Sony patched
   a pass, the public source may differ FOR THAT PASS — note it in PROGRESS.
2. Obtain GCC 2.7.2 source (GNU FTP: `ftp.gnu.org/gnu/gcc/`, `gcc-2.7.2.tar.gz`).
   Put it OUTSIDE the repo (it's large, not ours). Identify the four files:
   `loop.c`, `global.c`, `cse.c`, `jump.c`.
- **CHECKPOINT:** commit a PROGRESS note with the cc1 verification result + source
  location. If cc1 is NOT stock, STOP and report before Phase 1.

## Phase 1 — Pilot: 53B40's `move_movables` (the ROI gate)

1. Read the ONE function `move_movables` in `loop.c` + the movable-scoring /
   threshold logic it uses. Write down the EXACT rule: the score formula, the
   threshold, what makes a constant hoist vs stay inline.
2. Cross-check against the `-dL` (`.loop`) dump 53B40 produces — the compiler
   states its decision; the source explains the rule.
3. Determine: is there a C form that stops `0x1F800070`'s uses from CSE-merging
   into one movable (so `0x40000000` wins the register)? Or is it C-unreachable?
- **CHECKPOINT:** write the decision-rule findings to
  `notes/wip/GCC_SOURCE_PROGRESS.md`. If a C form is found, test it in-tree
  (clean rebuild + full-binary sha1 + mutation test) and, on match, follow the
  standard match ritual (below). If unreachable, document the required alternate
  lever and STOP the pilot — report ROI.

## Phase 2 — Generalize (ONLY if Phase 1 pays off)

Same read-the-rule treatment, one pass at a time, each tested against a parked
function in its genus:
- `global.c` — register-mirror ties (allocno priority; we already have the
  formula in COMPILER_IDIOMS §2 — source confirms/extends).
- `cse.c` — the value-merge wall.
- `jump.c` — branch layout / delay-slot.

## Phase 3 — Productize

The decision rules become a standalone **"GCC 2.7.2 codegen decision reference"**
(its own repo + book chapters — portable to ALL PSY-Q GCC decomps). Only after
Phases 1-2 prove value.

## CHECKPOINT / SAVE PROTOCOL (mandatory — read this)

This expedition WILL span multiple sessions. Save constantly:
1. **Maintain `notes/wip/GCC_SOURCE_PROGRESS.md`** as the running log: what was
   read, what rule was learned, what was tested, what's next. Update it at every
   checkpoint marked above — never leave findings only in chat.
2. **Commit after every checkpoint** (on `dev`; verify branch first). Small,
   frequent commits — a session can die at any time.
3. **On any hash-verified finding, update `notes/LESSONS.md`** (with the function
   name as provenance) AND `notes/COMPILER_IDIOMS.md` if it's a compiler property.
4. **On pausing/handing off, update `HANDOFF.md`** with the current state and the
   exact next step, and point it at `GCC_SOURCE_PROGRESS.md`.
5. If run as a background worker: return a SUMMARY (rule learned, match/park
   result, next step) — not raw source dumps.

## Standard match ritual (if Phase 1 produces a match)

`touch src/rock_neo/*.c && rm -f build/rock_neo.elf build/src/rock_neo/moji.c.o`
→ `make CPP=cpp check_rock_neo_only` (expect OK) → mutation test (break a
constant, hash MUST fail; restore, OK) → update progress.md/activity.md/LESSONS
→ commit on dev.
