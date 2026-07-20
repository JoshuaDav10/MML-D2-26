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
