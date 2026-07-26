# Deep-research prompt — scaling the MML decomp (the five throughput questions)

Paste the block below into deep research mode. Written 2026-07-26, from
[`RESEARCH_QUESTIONS.md`](RESEARCH_QUESTIONS.md). House style follows
[`DEEP_RESEARCH_PROMPT_reload.md`](DEEP_RESEARCH_PROMPT_reload.md): specific, names primary
sources, demands artifacts and prior art rather than tutorials, and treats a confident
"this does not exist" as a valuable answer.

**All figures below were measured from `build/function_map.json` on 2026-07-26**, not
copied from prose. (`RESEARCH_QUESTIONS.md` §1 mixed denominators — its "28 over 200 / 11
over 300" counted only the 204 C-mapped stubs, while its "211 over 120" counted all 818
unmatched engine functions. The block below uses the all-unmatched basis throughout.)

When the report returns, file it as `notes/RESEARCH_FINDINGS_scaling.md` with the same
**CANDIDATE, NOT HASH-GATED** banner used by
[`RESEARCH_FINDINGS_gcc272_idioms.md`](RESEARCH_FINDINGS_gcc272_idioms.md). Nothing from a
research report enters [`LESSONS.md`](LESSONS.md) until a real function matches on the hash.

---

I am running a **byte-exact matching decompilation** of a PlayStation 1 game — Mega Man
Legends (US, `ROCK_NEO.EXE`) — and I need to make a set of **throughput decisions**. I am
not looking for tutorials or explanations of how decompilation works; I understand the
theory and have a working pipeline. I need **artifacts, links, named repositories, named
functions, and prior art**, plus honest nulls where nothing exists.

**My toolchain, exactly.** PSY-Q SDK. `cc1` reports `GNU C 2.7.2.SN32.3.7 Build 0001` — an
**SN Systems patched gcc 2.7.2**, not stock FSF. Flags: `-mcpu=3000 -O2 -G8 -msoft-float
-fpcc-struct-return -mgas -funsigned-char`. Assembly via a maspsx-style pipeline, verified
by full-binary SHA-1 against the retail executable. I also build 205 CD overlay binaries
byte-identically.

**My scale, measured today.** The main executable holds **1,119** functions, of which
**301 are matched** (26.9%). The 37 code-bearing stage archives hold **10,107 function
instances** but only **~7,064 unique bodies**; whole-game denominator ~**8,183**, so I am
at ~**3.7%**. Of the 818 unmatched main-executable functions: **211 exceed 120
instructions**, **113 exceed 200**, **50 exceed 300**, and the largest is **2,383**. At the
small end: **76 are ≤10 instructions, 173 ≤20, 248 ≤30, 377 ≤50**.

Please answer these five questions.

---

**QUESTION 1 — Is a ~120-instruction matching ceiling normal, or is it a skill gap?
(highest value)**

The largest function this project has *ever* matched is 119 instructions (a script-VM task
initializer). **Zero matched functions exceed 120 instructions**, across the project's
entire history, while 211 unmatched ones do.

I want to know whether other **gcc 2.7.x / PSY-Q PSX** decompilation projects *routinely*
match functions in the 300–800 instruction range. Please look at concrete repositories and
count, if you can: **Castlevania: Symphony of the Night** (`Xeeynamo/sotn-decomp`), Spyro,
Crash Bandicoot, Ape Escape, Silent Hill, Metal Gear Solid, Legend of Dragoon, Tomb Raider,
Klonoa, and anything else on the PSY-Q 2.7.x generation. Also search **decomp.me** for
large *matched* scratches on the `ps1` / `psyq` presets.

Specifically:
- Name **actual functions ≥300 instructions that are matched** in public repos, with links
  to both the C and the target asm if possible.
- What is the **size distribution of matched functions** in a mature PSX project — do the
  big ones get matched, or does every project have a long tail of large `INCLUDE_ASM`
  stubs that never close?
- What **workflow** is used at that size: decomp-permuter, m2c drafts, hand iteration
  against a diff tool, or splitting the function into helper statics and matching pieces?
- Do large matches **cluster in projects with better compiler-internals documentation**, or
  with a particular tool?
- Is there a known **inflection size** where PSX projects tend to give up and accept
  assembly?

Why this decides something: if 300+ routinely matches elsewhere, my ceiling is a skill gap
and 211 functions are recoverable, moving my realistic main-exe target from ~908 to 1,119.
If it doesn't, that ceiling is a property of the toolchain and my projection stands.

---

**QUESTION 2 — Does m2c actually accelerate the small-function pool on this compiler
generation?**

`matt-kempster/m2c` (formerly mips_to_c) produces draft C from MIPS. It is **not installed
here**, though a context-generator script for it exists in-tree, so someone once intended
to use it. I have **248 unmatched functions ≤30 instructions and 377 ≤50** — if m2c halves
per-function time on that pool it is my single biggest throughput lever; if its output must
be rewritten anyway, installing it is a distraction.

- For **small** functions (20–60 instructions) on **PSX gcc 2.7.x**, do practitioners use
  m2c output as a *starting draft* or only as a *control-flow reference* they then discard?
- Which m2c **flags/settings** matter for this compiler generation specifically
  (`--target mipsel-gcc-c`? struct/typemap context? `--stack-structs`? knowledge base
  files)? What does a good context file contain?
- Is there testimony — issues, READMEs, Discord logs, project wikis — that quantifies the
  speedup, or that reports m2c being *counterproductive* because it optimises for
  behavioural equivalence rather than for the source shape that reproduces the original
  codegen?
- How does m2c interact with **decomp-permuter** in practice — is the standard pipeline
  m2c draft → permuter → hand-fix?

---

**QUESTION 3 — How do projects structure one function that must appear, byte-identical, in
N separate binaries?**

My 37 code-bearing stage archives contain 10,107 function instances but only ~7,064 unique
bodies: **1,218 distinct bodies appear more than once, covering 4,261 instances (~3.5x
each)**. One trivial body occurs 363 times. Some engine functions also have byte-identical
twins inside stage overlays. Exactly **1 of 37** archives has been converted to a C segment
so far, and it contains only stubs.

- What is the **build-system pattern** for shared-across-overlays code in projects that
  have solved this — one shared `.c` compiled once per overlay with a different linker
  script? A common body `#include`d per overlay? Link-time symbol aliasing? Something else?
- Concretely, how do **SOTN** (which has per-stage overlays with substantial shared code),
  and any Crash / Spyro / Ape Escape overlay-based project, lay this out on disk and in
  their makefiles?
- How do they **avoid double-counting** in progress metrics when one source function
  produces many binary instances? (I care about this: it decides whether my stage workload
  is ~7,064 units or closer to ~1,800.)
- Are there **splat** configuration patterns or community tooling specifically for
  deduplicated overlay code?

---

**QUESTION 4 — Can PSY-Q SDK and libgcc functions be obtained rather than reverse-engineered?**

Some unknown fraction of my 614 unsplit main-exe functions is library code — memcpy/memset/
strlen-shaped, GTE helpers, soft-float and 64-bit-integer helpers (`__divdi3`, `__ashldi3`,
`__muldi3`), and PSY-Q library routines. I have a heuristic candidate list; **zero are
confirmed**. Library code is matchable from *known* source or *known objects* — a
fundamentally cheaper class of work than reverse-engineering.

- Are the **PSY-Q SDK libraries** available in a form usable for matching — original
  `.LIB`/`.OBJ` archives, and the tooling to convert them (I am aware of
  `psyq-obj-parser` in the PCSX-Redux tree; is that the standard route)? Do projects
  **link the original library objects directly** rather than writing C for them, and is
  that considered an acceptable match?
- Is there a published **fingerprint / signature database** of PSX SDK function bodies
  (anything FLIRT-like, or a community table of known SDK function hashes) that would let
  me identify library code in bulk instead of one function at a time?
- For **libgcc 2.7.2 MIPS helpers**: which source file are they from, and do SN-shipped
  variants differ from FSF? Have projects matched them from the FSF `libgcc2.c` /
  `libgcc1.S` sources at these flags?
- Any published **PSY-Q header/source leaks or authoritative reconstructions** beyond the
  public headers?

---

**QUESTION 5 — What does a disciplined decomp-permuter workflow actually look like?**

Measured locally: **3 wins in 11 fair trials (27%)**. Two intuitions were refuted by my own
logs — a low score does *not* imply proximity (one function sat at score 20 for 6,903
iterations with no improvement), and a high score is *not* fatal (another went 100,430 →
260 in about 60 seconds). I also know locally that **a permuter score of 0 is not a byte
match** (it normalises jump targets; one scratch scored 0 with a `j` to the wrong label),
and that a genuine workdir zero **can fail to transfer in-tree** when the scratch used stub
types instead of the real structs.

- What **settings** do high-throughput projects actually run — `--stack-diffs`, branch-target
  handling, thread counts, `--best-only`, seeds, iteration budgets before abandoning?
- Is there a **policy** for *when* to reach for the permuter versus hand iteration — a size
  band, a score threshold, a "structure is right, registers are wrong" test?
- Do projects **gate every permuter result through an in-tree byte comparison**, and have
  others documented the score-0-is-not-a-match trap?
- Are there **forks or wrappers** with better scoring (positional/exact rather than
  alignment-based), or standard patches people apply?
- What do practitioners say about **when a function is permuter-hopeless** and should be
  left as assembly?

---

**Output format.** For each question: a **direct verdict first** (found / not found /
partial), then the specific links with a sentence on what each actually contains, then a
short assessment of how usable it is for my situation. Prefer primary sources — repository
files, commits, issue threads, project docs, archived Discord/forum posts — over general
articles. Quantify wherever the source allows (counts of matched large functions, measured
timings, hit rates).

**If something does not exist, say so plainly rather than substituting adjacent material.**
A confident "no public PSY-Q fingerprint database exists" is as useful to me as finding
one. Please flag clearly anywhere you are inferring rather than citing — I will be acting
on this, and a plausible-sounding wrong lead costs me more than a gap.
