# HANDOFF — MML Decomp session state (2026-07-05, overnight)

> **Read this first.** You are (probably) Claude Fable in Claude Code, resuming a
> Mega Man Legends (PSX) matching decompilation. This file + `CLAUDE.md`
> (auto-loaded) + `notes/LESSONS.md` are your complete context. The older
> `FABLE_HANDOFF.md` / `MML_DECOMP_CHECKPOINT.md` are historical (Stage 0/1);
> this file supersedes their "what to do next" sections.
>
> **Reference corpus**: `notes/DOCUMENT_INDEX.md` maps an external doc corpus
> at `notes/reference/` (symlink): MIPS R3000 manual, nocash PSXSPX hardware
> spec, PSYQ SDK docs, PSX assembly notes, MML-specific notes. Index-driven
> lookup only — pull the file/line-range you need, never whole docs into
> context. (Prior sessions missed this; don't repeat that.)

## State (all verified, none of this is aspirational)

- **Branch: `dev`** — ALL commits go here. NEVER commit to or merge `main`;
  only the user promotes to main. Push target: `origin` (JoshuaDav10/MML-D2-26).
- **The build matches byte-for-byte**.
  `make CPP=cpp check_rock_neo_only` prints OK; also verifiable with
  `cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe` (raw byte compare).
- **Matched: 117 / 475** functions (~4.8% of instruction volume, 367 active
  stubs left) — small-function harvest phase. See `progress.md`.
- **The pipeline can emit $gp-relative (sdata) access** via
  `tools/gprel.py` (between maspsx and patchasm); as of 2026-07-05 it also
  gp-rewrites refs to small `.comm` symbols (tentative definitions such as
  moji.c's `u8 Moji_flag[8];` — the COMMON is intentional, splat carves the
  symbol out of the extracted data). Read LESSONS.md §2 before declaring
  any extern.
- Overlays (ST**) still don't link — expected, ignore those errors, later expedition.

## What was accomplished in the OVERNIGHT 2026-07-05 session (most recent)

1. **18 more matches (117 total)** in three hash-verified batches across
   moji/scene/cd/main/sound/sub_scrn. All ≤22-line stubs are now exhausted
   up through ~22 asm lines; next targets start at 23 lines
   (`wc -l` the stub list per the inner loop below).
2. **gprel.py `.comm` support** — moji.c's `u8 Moji_flag[8];` tentative
   definition (COMMON on purpose; splat carved 0x80098A58 out of the data)
   now gets its refs gp-rewritten like census-approved externs. Unblocked
   func_80057DB8 and any future Moji_flag-touching moji function.
3. **New idioms in LESSONS.md "2026-07-05 (overnight)"**: compute-into-locals
   at the load site; alias-forcing read via `&D_80098199 - 1`; ternary
   chains land in $v0 (unlike single ternaries); call-result variable reuse;
   parenthesization steering addu order; and "a 4-byte data shift can be a
   short FUNCTION, not a COMMON leak — check the function diff first".
4. **The scratch-TU brute-force loop is now the workhorse**: pipe variants
   through `cpp|cc1|maspsx` directly (see the perm scripts pattern in this
   session's history / LESSONS night-additions). Most functions this session
   were matched in the scratchpad BEFORE touching the tree.
5. New typed fields: MOJI_TASK.xBC (u16, cleared by several opcode
   handlers), MOJI_TASK.x7C (u8 index into the D_8008AB08 fn table).

## What was accomplished in the LATE-NIGHT 2026-07-04 session

1. **17 more matches (99 total)** across moji/player/scene/cd/main/sound.
   New typed knowledge: SCENE_WORK struct (include/rock_neo/scene.h),
   PL_WORK key-mask fields (x11C/x11E vs x138/x13E) + x9/xA state fields,
   MOJI_TASK second call stack (stack2[8] @0x4C, u16 xC0).
2. **Sce_flag_test matched** — the scene-flag bit reader (many callers
   ahead). Its three tricks (dead 8-byte local array for the empty frame,
   param reassignment for in-place srl, mask-as-local so the load schedules
   mid-computation) are in LESSONS.md "late night additions".
3. Player key-test family (func_800406A8/DC, func_80040B34) share one
   shape — more siblings likely nearby in player.c.

## What was accomplished in the NIGHT 2026-07-04 session

1. **18 more matches (82 total)** — all ≤14-instruction functions across
   main/sound/cd/Code800133D8/scene/moji, hash-verified + mutation-tested.
   Highlights: func_80012F78 (first newly-written gp-relative C),
   the CD_CMD command-queue writer family (new struct in cd.c),
   MOJI_TASK script call stack discovered (stack[8] @0x18, u16 sp @0xBE;
   func_80054AB4 is the script "return" opcode).
2. **New matching idioms** in LESSONS.md "night additions": scalar-pointer
   vs array decl by use count, volatile for reload-after-store,
   ternary-vs-if/else register choice, local init order, and the
   brute-force-orderings-in-a-scratch-TU tactic (fast, decisive).
3. gprel.py now drops ALL small `.extern`s (COMMON-leak proofing).

## What was accomplished in the LATE 2026-07-04 session

1. **Salvaged an uncommitted, unverified WIP batch** left in the tree (it did
   NOT build to a match — reminder: never leave unverified WIP uncommitted
   and unlabeled; stash or commit-to-a-branch with a WIP marker instead).
2. **Discovered and fixed the sdata/gp-relative pipeline gap** — the single
   most important structural fix since the patchasm reorder pass. The C
   pipeline could never emit $gp-relative access (GAS -G0 expanded every
   small-extern ref to lui/$at; cc1's `.extern sym,size` leaked COMMON
   symbols that silently shifted the data segment). New `tools/gprel.py`
   stage (maspsx → gprel → patchasm) fixes this via a census of `%gp_rel(`
   in the extracted asm. Full mechanism: LESSONS.md §2 (rewritten — the old
   §2 was aspirational and wrong).
3. **11 new matches** (main func_80012FA4/FC8; moji func_80054410, 5457C,
   54B88, 5563C, 56128, 57124, 58C08; scene func_8001DDC0 — first
   gp-relative match; sound func_8001B2F0), hash-verified + mutation-tested.
4. **Recount**: 7 upstream game.c functions were compiled+matching all along
   but uncounted → true total 64 matched / 420 active stubs. Counting method
   in LESSONS.md ("Counting matched functions").
5. **Volume metric**: ~841 of ~31,300 instructions matched (~2.7% of the
   main exe's mapped function volume; 64/475 = 13.5% by function count).

## What was accomplished in the earlier 2026-07-04 session (chronological)

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
2. DONE (late 2026-07-04 session): main.c func_80012FA4/FC8, the moji
   script handlers (func_80054410/5457C/54B88/5563C/56128/57124/58C08),
   scene func_8001DDC0, sound func_8001B2F0 — all hash-verified.
   Many remaining small funcs touch sdata globals; gprel.py now makes those
   matchable. All trivial 8-line (jr ra/nop) stubs are exhausted —
   everything remaining requires actually reading the asm.
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
