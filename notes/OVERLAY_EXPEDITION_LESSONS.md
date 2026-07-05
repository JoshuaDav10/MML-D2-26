# Overlay Expedition — Lessons Learned

Living log for the overlay-expedition branch. One entry per sprint (Phase 0
investigation, Phase 1 FONT, etc.). Append at the **end** of each sprint before
handoff.

**Related:** `notes/OVERLAY_EXPEDITION.md` (mission brief), `HANDOFF.md`,
`CLAUDE.md`.

---

## How to use this file

Each sprint entry should include:

1. **Sprint** — phase name, date, agent/session if useful
2. **What we did** — one paragraph
3. **Lessons** — durable facts that change how later work is planned (not a commit log)
4. **Doc impact** — which project docs should be updated, if any
5. **Open questions** — blockers or things to verify in the next sprint

Do **not** duplicate `notes/LESSONS.md` (main decomp session). This file is
overlay-expedition only.

---

## Sprint: Phase 0 — Investigation (2026-07-05)

**What we did:** Confirmed the 19 missing `CDDATA/DAT/*.BIN` files (no config
under `config/overlay/`). Inspected headers, parsed dashchunk layout, compared
against the 186 configured archives. Produced the investigation table; no code
or config edits.

### Lessons

1. **All 19 missing files are data dashchunk archives, not code overlays.**
   Every file uses the archive format in `tools/buildoverlay.py` (header at
   `0x0`, path string at `0x40`, payload at `0x800`). Inner paths are `.TIM`,
   `.dat`, or `.vab` — not `progbin` or PS-X EXE. None need splat function/asm
   extraction.

2. **The expedition brief's guess table was wrong for 11 files.** ST17B,
   ST19B, ST0B_00D/00E/01B, ST1E_06, and EXIT_* were labeled "stage overlay"
   or "code+data overlay." They are TIM/VAB/font **asset packs**, often small
   standalone archives. Naming similarity to `ST17.BIN` / `ST0B.BIN` is
   misleading — those big stage BINs contain progbin code; the `*B` / `EXIT_*`
   siblings do not.

3. **Phase 3 in the brief ("code overlays") should be re-scoped.** Hard cases
   are **multi-chunk data archives** and **novel chunk types**, not MIPS symbol
   iteration. Work is: per-chunk yaml + `build.json` + data subsegments +
   `cmp` — same family as `HEAD00`, `SUB_WPN`, `ST0D_00D` EMBIN data chunks.

4. **Chunk layout uses a fixed second-chunk offset for many type-1 TIM packs.**
   After a type-1 chunk with size `0x8120` or `0x8040` at offset `0`, the next
   valid header is often at **`0x8800`** (not the naive `0x9000` from size
   alignment alone). Examples: `SUB_WPN`, `ST19B`, `ST17B`, `ST1E_06`, `GAUGE`.
   Smaller first chunks (`0x4120`, `0x4040`) use different follow-on offsets
   (`0x5000`, `0x4800`). Parser logic must match what splat yaml `start:` uses.

5. **Three chunk types have no standalone rebuild precedent in the 186.**
   - **Type 3** — font `.dat` (`FONT.BIN`, `KAIFONT.BIN`)
   - **Type 9** — KOUKA-style TIM (`EXIT_MAP`, `EXIT_SUB`, part of `EXIT_SUP`);
     appears inside stage archives but is pass-through there, never rebuilt alone
   - **Type 10** — (`GAUGE.BIN`, part of `EXIT_SUP`); zero examples in the 186

   Types **1** (TIM) and **5** (VAB) appear in configured archives but often as
   pass-through; standalone rebuild templates are still thin (`HEAD00` pattern
   is the closest for single-chunk).

6. **Useful layout analogues (same shape, different chunk types):**
   - Single-chunk data: `splat.us.HEAD00`, `splat.us.LEG00`
   - Two chunks at `0` / `0x8800`: `splat.us.SUB_WPN` (TIM + MSG; ST19B is TIM + TIM)
   - Two TIMs + terminator: `ST1E_06` layout (cleanest reference)
   - `EXIT_SUB` is a **prefix** of `EXIT_MAP` (first 40960 bytes identical)

7. **Recommended phase order (revised from brief):**
   - Phase 1: `FONT.BIN` (single chunk, novel type 3)
   - Phase 2: `KAIFONT`, `MAP_A*`/`MAP_B*`, `ST0B_00E`/`ST0B_01B`, `BS16VAB1`
   - Phase 3: `ST19B`, `ST17B`, `ST0B_00D`, `ST1E_06` (multi-chunk TIM)
   - Phase 4: `GAUGE`, `EXIT_*`, `GAMEOVER` (types 9/10, 3+ chunks, TIM+VAB)

### Doc impact — worth updating?

**Yes, selectively.** The measured 186/205 facts in `CLAUDE.md` and
`HANDOFF.md` are already correct. What is **stale or misleading**:

| Document | Update? | Why |
|----------|---------|-----|
| `notes/OVERLAY_EXPEDITION.md` | **Yes** | Guess table (lines 18–34), Phase 3 title/scope ("code overlays"), imitation hint "ST17 vs ST17B" |
| `notes/OVERLAY_EXPEDITION_LESSONS.md` | **This file** | Sprint log — append each sprint |
| `CLAUDE.md` | **Later (Phase 4)** | Add pointer to lessons log; add `check_overlays` when it exists |
| `HANDOFF.md` | **Optional** | One-line link to lessons log; already has measured overlay summary |
| `notes/LESSONS.md` | **No** | Expedition brief marks untouchable; keep main-session lessons separate |

Do **not** rewrite history in the brief's ground-truth counts (205 / 186 / 19) —
those remain accurate. Patch **classifications, phase names, and imitation targets**
once Phase 0 is reviewed and Phase 1 is approved.

### Open questions

- Exact splat yaml / `build.json` tuple for **type 3** font chunks (no template).
- Whether **type 9** and **type 10** chunks need splat extension tweaks or only
  data subsegments like type 1.
- `GAMEOVER.BIN` and `EXIT_SUP.BIN` tail layout — multiple chunks confirmed;
  full rebuild may need more than two yamls.
- `ST17B` padding after second TIM — may pass-through via copy-and-emplace in
  `buildoverlay.py` without a third yaml if first two chunks rebuild cleanly.

---

## Sprint: Phase 1 — FONT.BIN (2026-07-05)

**What we did:** Patched `OVERLAY_EXPEDITION.md` with Phase 0 corrections. Added
`config/overlay/splat.us.FONT/` (`ovl0__font.dat.yaml`, `build.json`) modeled
on `splat.us.HEAD00`. Ran splat split + full build on `overlay-expedition`.

### Lessons

1. **Type-3 font chunks use the same HEAD00 yaml shape** — `dashchunkheader`
   at `0`, single `[0x800, data, …]` subsegment, end `[0x8800]`. `build.json`:
   `[ "ovl0__font.dat", 0, 3, 32768, 0 ]` (third field = header type 3, fourth
   = chunk payload size `0x8000`).

2. **`vram: 0x80010000` worked** for link script generation (from header field
   at `0x10`). No splat extension changes needed for type 3.

3. **First clean result:** `cmp disks/us/CDDATA/DAT/FONT.BIN build/FONT.BIN`
   silent after `rm -rf build && make CPP=cpp`. Overlay count **187/205**.

### Doc impact

- `OVERLAY_EXPEDITION.md` patched (Phase 0 table + phase renames).
- This file updated.

### Open questions

- Confirm `vram` choice is stable if FONT chunk is ever split further (likely
  N/A for pure data).

<!-- Next sprint: append below this line -->

---

## Sprint: Phase 2 — Single-chunk data (2026-07-05)

**What we did:** Added overlay configs for all 9 remaining single-chunk archives:
KAIFONT, MAP_A1–B3, BS16VAB1, ST0B_00E, ST0B_01B. Same HEAD00/FONT pattern;
splat split + clean build on `overlay-expedition`.

### Lessons

1. **FONT template generalizes cleanly** — type **1** TIM (`0x10220` end `0x10A20`), type **5**
   VAB (`0x22A30` end `0x23230`), and smaller type-1 TIM (`0x4120` end `0x4920`) all
   byte-match with `vram: 0x80010000` and `build.json` third field = header type.

2. **MAP_A* / MAP_B* are one mechanical family** — identical layout; only inner path
   and archive name differ. Good candidate for a single commit / shared generator script.

3. **Overlay count now 196/205.** Remaining 9 are all multi-chunk or novel types
   (Phase 3 / 3b).

### Verification

```
rm -rf build && make CPP=cpp
cmp disks/us/CDDATA/DAT/{KAIFONT,MAP_A1,MAP_A2,MAP_B1,MAP_B2,MAP_B3,BS16VAB1,ST0B_00E,ST0B_01B}.BIN build/*.BIN  # all silent
make CPP=cpp check_rock_neo_only  # OK
overlay sweep: 196/205
```

<!-- Next sprint: append below this line -->

---

## Sprint: Phase 3 — Two-chunk TIM archives (2026-07-05)

**What we did:** Added overlay configs for four dual-chunk TIM archives modeled on
`SUB_WPN` layout (chunk0 @0 through `0x8800`, chunk1 header @`0x8800`, data
@`0x9000`): ST19B, ST17B, ST0B_00D, ST1E_06. Eight yaml files + four
`build.json`. Splat split + clean build on `overlay-expedition`.

### Lessons

1. **SUB_WPN two-chunk pattern generalizes** — `ovl0` end `[0x8800]`, `ovl1`
   start/end at header/data boundaries; `build.json` second chunk offset
   `34816` (`0x8800`). All four archives byte-match without a third yaml for
   trailing padding/terminator bytes (copy-and-emplace preserves tail).

2. **ST1E_06 staff roll TIMs** — slightly smaller payload (`0x8040` vs `0x8120`)
   but same header layout; end `0x10120` for ovl1.

3. **Overlay count now 200/205.** Remaining 5 are Phase 3b novel types
   (GAUGE type 10, EXIT_* type 9, EXIT_SUP 4-chunk, GAMEOVER multi).

### Verification

```
rm -rf build && make CPP=cpp
cmp disks/us/CDDATA/DAT/{ST19B,ST17B,ST0B_00D,ST1E_06}.BIN build/*.BIN  # all silent
make CPP=cpp check_rock_neo_only  # OK
overlay sweep: 200/205
```

<!-- Next sprint: append below this line -->

---

## Sprint: Phase 3b — Novel chunk types (2026-07-05)

**What we did:** Added overlay configs for the final five archives: GAUGE (type 1
+ type 10 headers), EXIT_SUB / EXIT_MAP (type 9 header-only + type 1 TIMs),
EXIT_SUP (mixed types 1/9/10 + VAB), GAMEOVER (TIM + two VABs + type-8 SEP).
28 yaml files + five `build.json`. Splat split + clean build on
`overlay-expedition`.

### Lessons

1. **Type 9 and 10 chunks are header-only (0x800 bytes)** when TIM dimension
   fields compute to zero data size. Yaml is `dashchunkheader` only, end at
   next chunk offset — no `code`/data subsegment.

2. **Type 10 duplicates type-1 asset references** — GAUGE01u appears as full
   type-1 chunk plus type-10 header stubs at `0x8800`; both must rebuild.

3. **GAMEOVER is four rebuildable chunks**, not two — second VAB (`m_gover.vab`
   @`0x24800`) and type-8 SEP (`m_gover.sep` @`0x4E800`) follow `e_gover.vab`;
   terminator + padding pass through after last chunk.

4. **Do not run `chunk2splatyaml.py` on the whole DAT folder** — it overwrites
   existing `build.json` entries and yaml for all 205 archives. Generate configs
   per-archive only.

5. **Overlay count now 205/205** — full DAT sweep byte-identical.

### Verification

```
rm -rf build && make CPP=cpp
cmp disks/us/CDDATA/DAT/{GAUGE,GAMEOVER,EXIT_MAP,EXIT_SUB,EXIT_SUP}.BIN build/*.BIN  # all silent
make CPP=cpp check_rock_neo_only  # OK
overlay sweep: 205/205
```

<!-- Next sprint: append below this line -->

---

## Sprint: Phase 4 — Check target + docs (2026-07-05)

**What we did:** Added `check_overlays` Makefile target (cmp sweep of all 205
CDDATA/DAT `.BIN` files); wired it into `make check`. Updated `CLAUDE.md`
build/verify section (205/205, lessons log pointer).

### Lessons

1. **`check_overlays` is the one-liner gate** — prints `205/205 overlays OK`
   or lists `MISSING`/`MISMATCH` names; exits non-zero on any failure.

2. **`make check` now includes overlay cmp** before the existing per-module
   sha1 loop; main exe still verified via `diff_rock_neo` + sha1.

3. **Expedition complete on `overlay-expedition`** — all 205 archives have
   configs and byte-match on clean rebuild.

### Verification

```
make CPP=cpp check_overlays   # 205/205 overlays OK
make CPP=cpp check_rock_neo_only  # OK
make CPP=cpp check            # OK (includes check_overlays)
```

<!-- Next sprint: append below this line -->
