# Counts — reconciliation of the several numbers this project quotes

**REGENERATE THIS FILE whenever the matched count changes.** It exists only to hold
numbers, so a stale entry here is simply wrong. `tools/check_docs.sh` fails if this file
does not contain the current count.

Last verified: **2026-07-26**, HEAD on `dev`, via `tools/audit_count.sh`
(clean `rm -rf build` + full-binary sha1 + raw `cmp` + `tools/census.py --matched`).
Note: func_8002F9C4 was (1) split from raw asm into a C stub via the phase-0 split
template, then (2) matched to real C. Splitting MOVES a function between buckets — it
does NOT change the total: unsplit 635→634, C-slice 484→485, **total stays 1119.**

| Number | Value | Meaning |
|---|---|---|
| `F .text` in all rock_neo `.o` | **487** | Functions currently split into C TUs. **NOT the mission denominator** — see below. |
| Functions still unsplit (raw asm) | **632** | `glabel`s in `asm/rock_neo/*.s`, never split into a C TU. |
| **Game functions in ROCK_NEO.EXE** | **1119** | 487 + 632. The MAIN-EXE denominator. |
| **Whole-game functions** | **~8,000** | 1119 main exe + ~7,010 unique overlay funcs (measured 2026-07-26, `notes/WORK_MAP.md` §7). THE whole-game denominator. |
| Matched (real C in tree) | **282** | `487 − 205`. Authoritative; hash OK + raw cmp byte-identical. |
| Active INCLUDE_ASM stubs | **205** | Stubs still pulled in AFTER cpp (ifdef-aware). |
| Raw `grep -c INCLUDE_ASM` | ~210 | **DO NOT USE.** Blind to `#define ACCEPT_REORDERING_BULLSHIT` in game.c/sub_scrn.c; 6 stubs are shadowed by an active `#else` body. Un-gating one is a NO-OP that reads as +1 — this caused the 2026-07-19 inflation. |
| By function count (C-mapped slice) | **57.9%** | 282 / 487 — the number historically quoted. Overstates the mission. |
| By function count (main exe) | **25.2%** | 282 / 1119. |
| **By function count (WHOLE GAME)** | **~3.5%** | 282 / ~8,000. The honest number. |

## ALWAYS quote both denominators
2026-07-26 audit: reporting only 281/484 overstates mission completion by ~2.3x, because
635 functions (57k instructions) are still linked as raw asm and were never in the
denominator. This is the same defect class as the ACCEPT_REORDERING_BULLSHIT inflation —
a completion metric defined by the current *source* rather than by the target *binary*.

## Why the two percentages differ
The small functions were matched first. The 203 remaining average ~120 instructions each,
so function-count progress runs far ahead of code-volume progress. Quote BOTH; quoting
only the function count overstates completion.

## How to regenerate
```
tools/audit_count.sh      # authoritative: rebuild + hash + raw cmp + census
tools/check_docs.sh       # verifies progress.md / HANDOFF.md / this file agree
```
