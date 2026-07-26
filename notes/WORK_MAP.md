# MML Decomp — Comprehensive Work Map (2026-07-26)

Built from **measured ground truth**, not inherited numbers. Every figure here is
reproducible from the scripts named. This exists because the project spent months
quoting a denominator (484) that was defined by what `census.py` could read, not by
the target binary. This map is denominator-first.

## 0. The finish lines (there are TWO — pick consciously)

The project goal in CLAUDE.md is "C source that recompiles to a byte-for-byte
identical binary." That sentence hides two different finish lines:

- **Finish line A — "the build is byte-identical."** Already TRUE for the 205
  overlays (`make check_overlays` = 205/205) even though they are asm-included, not
  C. On this line the overlays are DONE.
- **Finish line B — "everything is readable C."** On this line the overlays are ~0%
  (they are raw asm includes) and ROCK_NEO.EXE is 25.1%.

**These are not the same project.** Most of this map targets ROCK_NEO.EXE (the main
executable) on finish line B, because that is the "unique core" and the overlays
already satisfy line A. Do not let the overlay count stampede the plan (see §4).

## 1. Verified state (from tools/audit_count.sh + tools/adversarial_audit.sh, HEAD fff5ab7)

| slice | count | note |
|---|---|---|
| **Matched & byte-verified (C)** | **281** | authoritative: clean rebuild + hash + cmp |
| Active stubs (mapped, not matched) | 203 | in the C slice, still INCLUDE_ASM |
| **C-mapped slice** | 484 | = 281 + 203 |
| Raw asm in ROCK_NEO.EXE, never split | ~628–635 | 56,459 insn, 64.6% of the exe by volume |
| **ROCK_NEO.EXE TOTAL** | **~1119** | the honest primary denominator |
| **Real completion** | **281 / 1119 = 25.1%** | ~7% by volume |
| Overlay `func_` labels (205 BINs) | ~10,000 | SEPARATE scope; already byte-match; heavy dup |

Binary builds byte-identical to the disc. The 281 are real and none is invalidated.

## 2. Is the raw-asm work "easier"? — MEASURED (tools/analyze_raw_asm.py over the 7 files)

628 functions, 56,459 instructions. **47% are leaf** (no `jal`/`jalr`).

| size bucket | #funcs | cumul% | #leaf | insns |
|---|---:|---:|---:|---:|
| tiny  ≤10 insn | 92 | 14% | 71 | 509 |
| small 11–25 | 134 | 35% | 70 | 2,360 |
| med   26–60 | 160 | 61% | 67 | 6,503 |
| large 61–150 | 126 | 81% | 47 | 11,799 |
| huge  151–400 | 93 | 96% | 28 | 22,032 |
| giant >400 | 23 | 100% | 13 | 13,256 |

**Answer: yes, for a large minority.** The early-project "fast and easy" era was small
leaf functions, and the raw-asm reservoir has the same fuel:
- **141 functions are ≤25 insn AND leaf** = the genuine fast-win pool (22% of 628).
- **226 functions are ≤25 insn** of any kind (35%).
- **386 functions are ≤60 insn** (61%) — normal matching pace.

**BUT** (the honest counterweight): **62% of the code volume lives in the 116
huge+giant functions (35,288 insn).** So function-count progress and volume progress
diverge sharply — you can move the *count* fast on small functions while the *volume*
percentage barely twitches. Report both, always (that split is the whole lesson).

Caveat: "small + leaf" is *necessary, not sufficient* for an easy match. Some small
functions still hit compiler walls (see §5). Treat 141/226 as **candidate** pools, not
promised matches.

## 3. The enabling step (do this FIRST) — split the 7 giant asm files

The 628 functions are invisible/unqueueable because they sit in 7 monster files
(`10638.s` = 1.66 MB / 280 funcs, `32A38.s` = 188 funcs, …). Splitting them into
per-function `.s` files under `asm/rock_neo/nonmatchings/…` (as the C-slice already
is) is what turns "64.6% invisible" into "628 rows in a work queue." It is:
- **Mechanical** (splat/spimdisasm reconfig; a function-mapper subagent job), no
  matching judgment required.
- **The precondition** for census visibility, per-function diffing, and permuter runs.
- The single highest-leverage move on the board. Everything in §4 depends on it.

## 4. The work map (tranches, ROCK_NEO.EXE / finish line B)

- **T0 — Split (mechanical).** Split the 7 files; add splat yaml entries in address
  order; regenerate stubs. Deliverable: 628 per-function files + census that counts
  them. No count change, but the denominator becomes navigable.
- **T1 — Fast wins: 141 small leaves (≤25 insn, no calls).** Batchable on cheap
  models + permuter. Plausibly the fastest 25%→~35% climb the project can get. NOT
  guaranteed; expect a wall minority.
- **T2 — Small non-leaf (~85 funcs ≤25 insn with calls).** Adds call-arg/ABI matching;
  still quick.
- **T3 — Medium (160 funcs, 26–60 insn).** Standard one-at-a-time pace.
- **T4 — Large/huge/giant (242 funcs, ~47k insn = 83% of raw volume).** The long grind;
  where compiler walls concentrate; where §5 pays for itself.
- **Parallel — Compiler-wall expedition (§5).** Not a tranche; a force multiplier that
  unblocks T3/T4 functions across the whole set.
- **Deferred — Overlays.** Already byte-match (line A). Only touch for line B, and only
  after measuring cross-overlay DUPLICATION first (likely most of the ~10k labels are
  shared library/actor code — dedup could collapse it enormously). Do not queue 10k
  rows before that measurement.

## 5. Compiler walls are an ASSET now, not a footnote

The 53B40 expedition (2026-07-14) proved the loop-hoist genus is C-REACHABLE and
extracted the exact `move_movables` rule (`COMPILER_IDIOMS.md §4`). With 242 large
functions ahead, walls will recur — cracking a *genus* amortizes across all of T3/T4.
This is the correct use of premium (Fable) tokens; grinding small leaves is not.

## 6. What could STILL be mis-measured (keep hunting)

1. **Overlay dedup is unquantified** — the ~10k label figure is raw, not unique. Measure
   before it drives any decision.
2. **Two finish lines conflated** — every future % must say which line and which
   denominator (exe vs whole-game, count vs volume).
3. **628 vs 635** — minor parse gap (data labels mislabeled as funcs, or vice-versa);
   reconcile during T0 splitting, not before.
4. **`src/ST1A/` exists** — one overlay is partially C. Confirm whether its progress is
   counted anywhere or is itself an untracked slice.

## Reproduce everything here
- Counts: `tools/audit_count.sh`, `tools/adversarial_audit.sh`
- Size distribution: `tools/analyze_raw_asm.py` (parses `asm/rock_neo/*.s`)
- Overlay labels: `grep -rhE '^glabel func_' asm/ --include=*.s | wc -l`
