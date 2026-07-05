# MML Decomp — Project Rules (Claude Code)

Matching decompilation of Mega Man Legends (PSX, US), binary ROCK_NEO.EXE.
Goal: C source that recompiles to a byte-for-byte identical binary.

## Build
- Activate venv first: `source .venv/bin/activate` (rabbitizer errors = venv not active)
- Build: `make CPP=cpp`  (main exe matches; ST** overlays don't link yet — expected, not a regression)
- Verify main exe: `make CPP=cpp check_rock_neo_only`  (prints OK on byte-for-byte match)
- Diff a function: `./diff.py <func>`  (plain mode: built exe vs original via mapfile; do NOT use `-o`/`-e`, no expected/ dir exists)
- Force rebuild: `rm -rf build`  (NEVER `make clean` — it deletes asm/ and assets/)

## Build gotchas (learned the hard way)
- `.c.o` objects are make *intermediate files*: deleting them does NOT force a
  rebuild. After editing pipeline tools (tools/patchasm.py, tools/maspx/…), run:
  `touch src/rock_neo/*.c && rm -f build/rock_neo.elf` then `make CPP=cpp`.
- tools/patchasm.py contains a function-reorder pass: cc1-27 at -O1+ defers all
  compiled C bodies past the top-level INCLUDE_ASM asm blocks; the pass moves
  them back to their in-place `.globl` markers. This is what lets real C sit
  mid-file between INCLUDE_ASM stubs. Don't remove it.
  - Limitation: `static` functions get no `.globl` marker and would stay at the
    end of the file. Avoid `static` (or keep such functions last in the file).
- tools/asm-differ (submodule) carries a LOCAL uncommitted patch for
  Python 3.14 (`ast.Num` → `ast.Constant` in eval_(), ~line 1012).
  `git submodule update --checkout` wipes it; symptom is diff.py failing with
  "Failed to parse --base-shift". See SETUP.md "Stage 1 fixes".

## Environment notes (2026 toolchain)
- Compiler is ./bin/cc1-27 (in-repo). Do NOT install mipsel-elf-gcc (fails on GCC 15, not needed).
- Preprocessor: system `cpp` via the `CPP=cpp` override (required on every make invocation).
- include/macro.inc has added no-op macros (nonmatching/enddlabel/alabel) for modern spimdisasm.

## Workflow rules
- One function at a time. Real work on branches: `git switch -c match/<func>`.
- Track mapped vs matched separately in progress.md. Mapping != matching.
- A function is DONE only when `make CPP=cpp check_rock_neo_only` prints OK
  (full-binary sha1) — not when its diff "looks right".
- Map functions in ascending address order when adding to the splat yaml.
- Update progress.md and activity.md when a function is matched.

## Delegation
- Route mechanical work (grep, stub creation, builds, diff runs) to the Haiku
  subagents in .claude/agents/ (function-mapper, build-runner).
- Keep matching-decomp reasoning (reading diffs, writing C) in the main
  strong-model thread.
- Subagents must return summaries and the few relevant lines, not full logs.
