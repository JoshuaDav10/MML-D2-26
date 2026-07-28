# Open research questions — raw material for a deep-research prompt

**Status: ANSWERED 2026-07-26 → [`RESEARCH_FINDINGS_scaling.md`](RESEARCH_FINDINGS_scaling.md)**
(CANDIDATE / NOT-HASH-GATED). Prompt: `DEEP_RESEARCH_PROMPT_scaling.md`.

Headlines: **Q1 the >120 ceiling is ours, not the toolchain's** (MGS's ~626 KB main exe is
100% matched on PSY-Q GCC 2.x — and we independently broke it ourselves at 137 insns).
**Q2 install m2c** as a draft generator, `--target mips-gcc-c`, output must be rewritten.
**Q3 solved pattern** — one shared `.c` compiled per overlay + SOTN's `dups`/`mipsmatch`
dedup tooling; count unique bodies. **Q4 PSY-Q libs are obtainable** via `psyq-obj-parser`
+ `ghidra_psx_ldr` FLIRT signatures; **no** public per-function hash DB. **Q5 permuter is a
late-stage regalloc tool only**; gate every score-0 through an in-tree SHA-1.

The questions below are kept as written, for the record of what was asked and why.

These remain the scoped questions, not answers. House style:
`notes/DEEP_RESEARCH_PROMPT_reload.md` — specific, names primary sources, and explicitly
asks for artifacts and prior art rather than tutorials.

Each question below states **why it matters in numbers**, so the eventual prompt can be
written against a decision rather than against curiosity. Ordered by expected value.

Context every prompt needs: byte-exact matching decompilation of **Mega Man Legends**
(PSX, US, `ROCK_NEO.EXE`), PSY-Q SDK, compiler reports `GNU C 2.7.2.SN32.3.7 Build 0001`
— an **SN Systems patched gcc 2.7.2**, not stock FSF. Flags:
`-mcpu=3000 -O2 -G8 -msoft-float -fpcc-struct-return -mgas -funsigned-char`.
Live figures: `notes/COUNTS.md`. Full inventory: `notes/FUNCTION_MAP.md`.

---

## 1. Is the >120-instruction wall ours, or everyone's?  ← highest value

**The observation.** The largest function this project has ever matched is
`MojiTaskExec` at **119 instructions**. Nothing above 120 has ever matched, across the
project's entire history. Meanwhile **211 of the 818 unmatched engine functions are over
120 instructions** — **113 over 200, 50 over 300**, largest 2,383 (`func_8004BDC8`).

> ⚠️ **Denominator fix (2026-07-26).** This line previously read "28 over 200, 11 over 300"
> — those count only the **204 C-mapped stubs**, not the 818 unmatched. Mixing two
> denominators inside one sentence is the exact failure this project keeps re-learning
> (see `notes/META_LESSONS.md`). Figures above are all-unmatched, measured from
> `build/function_map.json`.

**The question.** Do other gcc 2.7.2 / PSY-Q decompilation projects — Symphony of the
Night, Spyro, Crash Bandicoot, Ape Escape, Silent Hill, Metal Gear Solid, Legend of
Dragoon, Tomb Raider — **routinely** match 300–800 instruction functions? What techniques
do they use at that size that we are not using?

> ## ✅ PARTIALLY ANSWERED 2026-07-26 — BY EXPERIMENT, not by research
> `Sub_screen_gauge_set` (**137 insns**) and `Sub_screen_gauge_set2` (**111**) both
> matched on essentially the first compile, from authentic Capcom source. 137 breaks a
> ceiling that had stood for the project's entire history (previous max: MojiTaskExec,
> 119; nothing above 120 had ever matched).
> **So the wall is not a toolchain property.** It was an effort-and-knowledge ceiling: past ~120
> instructions, reverse-engineering the exact source shape by hand stops converging.
> Given the real source, size stopped mattering.
> Two consequences: (1) the 211 unmatched >120 functions are recoverable IN PRINCIPLE,
> so the engine target is nearer 1,119 than ~908; (2) the highest-leverage lever is
> anything that supplies source shape — the leak, m2c (question 2), or better
> compiler-internals tooling — not more hand-iteration.
> Still open: whether OTHER projects routinely match 300-800 insn functions *without*
> original source, and what workflow they use. That is the part worth researching.

**Why it matters.** This single answer moves the realistic engine target between **~908
and 1,119**:
- If they do → our ceiling is a *skill gap*, and 211 functions are recoverable.
- If they don't → it is a real property of the toolchain, and our projection stands.

**What a good answer contains:** named functions of that size in public repos with their
matching C, the workflow used (permuter? m2c draft? hand iteration? diff-driven?), and
whether large matches cluster in projects with better compiler-internals documentation.

---

## 2. Does m2c actually accelerate the small-function pool?

**The observation.** `matt-kempster/m2c` (formerly mips_to_c) turns MIPS into draft C for
hand-refinement. It is **not installed here**, though `tools/m2ctx.py` exists to generate
its context files, so someone previously intended to use it. High-output decomp projects
lean on it heavily.

**The question.** For 20–60 instruction functions, does an m2c draft meaningfully reduce
time-to-match versus reading the asm and writing C directly? Or is the draft usually
*worse* than starting fresh — because it optimises for behavioural equivalence, not for
the source shape that reproduces the original codegen?

**Why it matters.** The engine has **248 unmatched functions ≤30 instructions** and **377
≤50** (re-measured 2026-07-26; was 265/394 before the +17 batch landed). If m2c halves the
per-function time on that pool, it is the single biggest throughput lever available. If it
produces C that must be rewritten anyway, installing it is a distraction.

**What a good answer contains:** testimony from projects using the same compiler
generation, m2c flags/settings that matter for PSX gcc 2.7.2 specifically, and whether
teams use it for a first draft or only for understanding control flow.

---

## 3. How do other projects handle one function living in N binaries?

**The observation.** The 37 code-bearing stage archives hold **10,107 function instances
but only 7,064 unique bodies**. **1,218 distinct bodies appear more than once, covering
4,261 instances — about 3.5x each.** One body (`jr $ra; nop`) occurs 363 times game-wide.
Separately, a handful of engine functions have byte-identical twins inside stage files.

**The question.** How do comparable projects structure a function that must appear,
byte-identical, in many separate binaries? A shared source file compiled per-overlay? A
per-overlay `#include` of a common body? Symbol aliasing at link time? Something else?
What does the build system look like?

**Why it matters.** This decides whether the stage-file workload is **7,064 units of work
or closer to 1,800**. It is the difference between "not in this lifetime" and "large but
finite". It also determines whether the overlay C pipeline should be built once generically
or per-archive.

**Local context:** exactly **1 of 37** code archives has been converted to a C segment so
far (`ST1A`, yaml line `- [0x2C0E4, c, eve19]`, containing 12 `INCLUDE_ASM` stubs and no
real C). The mechanism works; it is simply unused.

---

## 4. Can PSY-Q SDK and libgcc functions be matched from published source?

**The observation.** `notes/wip/SDK_CANDIDATES.md` flags candidates by structural
heuristic — memset, memcpy, strcpy/strlen, GTE-heavy, libgcc-style shift/divide helpers,
small math leaves. **Zero are confirmed.** The note itself says false-positive rate is
moderate and manual confirmation is required.

**The question.** Are PSY-Q library sources, official disassemblies, or authoritative
reconstructions published anywhere? Same for the libgcc 2.7.2 MIPS helpers (`__divdi3`,
`__ashldi3`, soft-float routines) that SN would have shipped. Have other PSX projects
built a *fingerprint database* of SDK functions?

**Why it matters.** SDK and libgcc functions are matchable from **known source** rather
than reverse-engineered — a fundamentally cheaper class of work. If even 50 of the 614
unsplit engine functions are library code, that is a fast, low-risk block. It also stops
us burning hours reverse-engineering something whose source is a download away.

---

## 5. How do high-throughput projects actually run decomp-permuter?

**The observation.** Measured here: **3 wins in 11 fair trials = 27%**, not the "3 for 3"
survivorship impression an earlier session recorded. Two beliefs were refuted by our own
logs: *"a low score means you are close"* (one function sat at score 20 for 6,903
iterations with zero improvement, another started at 80 and zeroed) and *"it cannot
converge from a high score"* (one went 100,430 → 260 in ~60 seconds).

Also known locally: **a permuter score of 0 is not a byte match** — it normalises jump
targets, and one scratch scored 0 with a `j` to the wrong label. And **a genuine workdir
zero can fail to transfer in-tree** when the scratch used stub types instead of the real
struct.

**The question.** What settings, seeds, iteration counts, and workflow do projects with
high match throughput actually use? When do they reach for the permuter versus hand
iteration? Do they gate every permuter result through an in-tree byte comparison?

**Why it matters.** The permuter is the only tool we have that scales without human
attention. A 27% hit rate is worth improving, and knowing *when* to deploy it matters more
than the hit rate itself — currently the decision is intuition, not policy.

**Local docs:** `notes/PERMUTER_GUIDE.md` (the `--stack-diffs` requirement, the
`--no-ignore-branch-targets` flag and its required local `objdump.py` patch, the kill rule,
the concurrency cap).

---

## Related, already written

- `notes/DEEP_RESEARCH_PROMPT_reload.md` — the SN gcc 2.7.2 reload / live-range-split
  question. Written 2026-07-25, launched + **RETURNED 2026-07-26**. Findings triaged in
  `notes/RESEARCH_FINDINGS_reload.md`; raw report at
  `notes/research_raw/2026-07-26_reload_report.md`.
  **Headline, VERIFIED here from primary sources:** the reconstructed SN 2.7.2 tree
  (`decompals/old-gcc` `2.7.2-cdk`) leaves `reload.c`/`reload1.c`/`local-alloc.c`/
  `global.c`/`cse.c`/`loop.c`/`combine.c`/`flow.c` **bit-for-bit stock FSF** — only the
  MIPS backend header, driver, build system and version banner are patched. Reading FSF
  2.7.2 source is reading our compiler's middle end.
  This partially answers question 1 below: same-generation projects (sotn, lom, spyro, esa)
  **do** carry permanent NONMATCHING functions for irreducible reload episodes — so some
  ceiling is real. What it does *not* answer is whether they routinely match 300-800
  instruction functions; that stays open and is still the highest-value question.
