# MML Decomp — Project Checkpoint & Handoff

> **Purpose of this file:** a compact, self-contained brief so a fresh Claude
> instance can pick up this project with full context and minimal token cost.
> Read this first; do not re-derive it from scratch.

**Last updated:** 2026-07-04
**Status:** ✅ Stage 0 complete — main executable builds byte-for-byte perfect.

---

## 1. What this project is

Decompilation of **Mega Man Legends (PSX, US release)** — internal name
Rockman NEO, binary `ROCK_NEO.EXE`. Goal: reconstruct C source that recompiles
into a **byte-for-byte identical** binary (matching decompilation).

- **Base repo:** fork of `ChrisNonyminus/mml1` (a ~2022-era WIP splat project).
- **User's repo:** `github.com/JoshuaDav10/MML-D2-26`
- **Local path:** `~/Documents/MML-D2-26` (ext4 — no filesystem issues)
- **Environment:** Arch Linux, Python 3.14, in a venv at `.venv/`

The eventual aim is an LLM-assisted workflow (Claude Code) where a strong model
orchestrates and cheap models do mechanical grunt work. **That part is NOT set
up yet** — Stage 0 was just getting the repo to build. See §7.

---

## 2. CURRENT CHECKPOINT (the latest state)

**Stage 0 (get a matching baseline) is DONE.**

- `make extract_disk` ✅ — disc unpacked to `disks/us/` (ROCK_NEO.EXE, SLUS_006.03, CDDATA, etc.)
- `make split_all` ✅ — splat disassembled everything into `asm/`
- `make CPP=cpp` ✅ — main executable compiles
- **`build/rock_neo.exe` sha1 == original `disks/us/ROCK_NEO.EXE` sha1**
  → `ffc08fd2b6113e84dc0e75ba63a4feaca0eb1d46` — **PERFECT MATCH.**
- Verify target: `make CPP=cpp check_rock_neo_only`

**Known-incomplete:** the ~150 overlays (`ST00`–`ST1A`) do NOT link yet —
`undefined reference` errors (e.g. `eve19_main`, `StXX_eve19_move`, jtbl labels).
This is a separate, later expedition and is NOT required to begin decomp work on
the main executable. Do not treat overlay link errors as a regression.

---

## 3. How to build (exact working commands)

```bash
cd ~/Documents/MML-D2-26
source .venv/bin/activate          # REQUIRED every new shell (venv has splat deps)
make CPP=cpp                        # build (main exe matches; overlays error — expected)
make CPP=cpp check_rock_neo_only    # verify main exe against official hash
```

**Gotchas that will bite a fresh session:**
- If you see `ModuleNotFoundError: rabbitizer` → the venv isn't active. `source .venv/bin/activate`.
- The `CPP=cpp` override is currently required (see §4). Plain `make` fails with
  `mipsel-elf-cpp: command not found` unless the Makefile has been patched.
- To force a rebuild use `rm -rf build` — do **NOT** use `make clean` casually:
  it also deletes `asm/` and `assets/`, forcing a full re-extract + re-split.

---

## 4. Fixes applied to make the 2022 repo build on a 2026 system

The whole Stage 0 struggle was **version drift**: an old repo meeting new tools.
None of these changed game code; they bridge toolchain gaps.

1. **`requirements.txt`** — original didn't list `rabbitizer`/`spimdisasm`
   (they live in splat's own requirements). Pinned the working set via
   `pip freeze`: `rabbitizer==1.16.2`, `spimdisasm==1.42.2`.

2. **`include/macro.inc`** — modern spimdisasm (1.42.2) emits marker directives
   the old `macro.inc` never defined. Added three no-op macros (they only let
   the assembler skip the markers; `glabel` on the following line does the real
   labeling):
   ```
   .macro nonmatching name, size
   .endm
   .macro enddlabel name
   .endm
   .macro alabel name
   .endm
   ```

3. **`CPP=cpp` override** — `mipsel-elf-binutils` provides binutils but NO
   compiler/preprocessor (the `mipsel-elf-gcc` AUR package fails to build on
   GCC 15). The game's own compiler `bin/cc1-27` (64-bit, runs natively) handles
   compilation; the preprocessor step just needs a plain `cpp`, so we point
   `CPP` at the system `/usr/bin/cpp`. **Not yet made permanent** — still passed
   on the command line. A one-line Makefile edit (`CPP := $(CROSS)cpp` →
   fallback to `cpp`) would remove the need to type it.

---

## 5. Toolchain inventory (what's installed and works)

| Role | What's used | Notes |
|------|-------------|-------|
| C compiler | `./bin/cc1-27` (in repo) | 64-bit ELF, runs natively. **Do not** need `mipsel-elf-gcc`. |
| Assembler/linker | `mipsel-elf-binutils` (AUR, installed) | `mipsel-elf-as`, `-ld`, `-objcopy`, etc. |
| Preprocessor | system `/usr/bin/cpp` | via `CPP=cpp` override |
| Disc tool | `mkpsxiso-git` (AUR, installed) | used by `make extract_disk` |
| Splat + libs | `tools/splat` submodule + venv | `spimdisasm 1.42.2`, `rabbitizer 1.16.2` |
| Diff | `tools/asm-differ` submodule | `./diff.py` — not yet exercised |
| maspsx | `tools/maspx/maspsx.py` | wraps cc1 output for `.c` builds |

`mipsel-elf-gcc` (AUR) **FAILS to build** on GCC 15.1.0 (`libcody` error).
It is **not needed** — don't waste time on it.

---

## 6. Repo layout (essentials only)

```
MML-D2-26/
├── Makefile                         # build recipe (line 266 = .c pipeline, 263 = .s)
├── requirements.txt                 # [modified] pinned deps
├── mml1.us.xml                      # disc layout; also needs a copy in disks/
├── config/
│   ├── splat.us.rock_neo.yaml       # MAIN function map
│   └── overlay/splat.us.ST**/       # ~150 overlay configs (not linking yet)
├── include/
│   ├── macro.inc                    # [modified] added 3 compat macros
│   └── rock_neo/*.h                 # game headers
├── src/rock_neo/                    # decompiled C + INCLUDE_ASM stubs
├── asm/                             # [generated by split_all, gitignored]
├── disks/                           # [gitignored] disc + extracted us/ tree
├── build/                           # [gitignored] output; rock_neo.exe lives here
├── hash/us/*.sha1                   # expected hashes (ground truth)
└── tools/                           # splat, asm-differ, maspx, helper scripts
```

Disc files (user-supplied, gitignored): `disks/mml1.us.track1.bin`,
plus `disks/mml1.us.xml` (copied from repo root — extract step needs it there).

---

## 7. What's next (NOT started)

The real objective beyond a building repo: an **LLM-assisted decomp workflow in
Claude Code**, with two explicit user constraints —
1. **Minimize premium-model (Fable) token usage** without hurting quality.
2. **Avoid the safeguards routing** that can redirect some Fable queries to Opus.

Planned shape (design only — no artifacts created yet):
- **Fable/strong model = orchestrator + judgment**: decide which function to
  tackle, read a diff, reason about why C isn't matching, plan the fix.
- **Cheap subagents (Haiku/Sonnet) = mechanical grunt work**: grep for unmapped
  functions, add YAML entries, create `INCLUDE_ASM` C stubs, run `make`/`diff.py`,
  report back. Kept in isolated context so their output doesn't bloat the main
  window.
- Artifacts to be produced later: `.claude/agents/*.md` subagent defs (with
  `model:` routing), a matching-workflow `SKILL.md`, a project `CLAUDE.md`, and
  a slash command or two.

**Important correction for any instance picking this up:** the user's older
strategy docs claim Fable has a "50% weekly usage gate," "auto-downgrades to
Opus for reverse-engineering/code work," and a "distillation trap." Per
Anthropic's own material these are inaccurate/exaggerated. The real mechanism is
a **session-level safeguards routing that triggers in <5% of sessions**, tuned
around genuinely sensitive topics (e.g. cyber). Legitimately decompiling a
legally-owned 1997 game is not the kind of thing it targets. Do **not** build the
workflow around defeating a threat that mostly doesn't exist (e.g. banning the
model from writing code, constant thread-recycling). Do apply real token hygiene:
index-driven context, don't paste raw hex/asm dumps, delegate boilerplate to
cheap models, keep the main context tight.

---

## 8. Decomp concepts (quick primer for a fresh instance)

- **Matching decomp:** rewrite each function in C so that compiling it yields the
  *exact same machine code*. Proven by hash/diff, not by "looks right."
- **`INCLUDE_ASM` stub:** a `.c` file that just pulls in the function's raw asm.
  A function in this state is *mapped* but *not decompiled*. Mapping is the easy
  5%; matching is the other 95%. **Track "mapped" and "matched" as separate
  numbers** — conflating them badly overstates progress (the old overview's
  "1,930 mapped / 85%" is a mapped count, not matched).
- **Inner loop:** pick a function → write/adjust C → `make` → `./diff.py FUNC`
  to compare against original → iterate until it matches → commit.
- **Git hygiene:** real work on branches (`git switch -c match/FUN_xxxx`), keep
  `main` clean, commit each matched function or logical fix separately.

---

## 9. TL;DR for a new Claude instance

The repo **builds and the main executable matches perfectly** (sha1
`ffc08fd2...`). To build: activate the venv, `make CPP=cpp`. Version-drift fixes
(3 macros in `macro.inc`, pinned `requirements.txt`, `CPP=cpp` for the missing
preprocessor) are done and committed. Overlays don't link yet — that's expected,
leave for later. Next task is setting up a Claude Code orchestration workflow
(strong model plans, cheap subagents do grunt work) with honest token hygiene —
not the exaggerated "Fable downgrade/gate" defenses from the old strategy docs.
