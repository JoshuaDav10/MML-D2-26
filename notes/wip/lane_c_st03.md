# LANE C — ST03 stage conversion plan

Produced read-only from the working tree (no files in the repo touched, no build run).
Everything below that is labelled VERIFIED was checked against bytes or against the
tracked config in this checkout. Everything labelled UNVERIFIED needs a build to confirm.

---

## 0. TL;DR for the main thread

1. ST03.BIN and ST03B.BIN are **byte-identical over their whole configured region**
   (0x0..0x45000), which includes the entire code chunk. VERIFIED by hashing.
2. Overlay yaml offsets use **`rom_offset = vram - 0x800FF800`** (not `- 0x8000F800`,
   which is the *main exe* constant). Derivation and cross-check in §2.
3. Add three lines to `config/overlay/splat.us.ST03/ovl0__PROGBIN_R3_ST03.BIN.yaml`,
   the *same three lines* to `config/overlay/splat.us.ST03B/ovl0__PROGBIN_R3_ST03.BIN.yaml`,
   re-split those two yamls, and you have a working C TU with auto-generated
   INCLUDE_ASM stubs you can replace one at a time. Exact lines in §4.
4. Best first target: a **32-function contiguous run at rom 0x1864..0x2118**
   (vram 0x80101064..0x80101918) — all ≤30 instructions, **zero rodata**, and 22 of
   the 32 are the *same* 17-instruction template with one differing constant. Draft C
   for 24 of the 32 (= all 24 distinct bodies in the run) is in §7.
5. Honest win accounting is in §6. The headline stage count is **unique bodies**, so
   ST03B is *not* +340; it is a free second, independently-linked proof of the same C.

---

## 1. How ST1A/eve19 is wired (the precedent) — VERIFIED

Files that matter:

| thing | path | tracked by git? |
|---|---|---|
| overlay yaml | `config/overlay/splat.us.ST1A/ovl0__progbin_r3_st1a.bin.yaml` | yes |
| curated symbol names | `config/overlay/splat.us.ST1A/syms.us.ovl0__progbin_r3_st1a.bin.txt` | yes (14 lines) |
| generated symbols | `config/overlay/.../generated.syms.us.<chunk>.txt` | yes (rock_neo syms + curated) |
| the C | `src/ST1A/ovl0__progbin_r3_st1a.bin/eve19.c` (345 lines, 12/12 matched) | yes |
| per-function asm | `asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19/*.s` | **no — gitignored** |
| bulk asm | `asm/ST1A/ovl0__progbin_r3_st1a.bin/{EF8,2CAEC}.s` | **no — gitignored** |
| linker script | `./ST1A.ovl0__progbin_r3_st1a.bin.ld` (repo root) | **no — `*.ld` gitignored** |

`.gitignore` lines 2 and 6 are `*.ld` and `asm/`. So **`asm/` and the `.ld` are pure
splat output** — unlike the main-exe phase-0 procedure in
`notes/wip/PHASE0_SPLIT_TEMPLATE.md`, you do **not** hand-split the `.s` or hand-edit
the `.ld` for an overlay. You edit the yaml and re-run splat. (That template's steps 6
and 7 are main-exe-only; ignore them here.)

The ST1A yaml's code segment:

```yaml
  - name: ST1A_r3_st1a_0
    type: code
    start: 0x00000800
    vram: 0x80100000
    subalign: 4
    subsegments:
      - [0x800, rodata]
      - [0xEC8, .rodata, eve19]     # rodata TAIL carved out for the C file
      - [0xEF8, asm]                # -> asm/ST1A/.../EF8.s
      - [0x2C0E4, c, eve19]         # -> src/ST1A/.../eve19.c + nonmatchings/eve19/*.s
      - [0x2CAEC, asm]              # -> asm/ST1A/.../2CAEC.s
      - [0x2CB58, data]
  - [0x39FE4]
```

Generated `.ld` body (proves the placement order comes straight from the yaml):

```
ST1A_r3_st1a_0_RODATA_START = .;
build/asm/ST1A/ovl0__progbin_r3_st1a.bin/data/800.rodata.s.o(.rodata);
build/src/ST1A/ovl0__progbin_r3_st1a.bin/eve19.c.o(.rodata);      <-- carved tail
ST1A_r3_st1a_0_TEXT_START = .;
build/asm/ST1A/ovl0__progbin_r3_st1a.bin/EF8.s.o(.text);
build/src/ST1A/ovl0__progbin_r3_st1a.bin/eve19.c.o(.text);
build/asm/ST1A/ovl0__progbin_r3_st1a.bin/2CAEC.s.o(.text);
```

**A C file's `.rodata` can only ever be a *tail* of the rodata section**, because the ld
emits `asm rodata` then `c .rodata`. That is the single structural constraint on which
functions may share a C segment (see §3).

`tools/buildoverlay.py` (VERIFIED by reading):
- `list_src_files()` globs `asm/<A>/<chunk>/*.s`, `asm/<A>/<chunk>/data/*.s`,
  `src/<A>/<chunk>/*.c`, `assets/<A>/<chunk>/*.bin`. The `nonmatchings/` subdir is **not**
  globbed — it is only pulled in textually by `INCLUDE_ASM`.
- C rule: `cpp | ./bin/cc1-27 | maspsx | tools/patchasm.py | as`.
  **Note: no `tools/gprel.py`**, and its own `CC_FLAGS` uses **`-G0`** where the Makefile's
  main-exe `CC_FLAGS` uses `-G8`. That is correct for overlays (`_gp = 0x0` in the ld) but
  it is a *second, independent* copy of the flags — the file says so at line 33.
- Rebuild is content-hash driven (`build/<A>.<chunk>.src_hash.json`), and it re-links if
  `build/rock_neo.elf` changed.
- Final step only splices the built chunk bytes into a **copy of the original archive**,
  so unconfigured chunks can never break.

Mixed C + `INCLUDE_ASM` in an overlay C segment is **PROVEN**, not assumed:
`git show dab7b94:src/ST1A/.../eve19.c` is the splat-generated file with **12**
INCLUDE_ASM stubs, and the file went 12 → 11 → … → 0 stubs across four commits
(`c5c2ca8`, `5aa48a9`, `7c54eaa`, `f592053`) with the build green throughout.
So you can land ST03 one function at a time; you do **not** need the whole segment at once.

Splat will not clobber your work: `tools/splat/segtypes/common/c.py:186` creates the
`.c` only `if not c_path.exists()`.

---

## 2. The offset convention — SHOWN, VERIFIED

The disassembly listing carries both numbers, so the constant is directly measurable.
From `asm/ST1A/ovl0__progbin_r3_st1a.bin/EF8.s`, last line:

```
    /* 2C0C8 8012B8C8 0B80043C */  lui  $a0, %hi(D_800AD168)
       ^rom   ^vram
```

    constant = vram - rom = 0x8012B8C8 - 0x2C0C8 = 0x800FF800

Cross-check against the segment header, which is the *reason* for the constant:

    segment start (rom) = 0x800, segment vram = 0x80100000
    rom = vram - vram_base + start = vram - 0x80100000 + 0x800 = vram - 0x800FF800   ✓

Two independent confirmations from the ST1A yaml itself:

| yaml line | rom | + 0x800FF800 | symbol actually there |
|---|---|---|---|
| `- [0x2C0E4, c, eve19]` | 0x2C0E4 | **0x8012B8E4** | `StXX_eve19_move = 0x8012b8e4` (curated syms) ✓ |
| `- [0x2CAEC, asm]` | 0x2CAEC | **0x8012C2EC** | `glabel func_8012C2EC` is the first label in `2CAEC.s` ✓ |

**Contrast with the main exe** (`config/splat.us.rock_neo.yaml` lines 29–33):
`- [0x800, header]`, then `start: 0x800`, `vram: 0x80010000` →
`rom = vram - 0x80010000 + 0x800 = vram - 0x8000F800`. That matches the CLAUDE.md
landmine. `notes/wip/PHASE0_SPLIT_TEMPLATE.md` §2 still says
`file_offset = vma - 0x80010000` — **that line is wrong** and is presumably the origin of
the landmine note; worth a CORRECTION line in that file (I did not edit it).

ST03 uses the identical overlay header shape (`start: 0x00000800`, `vram: 0x80100000`), so:

> **ST03 overlay rule: `rom = vram − 0x800FF800`, i.e. `vram = rom + 0x800FF800`.**

ST03 ovl0 layout (from the yaml, VERIFIED against the asm):

```
0x00000 .. 0x00800   chunk header (dashchunkheader)
0x00800 .. 0x00CBC   .rodata   (vram 0x80100000..0x801004BC)   36 symbols, 35 of them jump tables
0x00CBC .. 0x14AE8   .text     (vram 0x801004BC..0x801142E8)   340 functions, 20,363 instructions
0x14AE8 .. 0x1D370   .data     (vram 0x801142E8..0x8011CB70)
```

---

## 3. ST03 == ST03B — VERIFIED MYSELF

```
disks/us/CDDATA/DAT/ST03.BIN    643,072 bytes  sha1 8a4a9211a11cb48945243859b804fce5a83216e8
disks/us/CDDATA/DAT/ST03B.BIN   718,848 bytes  sha1 3808f7ff378ffd4c8a3f5455ade751b572ab3704
```

The *archives* differ (different size, different sha1) — but the code chunk does not.
Byte-comparing region by region:

| region | ST03 sha1 (first 16) | ST03B sha1 (first 16) | verdict |
|---|---|---|---|
| ovl0 chunk header 0x0..0x800 | `94d08a6ac38c246b` | `94d08a6ac38c246b` | IDENTICAL |
| ovl0 rodata 0x800..0xCBC | `04a0b898f57fde7b` | `04a0b898f57fde7b` | IDENTICAL |
| ovl0 **text** 0xCBC..0x14AE8 | `564a0b32f60be1f2` | `564a0b32f60be1f2` | IDENTICAL |
| ovl0 data 0x14AE8..0x1D370 | `caa22df4956c6e68` | `caa22df4956c6e68` | IDENTICAL |
| ovl0 whole chunk 0x0..0x1D370 | `865ba94214e49e2b` | `865ba94214e49e2b` | IDENTICAL |

**First differing byte anywhere in the two files: offset 0x45000.** The last configured
chunk (`ovl4__SCR_DATA_ST03.HED`) ends at 0x44C78, so *all five configured chunks of both
archives are byte-identical*; the divergence is entirely in trailing, unconfigured
map/scene data. The two `build.json` files are literally identical text, and both
`ovl0__PROGBIN_R3_ST03.BIN.yaml` files are identical apart from the `ST03`/`ST03B` name
substitutions. Corroborated independently by `build/function_map.json`: ST03 and ST03B
each have 340 rows with **identical name lists and identical body-hash multisets**.

Consequence: **any C that matches ST03 matches ST03B with zero extra matching work** —
but it is *not* automatic. ST03B is a separate archive with its own
`asm/ST03B/…`, its own `ST03B.ovl0__PROGBIN_R3_ST03.BIN.ld`, and its own
`src/ST03B/…`. You must mirror the yaml edit and provide the source file (§5).

---

## 4. The exact yaml edit

Current ST03 ovl0 `subsegments` block:

```yaml
    subsegments:
      - [0x800, rodata]
      - [0xCBC, asm]
      - [0x14AE8, data]
  - [0x1D370]
```

### 4a. Recommended first cut — the 32-function run

```yaml
    subsegments:
      - [0x800, rodata]
      - [0xCBC, asm]
      - [0x1864, c, Code80101064]
      - [0x2118, asm]
      - [0x14AE8, data]
  - [0x1D370]
```

Arithmetic:
- first function of the run = `func_80101064` → `0x80101064 − 0x800FF800 = 0x1864`
- last function of the run  = `func_801018F8`, size 0x20 → end vram `0x80101918`
  → `0x80101918 − 0x800FF800 = 0x2118`

Splat will then emit `asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/CBC.s` (0xCBC..0x1864),
`.../2118.s` (0x2118..0x14AE8), `src/ST03/ovl0__PROGBIN_R3_ST03.BIN/Code80101064.c`
(32 INCLUDE_ASM stubs) and `asm/ST03/.../nonmatchings/Code80101064/*.s`.
**No `.rodata` subsegment is needed** — none of the 32 functions touches
0x80100000..0x801004BC (checked by scanning every `jtbl_` and every `%hi/%lo(D_8010xxxx)`
in the range).

### 4b. Optional zero-risk warm-up (prove the wiring in one build)

Two adjacent 2-instruction `jr $ra / nop` functions:

```yaml
      - [0x13AAC, c, Code801132AC]
      - [0x13ABC, asm]
```

`0x801132AC − 0x800FF800 = 0x13AAC`; `func_801132B4` is 8 bytes, ending at
`0x801132BC − 0x800FF800 = 0x13ABC`. If you do both 4a and 4b the block reads:

```yaml
    subsegments:
      - [0x800, rodata]
      - [0xCBC, asm]
      - [0x1864, c, Code80101064]
      - [0x2118, asm]
      - [0x13AAC, c, Code801132AC]
      - [0x13ABC, asm]
      - [0x14AE8, data]
  - [0x1D370]
```

### 4c. File naming

ST1A used Capcom's own filename (`eve19`) because the leaked/TCRF source gave it. **We
have no such name for ST03** — the curated syms file
`config/overlay/splat.us.ST03/syms.us.ovl0__PROGBIN_R3_ST03.BIN.txt` is **empty (0 lines)**,
so every ST03 symbol is currently `func_XXXXXXXX`. I recommend the offset-named
convention already used for main-exe phase-0 splits (`src/rock_neo/Code8002F9C4.c`):
**`Code80101064.c`**. Rename later if the real name turns up; the yaml name and the
filename must stay in sync.

Circumstantial evidence about what this file *is*: `func_801140F8`'s body hash is
**identical to `eve19_main_dummy`** (`evp->routine_0 = R0_END;`, i.e. `p->x8 = 1`), and
`asm/ST03/.../data/14AE8.data.s` contains a series of 10-entry routine tables
(`D_80114F7C`, `D_80114FA0`, …) whose entries are these functions padded with the engine
default `0x8001DEDC`. That is exactly eve19's `eve19_move_tbl` / `eve19_main_tbl` shape.
So ST03's program is almost certainly one or more `eveNN.c` files using the same
`EVE_WORK` struct that is already typed at the top of `src/ST1A/.../eve19.c`. **Reuse that
struct definition** rather than inventing a new one.

---

## 5. Step-by-step recipe

Baseline first — the repo's own rule is that a count claim needs a delta:

```bash
source .venv/bin/activate
tools/check_docs.sh                     # ~1s, must be clean before you start
python3 tools/gen_map.py                # refresh build/function_map.json
python3 tools/dups.py --archive ST03    # record the "already matched" number
```

Then:

1. **Back up the generated asm** (it is gitignored; a bad splat run has cost this project
   asm before — see `notes/wip/EVE19_RODATA_CARVE.md` step 2):
   ```bash
   cp -r asm/ST03 /tmp/asm_ST03.bak && cp -r asm/ST03B /tmp/asm_ST03B.bak
   cp ST03.ovl0__PROGBIN_R3_ST03.BIN.ld ST03B.ovl0__PROGBIN_R3_ST03.BIN.ld /tmp/
   ```

2. **Edit both yamls** — the same subsegment lines from §4 go into
   `config/overlay/splat.us.ST03/ovl0__PROGBIN_R3_ST03.BIN.yaml` **and**
   `config/overlay/splat.us.ST03B/ovl0__PROGBIN_R3_ST03.BIN.yaml`
   (note: ST03B's yaml really is named `…ST03.BIN.yaml`, not `…ST03B.BIN.yaml`).

3. **Refresh the generated syms** (the Makefile only does this inside `split_all`):
   ```bash
   for A in ST03 ST03B; do
     cat config/syms.us.rock_neo.txt \
         config/overlay/splat.us.$A/syms.us.ovl0__PROGBIN_R3_ST03.BIN.txt \
       > config/overlay/splat.us.$A/generated.syms.us.ovl0__PROGBIN_R3_ST03.BIN.txt
   done
   ```

4. **Re-split just those two chunks** (regenerates `asm/ST03*/…` and both `.ld`s):
   ```bash
   python3 tools/splat/split.py config/overlay/splat.us.ST03/ovl0__PROGBIN_R3_ST03.BIN.yaml
   python3 tools/splat/split.py config/overlay/splat.us.ST03B/ovl0__PROGBIN_R3_ST03.BIN.yaml
   ```
   Sanity-check the new `.ld` contains, in this order:
   `…/CBC.s.o(.text); …/src/ST03/…/Code80101064.c.o(.text); …/2118.s.o(.text);`

5. **Build the stub-only state and prove it is still byte-identical** before writing any C:
   ```bash
   make CPP=cpp chunks ; echo "exit=$?"      # READ THE EXIT CODE (see gotcha below)
   make CPP=cpp check_overlays               # must print 205/205 overlays OK
   ```
   *Gotcha, already burned once (EVE19_RODATA_CARVE.md):* a failed `make chunks` leaves the
   previous BINs in place and `check_overlays` then reports **205/205 on stale files**.
   Always read the exit code before believing the gate.

6. **Replace stubs with C one at a time**, in ascending address order, rebuilding after
   each (or each small batch). Drafts in §7.

7. **Mirror to ST03B.** Once a function is real C in ST03, the ST03B copy must contain the
   same text. Two options:
   - *Simplest, safest:* keep `src/ST03B/ovl0__PROGBIN_R3_ST03.BIN/Code80101064.c` as a
     literal copy with the ST03B INCLUDE_ASM paths, produced mechanically:
     `sed 's/ST03\//ST03B\//g; s/splat\.us\.ST03/splat.us.ST03B/g' src/ST03/…/Code80101064.c > src/ST03B/…/Code80101064.c`
     (add a tiny make/CI check that the two files differ only in those tokens).
   - *Single source of truth:* once the file has **no INCLUDE_ASM left**, make the ST03B
     path a git symlink to the ST03 file. `list_src_files()` uses `os.listdir` + `open()`,
     both of which follow symlinks, and git tracks symlinks natively. **UNVERIFIED** — I
     could not build to confirm; try it only after the file is pure C.

8. **Verify and count.**
   ```bash
   make CPP=cpp chunks && echo ok         # exit code!
   make CPP=cpp check_overlays            # 205/205
   cmp build/ST03.BIN  disks/us/CDDATA/DAT/ST03.BIN
   cmp build/ST03B.BIN disks/us/CDDATA/DAT/ST03B.BIN
   python3 tools/gen_map.py               # re-derives MATCHED from each overlay's own .map
   ```
   `tools/gen_map.py` proves linkage from `build/<ARCHIVE>.<chunk>.map`, which is the
   overlay analogue of the `rock_neo.ld` phantom guard — so a `.c.o` the linker ignored
   will not be counted. `tools/census.py --matched` will **not** see any of this (it globs
   only `build/src/rock_neo/*.o`); the stage number comes from `gen_map.py`, exactly as
   `notes/COUNTS.md` says.

**Do not** run `make clean` (deletes `asm/` and `assets/`). **Do not** run
`tools/audit_count.sh` casually — it does a clean rebuild and deletes the overlay BINs
(`make CPP=cpp chunks` afterwards or `check_overlays` fails).

---

## 6. What ST03 is worth — honest accounting

From `build/function_map.json` (340 ST03 rows) and `tools/dups.py`:

```
ST03 function instances     : 340
ST03 distinct bodies        : 309
size: min 2, median 41, mean 60, max 495; total 20,363 instructions
  ≤10 insn  40      31-50  53      121-200  31
 11-20 insn  66      51-80  58      201-400   8
 21-30 insn  35     81-120  47       401+     2
```

### 6a. "Already matched elsewhere" is 9 instances but only **1 new unique body**

`tools/dups.py --archive ST03` prints `already matched elsewhere : 9`. Those nine are:

| ST03 function | insns | identical to |
|---|---|---|
| `func_80101158`, `func_801012BC`, `func_801048D8`, `func_80104CFC`, `func_8011027C`, `func_801132AC`, `func_801132B4`, `func_801133AC` | 2 each | `func_8003FFA8` (`src/rock_neo/player.c:452`, `void func_8003FFA8(void) {}`) |
| `func_801140F8` | 3 | `eve19_main_dummy` (ST1A) |

Eight of them share **one** body. The headline stage figure in `notes/COUNTS.md`
("Matched — STAGE (unique bodies) 12") is computed by `gen_map.py` as
`len({body for STAGE rows with state==MATCHED})`, so:
- the eight empties together add **+1** unique stage body (the empty body is currently
  matched only in the ENGINE realm, not the STAGE realm);
- `func_801140F8` adds **+0** (its body is already in the STAGE matched set via
  `eve19_main_dummy`).

So the true "free win from already-solved bodies" is **+1 unique body for ~10 minutes of
work**, not +9. Flagging this because it is exactly the class of number this project has
over-claimed before.

### 6b. ST03B is free, but it is not +340 on the headline

ST03B's 340 rows carry the *same* body hashes, so they add **0** unique bodies. What
ST03B buys is real but different: 340 extra *function instances* covered (680 total), and
a second, independently-linked overlay that must also come out byte-identical — a free
correctness double-check on every line of C you write.

**Also flagging:** `tools/dups.py --archive ST03` reports
`also present in other archives : 340`, which reads as "everything in ST03 is shared".
It is not — that count includes ST03B. Excluding the twin, **67 of 340 ST03 instances
(43 distinct bodies) have an exact twin outside ST03/ST03B**; 273 instances live only in
ST03+ST03B.

### 6c. The real leverage: 43 bodies that show up in ~40 other archives

Those 43 shared bodies cover **1,119 instances outside ST03/ST03B** (757 excluding the
trivial empty body). Top ones, all small:

| ST03 name | insns | external instances | external archives |
|---|---|---|---|
| `func_80101158` (empty) | 2 | 362 | 41 |
| `func_80107F3C` | 8 | 131 | 32 |
| `func_801083DC` | 10 | 66 | 22 |
| `func_80110128` | 8 | 48 | 19 |
| `func_8010FB18` | 8 | 41 | 18 |
| `func_80104B40` | 16 | 30 | 19 |
| `func_801080E4` | 7 | 23 | 22 |
| `func_8010A4EC` / `func_8010A44C` / `func_8010A224` / `func_8010A02C` / `func_80109FD4` / `func_80109E94` / `func_80109358` / `func_80108CF4` / `func_80108828` / `func_80108648` / `func_80108404` | 4–53 | 22 each | 22 each |

These are the ones to prioritise *after* the easy run: each is a small function whose C,
once written, is a direct copy-paste into 18–41 other archives when those get converted.

### 6d. Expected win from the §4a segment

The 32-function run contains **24 distinct bodies**, none currently in the STAGE matched
set → **+24 unique stage bodies** (12 → 36, a 3× jump on the stage count), covering 32
ST03 instances + 32 ST03B instances. Duplicate pairs inside the run (write once, the
second is a literal re-type):
`801010C8≡80101160`, `80101114≡801011AC≡80101308`, `80101158≡801012BC`,
`8010156C≡80101638`, `801015B0≡8010167C`, `801015F4≡801016C0`, `80101778≡801017D8`.

Whole-archive ceiling: 309 distinct bodies, 1 already matched → **up to +308 unique stage
bodies** from ST03 alone (vs. 12 today).

---

## 7. Draft C — the 15 easiest, plus the whole 32-run

All listings below were transcribed from
`asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/CBC.s` in this checkout. **UNVERIFIED against the
compiler** — I could not build. Treat as first drafts.

### 7.0 File preamble

```c
#include "common.h"

/* Work struct: offsets recovered from the ST03 asm. Field 0x08 lines up with
   eve19's EVE_WORK.routine_0 (func_801140F8 has the same body hash as
   eve19_main_dummy), so this is very likely Capcom's EVE_WORK. Only the fields
   ST03 touches are typed. */
typedef struct EVE_WORK {
    u8  pad0[0x3];   /* 0x00 */
    u8  type;        /* 0x03 */
    u8  x4;          /* 0x04 - "already fired" latch, set to 0xFF */
    u8  pad5[0x3];   /* 0x05 */
    u8  routine_0;   /* 0x08 */
    u8  routine_1;   /* 0x09 */
    u8  routine_2;   /* 0x0A */
} EVE_WORK;

/* --- ST03-internal --- */
void func_80103AB0(s32 idx);
void func_80101B70(void);

/* --- engine --- */
s32  Sce_flag_test(s32 id);
void Sce_flag_on(s32 id);
void *func_80031418(void);
void func_8001EB98(void *p);

extern u8 D_801151DC[];   /* 0x801151DC, in the .data subsegment */
```

### 7.1 The 17-instruction template (22 functions) — the big win

Every one of these is byte-for-byte the same except the constant:

```
addiu $sp,-0x18 / sw $s0,0x10 / addu $s0,$a0,$zero / sw $ra,0x14
lbu $v0,0x4($s0) / nop / bnez $v0,.L / nop
jal func_80103AB0 / addiu $a0,$zero,<K>
addiu $v0,$zero,0xFF / sb $v0,0x4($s0)
.L: lw $ra,0x14 / lw $s0,0x10 / addiu $sp,0x18 / jr $ra / nop
```

```c
void func_80101114(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x15);
        evp->x4 = 0xFF;
    }
}
```

Constants, in file order (this is the whole run; write them in **this order** — the C
file's function order must match the segment layout):

| function | K | function | K | function | K |
|---|---|---|---|---|---|
| `func_80101114` | 0x15 | `func_8010134C` | 0x1A | `func_801014E4` | 0x20 |
| `func_801011AC` | 0x15 | `func_80101390` | 0x1B | `func_80101528` | 0x21 |
| `func_801011F0` | 0x17 | `func_801013D4` | 0x1C | `func_8010156C` | 0x22 |
| `func_80101234` | 0x18 | `func_80101418` | 0x1D | `func_801015B0` | 0x23 |
| `func_80101278` | 0x19 | `func_8010145C` | 0x1E | `func_801015F4` | 0x24 |
| `func_801012C4` | 0x13 | `func_801014A0` | 0x1F | `func_80101638` | 0x22 |
| `func_80101308` | 0x15 |  |  | `func_8010167C` | 0x23 |
|  |  |  |  | `func_801016C0` | 0x24 |

Two-call variant (19 insns), `func_801010C8` and `func_80101160`, identical bodies:

```c
void func_801010C8(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x13);
        func_80103AB0(0x25);
        evp->x4 = 0xFF;
    }
}
```

### 7.2 The rest of the 32-run

```c
/* 0x80101064, 25 insns — func_80031418 returns a pointer to a small record */
void func_80101064(EVE_WORK *evp) {
    u8 *p;
    if (evp->x4 == 0) {
        p = func_80031418();
        if (p != NULL) {
            p[0] = 1;
            p[2] = 9;
            p[4] = 2;
        }
        evp->x4 = 0xFF;
    }
}

/* 0x80101158 and 0x801012BC, 2 insns each */
void func_80101158(EVE_WORK *evp) {}
void func_801012BC(EVE_WORK *evp) {}

/* 0x80101778 and 0x801017D8, 24 insns, identical bodies */
s32 func_80101778(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        if (Sce_flag_test(0x235)) {
            Sce_flag_on(0x7E2);
            Sce_flag_on(0x7E3);
            Sce_flag_on(0x7E4);
            Sce_flag_on(0x7E8);
        }
    }
    Sce_flag_on(0x1FF);
    return 0;
}

/* 0x80101838, 30 insns */
s32 func_80101838(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        if (Sce_flag_test(0x238)) {
            if (Sce_flag_test(0x239)) {
                func_80101B70();
            } else {
                Sce_flag_on(0x239);
                func_8001EB98(D_801151DC);
            }
        }
    }
    Sce_flag_on(0x1FF);
    return 0;
}

/* 0x801018B0, 18 insns */
s32 func_801018B0(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        if (Sce_flag_test(0x239)) {
            func_80101B70();
        }
    }
    Sce_flag_on(0x1FF);
    return 0;
}

/* 0x801018F8, 8 insns — LAST function of the segment */
s32 func_801018F8(void) {
    Sce_flag_on(0x1FF);
    return 0;
}
```

**`func_80101704` (29 insns) is the one awkward member of the run.** Its asm is

```
lbu $v1,0x4($a0) / beqz $v1,.L2C / addiu $v0,$zero,0xFF   (delay slot)
beq $v1,$v0,.L68 / addiu $v0,$zero,0x1                    (delay slot, then DEAD)
j .L5C / nop
.L2C: <the Sce_flag_test(0x235) block, as in func_80101778>
.L5C: jal Sce_flag_on(0x1FF) / addu $v0,$zero,$zero
.L68: lw $ra / addiu $sp / jr $ra
```

i.e. the `x4 == 0xFF` arm returns with `$v0` still holding the comparison constant 0xFF,
and the `li $v0,1` in the `beq` delay slot is dead. That is a compiler artifact I cannot
confidently reproduce from a clean source shape. **Leave it as `INCLUDE_ASM` in the first
pass**; the segment still builds (proven by eve19's 12→0 stub progression). Come back to
it with `./diff.py` once the other 31 are green.

### 7.3 Easy singletons outside the run (each needs its own tiny c segment, or wait
until you convert its neighbourhood)

```c
/* 0x801132AC / 0x801132B4 / 0x801133AC / 0x801048D8 / 0x80104CFC / 0x8011027C, 2 insns */
void func_801132AC(void) {}

/* 0x801140F8, 3 insns — body-identical to eve19_main_dummy */
void func_801140F8(EVE_WORK *evp) { evp->routine_0 = 1; }

/* 0x8010F238, 4 insns */
void func_8010F238(EVE_WORK *evp) { evp->routine_0 = 1; evp->routine_1 = 0; }

/* 0x8010A44C, 4 insns — andi 0xFF / xori 0x11 / sltiu 1 */
s32 func_8010A44C(u8 a) { return a == 0x11; }

/* 0x801071E0, 5 insns */
void func_801071E0(u8 *p) { *p &= ~2; }

/* 0x801080E4, 7 insns — lb (SIGNED) at 0xA7 */
void func_801080E4(WORK *p) { if (p->xA7 < 0) { p->xA = 0; } }

/* 0x80107F3C, 0x8010A8D4, 0x8010D3C4 — 8 insns each, identical */
void func_80107F3C(void) { func_80031824(); }
   /* NOTE: `void f(WORK *p) { func_80031824(p); }` produces the same bytes —
      $a0 is already in place, so the two are indistinguishable from the asm.
      This body has 131 twins in 32 other archives; worth getting the
      *semantics* right even though either spelling matches. */

/* 0x8010A8F4 (8 insns) and 0x8010D3E4 (see caveat in §8) */
void func_8010A8F4(WORK *p) { if (p->xA == 0) { p->xA++; p->xAC = 0; } }

/* 0x80103A88, 10 insns */
void func_80103A88(void) { func_8001EC0C(); func_8001F070(); }

/* 0x801083DC, 0x80108CCC, 0x80109288 — 10 insns each, identical.
   66 external twins in 22 archives. u16 counter at 0x350. */
void func_801083DC(WORK *p) { if (--p->x350 == 0) { p->xA = 1; } }
   /* the `addiu $v0,$zero,1` sits in the bnez delay slot — LESSONS "assignment
      placed before a branch lands in that branch's delay slot". If u16 gives
      `andi 0xffff` instead of the observed `sll 16`, try s16. */

/* 0x8010C204, 8 insns */
void func_8010C204(WORK *p, u8 on) { if (on) { p->x107 = 0; } else { p->x107 = -1; } }

/* 0x8010C224, 10 insns */
void func_8010C224(WORK *p, u8 on) {
    if (on) { p->xFF = 0;  p->x103 = 0;  }
    else    { p->xFF = -1; p->x103 = -1; }
}
```

`func_80112300` / `func_80112324` (9 insns each) look trivial but are **not** — they
contain `addu $a1,$a0,$zero` then `addiu $a0,$a1,0xCC`, a surviving register-to-register
copy. Per `notes/LESSONS.md` ("When does a register-to-register copy survive?") that is a
reload live-range split, not something you can write in C. Skip them in the first pass.

---

## 8. Flags — things I could not verify, and traps

1. **`func_8010D3E4` / `func_8010D400` have an ambiguous boundary.** spimdisasm gives
   `func_8010D3E4` size 0x1C (7 instructions) ending on a bare `jr $ra` with **no delay
   slot**, and starts `func_8010D400` with a lone `nop` before its real prologue at
   `0x8010D404`. There *is* a genuine `jal func_80103400`-style call to `0x8010D400`
   (`CBC.s` line 5530: `jal func_8010D400`), so the entry symbol is real. The body is
   otherwise byte-identical to `func_8010A8F4` (8 insns, with the nop). **Do not put
   either function in an early c segment** until someone decides which one owns the nop —
   getting it wrong shifts every subsequent byte.
2. **`func_80101704`** — dead `li $v0,1` in a delay slot, return value is a reused
   comparison constant. Left as INCLUDE_ASM in the plan.
3. **Rodata is strictly text-ordered in ST03** (verified: `jtbl_80100004` → the first
   text function `func_80101D54`; `jtbl_801004A0` → the *last* text function
   `func_80114140`; all 35 tables in between line up monotonically). Consequence: **a c
   segment containing any function with a `switch` jump table must be a suffix of .text**,
   because its `.rodata` can only be appended after the asm rodata. The §4a run has no
   jump tables, so this does not bite yet — but it will constrain the second and third
   segments. The suffix candidates and their rodata carves are:
   - `func_80114140` alone → carve `- [0xCA0, .rodata, <name>]`
   - `func_801138D8`..end → carve `- [0xC88, .rodata, <name>]`
   - `func_80111524`..end → carve `- [0xC58, .rodata, <name>]`
4. **Everything in §7 is UNVERIFIED against the compiler.** No build was run in this lane.
5. **The symlink idea for ST03B (§5 step 7) is UNVERIFIED.** The mechanical `sed` copy is
   the safe path.
6. **`notes/wip/PHASE0_SPLIT_TEMPLATE.md` §2 states `file_offset = vma - 0x80010000`,
   which is wrong** (it omits the 0x800 header; the correct main-exe constant is
   `0x8000F800`, per CLAUDE.md). I did not edit the file. Worth a visible CORRECTION line
   rather than a retro-edit, per the repo's own no-retro-edit rule.
7. **`tools/dups.py --archive ST03`'s "also present in other archives: 340" is
   misleading** — it counts ST03B. The honest figure is 67 instances / 43 bodies. Worth a
   one-line fix in `tools/dups.py` (exclude byte-identical twin archives, or report both
   numbers).
8. **I did not run any build, splat, `make`, or `audit_count.sh`**, per the lane
   constraints. No file in `/home/X/Documents/MML-D2-26` was created, edited or deleted.
   The only files I wrote are `/tmp/lane_c_st03.md` (this file) and
   `/tmp/st03_funcs.txt` (a scratch JSON dump of the 340 ST03 function
   name/size/rom/vram tuples — handy for the main thread).
