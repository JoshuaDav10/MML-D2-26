# Counts — reconciliation of the several numbers this project quotes

**REGENERATE THIS FILE whenever the matched count changes.** It exists only to hold
numbers, so a stale entry here is simply wrong. `tools/check_docs.sh` fails if this file
does not contain the current count.

Last verified: **2026-07-26**, HEAD on `dev`, via `tools/audit_count.sh`
(clean `rm -rf build` + full-binary sha1 + raw `cmp` + `tools/census.py --matched`).
Note: func_8002F9C4 split from raw asm into C stub (phase-0 split template) — 
unsplit count decreases from 635 to 634, total denominator 1119 → 1118.

| Number | Value | Meaning |
|---|---|---|
| `F .text` in all rock_neo `.o` | **484** | Functions currently split into C TUs. **NOT the mission denominator** — see below. |
| Functions still unsplit (raw asm) | **634** | `glabel`s in `asm/rock_neo/*.s`, never split into a C TU and never counted. |
| **Game functions in the executable** | **1118** | 484 + 634. **THE mission denominator.** |
| Matched (real C in tree) | **281** | `484 − 204`. Authoritative; hash OK + raw cmp byte-identical. |
| Active INCLUDE_ASM stubs | **204** | Stubs still pulled in AFTER cpp (ifdef-aware). |
| Raw `grep -c INCLUDE_ASM` | 210 | **DO NOT USE.** Blind to `#define ACCEPT_REORDERING_BULLSHIT` in game.c/sub_scrn.c; 6 stubs are shadowed by an active `#else` body. Un-gating one is a NO-OP that reads as +1 — this caused the 2026-07-19 inflation. |
| Splat nonmatchings `.s` files | 476 | Historical asm reference; NOT the denominator. |
| By function count (C-mapped slice) | **58.1%** | 281 / 484 — the number usually quoted. Overstates the mission ~2.3x. |
| By function count (ACTUAL) | **25.1%** | 281 / 1118. |
| By instruction volume (C-mapped slice) | ~21% | 6,584 of 30,903 words in the split TUs. |
| By instruction volume (ACTUAL) | **~7.1%** | 6,204 of 87,902 words of game code in the exe. |

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
