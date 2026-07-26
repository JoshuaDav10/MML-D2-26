# MML Decomp — Whole-Game Strategy (2026-07-26)

Target (measured, `notes/WORK_MAP.md`): **~8,000 unique functions**, of which **281
done (~3.5%)**. This plan orders the work by **leverage per unit effort**, measured
with `tools/leverage_analysis.py`, not guessed.

## The leverage reality (measured — read this before believing any shortcut)

I hoped duplication would collapse the 8,000. It does NOT, much:
- 10,087 overlay copies → **7,046 UNIQUE** functions (1.4x). **5,905 appear exactly
  once.** De-dup saves you from doing identical work twice; it does NOT shrink the
  ~7,000 unique workload. Match-once-cover-many, ranked most-copied first: top 1,000
  matches cover 37% of *copies*, top 2,000 cover 49% — but each is still a distinct
  match. **The honest workload is ~7,000 unique functions. Plan for that.**

So leverage is NOT "duplication makes it small." Leverage is four narrower things,
each measured:

1. **A tiny, heavily-shared engine/library core.** Only **37 unique functions appear
   ≥10x** (one 342x, one 144x, one 72x…), covering 1,246 copies, and they are TINY
   (mostly 2–16 insn). Matching these 37 is cheap and their C bodies get reused most.
   They live in the `0x8010xxxx` overlay-engine region.
2. **The shared R3 engine core (~1,141 funcs, `0x8010xxxx`)** linked into every stage
   program. Decompile it ONCE → it satisfies all ~26 stages, and typing its structs
   propagates to all stage-specific code. This is the "paint the shared hallway" move.
3. **Library / PSY-Q SDK / libc functions have KNOWN PUBLISHED SOURCE.** The 37-core
   and many small leaves are almost certainly stock Sony SDK / libgcc / libc (GTE math,
   memcpy, div, printf). Fingerprint-match them against published source and other PSX
   decomps → near-instant, no permuter grind. Highest ROI on the board.
4. **Fast-win small leaves for momentum:** 141 (main exe) + **564 (overlay, unique,
   ≤25 insn, leaf)** ≈ ~700 quick, batchable, cheap-model matches.

Main-exe ∩ overlay overlap is thin (only 5 byte-identical unique funcs) — the main
executable and the stage programs are largely SEPARATE codebases. Don't expect the
281 done to give free overlay progress.

## CORRECTION — Phase 1 pressure-test (autonomous, 2026-07-26 PM), evidence over hope

I pressure-tested lever #3 ("top-shared functions are stock SDK with known source") by
reading the actual bodies. **It was wrong, and the truth is more useful:**
- The most-shared overlay funcs are NOT SDK: `func_8010E3B0` (342x) is a bare
  `jr $ra; nop` no-op stub; `func_80103A48` (144x) is a THUNK into `func_80031824`
  which lives in the MAIN EXE; `func_8010976C` (72x) is small shared game logic (a
  counter decrement). They are the shared R3 ENGINE, not an external library.
- **The real relationship between main exe and overlays is DEPENDENCY, not duplication.**
  Only 5 bodies are byte-identical, BUT **681 distinct main-exe functions are called
  (jal) from the overlays.** The main executable is the shared RUNTIME/API that every
  stage program calls into.

**Strategic consequence (raises the main exe's priority):** finishing/naming/typing the
1,119-function main exe is NOT "just one of several jobs" — it is the foundation all
~7,000 overlay functions call. Every main-exe function named + typed makes overlay code
readable for free. So: **do the main exe FIRST/most**, and target SDK-fingerprinting at
the main exe (where memcpy/GTE/PSY-Q SDK actually live), not the overlay stub tail.
Lever #3 below is downgraded to "unconfirmed; look in the main exe"; levers #1/#2/#4 stand.

## Phased plan (dependency-ordered; leverage front-loaded)

### Phase 0 — Infrastructure & visibility (mechanical; unlocks everything)
Nothing else is efficient until these exist. All are function-mapper/tooling work, no
matching judgment.
- **Split the 7 main-exe giant files** into per-function `.s` (WORK_MAP §3).
- **Splat config for the ~51 stage-program overlays** so their ~7,046 functions become
  per-function addressable + diffable + permuter-able.
- **De-dup manifest + match-propagation tool:** map each unique function → all its
  byte-identical copies, so ONE accepted match auto-applies to every copy across every
  stage. This is what makes leverage #1/#2 automatic instead of manual.
- **Whole-game census:** extend `census.py`/`audit_count.sh` to count against ~8,000,
  reporting all three denominators + by-volume. Never measure against 484 again.

### Phase 1 — Free & near-free wins (highest ROI)
- **Library/SDK fingerprinting.** Build/import a signature DB (other PSX PSY-Q decomps
  publish these). Auto-identify stock functions; drop in known C. Start with the 37
  heavily-shared core + the small leaves. Potentially hundreds of near-instant matches.
- **The 5 main∩overlay shared functions** — one C body satisfies both sides.

### Phase 2 — The shared R3 engine core (~1,141 funcs)
- Decompile the `0x8010xxxx` engine once. Type its structs (they propagate to all stage
  code). Turns ~26 stages from "opaque" into "engine + thin stage-specific layer."
- This is where the Fable **compiler-wall expedition** pays off repeatedly — engine
  functions are big and recur; cracking a genus (53B40 done) unblocks a class.

### Phase 3 — Fast-win momentum harvest (~700 small leaves)
- Batch on cheap models + permuter + the ratchet-protocol background workers. This is
  the visible-progress engine; run it continuously in parallel with Phase 2.

### Phase 4 — The long grind (the ~5,000 one-off stage functions)
- Medium/large per-stage logic, one at a time, standard workflow.
- Parallel Fable wall-cracking continues to unblock hard classes.

## Cross-cutting efficiency multipliers (apply throughout)
- **Model routing:** cheap models for batch/small; Fable only for walls, big functions,
  toolchain forensics (see memory `model-cost-preference`).
- **Background workers with the RATCHET protocol** (memory `bg-worker-ratchet`):
  pre-bank setup, micro-checkpoint commits, so usage-limit kills cost one step.
- **Struct/type/name propagation:** every typed struct + named function is executable
  knowledge that speeds every later user of it. Prioritize engine-core types.
- **Signature DB is shared across the whole game** — a function fingerprinted once is
  known in all 206 binaries.

## Honest payoff shape (do not oversell)
Front-loaded: Phase 0–1 + engine core + small leaves can plausibly move 3.5% → ~15–20%
relatively efficiently (library/dedup/small-leaf density is real). After that it is a
long grind on ~5,000 genuinely-unique one-off functions — the project is big and stays
big. The win of this plan is doing the cheap 20% cheaply and not mistaking it for done.

## Immediate next actions (Phase 0, in order)
1. Split `asm/rock_neo/*.s` (7 files) → per-function; add splat entries in addr order.
2. Prototype the de-dup manifest from `tools/leverage_analysis.py` groups.
3. Splat-config ONE stage program (e.g. ST00, 518 funcs) end-to-end as the template.
4. Extend census to the whole-game denominator.

## Reproduce
- Leverage numbers: `tools/leverage_analysis.py`
- Scope/de-dup: `tools/overlay_scope.py` ; main-exe sizes: `tools/analyze_raw_asm.py`
