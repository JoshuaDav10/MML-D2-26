# Next steps — the plan, in priority order

Written 2026-07-28 against **362 matched** (350 engine + 12 stage) of 8,183.
Live numbers: `notes/COUNTS.md`. Inventory: `notes/FUNCTION_MAP.md`.
Regenerate the evidence behind everything here with `tools/gen_map.py && tools/dups.py`.

## What changed our thinking

Two beliefs died this week, and both change the plan:

1. **Big functions are not impossible.** Metal Gear Solid's decomp has **5,423 matched
   functions, largest 4,157 lines of C**, on our compiler generation (measured by cloning
   it, not inferred). We independently matched a 137-instruction function from leaked
   source. The old ~120-instruction ceiling was **effort**, not the toolchain. So the
   engine target is ~1,119, not ~908.
2. **The leverage is in the stage files, not the engine.** Of 1,363 unmatched functions
   that share a shape with something we have already solved, **1,331 are in stage files
   and only 32 are in the engine.** Every template we own pays out when we convert
   archives — not while we grind the engine.

Corollary that should drive scheduling: **anything that supplies source shape beats more
hand-iteration.** Leaked source, m2c drafts, library source, templates. In that order.

---

## 1. Convert stage archive #2 — highest leverage, do this first

`eve19` (12 functions) is done and already earned three templates covering **1,340**
functions. The dedup tool now says which archive to take next:

```
tools/dups.py                 # ranked list
tools/dups.py --archive ST03  # what fraction of it is already solved
```

**Take a twin pair.** `ST03≡ST03B`, `ST04≡ST04B`, `ST10≡ST10B`, `ST11≡ST11B`,
`ST0C≡ST0CB≡ST0CC` are byte-identical repackagings — **convert one and its twin is free.**
This is also why the old "called by 37 stages" figure was inflated: those were archive
directories, not distinct programs.

**Expect a large free chunk on arrival:** 348 unmatched stage functions are exact copies of
a 2-instruction function we already matched, and 1,331 more share a solved shape.

**Gate:** `make CPP=cpp check_overlays` must still print 205/205.

## 2. Use m2c on the small engine pool

Installed and working at `tools/m2c` (it was already in the tree, unused). Feed it with the
existing `tools/m2ctx.py`.

```
python3 tools/m2c/m2c.py --target mips-gcc-c -f <func> <asm file>
```

**Pool: 248 unmatched engine functions ≤30 instructions, 354 ≤50.**

Treat the output as a **draft** — it optimises for behaviour, not for the source shape that
reproduces the original register allocation. Rewrite the expressions; keep the control flow.

**Keep-it test:** median time-to-first-compilable-draft on that pool drops ≥30%.
**Kill-it test:** you discard essentially all of its expression-level output.

## 3. Identify the library code before reverse-engineering it

`tools/fetch_vendor.sh` downloads (gitignored, ~62 MB):

| source | what it gives |
|---|---|
| `psyq_sdk` | 74 `.LIB` files (PSY-Q 4.3 / 4.4 / 4.5) |
| `psy-q-decomp` | 79 C files — reference source for SDK routines |
| `mips-gcc-2.7.2` | `libgcc2.c` — `__divdi3`, `__ashldi3`, `__muldi3`, `__cmpdi2`, … |
| `psyq-obj-parser-src` | converts Sony `.OBJ`/`.LIB` → modern ELF objects |

**Do:** run `psyq-obj-parser` over the 4.3 libraries, fingerprint the results, and check
them against the 614 unsplit engine functions. Convert `notes/wip/SDK_CANDIDATES.md` from
heuristic guesses into confirmed identifications.

⚠️ **Version caveat, do not paper over it.** Our cc1 reports `2.7.2.SN32.3.7`, which ships
from **PSY-Q 4.0**. The mirror has **4.3 at the earliest**. Close, not identical. Verify any
identification against our own bytes. SN-patched builds can also differ from stock FSF.

**Payoff test:** if >50 of the 614 unsplit engine functions confirm as SDK/libgcc, that is a
large, low-risk block matched from known source rather than reverse-engineered.

## 4. Break the large-function band deliberately

97 unmatched engine functions sit in 120–200 instructions, 63 in 200–300, 50 above 300.

We have matched 137 instructions **from source**. The open question is whether the workflow
closes one **without** source. Run it as an experiment on 3–5 functions in the 120–200 band:

> m2c draft → hand-match structure against `./diff.py` → permuter for the register residue
> → full-binary SHA-1 gate.

**If it fails:** the blocker is a specific compiler-internals blind spot — most likely
`-fpcc-struct-return` struct returns, soft-float sequencing, or `-G8` small-data addressing.
Isolate it on a decomp.me scratch and ask the community rather than grinding.

## 5. Standing rules adopted this week

- **Permuter:** only when the structure is right and registers/ordering are wrong. Always
  `-j`. **Never accept score 0 without an in-tree rebuild and a full-binary SHA-1** — score 0
  is scorer-relative by design (stack ignored without `--stack-diffs`, jump targets
  normalised). Our 27% hit rate is a normal band for hard functions.
- **When research says "could not fetch GitHub" — clone the repo and measure.** That is how
  the >120 ceiling question went from inference to measurement.
- **Counting objects is not counting the binary.** A `.c` can compile to a `.o` the linker
  never uses; the hash stays green because the original bytes shipped. Guards live in
  `census.py` and `gen_map.py`.

## Known gaps

- **9 of 362 matched functions still carry no fingerprint** (no archived asm found), so
  template detection is very slightly incomplete. Was 350; fixed 2026-07-28.
- **Stage progress is counted per archive, not deduped across archives.** Matching a body in
  ST03 does not automatically mark its ST05 twin done. `tools/dups.py --twins` is the manual
  bridge; a build-level solution is the SOTN `dups`/`mipsmatch` pattern.
- **No overlay C pipeline beyond ST1A.** The mechanism is proven; it has been used once.
