# HANDOFF — MML Decomp session state (2026-07-04, end of day)

> **Read this first.** You are (probably) Claude Fable in Claude Code, resuming a
> Mega Man Legends (PSX) matching decompilation. This file + `CLAUDE.md`
> (auto-loaded) + `notes/LESSONS.md` are your complete context. The older
> `FABLE_HANDOFF.md` / `MML_DECOMP_CHECKPOINT.md` are historical (Stage 0/1);
> this file supersedes their "what to do next" sections.

## State (all verified, none of this is aspirational)

- **Branch: `dev`** — ALL commits go here. NEVER commit to or merge `main`;
  only the user promotes to main. Push target: `origin` (JoshuaDav10/MML-D2-26).
- **The build matches byte-for-byte** with 30 functions decompiled to real C.
  `make CPP=cpp check_rock_neo_only` prints OK; also verifiable with
  `cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe` (raw byte compare).
- **Matched: 32 / 475** functions (2 upstream + 30 this session), ~0.5% of
  instruction volume — small-function harvest phase. See `progress.md`.
- Overlays (ST**) still don't link — expected, ignore those errors, later expedition.

## What was accomplished this session (chronological)

1. **Diff loop works**: `./diff.py <func>` (root symlink → tools/asm-differ).
   Needed: watchdog+levenshtein (now in requirements.txt) and a **local patch
   in the asm-differ submodule** (Python 3.14: `ast.Num`→`ast.Constant`,
   ~line 1012 in eval_()). ⚠️ That patch is UNCOMMITTABLE (submodule) — if
   diff.py fails with "Failed to parse --base-shift", reapply it (SETUP.md
   "Stage 1 fixes" has details).
2. **Killed the blocker that stopped upstream**: cc1-27 at -O2 defers all C
   bodies to end-of-TU after the INCLUDE_ASM asm blocks (upstream's
   `ACCEPT_REORDERING_BULLSHIT`). `tools/patchasm.py` now has a reorder pass
   (moves each deferred `.ent/.end` body to its in-place `.globl` marker).
   Real C now works anywhere in a file. Limitation: no `static` functions.
3. **Matched 30 functions** across moji/scene/cd/debug/sound/sub_scrn/
   Code800133D8. Introduced `MOJI_TASK` struct (include/rock_neo/moji.h) —
   field 0x14 is `u8* script`; the tiny moji funcs are script-opcode handlers.
4. **Wrote the knowledge base**: `notes/LESSONS.md` = compiler idioms proven
   against the hash (gp-vs-lui extern rules, swap idiom, aliasing double-loads,
   scheduler interleaving). READ IT before writing any C.
5. **Proved anti-hallucination ritual to the user** (they are — rightly —
   wary from a past attempt): mutation test = break a matched function,
   check MUST fail, restore, OK. Progress is gated on the byte compare, never
   on "looks right". Keep it that way and keep saying so honestly.

## The inner loop (per function / small batch)

```bash
source .venv/bin/activate                      # ALWAYS first (rabbitizer error = forgot)
# pick target: wc -l asm/rock_neo/nonmatchings/*/*.s | sort -n
cat asm/rock_neo/nonmatchings/<file>/<func>.s  # read the asm
# write C in src/rock_neo/<file>.c replacing the INCLUDE_ASM line
rm -f build/rock_neo.elf && make CPP=cpp build_rock_neo_only
make CPP=cpp check_rock_neo_only               # OK = matched
# mismatch? head -30 build/rock_neo.diff isolates the function;
#   ./diff.py --format plain <func> for instruction-level view; iterate.
git add -A src/rock_neo include/rock_neo progress.md activity.md && git commit  # on dev
```
Update progress.md counts + activity.md line per batch. Commit message style:
see `git log` on dev.

## Critical gotchas (cost real time; do not rediscover)

- `.c.o` files are make *intermediate* files: after editing pipeline tools
  (patchasm.py etc.), `touch src/rock_neo/*.c && rm -f build/rock_neo.elf`,
  else make silently reuses stale output.
- NEVER `make clean` (deletes asm/ + assets/ → full re-extract). `rm -rf build` only.
- `CPP=cpp` on every make invocation.
- Extern declarations: read the target asm FIRST — `($gp)` access ⇒ plain
  scalar extern; `lui/%lo` ⇒ unsized array extern (`extern u8 foo[];`).
  Getting this wrong flips addressing mode and mismatches. (LESSONS.md §2.)
- moji.h's `Moji_flag` is `u8[8]` (matches moji.c definition) — don't "fix" it back to u32.

## Where to pick up (in order of value)

1. **Continue small-function harvest**: `wc -l asm/rock_neo/nonmatchings/*/*.s
   | sort -n` — everything ≤25 lines is quick wins; many moji script handlers
   share MOJI_TASK vocabulary.
2. **main.c small funcs** (func_80012FA4/80012FC8, ~15 lines) and the
   already-written-but-disabled functions in game.c behind
   `#ifndef ACCEPT_REORDERING_BULLSHIT` — the reorder pass likely makes them
   viable now: enable one (flip its ifdef), build, check. If OK, upstream
   pre-wrote several matches for free. Verify each individually.
3. **Medium functions (25–80 lines)**: consider installing m2c
   (github.com/matt-kempster/m2c) for draft C — `tools/m2ctx.py` already
   exists for generating its context. Drafts are never trusted, only iterated
   against the diff.
4. Big moji handlers (~500 lines) only after the struct fills in more.

## Delegation & token hygiene

- `.claude/agents/build-runner.md` + `function-mapper.md` (both Haiku) exist —
  route builds/greps there when output would bloat context; they return
  summaries. Keep C-matching reasoning in the main thread.
- Don't paste whole asm files into context for big functions; read the span.
- The user clears context between sessions deliberately — everything worth
  knowing must land in this file, LESSONS.md, progress.md, or commits.

## Verification contract with the user (do not weaken it)

- "Matched" claims ONLY after `check_rock_neo_only` prints OK in that session.
- The user can independently run: `make CPP=cpp check_rock_neo_only` and
  `cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe`.
- If asked whether progress is real, offer the mutation test, don't just assert.
