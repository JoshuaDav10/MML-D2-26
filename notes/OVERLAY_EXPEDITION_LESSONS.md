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
