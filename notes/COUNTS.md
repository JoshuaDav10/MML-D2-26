# Counts — reconciliation of the several numbers this project quotes

**REGENERATE THIS FILE whenever the matched count changes.** It exists only to hold
numbers, so a stale entry here is simply wrong. `tools/check_docs.sh` fails if this file
does not contain the current count.

Last verified: **2026-07-25**, HEAD on `dev`, via `tools/audit_count.sh`
(clean `rm -rf build` + full-binary sha1 + raw `cmp` + `tools/census.py --matched`).

| Number | Value | Meaning |
|---|---|---|
| `F .text` in all rock_neo `.o` | **484** | Functions actually emitted from current `src/rock_neo/*.c`. THE denominator. |
| Matched (real C in tree) | **281** | `484 − 203`. Authoritative; hash OK + raw cmp byte-identical. |
| Active INCLUDE_ASM stubs | **203** | Stubs still pulled in AFTER cpp (ifdef-aware). |
| Raw `grep -c INCLUDE_ASM` | 209 | **DO NOT USE.** Blind to `#define ACCEPT_REORDERING_BULLSHIT` in game.c/sub_scrn.c; 6 stubs are shadowed by an active `#else` body. Un-gating one is a NO-OP that reads as +1 — this caused the 2026-07-19 inflation. |
| Splat nonmatchings `.s` files | 475 | Historical asm reference; NOT the denominator. |
| By function count | **58.1%** | 281 / 484. |
| By instruction volume | **~20%** | ~6,200 of 30,903 instruction words; 24,699 remain across the 203 stubs. |

## Why the two percentages differ
The small functions were matched first. The 203 remaining average ~120 instructions each,
so function-count progress runs far ahead of code-volume progress. Quote BOTH; quoting
only the function count overstates completion.

## How to regenerate
```
tools/audit_count.sh      # authoritative: rebuild + hash + raw cmp + census
tools/check_docs.sh       # verifies progress.md / HANDOFF.md / this file agree
```
