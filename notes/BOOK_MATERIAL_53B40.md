# Book material — the 53B40 expedition (raw; for condensation into MML-decomp-journal/book)

**Status:** source material, not final prose. Written 2026-07-25 while the work was fresh.
Destination: the two-book textbook in the separate `MML-decomp-journal` repo.
Suggested home: Book II (the "walls" / advanced-matching half), as a full worked chapter.

---

## What this chapter is actually about

It is the best worked example in the project of a specific, teachable thing:
**how to tell the difference between "I haven't found the right C yet" and "the compiler
generated this without being told to."** That distinction is the whole skill ceiling of
matching decompilation, and 53B40 is where we learned to draw it rigorously instead of
by feel.

It is also, honestly, a chapter about **five sessions spent not landing a function** —
and why that was still worth it. The pedagogy is in the method, not the trophy.

---

## The setup (for a reader who just finished Book I)

`func_80053B40` is moji.c's text/font renderer: 495 instructions, one of the twelve
largest functions in the game. By the time this chapter opens, the body logic is already
correct — the glyph-cell divide magic, the UV block, the control flow all verified. What
remains is entirely *how the compiler assigned registers and stack slots*. Nothing about
the program's meaning is in question. That is the situation this chapter teaches.

The measurement discipline matters and should be stated early for the reader:
- The scoreboard is **`tools/audit_count.sh`** — clean rebuild, full-binary hash, raw
  `cmp`, authoritative count. Binary outcome, no partial credit.
- The working thermometer is **positional row mismatches** (how many of the 495
  instructions are still wrong). It must reach **0**.
- **These are different things.** The thermometer guides work; only the scoreboard
  counts. A chapter sidebar on this would be well placed — see "the two rulers" below.

---

## Narrative arc (the order things were actually learned)

### 1. Three rulers, and the discipline of reconciling them
Early confusion in this expedition came from quoting three different numbers as if they
were one: the permuter's internal weighted score, the scratch-build `bytecmp` count, and
the true in-tree diff. They disagree by small constants for legitimate reasons (the
scratch build hasn't resolved `%gp_rel` addressing yet; the permuter normalizes branch
targets). The lesson for the reader is not "use ruler X" but **"know which ruler you are
holding, and never compare across them."** We eventually pinned it: in-tree 68, scratch
73, permuter-positional 70 — all the same state.

### 2. A cheap win that teaches a real rule
Our `RECT` local sat at `0x20($sp)`; the target's at `0x18($sp)`. Cause: a `volatile`
local declared *before* it, which (being volatile) is forced to memory and claimed the
lower slot. **gcc-2.7 assigns stack slots in declaration order.** Moving one declaration
line fixed five mismatched instructions at once.

This is the chapter's "hopeful middle": a real, reusable rule, found by looking at
addresses rather than guessing at code. 75 → 70.

### 3. The discovery that reframed everything: the padding
The draft contained a permuter-introduced `volatile unsigned short new_var11`, assigned
`m->flags & 0x100000` — a value that, truncated into a 16-bit variable, **is always
zero.** The test it guarded was therefore always true: semantically dead code.

But deleting it made things *worse*: the word count dropped to 494 and the stack frame
shrank from `0x50` to `0x48`. The dead variable was **structural padding** — it was
holding both our instruction count and our frame size at the right values for entirely
the wrong reasons.

That is the moment the chapter should turn. We were not "70 instructions from correct."
We were carrying fake instructions that happened to fill the same space as real ones we
had never found. **A number can be right for the wrong reason, and that is more dangerous
than being wrong**, because it hides the real defect and terminates the search early.

### 4. Reading the compiler instead of guessing at it
Two things then got established by reading the actual gcc-2.7 source (`local-alloc.c`,
plus the earlier `loop.c` work from teeth 1–4) rather than by trial and error:

**(a) Constants can never cause spills.** gcc marks constant-valued pseudos with
`REG_EQUIV`; under pressure reload *rematerializes* them (re-emits `lui/ori`) instead of
spilling. So the intuitive lever — "hoist a constant into a variable to raise register
pressure" — is structurally incapable of producing stack spills. Three experiments
confirmed the prediction before we understood it; the source explained why.

**(b) The rule that governs a surviving register-to-register copy.** The target contains
`addu $s1,$v0,$zero` — load into a scratch register, then copy into a preserved one. Our
build folds it into a single direct load. `local-alloc.c:combine_regs` deletes such a
copy **iff the source register is dead at the copy**; and `cse`, which runs earlier,
*always* makes it dead for a plain `b = a` by proving the two hold the same value.

So the copy cannot be produced by any spelling of that assignment. We verified this
empirically in the most direct way available: we rewrote downstream uses to reference the
*other* variable — semantically identical, specifically designed to keep it alive. The
score did not move. Twice. Exactly as the source predicted.

### 5. The verdict, and the discipline of scoping it
The conclusion: that instruction is a **reload live-range split** — a compiler-internal
artifact produced under register pressure — and it is the *same phenomenon* as the other
open mystery, the eight bytes of stack the target reserves and never touches. Two
symptoms, one cause, neither written by a human.

**The scoping matters and should be taught explicitly.** The defensible claim is "no
spelling of *this statement* can produce it — that shape is provably always deleted." The
overclaim, which I made first and had to walk back, is "no C program can produce it." A
structurally different whole function could plausibly put reload into the same state.
Knowing the difference between *"I proved this door is locked"* and *"I proved there is
no door"* is the intellectual honesty the whole book is arguing for.

---

## Themes worth pulling into the book's spine

1. **Proof beats persistence.** Roughly twenty source-level attempts failed. One
   afternoon reading `local-alloc.c` explained *why all twenty had to fail* and closed
   the search. Reading the compiler is cheaper than out-guessing it.

2. **Negative results are deliverables.** "Constant-hoisting can never create spills" is
   permanently valuable — it deletes an entire branch of the search tree for every future
   function, in this project and any other PSY-Q decomp.

3. **Be suspicious of numbers that look good.** The padding episode is the cautionary
   tale: right frame size, right instruction count, wrong reasons. Pair this in the book
   with the count-inflation incident (matched-count over-reporting, corrected by building
   `audit_count.sh`) — they are the same failure mode in two different domains:
   *verifying a proxy instead of the claim.*

4. **Know when to stop.** Five sessions, one function, not landed. The correct move is to
   bank the transferable knowledge and redirect to targets that still respond. The book
   should say plainly that this is a *skill*, not a defeat — and that the durable output
   of the expedition was four reusable compiler rules, not a +1 on the counter.

5. **Some artifacts are below the source level.** The deepest lesson: a matching decomp
   assumes every byte traces to something a human wrote. Sometimes it doesn't. Reload
   invents instructions. When that happens the honest options are to reproduce the
   compiler's *state* (very hard), accept a near-match, or accept-as-asm — and the
   original expedition brief was right to define "proof it's unreachable" as a win.

---

## Concrete assets available for the chapter

- The three-ruler reconciliation (68 / 70 / 73) with the reason for each gap.
- Before/after stack-slot tables (ours vs target) showing the `rect` fix.
- The callee-saved register map, ours vs target: **identical on 7 of 8 registers** —
  a striking visual for "so close, and yet".
- The `combine_regs` excerpt with the decisive `if`, annotated.
- The full table of ~20 attempted levers with outcomes (teeth 10–15 in
  `notes/wip/GCC_SOURCE_PROGRESS.md`) — excellent raw material for an appendix on
  "what disciplined failure looks like."

## Where the technical detail already lives (do not duplicate, cite)
- `notes/wip/GCC_SOURCE_PROGRESS.md` teeth 1–15 — the full forensic trail.
- `notes/LESSONS.md` — the four distilled, reusable rules.
- `notes/wip/53b40_draft_permbest.c` (70 rows, padded) and `53b40_draft_honest.c`
  (96 rows, semantically clean) — the two bases, and why both exist.
