# MML-D2-26 Fresh Setup Summary

**Date:** 2026-07-04  
**Local path:** `/home/X/Documents/MML-D2-26`  
**GitHub remote:** [JoshuaDav10/MML-D2-26](https://github.com/JoshuaDav10/MML-D2-26)

This document records what was done to bootstrap a clean Mega Man Legends decomp workspace from upstream, without reusing the old `MML1x10` / `MML_Decomp_Project` history.

---

## What was done

### 1. Cloned upstream

Cloned [ChrisNonyminus/mml1](https://github.com/ChrisNonyminus/mml1) with submodules into `~/Documents/MML-D2-26`:

- `tools/splat`
- `tools/maspx`
- `tools/asm-differ`

Upstream layout was **not** reorganized. Splat, the Makefile, and build paths depend on the existing directory structure.

### 2. Wired git remotes

| Remote | URL | Role |
|--------|-----|------|
| `upstream` | `https://github.com/ChrisNonyminus/mml1.git` | Chris's repo — pull fixes with `git fetch upstream && git merge upstream/master` |
| `origin` | `https://github.com/JoshuaDav10/MML-D2-26.git` | Your repo |

Local branch was renamed from upstream's `master` to `main` to match the empty GitHub repo.

### 3. Added a thin tracking layer (new files)

These files were added on branch `setup/initial-tracking-layer` and merged into `main`:

| File | Purpose |
|------|---------|
| `notes/DOCUMENT_INDEX.md` | Master index of external PSX/MIPS/MML reference docs |
| `notes/reference` | Symlink to `../../MML/Decompilation documents/` |
| `progress.md` | Separate **mapped** vs **matched** counters (starts at 0) |
| `activity.md` | Terse per-session log |
| `plan.md` | Phase checklist and git hygiene rules |
| `disks/README.md` | Instructions for local disc layout (no binaries committed) |

### 4. Adjusted `.gitignore`

Upstream rules were kept (`disks/`, `asm/`, `build/`, `assets/`, etc.). Added:

- Exception so `disks/README.md` can be tracked while `disks/*` stays ignored
- `*.cue`, `*.iso`, `*.img`, `*.elf`, `*.map`
- Broader `.vscode/` ignore (replaced single-file entry)

### 5. Committed locally

```
7f40a0b Add tracking layer and reference doc index for fresh decomp restart.
964bad3 update build system          ← upstream tip at clone time
```

### 6. Not done yet (manual steps)

- **`git push -u origin main`** — failed here due to missing GitHub credentials in the agent environment. Run locally to publish.
- **Baseline build** — `make extract_disk`, `make split_all`, `make` not run yet.
- **Disc files** — not copied into `disks/` (gitignored by design).
- **Old repo** — `MML1x10` / `MML_Decomp_Project` left untouched.

---

## File structure

Legend:

- **cloned** — from ChrisNonyminus/mml1 upstream
- **added** — created during this setup
- **adjusted** — modified during this setup
- **generated later** — gitignored; created by build, not present yet
- **external** — lives outside repo via symlink

```
MML-D2-26/
├── .clang-format                          [cloned]
├── .gitignore                             [adjusted]
├── .gitmodules                            [cloned]
├── Makefile                               [cloned]
├── README.md                              [cloned]
├── requirements.txt                       [cloned]
├── diff_settings.py                       [cloned]
├── mml1.us.xml                            [cloned]
│
├── SETUP.md                               [added] ← this file
├── activity.md                            [added]
├── plan.md                                [added]
├── progress.md                            [added]
│
├── notes/                                 [added]
│   ├── DOCUMENT_INDEX.md                  [added]
│   └── reference -> ../../MML/Decompilation documents/   [added, external symlink]
│
├── disks/                                 [gitignored except README]
│   └── README.md                          [added]
│
├── docs/                                  [cloned]
│   └── CHUNKS.md
│
├── bin/                                   [cloned]
│   ├── cc1-26
│   └── cc1-27
│
├── config/                                [cloned]
│   ├── splat.us.main.yaml
│   ├── splat.us.rock_neo.yaml             ← main executable function map
│   ├── syms.us.rock_neo.txt
│   ├── undefined_funcs_auto.us.rock_neo.txt
│   ├── undefined_syms_auto.us.rock_neo.txt
│   └── overlay/                           ← per-overlay splat configs
│       ├── splat.us.ST00/
│       ├── splat.us.ST01/
│       ├── … (~150+ overlay directories)
│       └── splat.us.ST1A/
│
├── hash/us/                               [cloned]
│   └── *.sha1                             (207 hash files)
│
├── include/                               [cloned]
│   ├── common.h
│   ├── include_asm.h
│   ├── macro.inc
│   ├── main.inc
│   ├── rock_neo.h
│   ├── types.h
│   ├── psxsdk/
│   │   ├── libapi.h
│   │   ├── libc.h
│   │   └── libgpu.h
│   └── rock_neo/
│       ├── cd.h, game.h, joy.h, moji.h, obj.h
│       ├── player.h, sce.h, sound.h, sub_scrn.h
│       └── Code800133D8.h
│
├── src/                                   [cloned]
│   ├── rock_neo/                          (~decompiled C + INCLUDE_ASM stubs)
│   └── ST1A/
│
├── tools/                                 [cloned]
│   ├── asm-differ/                        [submodule]
│   ├── maspx/                             [submodule]
│   ├── splat/                             [submodule]
│   ├── buildoverlay.py
│   ├── chunk2splatyaml.py
│   ├── chunkunpacker.py
│   ├── generate_rock_neo_syms.py
│   ├── m2ctx.py
│   ├── patchasm.py
│   ├── regexr.py
│   ├── rename_symbol.py
│   └── splat_ext/
│
├── asm/                                   [generated later, gitignored]
├── assets/                                [generated later, gitignored]
├── build/                                 [generated later, gitignored]
└── logs/                                  [generated later, gitignored]
```

### External reference corpus (not in repo)

Linked at `notes/reference/` → `/home/X/Documents/MML/Decompilation documents/`:

```
Decompilation documents/
├── mips_manual_chunks/        + INDEX.txt
├── PSXSPX/psxspx_sections/    + INDEX.txt
├── psyq/                      + MAIN_INDEX.txt
├── PSYQ_Documentation/
├── drhell_psx_research/
├── padua_psx_docs/
├── mml_specific_docs/
├── MML_Decompilation_Project_Overview.md   (historical — do not trust old % figures)
└── *.zip archives
```

---

## Git workflow going forward

```bash
# Publish (once authenticated)
cd ~/Documents/MML-D2-26
git push -u origin main

# Pull upstream fixes
git fetch upstream
git merge upstream/master

# Do real work on branches
git switch -c match/FUN_8006xxxx
# ... work, verify match, merge to main
```

---

## Next phase (from plan.md)

1. Install toolchain (mips cross-compiler, mkpsxiso, python venv)
2. Place US disc files in `disks/` (see `disks/README.md`)
3. `make extract_disk` → `make split_all` → `make`
4. Record baseline mapped/matched counts in `progress.md`

---

## Stage 1 fixes — diff loop + first match (2026-07-04, later session)

Version-drift fixes made while validating the asm-diff inner loop:

1. **asm-differ needs extra deps** — added `watchdog` and `levenshtein` to
   `requirements.txt` (`colorama` was already pinned). Without them `diff.py`
   exits at startup.

2. **`tools/asm-differ/diff.py` — LOCAL SUBMODULE PATCH (not committable
   here).** Python 3.14 removed `ast.Num`/`node.n`; asm-differ's integer-arg
   parser dies with "Failed to parse --base-shift". Fixed by replacing
   `ast.Num`/`.n` with `ast.Constant`/`.value` in `eval_()` (~line 1012).
   ⚠️ `git submodule update --checkout` will WIPE this fix — reapply if diff.py
   starts failing on `--base-shift` again. (Long-term: bump the submodule.)

3. **`diff.py` symlink at repo root** → `tools/asm-differ/diff.py`, so the
   documented `./diff.py <func>` invocation works.

4. **`tools/patchasm.py` — function reorder pass (the big one).** cc1-27 at
   `-O1+` defers ALL compiled C function bodies to the end of the translation
   unit, after every top-level `__asm__` (`INCLUDE_ASM`) block. Mixing real C
   with INCLUDE_ASM stubs therefore scrambled `.text` layout (this is what
   upstream's `ACCEPT_REORDERING_BULLSHIT` guards were about). cc1 does emit an
   in-place `.globl NAME` at each function's true source position, so patchasm
   now moves each deferred body (`.ent`→`.end` block without a `.include`) back
   up to its marker. Verified: full binary sha1 still matches with a real C
   function mid-file. Limitation: `static` functions have no `.globl` marker
   and would stay at the end — make them non-static or keep them at file end
   until that's needed.

5. **Make gotcha:** the `.c.o` objects are *intermediate files* to make —
   deleting them does NOT force a rebuild if the `.elf` is newer than the
   sources. After changing `tools/patchasm.py` (or other pipeline tools), run
   `touch src/rock_neo/*.c && rm -f build/rock_neo.elf` before `make CPP=cpp`.

First matched function: `func_800542FC` (moji.c) — `arg0[5]++` accessor,
verified byte-for-byte (sha1 `ffc08fd2...` holds).

## Verification hooks

After your first successful `make CPP=cpp`, install the local gates with
`make install_hooks`. This installs two symlinks into `.git/hooks/`.

**pre-commit** — if the staged change touches `progress.md`, `HANDOFF.md`,
`activity.md`, `notes/COUNTS.md`, or `src/rock_neo/`, it runs
`tools/check_docs.sh` and aborts the commit when a doc states a matched count
that disagrees with the built objects. Added after the 2026-07-26 audit, which
found the project's completion denominator had silently excluded 64.6% of the
binary for months while every "safeguard" reported success. Bypass deliberately
with `git commit --no-verify`.

**pre-push** — before every `git push`, the hook runs
`make CPP=cpp check_rock_neo_only` and `make CPP=cpp check_overlays` against
your current `build/` tree (it does not `rm -rf build`). If either gate fails,
the push is aborted with the gate name; rebuild and re-check, or use
`git push --no-verify` to skip the hook when you intentionally push WIP.

## Tooling — parts list

The matching-decomp toolchain used by this project:

| Tool | Location | Role |
|------|----------|------|
| `cc1-27` | `bin/cc1-27` | the actual GCC 2.7.2 compiler (in-repo; do NOT install a modern mipsel-gcc) |
| system `cpp` | via `CPP=cpp` | C preprocessor (required on every `make`) |
| maspsx | `tools/maspx/` | translates cc1 GNU-syntax asm → Sony ASPSX-compatible asm |
| gprel.py | `tools/gprel.py` | rewrites small-symbol refs to $gp-relative (sdata) access |
| patchasm.py | `tools/patchasm.py` | function-reorder pass (moves deferred C bodies back to their `.globl`) |
| splat | `tools/splat/` | ROM splitter / disassembler (produces the nonmatching asm + yaml) |
| asm-differ | `tools/asm-differ/` (`./diff.py`) | instruction-level diff of built vs original |
| m2c | `tools/m2c/` | decompiler — generates rough C drafts to iterate from |
| bytecmp.sh / tryfn.sh | `tools/` | scratch-TU pipeline: compile one function → .o → byte-compare vs splat .s |
| **decomp-permuter** | `tools/decomp-permuter/` | **automated matcher for the hard tail** (see below) |

### decomp-permuter (adopted 2026-07-12 for the giant walls)

Third-party tool by Simon Lindholm (`github.com/simonlindholm/decomp-permuter`),
the same author as asm-differ. It automates the semantics-preserving
guess-and-check that humans do by hand: it mutates a near-matching C function
(reorder statements, add temporaries, change types, etc.), recompiles each
variant, and scores its assembly against the target — thousands of variants,
purely on local CPU (no LLM/tokens). It is the standard community answer to
register-allocation / instruction-scheduling walls where the logic is already
correct but one value lands in the wrong register or slot — e.g. **`func_80053B40`**
(the OTPTR/0x1F800070 movable-swap; see `notes/wip/53B40_PROGRESS.md`).

**Status: SET UP and running (2026-07-12).** The clone is `.gitignore`d (it's an
embedded git repo; don't commit it). Reproduce the setup:

1. `git clone --depth 1 https://github.com/simonlindholm/decomp-permuter tools/decomp-permuter`
2. `pip install pycparser toml` (into `.venv`).
3. **Local patch** (like the asm-differ Py3.14 patch — not committable, lives in
   the ignored clone): in `tools/decomp-permuter/src/objdump.py`, add
   `"mipsel-elf-objdump"` as the first entry of the **mips** arch `executable=[...]`
   list (~line 211). Our toolchain uses `mipsel-elf-*`, not the tool's default
   `mips-linux-gnu-*`; without this the scorer errors with "Could not find any
   objdump executables."

Per-function working dir (example `tools/decomp-permuter/mml_53B40/`):
- `base.c` — our draft, preprocessed for pycparser:
  `cpp -Iinclude -undef -Wall -lang-c -fno-builtin -P -Dmips -D__GNUC__=2 … <draft>.c > base.c`
  (same `-D` set as `tryfn.sh`, plus `-P` to drop line markers).
- `target.o` — assemble the reference: prepend `.include "macro.inc"` to the
  splat `asm/rock_neo/nonmatchings/<mod>/<fn>.s`, then
  `mipsel-elf-as -Iinclude -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0 target.s -o target.o`.
- `compile.sh` — feeds preprocessed C straight into `cc1-27 | maspsx | gprel |
  patchasm | mipsel-elf-as` (the `tryfn.sh` pipeline minus `cpp`); invoked by the
  permuter as `./compile.sh input.c -o output.o`.
- `settings.toml` — `func_name = "<fn>"` and `compiler_type = "gcc"`.

Run: `python3 tools/decomp-permuter/permuter.py tools/decomp-permuter/<dir> -j 8 --stop-on-zero --better-only`.
It writes each improved candidate to `<dir>/output-<score>-N/` (a score of 0 =
byte-perfect). **A score-0 result is still only a lead**: copy its C back into the
real tree and confirm via `make CPP=cpp check_rock_neo_only` OK + mutation test
before claiming a match. Workflow value: the search runs token-free on CPU; the
model only re-engages to verify/land a result.
