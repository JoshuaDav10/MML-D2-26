# Codex brief — moji.c small-stub harvest (MML matching decomp)

## What this is
Matching decompilation of Mega Man Legends (PSX, US). The goal is C source
that GCC 2.7's cc1 (in-repo at `./bin/cc1-27`) compiles at -O2 to
BYTE-IDENTICAL machine code. "Looks equivalent" counts for nothing here;
only byte equality counts, and it is checked mechanically.

## Your workspace
- Work ONLY in `/home/X/Documents/MML-codex-moji` — a git worktree on
  branch `codex-moji`.
- The main checkout `/home/X/Documents/MML-D2-26` is READ-ONLY reference
  (the extracted asm lives there under `asm/rock_neo/nonmatchings/`, and
  its Python venv is shared).
- NEVER push to any remote. NEVER commit to `dev` or `main`. Commit only
  to `codex-moji`. A supervising session cherry-picks your verified work
  onto `dev` and runs the full-binary hash gate there.

## Scope
- You may edit exactly ONE file: `src/rock_neo/moji.c`.
- `include/rock_neo/moji.h` is READ-ONLY. Its MOJI_TASK layout is verified
  against the binary and other agents depend on it. If a function seems to
  need a header change, record exactly what is needed in
  `notes/wip/codex_moji_parked.md` and skip that function.
- SKIP these functions entirely:
  - `func_80053B40` (522-insn giant, owned by another agent),
    `func_80053AA4` (adjacent region),
  - `func_80056778` (parked: delay-slot const-hoist genus, known blocker),
  - `func_80059E74`, `func_8005ACA8`, `func_80057FF8`, `func_80056820`
    (200+ insn giants — not harvest material),
  - any function whose asm jumps through a loaded table (`jr $reg` after a
    table load) — the rodata-carve recipe doesn't work from a worktree;
    list those in the parked file instead.

## Required reading, in order (all inside your worktree)
1. `notes/LESSONS.md` — hash-proven compiler idioms. THE most important
   file. Especially: §2 extern rules (asm `($gp)` access ⇒ plain scalar
   extern; `lui`/`%lo` pair ⇒ unsized array extern `extern u8 foo[];` —
   read the target asm BEFORE declaring anything), K&R vs ANSI declaration
   effects, store-order rules, and the 2026-07-11 entry (s16/HImode
   locals, `||`-shape arm layout, direct returns).
2. `HANDOFF.md` — "Critical gotchas" and "The inner loop" sections.
3. `src/rock_neo/moji.c` itself — 94 matched functions are your template
   library. Before drafting a target, find the 2–3 matched functions whose
   asm shape is closest and imitate their C.
4. `include/rock_neo/moji.h` — the MOJI_TASK struct (script/script2
   streams, stack/stack2 call stacks, flags word).

## Workflow per function
1. Pick the smallest remaining stub. The `INCLUDE_ASM(...)` lines in
   moji.c name them; get sizes with
   `wc -l /home/X/Documents/MML-D2-26/asm/rock_neo/nonmatchings/moji/<func>.s`
   and sort ascending.
2. Read the full asm. Note every global's access pattern (gp vs lui)
   BEFORE writing an extern.
3. Draft in a scratch translation unit first (pattern:
   `#include "rock_neo.h"` + `#include "rock_neo/moji.h"` + the few decls
   you need + the one function).
4. Verify:
   ```
   source /home/X/Documents/MML-D2-26/.venv/bin/activate
   CPP=cpp tools/bytecmp.sh <func> <scratch.c> \
     /home/X/Documents/MML-D2-26/asm/rock_neo/nonmatchings/moji/<func>.s
   ```
   Success = `0 hard mismatch(es)` AND every reloc flag points at a
   plausible symbol (jal → the callee you wrote, %hi/%lo → the extern you
   named). A rabbitizer import error means you forgot the venv line.
5. Only then: replace the INCLUDE_ASM line in moji.c with the body (match
   the surrounding house style; no narration comments) and commit to your
   branch: `moji <func> BYTECMP-VERIFIED: <one-line shape description>`.
   One commit per function or sibling group.
6. If a target resists after ~6 genuinely different C shapes, park it:
   append the target, the closest-diff summary, and the shapes tried to
   `notes/wip/codex_moji_parked.md`, commit, and move on. Parking is
   cheap; grinding is not.

## Claim discipline (non-negotiable)
- Your maximum claim is "BYTECMP-VERIFIED". Never say "matched" — that
  word is reserved for the full-binary sha1 gate, which is not yours to
  run. Never weaken this.
- If bytecmp itself errors, STOP and record the error verbatim; do not
  invent an alternative verification method.
- Do not run `make` anywhere. Do not modify anything in the main checkout.

## moji-specific idioms (these cost whole sessions to learn — reuse them)
- Most stubs are script-opcode handlers: read operands from `m->script` or
  `m->script2`, advance the pointer by the opcode length, return 0 or 1.
- `MojiTaskExec` is declared K&R ON PURPOSE (a `-1` argument must not be
  truncated through a u8 ANSI prototype). Do not "fix" it.
- The CALL-opcode family template (see matched func_800564C8/56558/56610/
  566CC): `base = m->x44; if (base) MojiTaskExec(no, base, op); else
  MojiTaskExec(no, D_8008CACC[idx], 0xFF);` with per-variant op-index
  remap tables.
- The wait-counter trio (func_800545C8/54700/557B8) shows the s16-local,
  `||`-shape, and direct-return knobs.
- `(s8)m->x71`-style casts force signed `lb`; `((u8*)sym)[k]` selects
  `lbu` per-site; an explicit `& 0xFFFF` into an `int` local pins the mask
  at the store site.
- Store order follows statement order. If the asm stores fields in an odd
  order, reorder the C statements, never the logic.

## Final report format
When you stop, summarize: functions BYTECMP-VERIFIED (name + one-line
shape), functions parked (name + blocker), jump-table functions found,
and the commit list on `codex-moji`.
