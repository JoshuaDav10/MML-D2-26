# Overlay Expedition — plan for a delegated agent (Cursor)

Written 2026-07-05 by the main decomp session (Claude). This is a
self-contained brief for an agent working on the overlay build. Read it
fully before touching anything.

**Patch 2026-07-05 (Phase 0 investigation):** Classifications, phase names, and
imitation targets below were corrected after byte-level analysis. Details in
`notes/OVERLAY_EXPEDITION_LESSONS.md` (Sprint: Phase 0). Ground-truth counts
(205 / 186 / 19) unchanged.

## Ground truth (measured 2026-07-05, do not trust older notes)

The project docs used to say "ST** overlays don't link." That is STALE.
Measured state:

- `disks/us/CDDATA/DAT/` contains **205 original .BIN files**.
- `make CPP=cpp` builds **186** of them into `build/*.BIN`, and ALL 186
  are **byte-identical** to the originals (verified via `cmp` sweep).
- **19 files are simply missing a config** — no splat yaml under
  `config/overlay/`, so the build never attempts them:

  | file | size | type (measured) | imitate |
  |---|---|---|---|
  | FONT.BIN | 36864 | data: type-3 font.dat (1 chunk) | `splat.us.HEAD00` |
  | KAIFONT.BIN | 36864 | data: type-3 kaifont.dat (1 chunk) | `splat.us.FONT` |
  | MAP_A1/A2/B1/B2/B3.BIN | 69632 ea | data: type-1 TIM (1 chunk each) | `splat.us.HEAD00` |
  | BS16VAB1.BIN | 147456 | data: type-5 VAB (1 chunk) | `splat.us.HEAD00` |
  | ST0B_00E/01B.BIN | 94208/38912 | data: type-1 TIM (1 chunk + pad) | `splat.us.HEAD00` |
  | ST19B.BIN | 55296 | data: 2× type-1 TIM @0/@0x8800 | `splat.us.SUB_WPN` |
  | ST17B.BIN | 106496 | data: 2× type-1 TIM @0/@0x8800 + pad | `splat.us.ST19B` |
  | ST0B_00D.BIN | 108544 | data: 2× TIM + terminator | `splat.us.ST1E_06` |
  | ST1E_06.BIN | 350208 | data: 2× TIM + terminator | `splat.us.SUB_WPN` layout |
  | GAUGE.BIN | 43008 | data: type-1 + type-10 TIM | no template (type 10 novel) |
  | GAMEOVER.BIN | 327680 | data: TIM + VAB + more | `HEAD00` + `BS16VAB1` |
  | EXIT_MAP/SUB.BIN | 83968/40960 | data: type-9 TIM + raw tail | `splat.us.HEAD00` (type 9 novel) |
  | EXIT_SUP.BIN | 307200 | data: 4 chunks (types 1/10/1/9) | `GAUGE` + `EXIT_MAP` |

  **None of the 19 are code/progbin overlays** — all are dashchunk asset archives.

The goal of this expedition: **all 205 files build and byte-match**, with
a check target that proves it in one command.

## The mission, phased

### Phase 0 — Investigate (NO edits, produce a report)
1. Confirm the 19-missing list yourself:
   `for f in disks/us/CDDATA/DAT/*.BIN; do b=$(basename $f);
   [ -f "build/$b" ] || echo $b; done`
2. For each missing file, determine what it is. Tools you have:
   - Compare against a SIMILAR already-configured **data archive** (e.g.
     `splat.us.HEAD00` for single-chunk, `splat.us.SUB_WPN` for two-chunk
     `@0`/`@0x8800`). Do **not** assume `ST17B` ↔ `ST17` progbin similarity.
   - `xxd file | head`: PS-X EXE header? VAB/VAG magic? Pure pixel data?
   - How does the existing build handle *data-only* files elsewhere
     (look at how assets/ and the yaml `bin` segment type are used)?
3. Deliverable: a table (file → type → which existing config to imitate).
   STOP and get this reviewed before Phase 1.

### Phase 1 — One data-only file end-to-end (probably FONT.BIN)
1. Create its config by imitating the nearest existing one.
2. Wire it into the build the same way the other 186 are wired
   (study `Makefile` lines ~89-145: `ALL_BIN_YAML_FILES`,
   `link_overlay_chunk`, `get_overlay_name`).
3. Gate: `cmp disks/us/CDDATA/DAT/FONT.BIN build/FONT.BIN` — must be
   SILENT (byte-identical). Then run the two regression gates below.
4. Commit exactly this one file's config. STOP for review.

### Phase 2 — Remaining single-chunk data (fonts, maps, VAB, small TIM)
Same recipe as Phase 1 (`HEAD00` pattern): KAIFONT, MAP_A*/B*, BS16VAB1,
ST0B_00E, ST0B_01B. One commit per file or per obvious family.

### Phase 3 — Multi-chunk data archives (ST19B, ST17B, ST0B_00D, ST1E_06)
Per-chunk yaml + `build.json` offsets (often second chunk at **`0x8800`**).
Data subsegments only — no progbin/asm decomp. One archive per commit, each
byte-verified.

### Phase 3b — Novel / large data (GAUGE, EXIT_*, GAMEOVER)
Types **9** and **10** chunks and 3+ chunk archives. Highest risk; do last.

### Phase 4 — Check target
Add/extend a make target so `make CPP=cpp check` (or a new
`check_overlays`) byte-verifies ALL 205 BINs, and update CLAUDE.md's
stale "don't link" line. Final deliverable: one command, 205 OKs.

## Hard rules (non-negotiable)

1. **Branch**: all work on `overlay-expedition`, branched from `dev`.
   NEVER commit to `dev` or `main`. Never rebase/merge into them —
   review happens via PR/diff from the main session.
2. **Untouchables**: do not edit `src/rock_neo/`, `include/rock_neo/`,
   `notes/LESSONS.md`, `progress.md`, `activity.md`, `asm/rock_neo/`,
   anything under `disks/` (originals!), or `hash/`. Your surface is:
   `config/overlay/`, `config/splat.us.*.yaml` for NEW modules only,
   `Makefile` (additively), `tools/buildoverlay.py` only if provably
   necessary, and this file's checklist.
3. **Regression gates after EVERY change** (both must pass before any
   commit):
   - `source .venv/bin/activate && make CPP=cpp check_rock_neo_only`
     → must print `OK` (the main exe must never break).
   - The 186 pre-existing BINs must still all byte-match:
     `for f in disks/us/CDDATA/DAT/*.BIN; do b=$(basename $f);
     [ -f build/$b ] && ! cmp -s $f build/$b && echo BROKE:$b; done`
     → must print NOTHING.
4. **Success is `cmp` silence, never "looks right".** A file counts as
   done ONLY when `cmp disks/us/CDDATA/DAT/X.BIN build/X.BIN` exits 0 on
   a clean rebuild (`rm -rf build`, then full make). Paste the actual
   command + output in the commit message.
5. **No force pushes, no history rewrites, no `make clean`**
   (`make clean` deletes extracted asm/assets — use `rm -rf build`).
6. If stuck on one file for more than ~an hour of iteration, write down
   what was tried in the commit-message-style note and move to the next
   file. Partial coverage with honest notes beats thrashing.

## Verification protocol (how the human checks the homework)

Anyone reviewing can validate a claim with zero trust:

```bash
git checkout overlay-expedition
source .venv/bin/activate
rm -rf build && make CPP=cpp          # full clean build
make CPP=cpp check_rock_neo_only      # main exe: OK
# overlay sweep — count matches, list misses:
m=0; for f in disks/us/CDDATA/DAT/*.BIN; do b=$(basename $f);
  if cmp -s "$f" "build/$b" 2>/dev/null; then m=$((m+1));
  else echo "NOT MATCHED: $b"; fi; done; echo "$m/205 byte-identical"
```

The number can only go UP from 186. Any regression below 186, or a
non-OK main exe, is an automatic reject of the whole branch state.
