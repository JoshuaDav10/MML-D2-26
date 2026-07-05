# Delegation Queue — parallel workstreams for Cursor / weaker agents

Written 2026-07-05 (Fable session). These are tasks that can proceed in
parallel with the main-thread matching decomp because they pass the
delegability test: **mechanical work with a binary pass/fail oracle, no
matching-decomp judgment required.**

## Rules common to EVERY brief below

- Branch per task off `dev`, named `infra/<slug>` (the ST1A fix stays on
  `overlay-expedition`). Push to origin. NEVER commit to `main` or directly
  to `dev` — the main thread audits and merges each task after independent
  verification (from-scratch rebuild + mutation-style tests, same ritual
  used to audit the overlay expedition).
- DO NOT TOUCH: `src/rock_neo/`, `include/rock_neo/`, `notes/LESSONS.md`,
  `asm/rock_neo/`, `progress.md`, `activity.md`. These are the
  matching-judgment core; a well-meaning "cleanup" there can silently break
  byte-matches.
- `source .venv/bin/activate` before any build. Every make invocation needs
  `CPP=cpp`. `make clean` is FORBIDDEN (deletes asm/ + assets/); use
  `rm -rf build` only.
- Reports must include the literal command outputs of the verification
  protocol. A report without them is not a completed task.
- Append a sprint entry to `notes/OVERLAY_EXPEDITION_LESSONS.md`? No — that
  file is overlay-only. Each brief below names its own report location.

Priority order: Brief 1 > Brief 2 > Brief 4 > Brief 5 > Brief 3.
(Brief 1 is a durability risk; the others are accelerators.)

**Status (2026-07-05):** Brief 2 **DONE** (`overlay-expedition`, `cfd738a`).
Brief 1 **PARTIAL** on `infra/submodule-patches` (`d952c39+`) — insurance
patches + local `mml-local` submodule commits done; **blocked on `gh auth`**
for fork/push + `.gitmodules` pin. See `notes/patches/README.md`.

---

## Brief 1 — Persist the uncommitted submodule patches (HIGHEST PRIORITY)

> **2026-07-05 update:** Insurance patches committed on `infra/submodule-patches`.
> Both submodules have clean `mml-local` branches (`maspx` `7511db8`,
> `asm-differ` `11e6d8c`). Fork/push + `.gitmodules` pin **blocked** — run
> `gh auth refresh -h github.com` then finish per `notes/patches/README.md`.

### Problem

Two load-bearing toolchain fixes exist ONLY as uncommitted working-tree
diffs inside git submodules. They are one `git submodule update --checkout`
away from destruction, and a fresh clone of the repo cannot reproduce the
matching build without them:

1. `tools/maspx` — `maspsx/__init__.py`, ~47 inserted lines (uncommitted).
   Contains the `ASPSX_REGFIRST_SYM_PREFIXES` reg-first `addu $at` ordering
   table AND the bare-constant-address load-delay-nop fix. Byte-matches of
   already-matched functions DEPEND on these.
2. `tools/asm-differ` — `diff.py`, ~4 changed lines (uncommitted). Python
   3.14 fix (`ast.Num` → `ast.Constant` in `eval_()`, ~line 1012). Without
   it `./diff.py` fails with "Failed to parse --base-shift".

`git -C tools/maspx diff` and `git -C tools/asm-differ diff` show the exact
patches. SETUP.md "Stage 1 fixes" describes the asm-differ one.

### Task

1. **Before anything else**: save both diffs to `notes/patches/` as
   `maspx-local.patch` and `asm-differ-local.patch` (insurance; commit these
   to the task branch first).
2. Fork both upstream repos to the user's GitHub account (`gh repo fork` —
   gh auth may be limited; if fork creation fails, STOP and report, do not
   improvise with vendoring).
3. In each submodule: create branch `mml-local`, commit the existing diff
   VERBATIM (no reformatting, no "improvements", no upstream rebase), push
   to the fork.
4. Update `.gitmodules` URLs to the forks, pin the submodules to the new
   commits, `git submodule sync`, commit in the superproject.

### Hard rules

- The committed patch content must be byte-identical to the current
  working-tree diff. Do not update the submodules to newer upstream
  revisions "while at it" — same base commit, plus the patch, nothing else.
- Do not run `git submodule update --checkout` before step 1+3 are done
  (it wipes the uncommitted patches — the exact disaster this task
  prevents).

### Verification protocol (all required)

1. `git -C tools/maspx status` and `git -C tools/asm-differ status` → clean
   (patches now committed, nothing uncommitted).
2. Destruction rehearsal: `git submodule update --init --checkout` then
   `git -C tools/maspx log --oneline -1` shows the mml-local commit and the
   patched lines are present (`grep ASPSX_REGFIRST_SYM_PREFIXES
   tools/maspx/maspsx/__init__.py`; `grep -n 'ast.Constant'
   tools/asm-differ/diff.py`).
3. Full clean build: `rm -rf build && make CPP=cpp`, then
   `make CPP=cpp check_rock_neo_only` → OK and
   `make CPP=cpp check_overlays` → `205/205 overlays OK`.
4. `./diff.py Sound_call` (any matched function) runs without the
   "--base-shift" parse error.
5. Fresh-checkout rehearsal: clone the superproject into a temp dir,
   `git submodule update --init`, copy `disks/` from the working repo
   (copyrighted, not in git), run extraction + build per SETUP.md, both
   checks pass. If extraction setup makes this impractical, say so
   explicitly and rely on 1–4 — do not fake it.

Report: `notes/patches/README.md` with fork URLs, pinned SHAs, and the
verification outputs.

---

## Brief 2 — ST1A silent link failure + swallowed overlay errors

**DONE** (2026-07-05, `overlay-expedition` `cfd738a`). See
`notes/OVERLAY_EXPEDITION_LESSONS.md` sprint "ST1A silent link failure fix".

Already fully briefed in **`notes/OVERLAY_ST1A_FIX.md`** (on both dev and
overlay-expedition as of f1be8f3). Work it on `overlay-expedition`. Summary:
feed the curated `syms.us.<chunk>.txt` to ld in `tools/buildoverlay.py`;
check every `os.system` return code; make the Makefile `chunks` target
propagate failures. Four-part verification protocol in the brief, including
an ST1A mutation test and a failure-propagation test.

---

## Brief 3 — m2c draft factory for the 100–199-insn tier

### Problem

The medium-function strategy (HANDOFF.md "Strategy for the medium/large
phase") calls for m2c (github.com/matt-kempster/m2c) rough drafts on
functions >100 insns. `tools/m2ctx.py` already exists to generate m2c
context. Nobody has installed m2c or produced the drafts.

### Task

1. Install m2c into `tools/m2c/` (git clone, pin a commit, note it). Its
   deps go into the existing `.venv` — record exact versions appended to
   `requirements.txt` ONLY if they don't conflict with existing pins
   (rabbitizer, spimdisasm etc. must not be upgraded; if pip wants to
   upgrade anything already pinned, STOP and report).
2. Generate the target list: every active stub whose asm file is 100–199
   instruction lines (use the cpp-based census — a naive grep of
   INCLUDE_ASM overcounts ifdef'd-out lines; see HANDOFF "Where to pick up
   next" item 1 for the method).
3. For each target, run m2ctx + m2c and write the raw output to
   `drafts/<tu>/<func>.c` with a header comment: function name, asm line
   count, m2c version/commit, and the literal text
   `UNTRUSTED DRAFT — never copy into src/ without byte-match iteration`.
4. `drafts/README.md`: how drafts were produced, how to regenerate one,
   and a table of function → draft status (ok / m2c-errored / context-missing).

### Hard rules

- NOTHING outside `drafts/`, `tools/m2c/`, and (maybe) `requirements.txt`
  changes. src/ is untouchable. Drafts are inputs to the main thread's
  judgment, not progress claims — no "matched" language anywhere.
- m2c failures on individual functions are EXPECTED (weird control flow,
  missing types). Record them in the README table; do not hand-edit drafts
  to make them "work".

### Verification protocol

1. `git status` shows changes only in the allowed paths.
2. `rm -rf build && make CPP=cpp && make CPP=cpp check_rock_neo_only` → OK
   (proves the environment/venv wasn't disturbed).
3. Spot-check: paste the draft for one named function and its asm line
   count; counts in the README table sum to the census count for the tier.

---

## Brief 4 — Formalize the scratch-TU tooling into tools/

### Problem

The main thread's matching workflow depends on two scripts that live only
in a session-scoped scratchpad and are recreated by hand every session
(HANDOFF "Where to pick up next" item 3):

- `tryfn.sh` — pipe one C file through the REAL pipeline
  (cpp | cc1 | maspsx | gprel.py | patchasm.py | as) to a .o and objdump it.
- `bytecmp.sh` — assemble a draft, extract the function's instruction words
  from objdump, byte-compare against the words in the splat .s comments
  (the `/* addr vram word */` columns), flagging reloc'd words for
  eyeballing instead of hard-failing on them.

Plus a third that doesn't exist yet:

- `census.py` — the accurate active-stub census: run each src/rock_neo/*.c
  through cpp with the build's exact flags (copy them from Makefile lines
  20–21), extract the `.include`d nonmatchings paths, wc -l the .s files,
  print sorted. (A naive grep of INCLUDE_ASM overcounts ifdef'd-out lines.)

### Task

Write `tools/tryfn.sh`, `tools/bytecmp.sh`, `tools/census.py` + a short
`tools/SCRATCH_TOOLS.md`. Derive the pipeline command from Makefile line
~266 — read it fresh, don't trust this brief's memory of it. Scripts must
run from repo root with the venv active and fail loudly (set -euo pipefail)
— a silent partial pipeline is exactly the failure mode this project keeps
getting burned by.

### Hard rules

- Read-only with respect to the tree: the scripts write ONLY under
  build/scratch/ (create it). They never touch src/, asm/, build/*.o used
  by make.
- Do not "improve" gprel.py/patchasm.py/maspsx while wiring them up.

### Verification protocol

1. `tools/census.py` output: total active stub count must equal
   475 − (matched count in progress.md); include the head of the sorted
   list in the report.
2. `bytecmp.sh` positive control: run it on the C body of an
   already-matched function (extract the single function into a scratch TU
   with the same includes) vs its original asm → 0 hard mismatches.
3. `bytecmp.sh` negative control: same draft vs a DIFFERENT function's asm
   → nonzero mismatches reported, nonzero exit.
4. `rm -rf build && make CPP=cpp && make CPP=cpp check_rock_neo_only` → OK
   (tree untouched by the tooling).

---

## Brief 5 — Pre-push verification hook

### Problem

The project has twice had green hashes on stale bytes. GitHub CI is off the
table (the build requires the copyrighted `disks/` contents, which are not
in the repo). A local pre-push hook is the next best structural guard.

### Task

1. `tools/git-hooks/pre-push`: runs, in order,
   `make CPP=cpp check_rock_neo_only` and `make CPP=cpp check_overlays`;
   nonzero → abort the push with a clear message naming the failed gate.
   It must activate the venv itself (hooks don't inherit the shell rc) and
   must NOT do a clean rebuild (too slow for a hook — it gates on the
   current build state; the clean-rebuild ritual stays a human/main-thread
   step, say so in the abort message).
2. `make install_hooks` target that symlinks it into `.git/hooks/`.
3. One paragraph in SETUP.md.

### Verification protocol

1. Install; `git push --dry-run` equivalent test: make a throwaway commit
   on the task branch, push → succeeds with both gates printing OK.
2. Sabotage test: `rm -f build/rock_neo.exe`, attempt push → hook must
   ABORT (and the message must mention which gate failed). Rebuild
   (`make CPP=cpp build_rock_neo_only`), push → succeeds.
3. Confirm `git push --no-verify` still works (documented escape hatch).

---

## Brief 6 — Callgraph + data-xref extraction (NEW 2026-07-05)

### Problem

Picking matching targets by FAMILY (shared callees, shared globals) beats
picking by size, and struct fields firm up fastest when every function
touching a symbol is known. Today that knowledge is rebuilt ad hoc by
grepping asm per question.

### Task

Write `tools/xref.py` (stdlib only) that scans every
`asm/rock_neo/nonmatchings/**/*.s` AND the matched C in `src/rock_neo/`
(via the extracted asm comments only — do NOT parse C) and emits
`notes/XREF.md` with three sections:

1. **Callgraph**: per function, the `jal` targets (and `j` tail-calls to
   other glabels); plus a reverse index (callee -> callers).
2. **Data xrefs**: per data symbol (`D_*`, named globals), the functions
   referencing it, with the access widths seen (lb/lbu/lh/lhu/lw/sb/sh/sw)
   and addressing mode (gp vs lui/at) per site.
3. **Family suggestions**: groups of 3+ unmatched functions sharing >=2
   callees or >=2 data symbols, sorted by group size ascending (small
   families first).

Regeneration must be one command (`python3 tools/xref.py`) and
deterministic (stable sort orders — the file will be diffed in git).

### Hard rules

- Read-only with respect to everything except `notes/XREF.md`.
- No new dependencies; no editing src/ or include/.
- Work on a branch off dev named `infra/xref`; commit there; do not merge.

### Verification protocol

1. Spot-check 3 functions by hand (`grep jal asm/.../<fn>.s`) against the
   callgraph section — counts and targets must agree exactly.
2. Spot-check one gp-accessed symbol (e.g. Moji_flag3) and one lui-accessed
   symbol (e.g. D_800988E8) — modes must be reported correctly.
3. Run twice; `git diff` between runs must be empty.

---

## Brief 7 — Matched-count tooling + total reconciliation (NEW 2026-07-05)

### Problem

progress.md's totals drift: the historical figure says 475 mapped
functions, but `tools/census.py` reports 263 active stubs while 221 are
matched (sum 484). The counting method exists in prose
(notes/LESSONS.md "Counting matched functions") but is not scripted, so
every session recounts differently.

### Task

1. Add `--matched` mode to `tools/census.py`: for each TU, count
   `F .text` symbols in `build/src/rock_neo/<tu>.c.o` (objdump -t), count
   active INCLUDE_ASM stubs (cpp-preprocessed, as the census already
   does), and report per-TU matched = functions-in-object minus stubs,
   plus totals. Requires a built tree; error out clearly if build/ is
   missing.
2. Reconcile the 475-vs-484 discrepancy: identify exactly which functions
   are counted by one method and not the other (e.g. upstream game.c
   functions, functions with no stub file, duplicated names) and write
   the findings into a short `notes/COUNTS.md` with the authoritative
   method stated.
3. Update the two total lines at the top of progress.md ONLY if the
   reconciliation proves them wrong, in a dedicated commit explaining the
   correction.

### Hard rules

- Do not change how check_rock_neo_only works; this is bookkeeping only.
- No matching claims: this tool counts, it does not verify. The hash
  remains the only match authority.
- Branch off dev named `infra/counts`; commit there; do not merge.

### Verification protocol

1. `tools/census.py --matched` total must equal the hand method from
   LESSONS.md on two TUs picked at random (show the arithmetic).
2. The reconciliation table in notes/COUNTS.md must sum exactly:
   matched + active stubs = authoritative total, no unexplained rows.

---

## What is NOT in this queue (do not pick up)

- Anything in `src/rock_neo/*.c` or `include/rock_neo/` — matching work.
- Overlay CODE decompilation (ST* progbin chunks) — premature until Brief 2
  lands and main-exe structs mature.
- `notes/LESSONS.md` edits — main-thread only.
- Upgrading toolchain components (cc1-27, GAS, spimdisasm, splat) — frozen.
