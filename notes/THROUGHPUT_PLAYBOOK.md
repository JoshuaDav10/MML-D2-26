# Throughput playbook — how to land 40+ verified functions in a session

Derived from the two highest-output waves this project has had (commits `594b814`,
`956066b`: **+17 then +27 engine matches**, 363 → 407 in one session), and from a
2026-07-29 run that managed only **+6** and taught the negative lessons.

Nothing here trades away verification. Every function still passes the same gates. The
gain comes entirely from **batch shape**, not from checking less.

---

## The shape that produced +44

```
10 concurrent read-only agents
  x 1-2 functions each
  each agent SELF-VERIFIES with bytecmp before reporting
  -> collect ~20-30 verified drafts
  -> land ALL of them in ONE build cycle
  -> one hash check, one overlay check, one commit
```

Wave 1: 10 agents × 2 functions → 18/20 verified → **17 landed in a single build**.
Wave 2: 30 agents (10 concurrent) × 1 function → 29/30 verified → **27 landed**.

## The shape that produced +6 (do not repeat)

```
4 agents x 7 functions each
  -> land each batch separately = 4 separate build cycles
  -> and 28 of the 34 were COPIES of already-solved bodies, which add ZERO
     to the headline count
```

Three separate mistakes, in order of cost:

1. **Wrong lane.** Transplanting a solved body into another archive raises *instances*,
   not *unique bodies*. It is real coverage work but it does not move the number. Check
   what a lane actually adds **before** spending a session on it.
2. **Batches too large per agent, too small per landing.** A build cycle costs the same
   for 7 functions as for 40. Seven agents' worth of work landed in four builds instead
   of one is three wasted cycles.
3. **Too few agents.** The main thread is the bottleneck; agents are cheap and parallel.
   Four agents starve the queue. Ten keep it full.

---

## The rules

### 1. Only work that moves the number
Rank by `tools/dups.py` and `tools/gen_map.py`, then pick from:
- **Templates first.** One solved shape = N functions. The 15-instruction dispatcher
  family gave 6 in a single batch. **Hunt these before anything else** — scan for repeated
  instruction skeletons and solve the representative.
- **Engine functions.** Every one is +1, no ambiguity.
- **New stage bodies** ranked by copy count (a body with 14 copies unlocks 14 instances
  once its archives are open).
- **Never transplants**, until the headline lanes are dry.

### 2. Ten agents, one to three functions each
Small assignments finish fast and keep the queue moving. `SendMessage` the same agent its
next assignment — it keeps its context and gets faster each round.

### 3. Accumulate, then land once
Do **not** land each agent's report as it arrives. Collect verified drafts in `/tmp` until
you have 20-40, then land them together:

```
tools/land_stage_batch.py <report> <SEG> <ARCH> [TWIN...]   # per report, no build
... repeat for every report ...
make CPP=cpp check_rock_neo_only     # ONE build
make CPP=cpp chunks                  # CHECK THE EXIT CODE
tools/checksizes.py ...              # per touched archive
make CPP=cpp check_overlays
```

If the batch fails, bisect: stub the suspects, rebuild, re-add. Still cheaper than N builds.

### 4. Agents must self-verify, and must be told what they cannot verify
Every agent runs `bytecmp` with a **unique** `BYTECMP_OBJ=/tmp/bc_<tag>_$$.o`.
Tell them explicitly: bytecmp cannot resolve `%hi`/`%lo` in a single-function scratch TU,
so it *flags* those rather than failing, and **where splat never attached a symbol it can
grade a correct function as a hard failure** (`func_80109034`: 2 hard mismatches, provably
correct). The real link settles those. An agent that says "verified except X, and here is
why" is worth more than one that says PASS.

### 5. Agents never touch the repo
Read-only, `/tmp` only, no `make`, no `splat`. And **no typedefs in their output** — the
archive keeps ONE proven typedef block. `cc1 -lang-c` accepts an unknown struct member
silently and can eliminate a body entirely; a re-derived struct is a real risk. Agents
report the *fields and externs they need* in prose; the main thread adds them.

### 6. If it is already solved, copy it — never re-derive
An agent re-deriving a solved function is the same risk for no upside. Cost three bodies
on 2026-07-29, one of which wanted the same global as both a byte array and a 16-bit
scalar (impossible in one file).

### 7. Dedupe the work list
`func_801015F4` went to two agents at once on 2026-07-29. Keep one assignment list.

---

## Known integration failures (all from `956066b` — these were the main thread's bugs,
## not the agents' C)

- **Line-level dedup destroys shared typedef openers.** Merge whole declarations, never lines.
- **`#define` has no trailing `;`** — a declaration splitter that assumes one drops macros.
- **Duplicate typedefs are a C89 error.** One block per file.
- **Any function whose asm mentions `jtbl_` needs a rodata carve first**, or splicing C
  orphans the `.L…` labels into link-time undefined references.
- **A scratch TU can verify and still compile short in-tree** if the real file has
  different types in scope (`func_800137D4`: 172 vs 188 instructions). `tools/checksizes.py`
  catches this — run it every time.
- **Scratch harness must be flag-identical to the Makefile.** `tools/tryfn.sh` was missing
  `-mel` and produced 8 phantom mismatches on correct C.

## Expected output

With the above: **40-60 verified functions per session** is the demonstrated rate, and the
+44 session did it *without* hunting templates. Template hunting should beat it.

The hard ceiling is the single serial build gate in the main thread. The real unlock —
still unsolved — is letting agents build independently; `-gcoff` embeds the absolute
source path in every object, so a worktree at any other path can never match. See
LESSONS.md.
