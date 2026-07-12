# Deep-research brief — gcc 2.7.2 matching-decomp idioms

Paste the block below into a research-capable chat (ChatGPT deep research,
Claude with web, Perplexity, etc.). It is self-contained.

---

## PROMPT TO PASTE

I'm doing a **matching decompilation** of a PlayStation 1 game: writing C that,
when compiled with the original toolchain, reproduces the shipped machine code
**byte-for-byte**. The toolchain is **GCC 2.7.2 (`cc1`) from the Sony PSY-Q SDK**,
targeting **MIPS I / R3000**, at **`-O2 -G8`**, assembled with Sony's ASPSX
assembler. I am stuck on two specific codegen behaviors and need to find how
other decomp projects have forced them from C source.

Please research and report, with concrete C examples and citations:

**Question 1 — Suppress CSE of a repeated constant address so it is NOT hoisted
as a loop invariant.** In a loop, an absolute address constant (e.g. a hardware
scratchpad address like `0x1F800070`) is used ~3 times. GCC 2.7.2's `cse.c`
value-numbers the three identical address computations into ONE pseudo, and then
`loop.c`'s `move_movables` treats that merged pseudo as a loop invariant whose
"savings" clears the hoist threshold, so it gets promoted into a callee-saved
register. The ORIGINAL binary instead keeps the address **inline** (`lui/ori` at
every use site) and hoists a *different*, fewer-use constant into the saved
register. I need a C source form that makes GCC 2.7.2 **treat the three uses as
distinct** (so no single invariant forms) while still emitting an inline
`lui/ori` at each site. Already tried and FAILED: `volatile` pointer, distinct
pointer types per site, `asm volatile("":::"memory")` between uses, `volatile`
locals, same-address-different-access-mode. What actually breaks constant-address
CSE / movable formation in GCC 2.7.x?

**Question 2 — Force reuse of a pointer already in a register instead of an
absolute store.** GCC 2.7.2 emits an absolute-addressed store (`sh $v1, %lo(sym)($at)`
with a fresh `lui`) where the original REUSES a struct base pointer already held
in a register (`addu $a0,$base,$zero; sh $v1,0($a0)`). cc1 keeps constant-folding
my pointer expression back to an absolute address. How do decomp projects force
GCC 2.7.x to address a global/struct store through a held base register? Related:
how do they get a store into a **branch-delay slot** by holding the target
address in a base register across the branch?

**Where to look (please search these):**
- **decomp.me** — the collaborative matching-decomp site. Search its scratches
  and the linked Discord/knowledge for GCC 2.7.2 / PSYQ / "psx" idioms, and for
  the terms "movable", "loop invariant", "CSE", "pointer reuse", "delay slot".
- **GitHub PSX decomp projects using PSY-Q GCC 2.7.2**, e.g.:
  - the Castlevania: Symphony of the Night decomp (`sotn-decomp`)
  - Metal Gear Solid, Frogger, Legacy of Kain, and other `psx`-tagged decomps
  - the `n64decomp`/`decomp-toolkit` and `decompals` orgs (some MIPS idioms
    transfer even from N64 IDO/GCC)
  - any repo containing `maspsx`, `maspsx.py`, `psyq-obj-parser`, or `.psyq`
- **maspsx** (the ASPSX-emulation assembler wrapper) issues/README — it documents
  many `cc1`-vs-ASPSX quirks (load-delay nops, `$at` expansion, gp-rel).
- **GCC 2.7.2 source itself** (`cse.c`, `loop.c` `move_movables`/`combine_movables`,
  `local-alloc.c`, `global.c`) — quote the exact condition that governs whether
  two identical SETs merge into one movable, and what suppresses it.
- **Decomp community writeups / blog posts** on "matching decompilation
  techniques", "fighting the register allocator", "loop invariant motion".

**Output format I want:**
1. For each question: the underlying GCC 2.7.x mechanism (cite file/function or a
   source), then 1-3 concrete C idioms that control it, each with the expected
   asm effect and a link to where it's used/documented.
2. A short list of the most useful PSX/PSY-Q decomp repos + specific files/PRs
   where these exact fights were won.
3. Any tooling (compiler flags, `maspsx` options, dump flags like `-dL`/`-dg`)
   the community relies on for diagnosing these.
Prefer primary sources (repo commits, decomp.me scratches, GCC source) over
general LLM recollection, and flag anything you're unsure about.

---

## Notes for whoever runs this
- Q1 is MML's `func_80053B40` blocker; Q2 is `func_8001BB4C` ("addressing-crux"
  + "jump-canonicalization"). Full local forensics: `notes/wip/53B40_PROGRESS.md`,
  `notes/wip/BB4C_PROGRESS.md`. Portable rules: `notes/COMPILER_IDIOMS.md`.
- The single most valuable outcome is a **general C idiom** for each, because both
  behaviors recur across many functions here (the "parked genera").
