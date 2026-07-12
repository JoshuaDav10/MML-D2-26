# Realistic plan for the giants (2026-07-12)

State at write: **270/484 fns matched (55.8%), ~15.4% instruction volume.**
The gap between those two numbers is the whole problem — half the functions are
done, but the code is volume-back-loaded into big functions.

## Where the remaining 25,777 unmatched instructions actually live
| Tier | Insns | % of remaining | # fns | Character |
|------|------:|---------------:|------:|-----------|
| **Giants ≥150** | 12,907 | **50%** | 52 | half the work, hardest |
| Mid 60–149 | 9,634 | 37% | 102 | steady, moderate risk |
| Small <60 | 3,236 | 13% | 73 | quick, mostly picked over |

**Conclusion:** small-stub harvesting tops out at ~13% of what's left. Finishing
requires the giants + mid tier. The giants are 50% of the work in 52 functions.

### Giant volume concentrates in two modules
| Module | Giant insns | # | Leverage |
|--------|------------:|--:|----------|
| **moji** | 4,163 | 15 | BEST — MOJI_TASK fully typed, 106 matched siblings, richest idiom base |
| **player** | 3,194 | 16 | PL_WORK only partially typed — needs struct buildout first |
| sub_scrn | 1,053 | 4 | |
| cd | 981 | 2 | includes BB4C (809) |
| debug | 942 | 5 | |
| Code800133D8 | 845 | 3 | |
| sound/game/main/scene | ~1,700 | 7 | |

moji + player = **57% of all giant volume in 31 functions.**

## Honest risk assessment
The two flagship giants have resisted multiple sessions + a Fable escalation.
They are genuinely hard: both blocked on gcc-2.7 movable/scheduling/fold fights,
the same class the whole PSX decomp community battles. **None of the candidate
idioms in `RESEARCH_FINDINGS_gcc272_idioms.md` is hash-proven yet.**

BUT — new leverage as of today: `func_80057C2C` PROVED that cc1 preheader
instruction scheduling is controllable from C (for-init comma order → preheader
order). That is the SAME genus as 53B40's movable-ranking blocker. We now have a
real, proven scheduling knob we did not have before.

## The plan — phased, session-sized, checkpointed

### Phase 0 — Consolidate (0.5 session, low risk, do first)
Fold today's 3 hash-proven idioms into `LESSONS.md` (they're giant-class):
1. AND-of-3 grouping: split a temporary to pin `(f & lit) & rt` vs cc1's
   reassociated `f & (lit & rt)`.
2. Mixed array/pointer addressing in a loop (pointer for one field, array for
   the rest) — from func_80053788.
3. **Preheader scheduling knob:** a named local in the for-init, positioned by
   comma order, maps to preheader instruction order — from func_80057C2C.
Payoff: every giant attempt starts from a stronger idiom base.

### Phase 1 — The two "one-blocker-away" flagships (high leverage, uncertain)
Both are structurally SOLVED; each is a single documented fault. ~1,300 insns.
- **53B40 (moji, 495)** — FIRST. Best-typed module, single movable-ranking fault
  (0x1F800070 vs 0x40000000 for $s7). Apply: (a) today's preheader-scheduling
  knob, (b) the untried Q1 idioms from the research doc (volatile-offset var,
  asm-`li` load) that the 6 prior attempts did NOT cover. Baseline draft +
  forensics in `notes/wip/53B40_PROGRESS.md`. Timebox: 1 focused session.
- **BB4C (cd, 809)** — SECOND. 2-word surplus from two parked genera
  (pointer-fold + delay-slot). Apply the Q2 idioms (opaque pointer anchor,
  local register binding). `notes/wip/BB4C_PROGRESS.md`. Timebox: 1–2 sessions.
  Riskier than 53B40 (bigger, two faults, cd struct less certain).
- **Rule:** timeboxed. If a flagship doesn't crack in its box, park with the
  incremental diff findings (as we did for 57924) and move on — do NOT let one
  giant eat a week. A near-match with a precise remaining-diff note is a real
  deliverable.

### Phase 2 — moji giant cluster (best expected ROI after flagships)
The other ~13 moji 150+ fns (func_8005ACA8 491, func_80059E74 457, func_80057FF8
329, func_80056820 316, func_80054E54 300, func_80058F18 249, func_80055CFC 245,
func_80056D10 232, …). They share MOJI_TASK vocabulary and the script-VM idioms
already proven. Each is a MojiTaskExec-scale focused effort. Highest hit-rate of
the giant tiers because the struct + idioms are mature. ~1 per session.

### Phase 3 — player struct buildout, then player giants
16 player giants (3,194 insns) are gated on PL_WORK being under-typed. Invest one
session mapping PL_WORK fields from the already-matched player functions, THEN
the giants become tractable siblings. Sequence: types first, matches second.

### Continuous — mid-tier harvest for steady green (parallel, low risk)
The 60–149 tier (102 fns, 9,634 insns, 37%) is the reliable-progress lane. Run it
between/around giant attempts so the counter always moves even when a giant
stalls. This is the "wins" pace; the giants are the "finish the project" work.

## Realistic ceiling / expectation-setting
- Flagships (Phase 1) if both land: ~+4% volume, ~19%.
- + moji cluster (Phase 2): ~+13% volume.
- + player (Phase 3) + mid-tier harvest: the path to ~60–70% volume.
- A byte-perfect 100% main exe is a multi-week effort and assumes the parked
  genera (jump-canonicalization, held-base-across-call) eventually get general
  idioms. Some functions may stay parked until then. That is normal for a
  matching decomp; honesty about it beats a false "on track."

## Immediate next action (recommended)
Phase 0 (consolidate idioms → LESSONS.md), then open **53B40** as the first
flagship attempt, timeboxed, using the new scheduling knob + untried Q1 idioms.
