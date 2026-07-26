# Meta-Lessons — how this project learned to measure itself honestly

> This document is deliberately written to be **shared and taught from.** It is the
> story of a measurement-integrity failure and its correction, distilled into
> portable lessons for ANY large reverse-engineering, decompilation, or
> metrics-driven engineering effort. The specific game (Mega Man Legends, PSX) barely
> matters; the failure mode is universal. If you read only one project doc to learn
> *how to run a project like this without lying to yourself*, read this one.

## The one-sentence lesson

**Define your denominator by the target, not by whatever your tooling can already
see** — or you will confidently report a fraction of a fraction of a fraction as "the
whole thing."

## The story (three nested boxes, each mistaken for the whole)

This project measured "percent complete" three times over its life. Each number was
internally consistent, defended by real gates, and **wrong by a widening margin** —
not because anyone lied, but because the denominator was always "the work already set
up to be countable," never the actual binary on the disc.

| Era | Reported | Denominator used | What it actually was |
|---|---|---|---|
| Early | "~58%" | 484 functions | The functions a human had hand-carved into C files. `census.py` only reads those C files, so "progress" was measured against *the pre-carved slice*, not the game. |
| Correction 1 (2026-07-26 AM) | "25%" | 1,119 functions | The whole main executable. The extra 635 sat in 7 giant un-split `.s` files — real linked code, but not C, so the counter was structurally blind to them. |
| Correction 2 (2026-07-26 PM) | **"~3.5%"** | **~8,000 functions** | The whole game. The main exe is just the boot/menu/field shell; ~7,000 more functions live in 205 disc overlays (stage/enemy code) that no count ever looked at. |

The game never grew. The ruler got honest, twice, in one day — and only because a user
refused to accept a number and said *"how can we hit a target we can't see?"*

## The meta-lessons (portable; teach these)

1. **The denominator is a design decision, and the lazy default is a lie.**
   "What can my script currently count?" is a denominator defined by tooling
   convenience. The correct denominator is defined by the *target* (here: functions in
   the shipped binaries). If those differ, every percentage you publish is fiction.

2. **Gates guard the numerator; almost nobody guards the denominator.**
   This project had real verification that each match was byte-correct (the numerator
   was honest). It had *zero* checks that the total was right. Ask of every metric:
   what proves the DENOMINATOR is the whole target?

3. **A gate you have never watched fail is a confidence generator, not a check.**
   Two safeguards here were broken outright — they exited 0 even on failure (captured a
   result and never tested it; checks buried in subshells that couldn't propagate). The
   fix (`tools/adversarial_audit.sh`) SELF-TESTS: it injects a failure and requires the
   gate to fail. If you can't demonstrate a gate failing, you don't have a gate.

4. **Trusting a number you didn't derive is repeating the original sin.**
   The 1,119 first arrived from a subagent. Repeating it would have been the same
   failure at a new scale. It had to be re-derived directly from the linker script +
   object sizes. **Re-derive load-bearing numbers from ground truth yourself.**

5. **Report every denominator and both axes; naming which one is the honesty.**
   58% (C-slice) / 25% (main exe) / 3.5% (whole game) are all "true." Quoting only the
   flattering one is the lie. Also split *count* vs *volume*: you can move a
   function-count fast on small functions while the byte-percentage barely twitches.

6. **Durable rules must be mechanically enforced, not remembered.**
   "It's in CLAUDE.md / in memory" = prose enforcement = the same failure mode (someone
   has to remember). The correction wired MECHANICAL triggers: a SessionStart hook and a
   git pre-commit hook. If a rule matters, a machine — not a person's memory — must
   enforce it.

7. **Verify the CLAIM, not an easier proxy.** "check prints OK after my edit" proves the
   binary matches now — NOT that your edit caused it (it may have already matched). The
   falsifying question for a match is "would it match WITHOUT my change?" Test *that*.

8. **Leverage must be measured, not hoped.** When facing the 8,000, the tempting story
   was "most are duplicates, so it's really small." Measured: only 1.4x duplication;
   ~7,000 are genuinely unique. The comforting shortcut usually isn't there — check
   before you lean on it. (`tools/leverage_analysis.py`.)

9. **Autonomous/premium work must ratchet.** Long unattended runs get killed (usage
   limits, context loss). Pre-bank the mechanical setup, commit micro-checkpoints, keep
   a one-line RESUME pointer — so a kill costs one step, not the whole run. (See
   `notes/wip/GCC_SOURCE_PROGRESS.md` for the pattern in practice.)

## Why this is worth teaching

Most decomp/RE writeups teach *techniques* (how to match a function). Almost none teach
*how to keep a multi-month, multi-agent project honest with itself.* That second skill
is rarer and more transferable. This project is being built to teach both — the
techniques in `notes/LESSONS.md` / `notes/COMPILER_IDIOMS.md`, and the integrity
discipline here.

## Provenance
Denominator corrections: 2026-07-26. Audit tooling: `tools/adversarial_audit.sh`,
`tools/audit_count.sh`, `tools/check_docs.sh`. Scope evidence: `notes/WORK_MAP.md`,
`tools/overlay_scope.py`, `tools/leverage_analysis.py`. Strategy: `notes/STRATEGY.md`.
