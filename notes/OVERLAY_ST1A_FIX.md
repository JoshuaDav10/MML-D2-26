# Overlay Fix Brief — ST1A silent link failure + swallowed build errors

Delegated task (Cursor or similar), same working style as
`notes/OVERLAY_EXPEDITION.md`. Branch: `overlay-expedition` (or a child of it).
Append a sprint entry to `notes/OVERLAY_EXPEDITION_LESSONS.md` when done.

## The problem (verified 2026-07-05, Fable audit)

`make CPP=cpp check_overlays` prints `205/205 overlays OK`, but ST1A.BIN is a
**false pass**: its code chunk never rebuilds. Two defects combine:

1. **ST1A's code chunk fails to link.** `logs/build_ST1A.log` shows undefined
   references to `eve19_main`, `eve19_main_tbl`, `eve19_main_area_*`,
   `eve19_end`, `eve19_main_dummy`. Their addresses exist in
   `config/overlay/splat.us.ST1A/syms.us.ovl0__progbin_r3_st1a.bin.txt`, and
   splat consumes that file — which is exactly why the symbols do NOT appear
   in the `undefined_syms_auto` / `undefined_funcs_auto` files that ARE passed
   to the linker. The ld command in `tools/buildoverlay.py` (line ~73) passes:
   - the generated per-chunk `.ld` script
   - `undefined_syms_auto.*.txt` and `undefined_funcs_auto.*.txt`
   - `build/generated.rock_neo.syms.txt` (made by
     `tools/generate_rock_neo_syms.py`, invoked from the Makefile `chunks`
     target)

   …but NOT the curated `syms.us.<chunk>.txt`. Any symbol defined only there
   (with a raw address, not present in rock_neo.elf) is invisible to ld.

2. **All failures are swallowed, and the fallback masks them.**
   - `tools/buildoverlay.py` calls `os.system()` for as/cc/ld/objcopy and
     never checks return codes.
   - `build_overlay()` (line ~211) copies the ORIGINAL
     `disks/us/CDDATA/DAT/<X>.BIN` into `build/` BEFORE emplacing rebuilt
     chunk bytes. When a chunk's `.elf.bin` is missing (because its link
     died), the emplace loop raises FileNotFoundError — after the copy.
   - Makefile `chunks` target runs each archive via
     `$(foreach …,$(shell $(BUILD_OVERLAY) …))` — `$(shell)` discards exit
     codes, so make succeeds anyway.

   Net effect: a crashed overlay build leaves a byte-perfect COPY of the
   original in `build/`, and `check_overlays` counts it as a match. The check
   cannot fail for this failure class — for ST1A today, or for any future
   overlay regression.

## The fix (scoped)

**Fix A — feed curated syms to ld.** In `tools/buildoverlay.py`'s link
command, additionally pass
`-T config/overlay/splat.{VERSION}.{archive}/syms.{VERSION}.{chunk}.txt`
**iff the file exists** (most archives don't have one; ST1A does).
Check the file's format is ld-script-compatible (`sym = 0xADDR;` lines — it
is). Do NOT touch the syms file contents.

**Fix B — stop swallowing failures.**
1. In `buildoverlay.py`, check every `os.system()` return code (as, cc
   pipeline, ld, objcopy, `ld -r -b binary`); on failure, print the failing
   command + archive/chunk to stderr and `sys.exit(1)`.
   Note the cc pipeline is a shell pipe — its exit code is the LAST stage
   (as). That still catches "partial .o" cases; deeper pipefail plumbing is
   out of scope.
2. In `build_overlay()`, only copy/emplace/write the final BIN after all
   configured chunks produced their `.elf.bin`. (With #1 aborting earlier
   this is nearly automatic; keep the copy where it is but let the abort
   happen before it.)
3. In the Makefile `chunks` target, replace the `$(foreach …,$(shell …))`
   line with a real recipe loop that propagates failure, e.g.:
   `@for a in $(ALL_ARCHIVES); do $(BUILD_OVERLAY) $$a || exit 1; done`
   (Keep `generate_rock_neo_syms.py` running before the loop; move it out of
   `$(shell)` too.)

## Hard rules

- Do not touch: `notes/LESSONS.md`, `src/rock_neo/`, `include/rock_neo/`,
  anything under `asm/rock_neo/`, splat yamls of archives other than ST1A
  (ST1A's yaml should also not need changes).
- Do not "fix" ST1A by editing its syms file values or by relaxing
  `check_overlays`.
- Do not commit to `main` or `dev`. Work on `overlay-expedition` (or branch
  from it), push to origin.
- `make clean` is FORBIDDEN (deletes asm/ + assets/). Use `rm -rf build`.
- Every make invocation needs `CPP=cpp`.

## Verification protocol (unfakeable — all four required)

1. **From-scratch build is clean**: `rm -rf build && make CPP=cpp 2>&1 | tee
   /tmp/ovl_fix_build.log`; then
   `grep -iE 'undefined reference|No such file|Error' /tmp/ovl_fix_build.log`
   must output NOTHING, and `grep -c . logs/build_ST1A.log` errors likewise.
2. **Checks pass on the fresh tree**:
   `make CPP=cpp check_overlays` → `205/205 overlays OK`;
   `make CPP=cpp check_rock_neo_only` → `OK`.
3. **ST1A mutation test (proves the chunk is now genuinely rebuilt)**: edit
   one `.word` in any file under `asm/ST1A/ovl0__progbin_r3_st1a.bin/data/`
   (change the VALUE, e.g. `0x0` → `0xDEADBEEF`; keep it 8 hex digits max —
   9-digit constants silently truncate back and the test becomes a no-op),
   rebuild that archive, and `check_overlays` MUST print `MISMATCH: ST1A.BIN`
   and exit non-zero. Restore the file, rebuild, check green again.
4. **Failure-propagation test (proves Fix B)**: temporarily rename one
   extracted `.s` under any archive's asm dir, run `make CPP=cpp` — make
   itself MUST exit non-zero (not just print and continue). Restore.

Report the literal command outputs for all four. A report without them is
not a completed task.

## Context pointers

- Audit that found this: session notes 2026-07-05 (Fable); task chip
  "Fix ST1A silent link failure and swallowed overlay errors".
- `tools/buildoverlay.py` line refs above are as of commit ee559d1.
- `logs/build_ST1A.log` — current failing link output.
