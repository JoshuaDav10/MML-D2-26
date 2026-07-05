# Handoff to Fable — MML Decomp Orchestration

> **You are Claude Fable 5, running in Claude Code, acting as the ORCHESTRATOR
> for a Mega Man Legends (PSX) matching-decompilation project.** This document
> is your mission brief. Read `MML_DECOMP_CHECKPOINT.md` first for full project
> state; this file tells you your role and what to do next.

**Handoff date:** 2026-07-04
**Precondition:** Stage 0 is complete. The repo builds and `rock_neo.exe` matches
the original byte-for-byte (sha1 `ffc08fd2b6113e84dc0e75ba63a4feaca0eb1d46`).

---

## Your role

You are the **judgment + orchestration** layer. Your scarce, expensive attention
should go to things only a strong model does well:
- Deciding which function to tackle next.
- Reading an asm diff and reasoning about WHY the C doesn't match (register
  allocation, instruction ordering, stack layout, compiler quirks).
- Writing/adjusting the C for a matching decomp.
- Verifying results and deciding when something is truly done.

You should **delegate mechanical work to cheaper subagents** (Haiku/Sonnet) — see
"Orchestration model" below. But note (important, factual): subagents share your
billing and each carries its own context, so a subagent-heavy workflow can use
~7× the tokens of a single thread. **Delegation saves money ONLY when the
subagent runs a cheaper model on genuinely mechanical work.** Do not spawn
strong-model subagents for trivia; do the trivial thing inline or route it to Haiku.

---

## What to do next — in strict order

**Do NOT jump to building subagents. The inner loop must be validated by hand first.**

### Step 1 — Validate the diff loop (no automation yet)
The diff tool `./diff.py` (asm-differ) has not been run in this project yet.
Confirm it works:
```bash
source .venv/bin/activate
make CPP=cpp            # ensure a current build exists
./diff.py -mo <some_func_name>   # try a known function from src/rock_neo/
```
If `diff.py` needs config (it reads `diff_settings.py`), sort that out now.
Goal: you can see a side-by-side original-vs-built asm diff for one function.
If the toolchain fights you here, expect the same theme as Stage 0 — 2022 repo
vs 2026 tools. Fix minimally; document each fix.

### Step 2 — Do ONE manual match, end to end
Pick a single SMALL function currently in `INCLUDE_ASM` stub form. Convert it to
real C, build, diff, iterate until it matches (clean diff / hash holds). Commit it
on a branch (`git switch -c match/<func>`). This defines "a unit of work" and is
the thing you'll later delegate around. **You cannot delegate a process you have
not performed once.**

### Step 3 — Set up orchestration scaffolding
Only after Steps 1–2 succeed. Create:
- **`CLAUDE.md`** in repo root (auto-loaded project rules). Draft content is in
  the appendix below — review and adapt.
- **`.claude/agents/` subagents** via the `/agents` command (interactive; it
  writes sharper descriptions than hand-authoring). Recommended set:
  - `function-mapper` (model: haiku) — grep asm for unmapped funcs, add YAML
    entries in address order, create `INCLUDE_ASM` C stubs, run `make CPP=cpp`.
    Tools: Read, Grep, Glob, Bash, Edit. Read-heavy, mechanical.
  - `build-runner` (model: haiku) — run builds/diffs and report pass/fail +
    relevant error lines only (NOT full logs). Tools: Read, Bash, Grep.
  - Keep C-matching reasoning for YOURSELF (Fable), not a subagent — it's the
    high-judgment part.
- Optionally a slash command to kick off "map + stub the next N functions".

### Step 4 — Run the assembly line
Loop: cheap agent maps/stubs/builds a batch in isolated context → you pick a
function and do the matching C → verify → user commits. Track **mapped** and
**matched** as separate counts in `progress.md`.

### Step 5 — (Later) Overlays
The ~150 `ST**` overlays don't link (`undefined reference` errors). Separate
expedition. Not required for main-executable progress. Defer unless asked.

---

## Orchestration model (how to delegate)

Subagents are markdown files in `.claude/agents/` with YAML frontmatter:
```
---
name: function-mapper
description: Maps unmapped MIPS functions. Use proactively for mechanical mapping,
  stub creation, and running builds. Not for matching-decompilation judgment.
tools: Read, Grep, Glob, Bash, Edit
model: haiku
---
<system prompt body: exact role, the build command `make CPP=cpp`, the stub
format, address-order rules, and "report only results, not full logs">
```
- Each subagent runs in its **own context window** — this keeps grep output,
  file dumps, and build logs OUT of your main thread. That context isolation is
  the real token win, alongside the cheaper model.
- Delegate: searching many files, adding YAML/stub boilerplate, running builds,
  extracting the few relevant error lines from a big log.
- Keep for yourself: reading diffs, matching C, architectural decisions.
- Ask subagents to **return summaries, not raw content** ("report the 3 failing
  lines", not "paste the log").

---

## Token hygiene (real, not superstition)

Do this:
- Route mechanical/high-volume work to **Haiku** subagents; keep your own turns
  focused on judgment.
- Use subagents' isolated context to keep dumps out of your window.
- Reference files by path and read only the relevant span; don't paste whole
  files or raw hex/asm dumps into your main context.
- Work one function at a time; commit often so context can be reset cleanly.
- Use `/clear` between unrelated tasks to drop stale context.

Do NOT do (these come from earlier user docs and are based on inaccurate claims):
- Do not refuse to write code or forbid yourself from implementation. You are the
  matching-decomp author; that's your core job.
- Do not constantly recycle/delete threads as a ritual "to save the budget" — the
  overhead of re-seeding context usually costs more than it saves. Reset context
  when it's genuinely stale, not on a superstition.
- Do not architect the whole workflow around avoiding a "downgrade." See below.

---

## On the "Fable downgrade / usage gate" concern (accuracy note)

The user's older strategy docs claim Fable has a "50% weekly usage gate,"
"auto-downgrades to Opus for reverse-engineering or code work," and a
"distillation trap." Per Anthropic's own material, these are inaccurate or
exaggerated. The real mechanism is a **session-level safeguards routing** that,
by Anthropic's description, triggers on average in **under 5% of sessions** and
is tuned conservatively around genuinely sensitive topics (e.g. cyber/bio). If a
query is redirected, it's answered by Opus 4.8 — a strong model, not a penalty.

**Decompiling a legally-owned 1997 game is not what that routing targets.** Work
normally. Describe the task for what it is (decompilation / reverse engineering
of the user's own game for preservation and matching source reconstruction).
Don't contort the workflow, don't self-censor legitimate code work, and don't
build elaborate defenses against a <5% event. Just do good decomp.

---

## Build & verify cheat-sheet

```bash
source .venv/bin/activate                 # every new shell
make CPP=cpp                              # build (main exe matches; overlays error = expected)
make CPP=cpp check_rock_neo_only         # verify main exe against official hash
./diff.py -mo <func>                     # asm diff for a function (validate in Step 1)
rm -rf build                             # force clean rebuild (do NOT use `make clean`)
```
Gotchas: venv must be active (`rabbitizer` errors = it isn't); `CPP=cpp` is
required until the Makefile is patched; `make clean` nukes `asm/`+`assets/`.

---

## Appendix — draft `CLAUDE.md` to place in repo root

```markdown
# MML Decomp — Project Rules (Claude Code)

Matching decompilation of Mega Man Legends (PSX, US), binary ROCK_NEO.EXE.
Goal: C source that recompiles to a byte-for-byte identical binary.

## Build
- Activate venv first: `source .venv/bin/activate`
- Build: `make CPP=cpp`  (main exe matches; ST** overlays don't link yet — expected)
- Verify main exe: `make CPP=cpp check_rock_neo_only`
- Diff a function: `./diff.py -mo <func>`
- Force rebuild: `rm -rf build`  (NEVER `make clean` — it deletes asm/ and assets/)

## Environment notes (2026 toolchain)
- Compiler is ./bin/cc1-27 (in-repo). Do NOT install mipsel-elf-gcc (fails on GCC 15, not needed).
- Preprocessor: system `cpp` via CPP=cpp override.
- include/macro.inc has added no-op macros (nonmatching/enddlabel/alabel) for modern spimdisasm.

## Workflow rules
- One function at a time. Real work on branches: `git switch -c match/<func>`.
- Track mapped vs matched separately in progress.md. Mapping != matching.
- A function is DONE only when its diff is clean / hash holds — not when it "looks right".
- Map functions in ascending address order when adding to the splat yaml.

## Delegation
- Route mechanical work (grep, stub creation, builds) to the Haiku subagents in .claude/agents/.
- Keep matching-decomp reasoning in the main (strong-model) thread.
- Subagents should return summaries and the few relevant lines, not full logs.
```

---

## TL;DR for the Fable instance receiving this

Stage 0 is done (main exe matches, sha1 `ffc08fd2...`). Your first jobs, in
order: (1) get `./diff.py` working, (2) match ONE small function by hand to define
the unit of work, (3) THEN scaffold `CLAUDE.md` + Haiku subagents for mechanical
work while you keep the matching judgment. Save Fable attention for matching; push
grep/stub/build to Haiku. Ignore the old "downgrade/gate" folklore — just do
legitimate decomp with sane token hygiene.
```
