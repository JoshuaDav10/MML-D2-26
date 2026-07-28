# Research findings — scaling the decomp (ceiling, m2c, overlays, SDK, permuter)

**Provenance:** deep-research report returned 2026-07-26 against
[`DEEP_RESEARCH_PROMPT_scaling.md`](DEEP_RESEARCH_PROMPT_scaling.md), answering the five
questions in [`RESEARCH_QUESTIONS.md`](RESEARCH_QUESTIONS.md).

> **STATUS — CANDIDATE, NOT HASH-GATED.** This is an LLM-authored web-research report.
> Repo names, file paths, flags and quotes below are **starting points to verify**, not
> facts. Nothing here enters [`LESSONS.md`](LESSONS.md) until a byte-for-byte
> `make CPP=cpp check_rock_neo_only` OK on a real MML function confirms it.
>
> The report **self-flagged one inference** (Q1) — recorded honestly below rather than
> laundered into a fact.

Pools re-measured from `build/function_map.json` at the time of writing (350 engine
matched, 769 engine unmatched). The report was written against slightly older figures.

---

## Q1 — Is the >120-instruction ceiling ours or the toolchain's?

### VERDICT: ours. Confirmed twice, from two independent directions.

**External evidence (the report).** FoxdieTeam/mgs_reversing states its Metal Gear Solid
main executables (`SLPM_862.47/.48/.49`) are **100% decompiled** and hash-verified against
retail, built with the PSY-Q SDK. A ~626 KB engine executable cannot be 100% matched
without matching many functions far above 120 instructions. Supporting: REDRIVER2
(Driver 2) completed including hand-done GTE code; Xeeynamo/sotn-decomp builds
byte-for-byte; FedericoMilesi/soul-re (PSY-Q/GCC mipsel) has large matched bodies. No
public PSX project documents a size above which matching is abandoned.

> ⚠️ **The report's own caveat, preserved:** it could **not** cite a single named function
> with a verified exact instruction count in the 300–800 band — GitHub blob pages resisted
> fetching. The conclusion rests on whole-binary 100% claims plus function-location
> evidence. Conclusive in aggregate; **not** a verified per-function datapoint.

**Local evidence (stronger, and ours).** Already settled by experiment before this report
arrived: `Sub_screen_gauge_set` (**137 insns**) and `Sub_screen_gauge_set2` (**111**) both
matched essentially on the first compile, from authentic Capcom source. 137 broke a
ceiling that had stood for the project's entire history.

**The two agree, and the local result is the more decisive one** — it is hash-gated, on
this binary, with this compiler. Treat the external report as corroboration, not as the
primary evidence.

### What it actually means

The wall was never a toolchain property. It was **effort-and-knowledge**: past ~120
instructions, reverse-engineering the exact source *shape* by hand stops converging. Given
the real source shape, size stopped mattering.

So the highest-leverage lever is **anything that supplies source shape** — leaked source,
m2c drafts, better compiler-internals knowledge — *not* more hand-iteration.

Current large-function inventory (engine, unmatched): **97** in 120–200, **63** in
200–300, **50** above 300. Engine target moves from ~908 toward **1,119**.

### Still genuinely open

Whether other projects match 300–800 instruction functions *without* original source, and
what workflow they use at that size. The report's answer to this is the inferred part.

---

## Q2 — Does m2c accelerate the small-function pool?

### VERDICT: install it, but treat output as a draft only.

- Invoke with **`--target mips-gcc-c`** (the older `--compiler gcc` form is deprecated);
  `-f <function>` limits to one function; `--structs` is now default.
- Needs a **context file** of typedefs/externs/prototypes for type inference. **We already
  have the harder half** — `tools/m2ctx.py` exists in-tree. SOTN maintains the equivalent
  (`M2CTX_APP`).
- **Why the output must be rewritten:** m2c targets a behaviourally-equivalent rebuild, not
  the source shape that reproduces the original register allocation. Its own README warns
  it "sometimes generates suboptimal code (especially for loops)".
- **Standard pipeline:** m2c draft → hand-fix to ~95% → decomp-permuter for the regalloc
  residue. Used this way by SOTN and MGS.

**Relevant pool now:** **248** unmatched engine functions ≤30 insns, **354** ≤50.

**Keep-it benchmark (from the report):** median time-to-compilable-draft on that pool drops
≥30%. **Demote-to-reference benchmark:** you find yourself discarding effectively all of
its expression-level output.

---

## Q3 — One function, byte-identical, in N binaries

### VERDICT: solved pattern; concrete tooling exists to copy.

- **Build pattern:** one shared `.c`, compiled **once per overlay**, each overlay linked
  with its own linker script and its own splat yaml.
- **Dedup tooling (SOTN, quoted from its Makefile):**
  `tools/dups` — `cargo run --release -- --threshold .90 --output-file ../gh-duplicates/duplicates.txt`;
  `mipsmatch`; `function_finder_psx.py --use-call-trees`.
- **MGS README warns about exactly our problem:** *"Since there is a lot of duplication in
  overlays, make sure that this function was not already decompiled in some other
  overlay."*
- **Progress metric:** count **unique bodies**. Our 10,107 stage instances overstate the
  work by ~1.4x against **7,064** unique. `gen_map.py` already reports both — keep it that
  way.
- No canonical splat "dedup overlay" feature exists; the SOTN trio is the de facto answer.

**Local status:** the stage pipeline is no longer theoretical — `eve19` is **12/12 matched**,
the first fully decompiled stage file. Next archive conversion should immediately reveal
how many of its functions are already-matched twins.

---

## Q4 — PSY-Q SDK and libgcc from published source?

### VERDICT: yes for obtaining and identifying. No public per-function hash database.

- **Convert the original libraries:** `psyq-obj-parser` (PCSX-Redux) parses Sony PSY-Q
  `.OBJ`/`.LIB` and emits modern-toolchain ELF `.o`; handles the whole official library set
  (only crt0 objects fail). decomp.me uses it in its backend. Alternative: `gitlab.com/jype/psyq2elf`.
- **Identify by signature:** `ghidra_psx_ldr` ships FLIRT-style PSY-Q signatures. The IDA
  route generates FLIRT `.pat`/`.sig` from SDK `.lib` via the SDK's `ppsx` tool.
- **Reference C:** `sozud/psy-q-decomp` is a WIP matching decomp of the SDK itself
  (targeting 3.5); `Xeeynamo/psyz` reuses it.
- **libgcc DImode helpers** (`__divdi3`, `__ashldi3`, `__muldi3`, `__cmpdi2`, …) come from
  FSF `libgcc2.c`, compiled per-symbol via `L_divdi3`-style defines.
  `decompals/mips-gcc-2.7.2` provides reference builds.
- **NO public per-function fingerprint hash table exists.** Use FLIRT-style signatures for
  bulk identification, not a hash lookup. This is a confident negative, not a gap in the
  search.
- **Version note:** per wiki.deco.mp, our exact `GCC 2.7.2.SN32.3.7 Build 0001` is the
  compiler "found starting from PSY-Q 4.0" — so target the PSY-Q 4.0-era library set.
- ⚠️ **SN-patched library variants may differ from stock FSF.** Match at our exact flags
  (`-mcpu=3000 -O2 -G8 -msoft-float -fpcc-struct-return -mgas -funsigned-char`) rather than
  assuming FSF sources reproduce SN codegen byte-for-byte.

**Benchmark:** if signature identification confirms >50 of the 614 unsplit engine functions
as SDK/libgcc, that materially shrinks the real reverse-engineering target.

---

## Q5 — Disciplined permuter workflow

### VERDICT: our local intuitions match official guidance. Policy, not tuning, is the gap.

- **Use the PSY-Q fork:** `FoxdieTeam/decomp-permuter-psyq` (or `twevs/…`).
- **Always `-j`.** `--stack-diffs` makes the scorer consider stack positions, but upstream
  warns the permuter "is currently quite bad at resolving stack differences" — so for stack
  mismatches, **hand iteration beats the permuter.**
- **When to reach for it (upstream README):** *"It's generally best towards the end, when
  mostly regalloc changes remain. If there are reorderings or functional changes, it's often
  easy to resolve those by hand."* Random mode is *"much more of a last ditch sort of
  thing… often finds nonsensical permutations that happen to match regalloc by accident."*
- **Score semantics — both of our traps are confirmed known behaviour.** Score 0 means the
  function matches *by the scorer's metric*; stack positions are ignored without
  `--stack-diffs`, and the scorer is alignment-based with jump-target normalisation. That is
  exactly why we saw a score-0 with a `j` to the wrong label, and a genuine workdir zero
  that failed to transfer in-tree because the scratch used stub types.
- **Score is not distance.** It is a penalty sum, not a monotone distance-to-match — which
  explains both refuted intuitions: a low score can be a local minimum (20 for 6,903
  iterations), and a high score can collapse instantly (100,430 → 260).
- **Our 27% (3/11) is a normal band for hard functions.** The permuter is a complement, not
  a solver.

**Policy to adopt:** invoke only when structure is right and registers/ordering are wrong;
always `-j`; **never accept a score 0 without an in-tree rebuild and full-binary SHA-1**;
abandon the permuter (not the function) when the residue is functional or stack-layout.

---

## Consequences for the plan

1. **Install m2c** (`--target mips-gcc-c`) wired to the existing `tools/m2ctx.py`; point it
   at the 354 functions ≤50 insns. Biggest near-term throughput lever.
2. **Port the SOTN dedup approach** before converting more stage archives, so the second
   archive reveals its already-matched twins instead of redoing them.
3. **Run a library sweep** — `psyq-obj-parser` + `ghidra_psx_ldr` signatures — to convert
   `wip/SDK_CANDIDATES.md` from heuristic guesses to confirmed identifications.
4. **Attack the large-function band deliberately.** Pick 3–5 in 120–200 (97 available) as a
   methods experiment. Note we have *already* matched 137 from source, so the question is
   specifically whether the workflow closes them **without** source.
5. **Adopt the permuter policy above** as a written rule, not intuition.
