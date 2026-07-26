# Using decomp-permuter on this project

The permuter is a **last-mile finisher**: given C that already *compiles to a
near-match*, it randomly applies thousands of semantics-preserving mutations
(reorder statements, swap equivalent expressions, add/remove temps, retype
locals) and keeps whatever scores closer to the target bytes. It is CPU, not
judgment. It **cannot** write initial C, understand a bare stub, or fix a wall
that no C shape reaches (see "What it can and can't do").

Everything below is verified against this repo's setup (`tools/decomp-permuter/`).

## One-time setup (already done, recorded here so it's reproducible)

- `pip install toml` in the venv — the permuter imports `toml`; without it it
  won't even start (`ModuleNotFoundError: No module named 'toml'`).
- `tools/patchasm.py` guards its `build/test.s` debug dump in a `try/except` so
  the pipeline doesn't crash when run from a permuter workdir (where `build/`
  doesn't exist relative to cwd). Without this every permuter compile fails with
  `FileNotFoundError: build/test.s`.
- The permuter clone is `.gitignore`d and carries its own local patch: in
  `src/objdump.py` (~line 211) `mipsel-elf-objdump` is the first entry of the mips
  arch executable list.

## Running it

```bash
source .venv/bin/activate
cd tools/decomp-permuter
python3 permuter.py <workdir> -j<threads> [flags]
```

Example (the existing register-alloc wall):

```bash
python3 permuter.py mml_53B40 -j4 --stop-on-zero
```

Useful flags:
- `-j<N>` — parallel threads (use most of your cores).
- `--stop-on-zero` — halt the moment a perfect match (score 0) is found.
- `--best-only` — only save candidates that beat the best score so far (less
  disk churn than the default, which saves every improvement).
- `--speed 1-100` — trade thoroughness for iteration rate.
- multiple workdirs can be passed at once: `python3 permuter.py dirA dirB`.

## ALWAYS pass `--no-ignore-branch-targets` too (2026-07-25 audit)

`ign_branch_targets` **defaults to True** (`src/main.py:777`), so the scorer rewrites every
branch/jump target to a literal placeholder. That is exactly how func_8001AE6C produced a
**false score 0** (a `j` to the wrong label scored as perfect). The prophylactic is free and
was never applied after that incident — apply it now:
`--stack-diffs --no-ignore-branch-targets`.

**LOCAL PATCH REQUIRED** (the clone is .gitignore'd, so re-apply after any re-clone):
`src/objdump.py` ~line 358 crashes on the strict path with
`ValueError: invalid literal for int() with base 0: 'a4'` — objdump emits some immediates
as bare hex. Wrap it:
```python
try:    _immv = int(imm, 0)
except ValueError: _immv = int(imm, 16)
repl += "+" + imm if _immv > 0 else imm
```
Verified 2026-07-25: with the patch, `mml_12350` runs strict cleanly (base score 30).

## KILL RULE + concurrency (2026-07-25 audit, measured)

**Every zero this project has ever produced arrived in <= 14 minutes** (51s, 5m30s,
14m02s; the false zero at 46s) — an upper bound of roughly 20k iterations. Ten workdirs
have since exceeded that with zero hits, one to 154,511 iterations.
- **Kill any run past ~20k iterations / ~20 minutes.** No win has ever come later.
- **Do NOT oversubscribe cores.** A 2026-07-25 snapshot had 25 worker threads on 12 cores
  (2x), which slows every run — including the first-20-minute window that is the ONLY
  regime where wins occur. Concurrency was actively counterproductive.
- Honest hit rate: **3 wins / 11 fair trials (27%)**, not the "3 for 3" claimed elsewhere.
  `mml_12350` is a labelled "pure register mirror" that has NOT converged, so even the
  register-mirror lane is 3-for-4, on n=4.

## "Low score means close" is REFUTED (2026-07-25 audit)

Score is NOT a distance. Counter-evidence from this repo:
- `mml_DDE4` at base **20 (2 rows)** — the tightest residual in the set — produced zero
  new-bests in 6,903 iterations. `mml_E4C4` started at **80** and zeroed.
- `mml_F828` sat at score **10 (ONE row)** for 69 minutes / ~4.6 CPU-hours without closing.
- A row-count metric has no partial credit: one early register mirror shifts every later
  row, so it overstates trivial mirrors and understates unreachable residuals.
Treat the score as "is it moving", never as "how far".

## ALWAYS pass `--stack-diffs` (2026-07-25 — we ran for weeks without it)

**By default the permuter's score IGNORES stack positions.** If your residual involves
frame size or stack-slot offsets — extremely common in this project — the score is blind
to the very thing you are trying to fix, and the search optimizes the wrong objective.

Worse: `src/randomizer.py:2254` contains a mutation whose docstring reads *"Inserts an
unused variable to adjust stack offsets. Probably only useful with --stack-diffs
enabled."* That mutation is precisely the `(void)&local` stack-reservation idiom that
solved 53B40's reserved-but-untouched spill slots by hand (LESSONS / tooth 16) — and
without the flag it is effectively dead weight.

Every 53B40 run before 2026-07-25 was launched WITHOUT `--stack-diffs`, which likely
explains a good share of why they stalled. `tools/permuter53b40.sh` now passes it.

Rule of thumb: pass `--stack-diffs` unless you have a specific reason not to. The score
scale changes (numbers get much larger), so do not compare scores across runs with and
without the flag — they are different metrics ("two rulers", see LESSONS).

Caveat from upstream docs: the permuter is "quite bad at resolving stack differences"
even with the flag on — so treat it as *scoring correctly*, not as *likely to fix it*.

## Reading the output

```
[func_80053B40] base score = 11395
iteration 1, 0 errors, score = 11395
...
[func_80053B40] found new best score! (10955 vs 11395)
wrote to mml_53B40/output-10955-1
```

- **score** = how far the candidate is from the target. **Lower is better; 0 is a
  perfect byte match.** (It's a weighted instruction-diff, not an instruction
  count — a score in the thousands can still be "a few registers off.")
- **base score** = the score of your `base.c` as-is.
- Every improvement is saved to `<workdir>/output-<score>-<n>/` containing the
  mutated `source.c`. **Score-0 output = your match** — copy that `source.c`'s
  function body back into the real tree and verify with
  `make CPP=cpp check_rock_neo_only` + the mutation test (never trust the
  permuter's score alone; gate on the real hash).
- `N errors` climbing = your `compile.sh` or `base.c` is broken; fix that first.

## Making a new target workdir

A workdir needs exactly three files (use `mml_53B40/` as the working template):

1. **`compile.sh`** — the pipeline wrapper. Copy `mml_53B40/compile.sh`
   verbatim; it feeds already-preprocessed C to `cc1-27 | maspsx | gprel |
   patchasm | as` (it skips `cpp` because the permuter regenerates preprocessed
   C itself). It is target-independent — the same script works for any function.
2. **`base.c`** — a **compiling near-match**: the preprocessed C for the function
   (with the right includes inlined) that builds and scores close but not 0. This
   is the part that requires you to have already reverse-engineered the function
   to a near-match by hand. No near-match, nothing to permute.
3. **`target.o`** — the original function's object (the bytes you're matching),
   i.e. the function's extracted `nonmatchings/*.s` assembled with the same
   `mipsel-elf-as` flags the build uses.

The `import.py` helper can scaffold this, but the custom pipeline makes the
manual copy-the-template route more reliable. Ask the main thread to wire up a
new target — it's fiddly and easy to get `target.o` subtly wrong.

## What it can and can't do — READ THIS BEFORE PICKING A TARGET

The permuter only searches the space of **semantics-preserving C rewrites**. So:

**Good targets (it can win):**
- Callee-saved register **mirrors** (two regs swapped) that resisted hand decl/
  init-order flips.
- **Instruction-scheduling** swaps, delay-slot fills, near-miss reorderings.
- Small functions (≤~40 insns) at a near-match — small search space, fast wins.

**Bad targets (it will grind forever and never hit 0):**
- **Value-based CSE walls** — e.g. `func_80053B40`, where the blocker is that N
  identical constant-address uses merge into one movable. *No* semantics-
  preserving mutation produces the fix, so the permuter cannot reach it. Proof:
  its base score is ~11395, its all-time best over **2820 saved candidates** is
  ~8535 — it crawled a little and stalled nowhere near 0. **Do not spend CPU on
  53B40.** It needs gcc-source study, not the permuter (see
  `notes/COMPILER_IDIOMS.md` §12 and the Book I "parked genera" chapter).
- **Jump-canonicalization / cross-block delay-slot** residue and
  **held-base-across-call** — also largely out of the permuter's reach.

**Rule of thumb:** point the permuter at a *fresh* near-match whose only problem
is register allocation or scheduling — usually one that just came off the
momentum lane and won't fall to a couple of hand knobs. Don't resurrect the known
parked genera; those are why they're parked.

## GOTCHA (2026-07-19): compile.sh must cd to the repo root

`tools/gprel.py` decides gp-vs-lui by a census of `asm/rock_neo/**/*.s`
**relative to CWD**. The permuter invokes `compile.sh` from its own workdir, so
the census came up empty and every small extern (incl. Moji_flag, D_80098B2C,
D_80098960) was left non-gp — inflating every candidate's score with phantom
lui/lw pairs (the stalled-at-9745 run suffered this on top of the lever-less
base). Fix (applied to mml_53B40/compile.sh, gitignored — reapply for new
workdirs): `realpath` the INPUT/OUTPUT args, then `cd $ROOT` before the
pipeline. With the tooth-6/7 base this dropped the 53B40 base score to 2595.

## LOCAL PATCH (2026-07-20): scorer length-change penalties raised

`src/scorer.py` (local, uncommittable — the permuter clone is gitignored;
REAPPLY after any re-clone): `final_score += abs(len(cand_seq) -
len(self.target_seq)) * 100000` appended at the end of Scorer.score().
Rationale: a byte match requires exact instruction-count parity, but the stock
scorer lets a deletion (100) pay for itself by "fixing" >20 regalloc diffs
(5 each) — near the endgame the search migrates into 494-word states that can
never reach 0 (observed repeatedly on 53B40: weighted 770 at 314 positional
mismatches vs the honest base's 193). First attempt raised
PENALTY_INSERTION/DELETION to 10000 — WRONG: the differ also classifies
same-length MOVED instructions as insert+delete pairs, so the base itself sat
on 60k of pair penalties and length-dropping still scored down. The length-
delta fence penalizes exactly the unlandable states and nothing else.
The positional gate (bytecmp words+hard count) remains the authoritative
accept/reject check for harvested candidates.


## Launching (2026-07-20): use tools/permuter53b40.sh, NEVER launch by hand

A hand-launch without the venv dies instantly on `import toml` while stale
forkserver processes keep `pgrep permuter` alive — this masked a dead permuter
for AN HOUR. The launcher kills stale runs (pidfile + pattern), proves the venv
imports, launches, writes `mml_53B40/permuter.pid`, and BLOCKS until
"base score" appears in the log (fails loudly otherwise). Watchdogs must check
(1) the pidfile PID via `kill -0` and (2) LOG GROWTH between checks — never
process-name greps. Adapt the script's workdir line for other targets.

## LOCAL PATCH v2 (2026-07-20): POSITIONAL scorer for the 53B40 endgame

The length-fence alone still let the annealer trade positional mismatches for
alignment credit (fenced run kept finding 495-word candidates that were 7-40
rows WORSE positionally). scorer.py now short-circuits `Scorer.score()` with:
same-index row comparison ×10 + length-delta ×100000. "New best" now equals
the authoritative gate metric. CAVEATS: (1) local, uncommittable — reapply
after re-clone; remove the `if True:` block to restore stock scoring for
NORMAL (non-endgame) targets — stock's partial credit is a better gradient
when the candidate is still far away; (2) scratch-env reloc rows that can
never match may put the reachable floor slightly above 0 — when the score
plateaus low, bytecmp the candidate (exit 0 = the real match) rather than
waiting for --stop-on-zero.

## FULL-TU objective (2026-07-20, the decisive setup for 53B40's endgame)

The single-function scratch TU and the real moji.c TU compile DIFFERENTLY
(register pressure, pseudo numbering, even whether a mutation folds: the
op+1<0x85 device saved a word standalone and COST one in-tree; the fossil's
0x50 frame appears FOR FREE in-tree). Measured: 75-hard scratch == 77-hard
in-tree with a DIFFERENT slot profile. Fix: base.c = `cpp` of the REAL
src/rock_neo/moji.c with the candidate spliced over its INCLUDE_ASM line
(strip # lines and blanks), plus a local objdump.py patch that slices the
candidate disassembly to one function via the PERMUTER_ONLY_FUNC env var
(exported by tools/permuter53b40.sh). target.o unchanged. Iterations are
~3x slower but the score is now the number that ships. Verify any adopted
candidate with tools/land53b40.sh (splice+build+hash dry-run) — cmp -l of
the linked exe is the FINAL word (77 words counted that way today).
