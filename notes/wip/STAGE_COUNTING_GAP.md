# The stage realm is uncountable — 6 verified matches are invisible

**Opened 2026-07-26, after the first-ever stage-overlay matches landed.**

## The situation

Six functions in `src/ST1A/ovl0__progbin_r3_st1a.bin/eve19.c` are matched and
verified (205/205 overlays byte-identical, each mutation-tested, `eve19.c.o(.text)`
confirmed linked at `0x8012b8e4-0x8012c2ec` in the ST1A map):

`StXX_eve19_move`, `eve19_main`, `eve19_end`, `eve19_main_dummy`, `check_block`,
`check_swing_camera`

**Every counting tool reports 301, exactly as before.** They are structurally
incapable of seeing these.

## Why

- `tools/census.py --matched` (authoritative, used by `audit_count.sh`) reads only
  `build/src/rock_neo/*.o`. Stage objects live in `build/src/<ARCHIVE>/<overlay>/*.o`
  and are outside its glob.
- `tools/gen_map.py` classifies engine functions `MATCHED`/`STUB`/`UNSPLIT` by
  objdumping built objects and running `census.cpp_stubs`. Its stage loop
  (`build()`, the `---- STAGES ----` block) hard-codes `state="STAGE"` for all
  10,107 instances with **no C-awareness whatsoever**.

This is the **same defect class as the two the 2026-07-26 audit found**: a metric
whose denominator is defined by what a script happens to parse, rather than by the
target. It was invisible before only because the stage count was genuinely zero —
the bug and the truth agreed by accident. The moment stage work started, they diverged.

## The fix (not yet done — needs care, this is the count system)

1. `gen_map.py`: add a stage analogue of `objdump_engine_c()` scanning
   `build/src/*/*/*.o` (excluding `rock_neo`), plus `census.cpp_stubs` over
   `src/*/*/*.c`. Emit `MATCHED`/`STUB` for those; in the raw-asm STAGE loop,
   **skip names already claimed by a stage C object** or they double-count.
   Note `census.cpp_stubs` is already generic over any `.c` path — it will work
   unmodified on overlay files.
2. `census.py`: add a stage mode. **Do not silently fold stage matches into the
   existing engine number** — `audit_count.sh`'s 301 is load-bearing in a dozen
   places and quietly turning it into 307 is exactly how an inflation event starts.
3. Report three numbers under the project's existing "always name which
   denominator" rule:
   - engine `301 / 1119`
   - stage `6 / ~7064`
   - whole game `307 / ~8183`
4. `gen_counts.sh` / `COUNTS.md` / `check_docs.sh` must be updated together, and
   the new gate needs a **committed negative test** (inject a fake stage match,
   confirm nonzero exit) per the standing rule that an unwatched gate is a
   confidence generator, not a check.

## Until it is done

Quote stage progress **explicitly and separately**, and never let it silently ride
inside the engine 301. The honest sentence today is:

> 301 engine matched (unchanged) + 6 stage matched (uncounted by tooling).
