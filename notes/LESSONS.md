# Decomp Lessons — accumulated compiler & workflow knowledge

Running log of what's been *proven against the hash* (not theorized). Every
future session should read this before matching. Add to it whenever a diff
iteration teaches something; this file is how the project gets smarter.

> **Companion docs (read alongside this one):**
> - `notes/META_LESSONS.md` — how the project learned to MEASURE itself honestly
>   (the 484→1,119→~8,000 denominator saga + portable integrity lessons). Teach from it.
> - `notes/WORK_MAP.md` — measured whole-game scope: ~8,000 unique functions,
>   281 done (~3.5% whole game / 25% main exe / 58% C-slice — name which, always).
> - `notes/STRATEGY.md` — leverage-ordered plan to tackle the ~8,000.
> **Scope fact:** the game is ~8,000 unique functions across ROCK_NEO.EXE (1,119) +
> ~51 stage-program overlays (~7,010 unique, currently all un-decompiled asm but
> byte-identical in the build). See WORK_MAP §7 for the de-dup evidence.

## How knowledge persists between sessions
- `CLAUDE.md` — auto-loaded rules (build commands, gotchas, branch policy).
- This file — compiler idioms and matching tactics, growing per session
  (chronological, with concrete MML symbols/functions = the PROVENANCE).
- `notes/COMPILER_IDIOMS.md` — the PORTABLE distillation: the same idioms
  reorganized by compiler mechanism, game symbols stripped, meant to carry to
  ANY PSX PSY-Q gcc-2.7.2 decomp. When a new idiom here is a property of the
  COMPILER (not this game's data), distill it into that file too.
- `notes/DOCUMENT_INDEX.md` — index to the external reference corpus
  (MIPS R3000 manual, PSXSPX hardware spec, PSYQ SDK docs) at
  `notes/reference/`. Index-driven lookup only; never whole-file pulls.
- `include/rock_neo/*.h` — struct layouts are *executable knowledge*: once a
  field is typed (e.g. MOJI_TASK.script), every later function gets it free.
- `progress.md` / `activity.md` — counters and per-session narrative.
- Git history on `dev` — each commit message records what matched and why.

## Permuter score-0 is NOT a guaranteed byte match (2026-07-14)

**SECOND failure mode (2026-07-25): a scratch zero may not TRANSFER in-tree.**
func_8001F828's permuter reached a genuine score 0 in its workdir, and the winning source
transcribed faithfully into scene.c still produced the SAME 5-row diff in-tree (census
said 282, but raw `cmp` DIFFERED — the hash caught it).
Cause: the permuter workdir's `base.c` is a SIMPLIFIED environment. For F828 it declared
`typedef struct { s8 _a[0x52]; s8 x52; } GAME_WORK;` (0x53 bytes) and only *declared*
`Sce_flag_test`, whereas in scene.c the struct is the full GAME_WORK and Sce_flag_test is
**defined in the same TU**. Those differences change register allocation / delay-slot
filling, so the mutation that wins in scratch is not the mutation that wins in-tree.
**Rule: a permuter workdir must replicate the tree environment as closely as practical**
(real struct definitions, same same-TU callees) or its zeros are only scratch-valid.
When a zero fails to transfer, do NOT hand-tune the transcription — fix the workdir
prelude and re-run. (E4C4/62C6C/57924 DID transfer; they had little or no struct/callee
context, which is why this went unnoticed until F828.)


The decomp-permuter's scorer **normalizes jump/branch targets**, so a candidate with
the WRONG control flow (a `j`/branch to the wrong label) can score **0** while being
byte-different. Real example: func_8001AE6C — permuter reported score 0 for a version
that moved `ret=0` to the wrong arm (`j .L1aee8` instead of `.L1aee4`); it was a FALSE
match (raw `cmp` differed at the `j` target). func_8001E4C4's score-0, by contrast, was
a TRUE match. **Rule: a permuter score-0 is a CANDIDATE, not a match.** Always land it
and gate on `tools/audit_count.sh` (full hash + raw `cmp` byte-identical); REVERT the
false ones. Never commit a permuter result on the score alone. (Same family as the
count-integrity gotcha below: verify the claim, not a proxy.)

## Counting matched functions — GOTCHA (2026-07-19)

**`#define ACCEPT_REORDERING_BULLSHIT` at the top of game.c AND sub_scrn.c**
(since commit 957191c) makes every `#ifndef ACCEPT_REORDERING_BULLSHIT` guard in
those files take the `#else` **body** branch. So a function that LOOKS stubbed
(`INCLUDE_ASM(...)` visible under `#ifndef`) is actually compiling its C body and
may already be matching. Consequences:
- A naive `grep INCLUDE_ASM` stub census **overcounts stubs** (it can't see the
  define) and makes "un-gating" such a body look like a new match when it's a
  NO-OP. This cost a false +7 COMMITTED count (peak 280 committed, real 273; +10 counting the
never-committed in-session 283). Audit 2026-07-25 found 5 of those 7 phantoms were
banked EARLIER, on 2026-07-12 by 3ca4b7d and f2e025e (both pure un-gates in sub_scrn.c,
which defines the macro) — so the true count entering 07-19 was 271 while docs said 276.
- **The ONLY authoritative matched count is `tools/audit_count.sh`** (which runs
  census AND gates on the full hash + raw cmp; census alone is NOT sufficient —
  it accepted the func_8001F828 false transfer). `tools/census.py --matched` (it
  reads the built `.c.o` intermediates) run AFTER `rm -rf build && make`.
  Corroborate with `make check_rock_neo_only` (OK) + `cmp disks/us/ROCK_NEO.EXE
  build/rock_neo.exe`. Never claim a count from a source grep.
- To find genuinely-stubbed functions, a stub must be BOTH `INCLUDE_ASM` AND not
  shadowed by an active `#else` body (i.e. not under a satisfied `#ifndef`).

## cc1-27 (GCC 2.7.2) codegen facts — verified byte-for-byte

### External techniques from another PSY-Q gcc-2.x decomp (2026-07-25, UNVERIFIED HERE)
Source: NFSHS-PSX-decomp (Caesar0007), commit 545c844 — same toolchain generation.
These are THEIR verified idioms, not yet confirmed against our hash. Test before trusting.

1. **addu operand order via explicit shift-in-expression.** Writing
   `*(int *)((base + (patch_idx << 2)) + 0x14)` (shift inline, base first) makes the
   base `addu` operand 1 — oracle `addu v0,s1,v0`. Contrast `base + patch_idx * 4 + 0x14`,
   which reassociates. Relevant to our Sce_flag_off addressing-order residual.
2. **A returned pointer variable materialises as a reg-to-reg COPY.** `return out;` on a
   pointer param emits `addu v0,s3,zero` at the epilogue ("void->non-void discriminator:
   a dead-base copy to $v0 = the return value"). This is the ONLY documented way we have
   seen to get a deliberate `addu rX,rY,zero` from C — possibly relevant to the
   live-range-split copy genus (53B40 render, func_80042154), though there the copy is
   mid-function, not an epilogue return.
3. **Do NOT let gcc tail-merge duplicate epilogues.** Their oracle keeps TWO separate
   `return 1` blocks; writing one shared return collapses them. (We independently found
   the inverse — a shared goto label PREVENTS a setcc collapse — so both directions are
   levers depending on the target.)
4. **Assignment placed before a branch lands in that branch's delay slot** — "stored in
   the beqz delay slot (even if 0)". We confirmed this works (func_8001F828) BUT it can
   extend the value's live range across a call in the other arm, forcing a callee-saved
   reg + save/restore. Tension is real; see HARVEST_NEARMATCHES func_8001F828.
5. **Loop increment placed last in the body lands in the loop-back branch delay slot.**
6. **Cache a loop-invariant count in a local and reuse it** rather than re-reading.
7. **Tentative definition (no initialiser) puts a symbol in .sdata** so maspsx -G expands
   it to gp-relative — matches our own gprel/.comm finding (LESSONS section 2).


**COMPILER BUG: cc1-27 SEGFAULTS on `break` inside a `do {} while` nested in an `if`**
(2026-07-25, hit while drafting Sce_flag_off). The build fails with
`make: *** [...] Error 139` (SIGSEGV) and NO error message — it looks like a build-system
problem, not a source problem, so it is easy to misdiagnose. Reproducer shape:
```c
if (cond) {
    do { if (*p == t) { *p = 0; break; } p++; } while (p < e);   /* <-- crashes cc1 */
    do { ... same ... } while (p < e);
}
```
**Workaround:** rewrite the loops with explicit labels and `goto` instead of `break`.
That compiles cleanly and produces the same semantics. (Verified: struct-vs-array
declaration of the walked object was NOT the trigger; removing `break` was.)
If you see `Error 139` from a `.c.o` rule with no diagnostic, suspect this first.


**Stack slots follow DECLARATION ORDER** (2026-07-25, func_80053B40). gcc-2.7 assigns
local stack slots in the order locals are declared. A `volatile` local declared before
an aggregate will claim the lower slot and push the aggregate up. Symptom: your struct
local sits at the wrong `N($sp)` and every store to it mismatches. Fix: reorder the
declarations. (Worth 5 rows on 53B40 — moving `RECT rect;` above a volatile.)

**Constants can never be spilled — they are REMATERIALIZED** (2026-07-25). gcc-2.7
attaches `REG_EQUIV` to constant-valued pseudos; under pressure reload re-emits the
`lui/ori` rather than spilling to stack. Consequence: "hoist a constant into a local to
raise register pressure" is a DEAD LEVER — it only trades instructions, it can never
produce stack spills. If you need genuine spills, you need live NON-constant (loaded or
computed) values.

**When does a register-to-register copy survive?** (2026-07-25, the rule that governs
live-range splits). There is no `regmove.c` in 2.7 — copies are eliminated by
`local-alloc.c:combine_regs` (line 1722). It ties the two pseudos (deleting the copy)
iff the SOURCE pseudo is DEAD at the copy insn; `block_alloc` scans FORWARD
(local-alloc.c:1090) so the "destination already has a quantity" escape never applies to
a first assignment. **And `cse` runs earlier and folds any plain `b = a` copy**, which
makes `a` dead — so a plain copy written in C is always deleted. Therefore a surviving
`addu $dst,$src,$zero` in a target usually indicates a RELOAD live-range split (a
compiler-internal artifact under register pressure), not something written in the source.
Look for corroborating evidence: reserved-but-never-touched stack slots in the target's
frame mean reload spilled pseudos whose accesses inheritance later removed.

**Held-pointer vs constant-folded absolute address** (func_8001D394, cd.c,
2026-07-14). Writing `CdMix(D_800AD140 + 0x14)` makes cc1 constant-fold the
symbol+offset into a single absolute address and emit `lui/addiu %hi/%lo(SYM+0x14)`
(here it even re-resolved to the neighbor symbol `&D_800AD154`). To instead make
cc1 HOLD the base symbol in a register and add the offset at runtime
(`addu a0, v1, 0x14` reusing a base loaded early and kept live across a branch),
assign the symbol to a local pointer first and offset THAT:
`u8 *p = D_800AD140; ... CdMix(p + 0x14);`. The local becomes a pseudo that reload
keeps in a register. **This is the lever for the parked BB4C addressing-crux**
(draft emits absolute `sh $v1, D_800...`; reference reuses a held `$s1`=r pointer)
— try a held local pointer for `r` there. Contrast: a bare `SYM + off` used once
gets folded to absolute.


1. **Function emission order**: at -O1+ all compiled C bodies are deferred to
   the end of the TU, after every top-level `__asm__` block. Fixed by the
   reorder pass in `tools/patchasm.py` (moves bodies to their in-place
   `.globl` markers). `static` functions have no marker — avoid `static`.

2. **Global addressing (-G8) — the real mechanism (rewritten 2026-07-04;
   the earlier version of this rule had never actually been exercised)**:
   - cc1 does NOT choose the addressing mode. It emits a bare memory operand
     (`sb $2,foo`) for every global, plus `.extern foo, SIZE` for small
     scalar externs, and delegates the sdata decision to the assembler.
   - Our GAS runs -G0, so by default every bare ref becomes lui/$at
     (2 instructions, address recomputed per access) — never $gp.
   - `tools/gprel.py` (between maspsx and patchasm) closes the gap: a small
     `.extern` symbol that is gp-accessed anywhere in the extracted original
     asm (census of `%gp_rel(` in asm/rock_neo/**/*.s) gets its bare refs
     rewritten to explicit `%gp_rel(sym)($gp)`, and load-delay nops that
     maspsx deleted (it assumed a multi-insn $at expansion) are restored.
     It also drops those `.extern` directives — GAS would otherwise emit a
     COMMON symbol that the linker allocates at a bogus address, silently
     shifting the whole data segment.
   - Declaration style is still the per-TU control:
     `extern u8 foo;` → gp candidate; `extern u8 foo[];` (+ `foo[0]` at use
     sites) → forced lui/$at. A pointer global accessed via lui:
     `extern T *foo[];` and use `foo[0]`.
   - The original binary mixes addressing PER TU for the same symbol:
     D_80098910 is gp-accessed in still-unsplit asm but lui-accessed by
     original game.c — hence game.c declares it as an unsized array. When a
     diff shows gp-vs-lui disagreement, flip the declaration in that TU, not
     the tool.
   - Symptom table: whole-binary pointer shifts after adding an extern ⇒
     COMMON leak (declaration made a small extern the pass didn't approve);
     `relocation truncated R_MIPS_GPREL16` ⇒ gp-rewrote a symbol out of gp
     range (declare it as array).

3. **Swap/temp-variable ordering drives register allocation.** For a byte swap,
   `t = x[a]; x[a] = x[b]; x[b] = t;` matched; the equivalent
   "load both into temps, then store both" form produced mirrored registers
   (addu operand order + which sum lands in a0). Write the *idiomatic* form
   first; only get creative when the diff says so.

4. **Statement order ≈ evaluation order, but the scheduler interleaves.**
   `m->x10 += 0xC; m->script += 1;` emitted lhu/lw/addiu/addiu/sh/sw —
   both statements' loads hoisted together. Don't fight interleaving; match
   the *statement* order to the first-use order in the asm.

5. **Aliasing blocks CSE**: `m->x3E = m->script[1]; m->script += 2;` loads
   `m->script` twice (sb through the struct may alias the pointer field), so
   two back-to-back `lw`s of the same field in asm ≠ a weird source construct.

6. **Return-value-in-delay-slot** (`jr ra` / `addiu v0,zero,N`) is just
   `return N;`. `addu v0,zero,zero` in the slot is `return 0;`.

7. **`li` vs `addiu`**: patchasm already rewrites `li reg,<0xFFFF` to addiu
   (assembler would emit ori otherwise). Existing behavior; keep it.

## Known struct facts
- **MOJI_TASK** (moji.c, arg to the func_8005xxxx family): 0x10 u16, 0x14
  `u8* script` (a script/text stream — most tiny moji funcs advance it),
  0x3E s8, 0x6C s32, 0x72 s8, 0x78 s8. The +N accessor families are script
  opcode handlers, which sizes their operands.
- **GAME_WORK.x53** at 0x53 (u8 counter, func_8001D974 increments it).
- **PL_WORK.rb_parts_sort_data** at 0x454, u8[0x20].
- Cd sentinel: `unknown_Cd_strucptr != &D_800A3A40` = "read not done" (Cd_read_sync2).

## Workflow tactics that worked
- Harvest sibling families together (12 moji accessors in one verified batch).
- One rebuild+hash validates an entire batch AND any refactor of prior
  matches simultaneously — refactoring to structs is cheap, do it early.
- When a function mismatches, the binary diff (`build/rock_neo.diff`)
  isolates WHICH function; then reason about the shape difference (register
  mirroring → reorder temps; extra/missing nop → aliasing or scheduling).

## Verification ritual (anti-hallucination)
- A batch is DONE only when `make CPP=cpp check_rock_neo_only` prints OK.
- Ground truth: `cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe` — direct
  byte comparison, no hashing, no trust required.
- Liveness audit (prove the C is really in the binary): deliberately break a
  matched function (`+1` → `+2`), rebuild — check MUST fail; revert — OK.
  If breaking the C didn't break the build, the C wasn't being compiled in.

## Session 2026-07-04 (evening) additions
- **Global-load vs pointer-store aliasing (func_80057124)**: cc1 will not
  hoist a load of a lui-addressed global above a store through a pointer
  (may-alias). If the target asm loads the global BEFORE the pointer store,
  the source must too: read the global into a local temp first
  (`s32 z = Game_work.zennyCount; m->script2 += 1; m->x40 = z;`).
- **`*p |= x` on a computed address**: writing the deref inline
  (`*(u16*)(C + (i<<7)) |= x`) recomputes the address for the store
  (lui/addiu %lo form). Computing the pointer into a local first
  (`u16 *p = (u16*)(C + (i<<7)); *p |= x;`) materializes the constant once
  via lui/ori and reuses one address register for lhu+sh (func_80012FA4/FC8).
- **diff.py misalignment red herring**: when an upstream function in the same
  file is the wrong size, diff.py's TARGET column for functions after it
  appears to show missing/extra instructions at the top. Check the original
  .s file before "fixing" a function that may already match (func_80058C08).
- **Counting matched functions**: `objdump -t build/src/rock_neo/X.c.o | grep
  -c "F .text"` minus active INCLUDE_ASM stubs (count `.include nonmatchings`
  in preprocessed source). This caught 7 upstream game.c functions that were
  compiled+matching but never counted.

## Session 2026-07-04 (night) additions — proven against the hash
- **Pointer-global declaration depends on USE COUNT in the TU**: the
  `extern T *foo[]; foo[0]` convention only matches functions that touch the
  pointer once (Cd_read_sync2). When a function reads AND writes it
  (read ptr, advance, store back), the array form makes cc1 CSE the array
  base into a register (lui/addiu + lw 0(reg)) — mismatch. Declare a plain
  scalar pointer (`extern CD_CMD *unknown_Cd_strucptr;`) to get bare
  lw/sw per access (GAS lui/$at form, matching the original). The gp census
  keeps scalar decls safe now; COMMON leaks are gone (gprel.py drops every
  small .extern, not just census-approved ones).
- **`volatile` for callback counters**: original vsync_cb stores the
  incremented counter then RELOADS it for the return value. A plain extern
  lets cc1 reuse the register (one lw short). `extern volatile s32 ...`
  reproduces the reload. If a diff shows a "redundant" load after a store
  of the same global, think volatile, not weird source.
- **Ternary vs if/else register choice**: `x = cond ? a : b;` put the temp
  in $a0; separate `if/else` with direct stores cross-jumped into the same
  single-store shape but with $v0 (func_8001997C). If the value register is
  mirrored on an if/else-shaped diff, flip between ternary and if/else.
- **Local init order drives register assignment** (func_8001DEE4): `i` then
  `p` initialized in that order gave i=$v1, p=$v0 matching the original;
  the reverse order mirrored them. Cheap knob before deeper surgery.
- **Statement order vs emission order — sched1/RA/sched2** (func_8005459C):
  cc1 schedules on pseudos, allocates, then schedules again. The matching
  source order (x10=x8; x12+=0xC; x3E=x70; script+=1) emits loads
  x8,x70,script first and pushes x12's store into the delay slot — nothing
  like the statement order. When 2-3 reorderings fail, brute-force ALL
  orderings in a scratch TU through the real pipeline (cpp|cc1|maspsx) and
  compare instruction streams — 4 variants per compile, seconds each,
  exact-match test without touching the tree.
- **MOJI_TASK is a script VM task**: 0x18 = `u8* stack[8]` call stack,
  0xBE = u16 stack pointer (func_80054AB4 is the "return" opcode:
  `m->script = m->stack[--m->xBE];` — the pre-decrement form is REQUIRED;
  two statements would reload xBE through the aliasing store and add an lhu).
- **CD_CMD queue**: unknown_Cd_strucptr points into a 0x10-stride command
  queue; writers fill {cmd, arg0, arg1} and advance the pointer (cmds seen:
  1, 4, 6). Cd_read_sync2's sentinel compare is the queue-drained check.

## Session 2026-07-04 (late night) additions — proven against the hash
- **Empty stack frame = dead local array**: Sce_flag_test allocates/frees an
  8-byte frame it never touches. Reproduced with an unused `u8 buf[8];`
  local — cc1 keeps frame space for local arrays even at -O2 when all real
  computation lives in registers. If a leaf function has `addiu sp,-N` with
  no stores to the frame, add a dead N-byte local array.
- **Param reassignment for in-place shifts**: `srl a0,a0,3` (result back in
  the argument register) comes from literally reassigning the parameter
  (`flagno = (u32)flagno >> 3;`), not from a fresh local.
- **Split computation across a load**: li 0x80 ... lbu ... srav (mask
  computation straddling the array load) = mask computed into a LOCAL in a
  separate statement; the scheduler then hoists the load between li and
  srav. Inline `(0x80 >> (x & 7))` keeps li+sra adjacent — one insn off.
- **Branch sense from the asm**: bgez that jumps TO the store/return-1 block
  = `if (x >= 0) { ...; return 1; } return 0;`. bltz jumping to return-0 is
  NOT equivalent codegen even though it's equivalent C.
- **`m->field += 1` as a call argument** (`f(a, b, m->script += 1)`) passes
  the incremented value straight from the RMW register — no reload. Args
  evaluate right-to-left, so the RMW's load comes first.
- **Struct fields confirmed**: MOJI_TASK stack2[8] @0x4C + u16 xC0 sp
  (script2 mirror of the script call stack); PL_WORK x9 (state id),
  u16 xA (sub-state), key masks x11C/x11E vs x138/x13E, x449; SCENE_WORK
  created (0x800C4C48, size 0xA8): x8/x9 bytes, x10/x18/x1C words, xA4 ptr.

## Session 2026-07-05 (overnight) additions — proven against the hash
- **`.comm` (tentative definition) blocks the gp rewrite — fixed in gprel.py**:
  `u8 Moji_flag[8];` in moji.c emits `.comm Moji_flag,8`. That COMMON is
  INTENTIONAL — splat carves such symbols out of the extracted data
  (89260.scommon.s starts at 0x80098A60, right after Moji_flag's 8 bytes at
  0x80098A58) and the linker's COMMON allocation lands it exactly there. But
  gprel.py only census-checked `.extern` symbols, so bare refs to Moji_flag
  in the defining TU stayed lui/$at (+1 insn per access). gprel.py now also
  treats small `.comm` symbols as census candidates (keeps the directive,
  rewrites the refs). Do NOT replace such a definition with an extern:
  nothing else defines the symbol and the link fails.
- **A 4-byte whole-data shift is not always a COMMON leak**: a C function
  that compiles one instruction short shrinks .text by 4 and shifts every
  data address after it — same symptom table entry, different cause. Check
  the function's own diff (instruction count) before hunting extern decls.
- **Compute-into-locals at the load site to keep values live across stores**
  (func_80054BB4/55CC4): `u8 c = m->x72 + 1; u32 f = m->flags | K;
  u8 *s = m->script2 + 3;` then plain stores afterwards reproduced
  "all three loads up front, stores late, adds between" (three temps live
  simultaneously, const in $a2). Writing `m->x72 = c + 1` at the store site
  instead lets cc1 collapse each temp back into a sequential RMW — different
  schedule, mismatched.
- **Alias-forcing read via adjacent symbol** (func_8001DD88): the original
  does sb D_80098199 THEN lh D_80098198 (the u8 pair read back as s16).
  `*(s16*)&D_80098198` let cc1 hoist the lh above the sb (different symbols
  = no alias). Writing the read as `*(s16*)(&D_80098199 - 1)` — the same
  address, but through the symbol just stored — forces the order and
  assembles to identical bytes (gas folds the -1 into the gp offset).
  volatile did NOT fix this (cc1 2.7 still reordered).
- **Ternary CHAIN puts the value in $v0** (func_8001FB54/FB8C):
  `x = a<0 ? K1 : a<6 ? K2 : ...` produced the compact
  bltz/slt/bnez-with-li-in-delay shape with the value in $v0. The if/else
  ladder version put the value in $a0 with extra j's. (Complements the
  night-session single-ternary lesson where the ternary got $a0 — chains
  behave differently from single ternaries.)
- **Reuse the call-result variable** (func_80019AA4): `v = f(x); if (v)
  v = 0x2D; else v = 0xFFFF;` keeps everything in $v0 and puts the taken
  branch's assignment in the delay slot. A separate result variable
  allocated $v1/$a0 and flipped the branch sense.
- **Parenthesization steers add order** (func_8005753C): `p + (p[1] + 2)`
  adds the constant to the byte BEFORE adding the pointer; `p + p[1] + 2`
  is the mirror. Bytes differ; match the original's addu order.
- **`m->x40 = g; f(0, m->x40, ...)`** (func_80057144): passing the field
  (not the local/global) as the argument reproduces the reload-after-store
  (aliasing forbids forwarding), matching lw x40 right after sw x40.

## Session 2026-07-05 (Opus) additions — proven against the hash
- **A called function's declared RETURN TYPE steers the caller's register
  allocation** (func_80057B70): `func_80043294(...)` is called for effect
  only, then `m->script2 += 1; return 1;`. Declared `void`, cc1 put script2
  in $v0 and needed a load-delay nop before the increment (one insn too
  long). Declared `s32` (its real type), $v0 stays reserved from the call
  return, so script2 lands in $v1 and the `return 1` (`li $v0,1`) fills the
  load-delay slot — exact match. When a leaf has a spurious nop after a
  post-call reload, check whether the callee should be typed non-void.
- **STALE-OBJECT FAKE MATCH — the failure mode the mutation test exists for**
  (func_800605DC): `.c.o` are make *intermediate* files. Converting a
  function from INCLUDE_ASM to C without `touch src/rock_neo/*.c && rm -f
  build/rock_neo.elf` leaves the OLD stub object linked; the hash then passes
  on the STUB, not your C. func_800605DC's C didn't even COMPILE (a `void` vs
  header `unknown_t` type conflict) yet last session's hash was green. ALWAYS
  clean-rebuild before claiming a match, and prefer the liveness mutation
  test (break the C → hash MUST fail). If breaking the C doesn't break the
  build, the C isn't in the binary. The objdump function count can't catch
  this (stale .o still exports the symbol + the stub source line is gone).
- **One symbol, two per-TU views**: Code800133D8_work is `CODE_800133D8_WORK`
  (s16 x0) in game.c (writes `.x0 = 1` as sh) but zeroed as three words in
  func_800133D8. Declaring `extern s32 Code800133D8_work[3];` locally in
  Code800133D8.c gives the three independent `sw $0,sym / sym+4 / sym+8`
  (constant-index global array → separate %hi/%lo per element, no base-reg
  CSE). Don't widen the shared struct to force word stores — it breaks the
  other TU's halfword access. Give each TU the view its codegen needs.
- **Script2 call-stack push mirrors script1** (func_800576C4): the stack2
  (0x4C) / xC0 (sp) / script2 (0x6C) trio behaves exactly like the
  stack/xBE/script trio (func_80057708): `m->stack2[m->xC0++] = m->script2 +
  1; m->script2 = D_8008CBA4[D_80098830]; return 1;`.

## Session 2026-07-05 (Fable, second) additions — proven against the hash
- **Symbol-indexed loads need a SEPARATE offset variable** (func_8001A1FC /
  func_8001A238): `for (i = 0; i < 5; i++, off += 8) if (tbl[off] == key)`
  emits the original's per-iteration `lui $at / addu $at,off / lbu
  %lo(sym)($at)` form (cc1 keeps the symbol+index operand). Deriving the
  offset from the counter (`tbl[i * 8]`) makes cc1 strength-reduce to a
  marching pointer (`la` once + `addu p,8`) — different bytes. Two induction
  variables in the source = symbol-indexed; one = pointer-march.
- **`((u8*)Game_work)[k] == key` gives a clean lbu with NO andi** when `key`
  is declared `s32`. Declaring the param `u8` makes cc1 re-mask it
  (`andi $4,$4,0xff`) even though callers already pass a byte. When the asm
  compares a param raw, the param is int-typed in the source.
- **Casting an s8-array symbol through (u8*) selects lbu** without disturbing
  the same TU's lb accesses of other elements (sound.c reads Game_work[0x53]
  as s8/lb but the 0x1BA table as u8/lbu). No second extern needed.
- **OPEN PROBLEM (CLOSED 2026-07-06: a `switch` on the byte field keeps the
  beq un-inverted — see the switch entry at the end of this file)
  — equality leg in a ternary chain (func_8001FCA4)**: cc1
  tree-folds `(v == 5) ? A : B` and `(v != 5) ? B : A` to the same RTL, and
  jump-opt then inverts the branch (bne→end with B in the delay slot). The
  original has the UN-inverted form (beq→end with A in the delay, then
  j→end with B in the delay). Chain-style ternaries produce legs 1–2
  correctly (value in $v0, bnez/delay); statement-style if/else produces the
  equality leg correctly but breaks legs 1–2 (beq+j shape, value in $a0).
  ~12 forms tried, none combine both. Re-stubbed. If another ==-inside-chain
  function matches later, back-port the trick here.

## Session 2026-07-05 (Fable, MojiTaskExec) additions — proven against the hash
- **Callee-saved register MIRRORING can be an allocno-priority TIE — and the
  tie-break is diagnosable, not guessable (MojiTaskExec).** cc1's global
  allocator (gcc 2.7.2 global.c) orders pseudos by
  `floor_log2(n_refs) * n_refs / live_length` (×10000, integer-truncated);
  ties break toward the LOWER pseudo number (params first). MojiTaskExec's
  `no` param (6 refs / 88) and the CSE'd 0x40000 mask constant (3 refs / 22)
  tie EXACTLY (12·22 = 3·88), so `no` stole $s2 from the mask. Diagnose with
  `cc1 ... -dl` and read `Register N used X times across Y insns` in the
  .lreg dump (pipe the same cpp output the build uses). The fix that matched:
  declare the byte-sized param as `u8` — its QImode entry copy adds one insn
  to the param's live length (88→89), dropping its priority below the
  constant's. Statement splits do NOT work for this (cse folds them before
  flow counts); the perturbing insn must survive cse.
- **`-dl`/`-dg` dumps are usable on cc1-27** and turn register-mirroring
  from trial-and-error into arithmetic: .greg shows allocation order +
  dispositions, .lreg shows per-pseudo refs/live-length and the full RTL.
  Worth reaching for whenever two callee-saved regs are swapped and the
  usual knobs (decl order, locals, ternary/if flips) don't move them.
- **A u8 param that the asm masks with andi at USE sites (not at entry) is
  legal**: `u8 op` arriving in $a2 stays raw; each body use re-masks
  (two andi's here — cc1 did NOT CSE them across the sb to x3E). This
  coexists with the earlier "raw compare ⇒ int param" rule: masked-at-use
  compares ⇒ u8 param.
- **Aliased-pointer table read before a store to the base struct**
  (`s = script_base + *(u16*)(script_base + op*2); m->x44 = script_base;`):
  compute the loaded value into a local BEFORE storing through m — cc1 won't
  hoist the lhu above `m->x44 = ...` itself (may-alias), and the scheduler
  then fills the lhu load-delay slot with the x44 store, matching the original.
- **`f = *(u32*)Moji_flag | K1; ...stores...; *(u32*)Moji_flag = f | (K2 >> no);`**
  reproduces "load+first-or early, shift+second-or late": splitting the OR
  chain across statements pins which constant merges before the stores.
  Writing it as one expression let cc1 reassociate (shift|K1 first) — mismatch.
- **Chained store `m->script2 = m->x48 = s;`** stores x48 then script2 from
  the same register with no reload (separate statements would reload via
  the aliasing rule). Slot-4 fields written as `Moji_work[4].flags` etc.
  reloc to the same bytes as the original's separate D_800BB9C8/D_800BBA8A
  symbols (constant-index into the extern array).
- **ASPSX $at-expansion operand order is PER-SYMBOL, not uniform**
  (func_80012F24): bare-symbol+reg accesses to main.c's D_801F81xx family
  expand `addu $at,REG,$at` (base-reg-first) in the original binary, while
  every other symbol (sound's Game_work+0x1BA, jtbl loads) expands
  `addu $at,$at,REG`. GAS and stock maspsx only produce the latter.
  tools/maspx now has ASPSX_REGFIRST_SYM_PREFIXES ("D_801F8") gating the
  reg-first order for loads AND stores (stores were previously left to GAS
  entirely). If a future function mismatches ONLY in an addu-$at operand
  order, add its symbol prefix to that table — don't hunt C shapes; the
  source form cannot control this (it's an assembler macro).
- **`sh $0,SYM($reg)` / `lw $x,SYM($reg)` (symbol-indexed with variable
  offset) comes from ARRAY INDEXING with a scaled index** (`D_801F8100[n <<
  6]`, `D_801F8108[n << 5]` — cc1 CSEs both scaled offsets into one n<<7
  register). Casting arithmetic (`*(u16*)((u8*)sym + off)`) makes cc1
  compute the address into a real register instead — different bytes.
- **Raw-constant pointers (0x801F8300) use lui/ori, symbols use lui/%hi**:
  `u16 **q = (u16 **)0x801F8300;` reproduces the lui/ori pair; an extern
  symbol declaration would assemble to %hi/%lo with a negative low offset
  (different bytes) when the low half >= 0x8000.
- **Same symbol-indexed slot read TWICE → cc1 CSEs the address (wrong
  bytes); alias the second read through a NEIGHBOR symbol** (func_80012FEC):
  two `D_801F811C[n << 5]` reads made cc1 materialize sym+(n<<7) into a
  register (lui/addiu/addu + two 0(reg) loads). The original keeps both in
  $at symbol-indexed form. Fix: write the second as
  `((u8 **)D_801F8114)[(n << 5) + 2]` — a different symbol+addend is
  different RTL, so no CSE, and %hi/%lo(D_801F8114+8) relocates to the
  same bytes as %lo(D_801F811C). volatile does NOT fix this (address CSE,
  not load CSE).
- **Queue-writer tail order needs an IN-PLACE increment** (Sound_call2):
  with trailing `q->x8 = args[1];` then pointer advance, `D_80098938 = q + 1`
  lets cc1's scheduler hoist the global store above the field store (global
  mem vs q-based mem look independent). `q++; D_80098938 = q;` makes the
  increment clobber q's own register — anti-dependence pins the order.
  Sound_call (no trailing arg loads) matches either way; prefer q++ form.
- **cc1 jump-canonicalization family (func_8001FCA4/FC50/FDE4, all scene
  0x8001FCxx-FDxx)**: when several ternary/if legs assign the same constant,
  cc1-27 cross-jumps them into one li block and/or inverts a beq→store-with-
  value-in-delay into bne→return. No source shape found (ladder, chain,
  goto, arm swaps all canonicalize identically). UPDATE 2026-07-06: a
  `switch` on the value defeats this — FCA4 matched; try switch on FC50/
  FDE4 next (see the switch entry at the end of this file).
- **Negative constant through a narrow ANSI prototype truncates at the CALL
  SITE** (sub_scrn's MojiTaskExec(..., -1)): `u8 op` in the prototype makes
  cc1 emit li a2,0xFF; the original has li a2,-1. Fix: declare the function
  K&R-style (`s32 MojiTaskExec();`) so callers pass default-promoted ints
  raw, while the DEFINITION keeps its ANSI narrow param (whose callee-side
  entry copy can be register-allocation-load-bearing). Mixed K&R decl +
  ANSI def is how the original code behaves.
- **Check for pre-existing gated drafts before writing C**: sub_scrn.c
  carried 10 draft bodies behind `#ifndef ACCEPT_REORDERING_BULLSHIT` from
  before tools/patchasm.py's reorder pass existed; 9 of 10 byte-matched
  once enabled. game.c defines the macro (its drafts were always live).
- **gp-vs-lui disagreement can be PER-FUNCTION, not just per-TU**
  (func_800600CC): sub_scrn.c's matched Sub_screen_rb_parts_set reads
  Moji_flag via $gp, but func_800600CC in the SAME TU needs the 2-insn lui
  form. Only different source expressions explain both: use a raw-address
  deref (*(u32 *)0x80098A58) for the lui-form site — GAS expands the bare
  constant to the identical lui/lw bytes, and gprel.py ignores it.
- **maspsx: bare-constant-address loads need the same delay-nop rules as
  bare-symbol loads**: cc1 emits `lw $2,-2146858192` for constant derefs;
  stock maspsx only nop-checked the symbol form, so the hazard nop before a
  dependent consumer was dropped (function assembled 1 insn short).
  tools/maspx now routes r_source-is-None loads (symbol OR constant) through
  the same branch.
- **A make error can hide behind a passing hash**: if a TU fails to COMPILE,
  make leaves the previous .o and the link/hash can still pass on stale
  bytes (this is how the moji.c K&R-vs-ANSI prototype conflict went
  unnoticed for two batches). Grep the make output for errors (or check
  make's exit status) BEFORE trusting check_rock_neo_only.
- **K&R declaration requires K&R definition when params have default
  promotions**: `s32 f();` + ANSI `s32 f(..., u8 op) {}` is a compile error
  in cc1 (C89 rule). Convert the DEFINITION to old-style
  (`s32 f(no, base, op) s32 no; u8 *base; u8 op; {}`) — cc1 generates
  byte-identical .text to the ANSI definition (entry copy preserved).

## Session 2026-07-05 (Fable, day/batch 10+) additions — proven against the hash
- **setcc (sltu) tail vs branch+li: share the return-0 by goto** (func_8001DE84):
  a function ending `if (x & 8) return 1; return 0;` gets its tail collapsed
  to `sltu $v0,$0,$v0` when the diamond is clean. The original's
  bnez-with-li-1-in-delay + fallthrough-0 shape came from an EARLIER leg
  jumping to the same return-0 (`goto zero;` ... `zero: return 0;`) — the
  extra inbound edge to the 0-store blocks jump.c's store-flag conversion.
  Separate `return 0;` statements do NOT work even though they're equivalent C.
- **Store-order + fn-table-load pinning: three addressing forms behave
  differently** (func_800629F0). Original: sh field1, sh field2, lw
  fntbl[i], la $a0,&struct.
  - RAW-ADDRESS stores (`*(u16 *)0x800C4C14 = a`): right bytes, but the
    scheduler hoists the symbol-indexed lw above them (constant vs symbol
    MEM disambiguates) and sinks one sh to the jalr slot. Mismatch.
  - SAME-SYMBOL field stores (`Debug_work.x4 = a`): order pinned (symbol
    MEM vs variable-indexed symbol MEM can't be disambiguated), but cse
    anchor-CSEs the &Debug_work call arg against a store address
    (la sym+4 ... addiu $a0,-4). Mismatch.
  - NEIGHBOR-SYMBOL stores (`((u16 *)&Scene_work)[-26]` where Scene_work =
    Debug_work+0x38): symbol MEM still pins the lw, and the arg's
    Debug_work ref no longer shares a symbol with the stores, so no anchor.
    %lo(Scene_work-52) assembles to the same bytes as %lo(Debug_work+4).
    Extends the batch-4 neighbor-symbol trick from loads to stores.
- **`p[i + K]` vs `*(p + i + K)` steer the index addu operand order**
  (func_8001319C): the bracketed form emitted addu $v0,$4,$2 (index first),
  the explicit left-assoc pointer arithmetic addu $v0,$v0,$4 (pointer
  first). Same bytes otherwise; flip between them on an addu operand mirror.
- **OPEN PROBLEM — surviving andi 0xFFFF truncations (func_80041EF4)**: the
  original masks two lhu-loaded u16s with andi 0xFFFF when passing one of
  them (if/else-selected) to func_80041F54's int-typed 4th arg. cc1's
  combine elides the masks in every source shape tried (~12): nonzero_bits
  accumulates across multi-set pseudos AND reassigned params (the dead
  entry copy is flow-deleted first), so all defs trace to lhu → provably
  ≤0xFFFF. Verified via -dj/-dc dumps: zero_extends present in .jump,
  gone after combine. A matching source needs a reaching def combine can't
  trace; lb/lw/call results change the bytes. Parked; revisit if another
  function keeps a "redundant" andi.
- **Register-birth family grows (func_8001A6DC, cf. func_80042044)**: when
  a call result must live across a second call whose arg derives from a
  still-live param (`a = f(n); b = f(n + 1);`), cc1 schedules the n+1 arg
  setup above the a=v0 copy, making a's and n's ranges disjoint → they
  share $s0 and the function is 2 insns short (no $s2 save/restore).
  Statement splits, named temps, and K&R protos don't stop the reorder.
- **Scratch TUs must mirror the real include chain**: func_8001E390 was
  drafted against local `void Sce_flag_on(s32);` decls, but rock_neo.h
  (pulled in via game.h) already declares them `unknown_t (unknown_t)` —
  in-tree that's a compile error. The unknown_t (int) prototypes produce
  the same bytes; drop local decls that shadow rock_neo.h ones, and check
  rock_neo.h before declaring any Sce_/Game_/Moji_ function locally.
- **A failed TU can still leave a fresh-looking .o**: the build pipes
  cpp|cc1|maspsx|...|as, so when cc1 errors mid-TU, GAS still assembles the
  truncated stream and writes a PARTIAL .o with a current mtime before make
  aborts. Combined with a stale exe, `check_rock_neo_only` prints OK. After
  any batch land, run the error grep on make output yourself AND check
  `cmp` + artifact mtimes; don't let a subagent's "pass despite error"
  reasoning stand (batch 12's cd.c decl conflict was reported as PASS).
- **Check for pre-existing K&R-era declarations before defining a stub**:
  earlier sessions declared unmatched callees with guessed signatures
  (cd.c's `func_8001D254(s32, s32, u8*)`). The real definition's types
  won the diff; updating the old declaration didn't change the matched
  caller's bytes (constant args). grep the TU for the function name before
  writing the definition.
- **The goto-shared-return trick (func_8001DE84) is a FAMILY tool**: any
  tail of the form `if (cond) return K; return 1-K;` that cc1 collapses to
  sltu/sltiu can be fixed by making the final return a label that an
  EARLIER branch also jumps to (func_8003F224 confirmed). Look for the
  original's fallthrough-into-li + shared epilogue label.
- **`and` operand order follows source operand order** (func_8003F224,
  func_80040224): `pl->field & k` puts k (the older live register) first
  in the emitted and (`and $v0,$k,$field`); `k & pl->field` mirrors it.
  Cheap first knob for a single-insn and mirror.

## Session 2026-07-06 (Fable + Cursor, batch 16 / FCA4) additions — proven against the hash
- **`switch` beats jump-canonicalization on equality legs** (func_800400B8,
  func_800164B4, and now func_8001FCA4 — the parked OPEN PROBLEM above is
  CLOSED): a `switch` on a byte-sized value keeps the ==K leg as an
  UN-inverted beq/beqz with the case body OUT of line (beqz/beq/j ladder),
  where the equivalent if/else or ternary chain gets tree-folded and
  jump-inverted (bnez with the body inline). cc1's switch expander
  (expand_case) emits an explicit compare/branch ladder for small sparse
  case sets, and jump.c does not re-canonicalize those branches. Recipe
  for FCA4: load the field into an s8 local, switch on it, group cases
  0..4 as fallthrough labels onto one body, ==5 leg its own case, default
  the third value; assign to a local and store once after the switch.
  Try `switch` FIRST on any parked jump-canonicalization sibling
  (func_8001FC50, func_8001FDE4, func_800199A4, func_80019918).
- **Switch case-TREE vs range-collapse is steerable by an explicit case**
  (func_8001FC50): `switch` with only cases 1..3 range-collapses to
  slti/blez (one bounds check). Adding an explicit `case 0:` that shares
  the default body forces the balanced case tree — beqz(==0), bltz(<0),
  slti 4 — matching the original's three separate exits.
- **Fallthrough-into-default keeps a shared constant leg un-inverted**
  (func_8001FC50): the inner `if (call() == 0) { out = 0x82; break; }`
  followed by FALLTHROUGH into `default: out = 0x81;` emits
  bnez→(default li in the branch delay slot, retargeted to the store) +
  li/j for the else — the original's exact shape. Writing the same logic
  as `out = call() ? 0x81 : 0x82;` inside the case gets inverted+inlined.
- **A switch may store through a LOCAL then one store after** (all three
  FCxx/FDE4 matches): assign the s16 local in each case, single
  `D_xxx = out;` after the switch. FDE4 shows duplicate constant case
  bodies (4→0xC4, 0xB→0xC4) do NOT get cross-jumped when written as
  separate cases — write them separately if the original has two li's.

## Session 2026-07-06 (Fable, jump-table infrastructure) — proven against the hash
- **Dense-switch jump tables now WORK from C** (func_8001FCE4, first table
  function matched). Mechanism: cc1 emits the table inline as
  `.rdata / .align 3 / $Ln: .word $Lcase...` inside the function body; the
  whole block survives maspsx/gprel/patchasm untouched and GAS maps .rdata
  to a .rodata section in the object. The table entries relocate against
  .text, so if the function's code matches, the table bytes match for free.
- **Placement recipe** (repeat per newly matched table function):
  1. Find the table's ROM range in the extracted rodata (comment column of
     `asm/rock_neo/data/*.rodata.s`, e.g. jtbl_80010920 = ROM 0x1120-0x113F).
  2. Split the rodata asm file around it (800.rodata.s now ends at ROM
     0x1120; 1140.rodata.s carries on) — the carved range comes from the
     C object instead.
  3. In rock_neo.ld, place `<tu>.c.o(.rodata)` between the two asm pieces.
  4. splat yamls updated to match (0x1120/0x1140 rodata subsegments); a
     re-extracted 1120.rodata.s is intentionally not linked (/DISCARD/).
- **Verified live**: full-binary sha1 + cmp on first build; separate
  mutation tests for a case VALUE (code bytes) and a case-to-body REMAPPING
  (table bytes only) both broke the check, restored OK. Map file shows
  scene.c.o .rodata at exactly 0x80010920, size 0x20.
- **Constraints to respect later**: a TU's tables all land in ONE .rodata
  section in FUNCTION ORDER, so a multi-table TU needs its matched table
  functions' original tables to be CONTIGUOUS in rodata (they are, for
  scene's F8DC/F9AC/FCE4 trio) or matched in address-contiguous groups
  with separate carves. The .main SUBALIGN(4) means a carve point that is
  not 8-aligned would fight cc1's `.align 3` — check alignment when carving
  (0x80010920 was 8-aligned; all jtbl starts observed so far are).
- **Switch codegen census for table functions**: sltiu bound + sll 2 +
  lui/addu/lw %lo + jr; case bodies as `j <shared store>` with the li in
  the delay slot. The FCE4 shape (byte switch, s16 local, single gp-half
  store after) is the same source template as the FCA4/FC50/FDE4 ladder
  family — cc1 picks table vs ladder purely by case density (8 dense
  cases → table; ~3-5 sparse → beq ladder).
- **asm/ and rock_neo.ld are GITIGNORED (generated state)** — the rodata
  split (800/1140) and the scene.c.o(.rodata) line in rock_neo.ld exist
  only in the local checkout. The committed source of truth is the splat
  yamls' `- [0x1120, .rodata, scene]` subsegment; anyone re-extracting
  must verify the regenerated rock_neo.ld places scene.c.o(.rodata) at
  0x1120 (splat's leading-dot syntax does this without extracting). A
  fresh clone that runs extraction gets it from the yaml; THIS checkout
  was hand-edited to match without re-running splat.

## Session 2026-07-06 (Fable, multi-table proof: scene F8DC/F9AC) — proven against the hash
- **MULTI-TABLE carve works: 3 jump tables from ONE TU, contiguous.**
  scene.c's func_8001F8DC/F9AC/FCE4 each emit a jump table; all three now
  come from scene.c.o(.rodata) at 0x800108C0 (size 0x80 = 0x30+0x30+0x20,
  in FUNCTION order = ascending table address). 800.rodata.s truncated at
  0x10C0; scene.c.o(.rodata) fills 0x10C0-0x113F; 1140.rodata.s from 0x1140.
  Constraint CONFIRMED: the TU's matched table functions must be
  address-contiguous in rodata (no unmatched asm table interleaved), because
  a TU's .rodata is one contiguous linker chunk. You cannot land just F8DC
  while F9AC's table stays in asm between F8DC's and FCE4's.
- **Case-0 `||` ternary beats the else-precompute (func_8001F9AC — the real
  unlock).** `if (test()!=0) out=A; else out=B;` inside a case makes cc1
  PRECOMPUTE `out=B` before the branch and invert it (jump.c). That extends
  `out`'s live range across the branch insn where the Sce_flag_test result
  is live in $v0, so `out` gets `conflicts: 2` in the .greg dump and is
  denied $v0 — it lands in $v1 and EVERY value store in the function
  mirrors ($v1 not $v0). Rewriting as `out = (test()==0 || test()!=0) ?
  A : B;` keeps `out` in $v0 (no premature def, no cross-branch liveness),
  fixing the whole function. Diagnosis path: `cc1 ... -dg` → read
  `;; N conflicts: ... 2 ...` (2 = $v0) in gccdump.greg; a value pseudo
  that conflicts with $v0 is being held live across a call-return branch.
- **F8DC case 0 uses the goto-shared-store idiom** (LESSONS func_8001DE84):
  two flag-test exits both `goto d0_3C;` where d0_3C does the single
  `D_800981D0 = 0x3C; return;` — the 0x3C loads land in the branch delay
  slots, one shared store. The 0x3D path is its own store+return.
- **gp_rel store shows as a bytecmp "hard mismatch" but is a FALSE POSITIVE**
  (offset unresolved in the scratch TU: `xx0982A7` expected vs `000082A7`
  actual — only the %gp_rel offset differs, base `82A7`/`83A7` matches).
  Confirmed harmless: it resolves at link (FCE4/F8DC/F9AC all matched
  in-tree with exactly these bytecmp flags). When the ONLY remaining
  bytecmp mismatches are gp_rel stores, land it.

## Session 2026-07-06 (Fable, cd sibling campaign for BB4C) — proven against the hash
- **Queue shift-down: field-by-field copy beats struct copy** (func_8001CAAC).
  A CD_CMD command-queue dequeue slides every 16-byte entry down one
  (`*dst = dst[1]`) until cmd==0. A whole-struct copy `*dst = dst[1]`
  strength-reduces to ONE induction variable and comes 2 insns short. Writing
  the four fields individually (`dst->cmd = dst[1].cmd; dst->arg0 =
  dst[1].arg0; ...`) makes cc1 keep a SECOND IV `a1 = &dst->xC` (base+0xC)
  that serves both the source reads (a1[+4..+0x10]) and the dest-tail stores
  (a1[-8..0]) while a2 handles cmd + the loop test — the original's exact
  two-cursor shape. Field copy, not struct copy, for shift loops.
- BB4C callee signatures now pinned: func_8001CAAC is `void(void)`.
- **PARKED — func_8001CB7C (BB4C callee, ~42 insns)**: decrement loop
  `for(i=0x7F;i>0;i--) func_8001D394(i)` then a CdReadyCallback(0)/
  CdSyncCallback(&func_8001CC08)/func_8001D254(9,0,&D_80098A98) arm sequence.
  Two open shape problems: (1) the original hoists `a0=0` (CdReadyCallback's
  arg) into the loop-exit bgtz DELAY SLOT, jumping over the intervening
  D_800AD142/D_8009896C code — a cross-block delay fill I haven't reproduced;
  (2) D_800AD142 is read+written via a SINGLE materialized address
  (`lui;addiu v1;lhu 0(v1);...;sh 0(v1)`), but every source form tried
  (plain `|=`, pointer local, nested-block pointer, split t=*p) emits two
  lui/%lo instead. Needs the address-CSE / array-decl angle next session.
- **PARKED — func_8001CF98 (BB4C callee, ~63 insns)**: byte-clear loop over
  [D_80098B38..D_80098B41] then arm via D_80082CD0[arg] (the [][3] s32 table,
  arg*12 index). Two issues: the clear-loop counter/pointer are register-
  SWAPPED from mine (original: counter=$v1, ptr=$v0; init counter first),
  and the arg*12 index (sll1/addu/sll2) is emitted in a different order.
  Retry with counter declared/initialized before the pointer.

## Session 2026-07-06 (Fable, cd siblings CB7C/CF98) — proven against the hash
- **func_8001CB7C MATCHED**: CD retry re-arm. Two knobs: (1) the flag
  D_800AD142 is read+written (`|= 0x8000`) — declare it `extern u16
  D_800AD142[]` and use `D_800AD142[0] |= 0x8000` so cc1 materializes the
  address ONCE (lui+addiu, then lhu/sh 0(reg)); a plain scalar `u16` emits
  two lui/%lo and mismatches. (2) put `D_8009896C = 0;` BEFORE the `|=` in
  source — the sb schedules between the lhu and sh, matching. The
  CdReadyCallback(0) arg (a0=0) auto-fills the loop-exit bgtz delay slot.
- **CdIntToPos is the PSYQ two-arg `CdIntToPos(int i, CdlLOC *loc)`** — NOT
  a one-arg function. In func_8001CF98 the `lui/addiu a1,&D_80098814`
  BEFORE the CdIntToPos call is its SECOND argument (the CdlLOC out-param),
  not a dead/hoisted store. Declare `void CdIntToPos(s32, u8 *)`. General
  lesson: an arg-register load that lives across a `jal` with no other use
  is almost always that call's argument — check the PSYQ signature before
  calling it a scheduling artifact.
- **PARKED — func_8001CF98 (BB4C callee, ~56 insns)**: structure fully
  understood — byte-clear loop over [D_80098B38..B41] (counter=$v1 init
  first, ptr=$v0), then D_80082CD0[arg] (the [][3] s32 table, arg*12 index
  kept in callee-saved $s0), CdIntToPos([arg][0], &D_80098814),
  D_8009881C=[arg][1], CdReadyCallback(&func_8001D078), func_8001D2BC(6,
  &D_80098814, &D_80098A98). D_80098814 must be `extern u8` + `&D_80098814`
  (scalar) so its address REMATERIALIZES at both call sites; an array decl
  CSEs it into callee-saved $s1 (wrong frame). REMAINING BUG: cc1 emits the
  `D_8009881C = D_80082CD0[arg][1]` load+store TWICE — once hoisted before
  CdIntToPos, once after (3 extra insns, duplicate epilogue). The original
  loads [arg][1] ONCE after the call. Needs -dj/-dc CSE-dump analysis to
  find the source form that stops the duplication. Signature is confirmed
  `void func_8001CF98(s32)` — BB4C is NOT blocked by this.
- **func_8001CF98 MATCHED (2026-07-06)** — resolved the parked duplication.
  The real conflict was between TWO CSEs that pull the temp in opposite
  directions:
  - The value `D_80082CD0[arg][0]` must load ONCE and that one register must
    serve both `D_80098A7C = ...` (the store) AND `CdIntToPos(..., )` (arg0).
    Writing the expression twice (`D_80098A7C = D_80082CD0[arg][0];
    CdIntToPos(D_80082CD0[arg][0], ...)`) makes cc1 load it into two regs
    ($4 for the arg, $3 for the store). Introduce a local `v` used in both
    spots to collapse to one load reused everywhere.
  - The INDEX `arg*12` (`&D_80082CD0[arg]`) must be CSE'd into a callee-saved
    reg and held ACROSS the call, because `[arg][1]` is read after it. But if
    you HOIST the local's init (`s32 v = D_80082CD0[arg][0];` at the
    declaration, i.e. before the intervening statements), cc1 computes the
    offset at the top, keeps raw `arg` in $s0, and RE-derives arg*12 twice.
  - Fix: declare `s32 v;` uninitialized, then ASSIGN `v = D_80082CD0[arg][0];`
    in the body right before its use. The two array accesses ([arg][0] and
    [arg][1]) then sit close together around the call → cc1 CSEs the arg*12
    offset into the callee-saved reg and holds it across the call, matching.
  GENERAL: declaration-init vs in-body-assignment of the same local is a
  scheduling knob — hoisted init computes address math early (breaks
  across-call index CSE); in-body assignment keeps the address a live CSE
  next to its sibling accesses. Distinct from the value-CSE (use a local to
  merge duplicate loads). The two knobs are independent and were both needed.

## Session 2026-07-06 (Fable, PM — moji CALL-opcode harvest) — proven against the hash
- **The moji script CALL-opcode family** (func_800564C8, 56558, 56610, 566CC,
  57184, 5497C, and cousins) all share one shape; use the matched ones as
  templates:
  ```c
  u8 *base = m->x44;
  if (base != 0) MojiTaskExec(m->script[1], base, (u8)(OP_EXPR));
  else           MojiTaskExec(m->script[1], D_8008CACC[IDX_EXPR], 0xFF);
  m->script += 3;
  ```
  Variants only change OP_EXPR / IDX_EXPR: a raw `script[2]`, or the index
  remapped through a byte table (`D_800BE2F8[...]`, `Player_work.remap[...]`).
  When the base!=0 arm biases the op by a constant `-k`, the null arm folds it
  into the table symbol: `D_8008CACC[x - 1]` compiles to base `D_8008CAC8`
  (= CACC − 4). Write the `-k` in BOTH arms; cc1 does the fold.
- **`(s8)m->x71` (and `m->x72` used as an index) forces the signed `lb`.** The
  field is declared u8/s8; whether cc1 emits `lb` vs `lbu` follows the cast at
  the USE site. For the *increment* of the same s8 field, the proven idiom is
  `u8 c = m->x72 + 1; …; m->x72 = c;` which emits `lbu` — so one function can
  read the same field both ways (signed index, unsigned increment).
- **Remap table INSIDE a big struct** (e.g. `Player_work + 0x454`, which is
  within Player_work's 0x5F4 span so the reloc is `Player_work+0x454`): declare
  a byte-view struct with an array field at that offset and index the MEMBER
  (`Player_work.remap454[i]`). This emits per-site `%hi/%lo(Player_work+0x454)`
  + index, exactly like the Game_work.s[idx] BB4C fix. Casting a pointer
  (`((u8*)&Player_work)[0x454+i]`) instead makes cc1 materialize the base with
  a runtime `lui/addiu` (one extra insn) — non-matching.
- **Where a `& 0xFFFF` mask lands is controlled by the C type**: a value saved
  in a callee-saved reg across a call and passed to an int/unknown_t param —
  `s32 v = f() & 0xFFFF; g(v, …);` masks at the STORE (andi then plain move at
  the call, matching func_80055B14). `u16 v = f(); g(v, …);` defers the mask to
  the CALL site (plain move at store, andi at call). Pick the form that matches.
- **if/else arm order = branch layout**: `if (X > 0) A; else B;` makes A the
  fall-through (cc1 emits the inverse test to skip A). If the asm branches TO
  the ">0" block and falls through to the other, invert the source to
  `if (X <= 0) B; else A;`. Cost func_800545C8 one iteration.
- **Store order of chained assignment matters**: `m->a = m->b = X;` stores b
  first (inner), then a. func_80055438 needed script2 stored before x48, so
  write `m->x48 = m->script2 = X;` (script2 is the inner/first store).
- **PARKED genus — join-block constant hoisted into a conditional-branch delay
  slot** (func_80056778): a loop/branch-invariant `lui` (here the `0x402000`
  flags constant, used only after the merge) gets pulled by cc1 into the `bnez`
  delay slot; the original keeps a `nop` there. Reordering the tail statements
  did not stop it. Same family as the giants' constant-hoist allocation puzzle.

## 2026-07-11 (Fable) — func_800557B8: HImode locals, ||-shape, direct returns

Three knobs that turned a +2-insn near-miss into a byte match (all verified
against the hash; the wait-counter trio 545C8/54700/557B8 now share one genus):

- **A `s16` local (HImode) does two things an `s32 local = (s16)cast` does
  not**: (1) it reserves an 8-byte frame temp area (sibling 54700 got the same
  8 bytes implicitly from `m->x4 -= 1`'s in-memory u16 RMW), growing the frame
  0x18→0x20; (2) the load emits `lh $t; addu $pseudo, $t` — a reg copy of the
  load temp into the HImode pseudo — and later arithmetic (`cnt - 1`) reads the
  pseudo while the compare (`blez`) folds onto the load temp. If you see
  lh + addu-copy + an unexplained 8 frame bytes, the source had a short local.
- **`if (A || !B) expire; else dec;` vs `if (!A && B) dec; else expire;`**:
  logically identical, but the `||` form emits blez→then, bnez(B un-inverted)→
  else, then-arm falls through with `j` past the else-arm placed LAST. The `&&`
  form cost +2 insns and inverted the branch senses. When the original's
  second test branches TO the small arm with an un-inverted bnez and the big
  arm falls through, write the condition as `||` with the big arm as `then`.
- **Direct `return k;` in each arm vs accumulating into `ret`**: the `ret`
  local materializes in a callee reg (here $a1) with a final `addu $v0,$a1`
  copy (+1 insn). If the original sets $v0 inside each arm, use direct returns.

## 2026-07-12 (Opus) — moji harvest: 3 giant-class knobs, proven against the hash

From func_80053AA4 / func_80053788 / func_80057C2C (all clean-rebuilt + sha1 OK +
mutation-tested). These control register/addressing/scheduling — the same class
of decision that blocks the giants, so they generalize well beyond moji.

- **AND-of-3 grouping — split a temporary to pin the accumulator.** `x = f & LIT
  & RT` where LIT is a constant mask and RT is a runtime value: cc1 reassociates
  the commutative `&` to `f & (LIT & RT)` (combines the two masks first, `and
  v0,v0,a0; and v1,v1,v0`). The original often wants `(f & LIT) & RT` with `f`
  as the running accumulator (`and v1,v1,a0; and v1,v1,v0`). Force it by
  splitting: `t = f & LIT; x = t & RT;`. (func_80053AA4, the Moji_flag mask.)
- **Mixed array-vs-pointer addressing in a struct-array loop.** In
  `for(i..) { Base[i].field = ...; }`, cc1 emits each store as absolute
  `%lo(Base+off)($at)` (with `$at = %hi + i*stride`). If the original holds a
  per-element base pointer for ONE field but stays absolute for the others,
  write exactly that mix: `T *m = &Base[i]; m->thatField = ...; Base[i].other =
  ...;`. cc1 honors the pointer form for `m->` and the array form for `Base[i]`.
  (func_80053788: script2/0x6C via `m`, flags/x6/x48 array-absolute.)
- **Preheader scheduling knob — for-init comma order → preheader insn order.**
  When a hoisted loop-invariant constant (e.g. a `0x80` shift base) lands on the
  wrong side of an accumulator/walking-pointer init in the loop preheader, make
  the constant a NAMED local and place it in the for-init at the exact comma
  position you want: `for (i=0, mask=0x80, p=Base; i<N; i++)` emits `i`, then
  `mask`, then `p` in that order in the preheader. A plain pre-loop statement
  schedules too early; a bare literal (hoisted) schedules too late. This is a
  real, controllable knob for preheader ordering — relevant to the 53B40
  movable-ranking fight. (func_80057C2C.)
- **Corollary (same fn):** a CONDITIONAL `count++` (incremented inside an `if`
  in the loop) is NOT a loop induction variable, so cc1 cannot strength-reduce
  `Base[count] = ...` into a walking pointer — you must write the walking
  pointer explicitly (`*p++ = ...`). And `u32` vs `s32` loop counter selects
  `sltiu` vs `slti` for the bound test.

## 2026-07-12 (Opus) — func_8001F6C4 (scene): 3 reusable knobs

Small (28-insn) scene function matched clean + mutation-tested. Three knobs, all
generalizable:

- **Call with NO arg-setup + different live values per path = ONE reused
  variable passed as the arg.** The `jal func_8001D7AC` had no preceding a0 load,
  yet a0 held `p[0]` on one path and `p[f+1]` on another. That's a single local
  reassigned along the way and passed at the end: `s32 a = p[0]; if(...){ a =
  p[f+1]; ...} func_8001D7AC(a);`. cc1 keeps `a` in a0 across both paths, so no
  reload appears. When a call takes a "leftover" register that differs per path,
  look for one variable reused as both a compared value and the argument.
- **A "+0xC whole-data shift" was NOT a COMMON leak — the function was 3 insns
  too long.** `.data`/`.bss` follow `.text`, so a function that compiles N insns
  long shifts EVERY data symbol by N*4. All of D_800893BC / D_800C356E/F /
  Game_work read as +0xC purely because my body had 3 extra insns. Confirm the
  function's own instruction count BEFORE hunting extern/COMMON decls (the Ch-5
  symptom-table caveat, seen live).
- **Hoist an unconditionally-safe load ABOVE the branch to fill its delay slot.**
  The original loads `D_800C356F` and computes `f*2` in the `bne` delay slot even
  though `f` is only used in the taken path. Writing `s32 f = D_800C356F[0];`
  BEFORE the `if` (not inside it) makes cc1 hoist the load and fill the delay
  slot with the `sll` — killing the 3 extra insns. A value used in only one arm
  can still be loaded before the branch if the load is side-effect-free.
- **Byte-in-bss symbols accessed via lui/%lo want the ARRAY decl** (`extern u8
  D_800C356E[];`, use `[0]`), not the scalar `extern u8 D_800C356E;` (which emits
  `.extern ,1` and risks the small-scalar COMMON path). Applied to D_800C356E and
  D_800C356F (rock_neo.h decl flipped to `[]`; it was unused elsewhere).

## 2026-07-14 (Opus/Fable) — GCC-source expedition: compiler identity + -dL is readable

Toolchain-forensics facts (directly verified by running the tool — *not* a codegen
match, so not hash-gated; the 53B40 verdict that uses these is still PENDING).

- **cc1-27 is SN-patched, NOT stock FSF gcc-2.7.2.** `./bin/cc1-27` banner:
  `GNU C 2.7.2.SN32.3.7 Build 0001 [AL 1.1, MM 40] BSD Mips` — an SN Systems /
  PSY-Q build (path marker `/home/aaron/sn-2.7.2`), recompiled as a modern x86-64
  Linux ELF. **Implication for source reading:** the public FSF `gcc-2.7.2` source
  (`~/src/gcc-2.7.2/`) is a *baseline for the mechanism only*. Generic optimizer
  passes (loop/cse/global) are usually untouched by SN (whose patches target
  MIPS/PSX codegen + the assembler interface), but you CANNOT assume byte-identity.
  **Ground truth = the actual `-dL`/`-dg` dumps from cc1-27**; where FSF source and
  the dump disagree, trust the dump and suspect an SN patch. (Provenance: Phase 0 of
  the GCC-source expedition, `notes/wip/GCC_SOURCE_PROGRESS.md`.)

- **`-dL` (loop pass) prints movable decisions in PLAIN TEXT — the wall is legible.**
  Adding `-dL` (and `-dg` for reg alloc) to the cc1-27 flags dumps `<base>.c.loop`
  with human-readable per-insn verdicts, e.g.:
  - `Insn 549: possible biv, reg 242, const = 528482416` (that's `0x1F800070`,
    flagged as a candidate basic induction variable)
  - `Insn N: regno R (life L), move-insn savings S not desirable` — rejected hoist
  - `Insn N: regno R (life L), move-insn savings S  moved to M` — accepted hoist
  - `Insn N: regno R (life L), done move-insn matches K` — **this movable was
    MERGED into an earlier identical one (insn K)** — i.e. the value-merge the
    53B40 analysis suspected, made visible. The accept rule keys on `move-insn
    savings` vs desirability and register `life`.
  **How to regenerate the dump for a permuter candidate** (must be the *candidate C*,
  not the shipped TU — a nonmatching func ships as an INCLUDE_ASM stub, so its loop
  pass never runs from the real TU):
  ```
  cpp <CPP_FLAGS from `make -n <obj>`> tools/decomp-permuter/mml_53B40/base.c > base.i
  ./bin/cc1-27 <CC_FLAGS from `make -n <obj>`> -dL -dg -dumpbase base.c base.i -o /dev/null
  # produces base.c.loop (movable text) and base.c.greg (reg alloc)
  ```

- **VERDICT — the 53B40 / BB4C loop-hoist genus is C-REACHABLE, not a toolchain wall.**
  Reading `move_movables`/`combine_movables` gave the exact hoist rule (now the full
  formula lives in `COMPILER_IDIOMS.md §4`): hoist iff
  `threshold*savings*lifetime >= insn_count`, and constants with identical source RTX
  always merge (summing savings+lifetime). Two compile-verified C levers stop the bad
  hoist: (1) route a constant's derefs through one REUSED local → multi-set pseudo →
  never a movable → inline `lui/ori` at each site; (2) a constant the loop pass can't
  hoist (too few uses) must be a PRE-LOOP local to land in a callee-saved reg. 53B40's
  remaining work is ordinary register-placement matching (`$fp`→`$s7` via the extern
  `D_800BB9C8` = Moji_work+0x310 spelling), NOT a compiler mystery. Match not yet
  hash-verified — mechanism proven by compile, ritual pending.

## 2026-07-19/20 (Fable, 53B40 campaign) — proven by structure/dumps (match pending)

- **`include/macro.inc`'s `li` macro DROPPED the low half of big constants**
  (`lui %hi(num)` with no ori) — every in-tree TU inherits it via include_asm.h's
  `.include "macro.inc"`. First hit: cc1-emitted `li $22,-2045222521` (0x86186187
  magic divisor). Now fixed (raw-shift lui + ori branch); full hash + 205/205
  overlays re-verified. Symptom to recognize: scratch-TU (no include) bytes
  differ from in-tree bytes on a lui/ori pair for the same C.
- **MOJI_TASK* pointer subtraction is EXACT division** — magic 0x1A1F58D1
  (196*M ≡ 4 mod 2^32) + `mflo; sra 2`. If target division shows mflo (not
  mfhi+sign-fix), write it as pointer subtraction, never (bytes)/(sizeof).
- **A hoisted address constant occupying a callee-saved reg is killed with a
  per-iteration local** whose movable fails the product test (late chain
  position, savings 1): `last = &SYM;` inside the loop tail — the address
  rematerializes as per-iteration lui/addiu in a CALLER-saved reg.
- **Preheader li order == in-loop first-set order of the movables**, and
  body-top locals are the knob: unconditional single-set locals at the top of
  the do-body (`tst = 0x40000; setflag = 0x40000000; fff = 0xFFFFFF;`) hoist in
  STATEMENT order and decouple site operand order from preheader li order.
  Conditional-arm sets are NEVER hoisted (P1/P2 probes). A pre-loop local
  instead lands before the guard branch and dbr steals it for the delay slot
  (target's slot held the strength-reduce giv init = nothing between the guard
  sltu and branch in the original).
- **combine_movables merges by identical src RTX only for single-set pseudos**:
  a 2-set pp local (assigned at exactly the two hot sites) blocks the merge
  while keeping per-site block-local regs — the anonymous form at ALL sites
  re-merges and re-hoists (verified both directions).
- **The moji inner glyph loop renders for op < 0x84** and calls D_8008AAC4 for
  >= 0x84 — all prior 53B40 drafts had the arms inverted (identical layout,
  ONE branch word differs). And func_8001D494's result is DISCARDED in 53B40
  (the &= store sits in the jal slot; no post-call store). Byte-diff-invisible
  semantic bugs: check branch SENSE against the compare+target, not just arms.
- **Structural-alignment diffing beats raw word-diffing for big functions**:
  scratchpad sdiff.py (SequenceMatcher over normalized opcode+regs of
  mipsel-elf-objdump vs splat .s) turns a 400-mismatch cascade into ~10
  actionable blocks. Rebuild it from GCC_SOURCE_PROGRESS tooth 6 if lost.
- **decomp-permuter workdirs must cd to repo root in compile.sh** (gprel census
  reads asm/rock_neo/**/*.s relative to CWD) — see PERMUTER_GUIDE.md. With a
  correct base its finds are HARVESTABLE (diff its output-*/source.c vs base,
  re-express cleanly, verify, reseed): x12 statement position and an m-alias
  (`mv = m; ... mv->x10 += ...`) each fixed whole scheduling clusters.

## 2026-07-20 (Fable, 53B40 endgame) — permuter epistemology + reload forensics

- **The permuter's weighted score DIVERGES from byte-truth near the endgame.**
  Its differ gives partial credit for alignment, so deleting an instruction that
  incidentally "fixes" >20 register diffs scores as an improvement — the search
  migrates into wrong-LENGTH states that can never reach a byte match, while
  announcing new bests. THE POSITIONAL GATE IS AUTHORITATIVE for accepting any
  candidate: `bytecmp` must show exact word-count parity AND a lower hard count.
  Local scorer patch (length-delta × 100000 fence) + the harvest protocol
  (stop the permuter BEFORE any output cleanup — a race cost a candidate) are in
  PERMUTER_GUIDE.md. First fenced run immediately produced a genuine positional
  improvement after hours of trap-chasing.
- **Permuter device families worth hand-adopting when seen in its diffs** (all
  byte-verified on 53B40): (1) named-constant locals at block tops (the
  tst/setflag/fff family — reorders preheader/li emission); (2) `x = m;` alias
  temps (force a reload/defeat CSE at one site); (3) always-true folded ifs
  with two IDENTICAL arms (`if (Moji_flag) {X} else {X}`) — fold away entirely
  but shift pseudo NUMBERING, which breaks allocno priority ties; (4) statement
  interleaves between a load and a copy (`pr = *X; rect.x = m->x8; prim = pr;`)
  — the intervening statement keeps cse from folding the copy.
- **Probe outcomes are BASE-RELATIVE: a C shape that regressed on an older draft
  can be neutral or a win later.** (The rp-precompute interleave went 37-worse →
  neutral across three base evolutions.) Blacklist probes per-base, not forever.
- **Dead stack bytes are a reload fossil, and the dump names the birthplace.**
  `-dg`'s "Need 1 reg of class X (for insn N)" lines identify the exact insns
  (here: an exact-div's LO→GR shift and a umulsi3_highpart's HI need) where
  reload spills hard regs; a frame reserving untouched slots means the ORIGINAL
  compile had pseudos evicted TO MEMORY at those insns (accesses later removed
  by inheritance). Matching such a frame is about recreating simultaneous-live
  PRESSURE at those insns, not about any single statement — single added locals
  measured neutral five different ways on 53B40 (see the fossil brief).

## 2026-07-26 (stage realm opened: eve19 12/12) — proven against the overlay hash

- **Leaked Capcom source MATCHES the US binary.** `eve19.c`, `sub_scrn.c`, `em1c.c` and
  `EM09_49.c` were left uncompiled in the **JP/PSP** `ST1E.BIN` and are transcribed on
  TCRF (page static since 2021-08-06; the US disc contains none of it — swept all 205
  BINs + the exe). Despite being the JP build, all 12 eve19 functions matched, and
  `check_block` was instruction-for-instruction correct on the FIRST compile. Treat the
  leak as near-verbatim but verify every constant against the target asm.
  Already-mined: upstream transcribed sub_scrn's sort functions and the whole symbol
  table from this page years ago (336 of 347 syms are real Capcom names).
- **cc1-27 DEFAULTS TO BIG-ENDIAN FOR UNALIGNED ACCESS.** It emits `lwl/lwr` and
  `swl/swr` with swapped byte offsets (`lwl base+0 / lwr base+3` instead of the
  little-endian `lwl base+3 / lwr base+0`). Latent for the project's first 301 matches
  because no compiled-C function had ever used an unaligned access — the 176 `lwl`s in
  built engine objects are all INCLUDE_ASM'd original assembly, not compiler output.
  **Fix: add `-mel`.** Verified endian-neutral otherwise (full engine rebuild hashes OK,
  205/205 overlays). Trigger: any struct with alignment 1 (all-char members) being copied.
- **THERE ARE TWO COPIES OF THE COMPILER FLAGS.** `Makefile` `CC_FLAGS` builds the
  engine; `tools/buildoverlay.py` has its own `CC_FLAGS` for overlays. Changing one does
  nothing to the other. This cost several confused rebuild cycles. Both now carry a
  comment naming the other.
- **Array-extern vs scalar-extern decides instruction count** (restating LESSONS §2
  because it was the last blocker on 4 of 12 functions): if the target materializes the
  address — `lui $r,%hi(SYM)` + `addiu $r,$r,%lo(SYM)` then `0($r)` — the extern is an
  UNSIZED ARRAY (`extern u16 SYM[];`, indexed `[0]`). If it uses `%lo(SYM)($r)` directly,
  it is a plain scalar. A held-pointer local (`u16 *fp = &SYM;`) is NOT equivalent and
  made things worse (-2 insns).
- **Overlay rodata can be carved exactly like engine rodata.** Add a named subsegment to
  the archive yaml (`- [0xEC8, .rodata, eve19]`), re-split, and the C file's own
  `.rodata` is linked at that address. Constraint: **all** functions sharing the carved
  region must be converted at once, because gcc emits rodata in first-use order and a
  partial conversion leaves the region incomplete. Back up `asm/` before re-splitting —
  it is gitignored.
- **`check_overlays` reports 205/205 on STALE BINs when the build failed.** `make chunks`
  can exit nonzero and leave the previous BINs in place. ALWAYS read the build exit code
  before believing the gate. Same false-pass class as the 2026-07-05 stale-object bug.
- **`.c.o` under `build/src/<ARCHIVE>/` are make intermediates too.** Touching the `.c`
  is not enough; delete the overlay's `build/<ARCH>.*` artifacts to force a real rebuild.
- **Parallel agents: analysis yes, builds no.** Reading asm, recovering offsets/constants
  and drafting C parallelise cleanly. The build tree and `build/` are shared and the
  verification gate is global, so integration + `make` + hash-check must stay serial in
  one thread. Agents are read-only and return C as text.

## 2026-07-26 (parallel self-verifying agent wave) — 17 matches in one build cycle

- **The harness that worked: producers self-verify, integration stays serial.**
  10 concurrent agents, 2 functions each, every agent proving its own work with
  `tools/bytecmp.sh` before reporting. 18/20 verified; 17 landed in ONE build cycle
  (306 -> 323). The main thread never debugged a function — only integrated.
  `bytecmp` now takes `BYTECMP_OBJ=<path>` so concurrent agents do not clobber each
  other's scratch object; without that they silently corrupt each other's results.
  Agents must be READ-ONLY on the tree (scratch `.c` in /tmp + their own object) —
  the build tree and `build/` are shared and the hash gate is global.
- **⚠️ A SCRATCH TU MUST START WITH `__asm__(".include \"macro.inc\"\n");`**
  In a real build that arrives free via `common.h` -> `include/include_asm.h`.
  `macro.inc` overrides GNU as's builtin `move` (which assembles to `or rd,rs,$zero`,
  0x…25) with `addu \a,\b,$zero` (0x…21). Without the include, ANY scratch TU
  containing a register-to-register move shows **phantom hard mismatches** — one agent
  chased three of them. `tools/bytecmp.sh` and `tools/tryfn.sh` do NOT add it.
  **Past bytecmp verdicts on move-containing functions may have been false failures.**
- **A "small" alias can defeat an unwanted address CSE** (func_8001F158). `Scene_work`
  is 0xA8 bytes, so with -G8 its address is "expensive" and cc1 CSEs it into a register
  (`la $3,Scene_work+164`), costing an instruction and losing a load-delay fill.
  Declaring a 1-byte alias for the same symbol —
  `extern u8 Scene_work_b __asm__("Scene_work");` — makes the address cheap, so cc1
  emits independent bare refs and fills the delay slot, matching the target.
- **Two agents can pick incompatible views of the same symbol in one TU.** In scene.c
  one wanted `#include "rock_neo/player.h"` (`PL_WORK Player_work`) and another needed
  `extern u8 Player_work[]` — and scalar-vs-array extern changes addressing, so they
  cannot coexist. Detect at merge time; land the majority and defer the odd one out
  rather than "fixing" a verified function's source shape on a hunch.
- **Integration hazards, all hit for real:** (1) a prose-stripping heuristic cut inside
  a multi-line comment and left an unterminated `/*` that swallowed later declarations
  — any such stripper must track comment depth; (2) de-duplicating declarations by
  substring match silently dropped needed ones — duplicate compatible `extern`s are
  legal C, so just emit them and let the compiler flag real conflicts.

## 2026-07-26 (grind wave 2: 30 agents) — 27 matches; the integration hazards are the bottleneck

323 -> 350 engine. 30 agents (10 concurrent, 3 internal waves), 1 function each,
29/30 self-verified with bytecmp. 27 landed; 2 pulled at integration.

**Agent verification is now reliable; MY INTEGRATION was the failure surface.** Every
single build failure this wave was mine, not the agents'. Recorded so they are never
re-invented:
1. **Dedup whole DECLARATIONS, never lines.** Line-level dedup deleted the shared
   `typedef struct {` opener that several agents emitted, orphaning the struct bodies.
   Split on `;` at brace depth 0 and dedup the resulting units.
2. **`#define` has no trailing `;`** — a declaration splitter that keys on `;` silently
   drops every macro. Cost a build cycle chasing `OT_LENGTH undeclared`.
3. **Duplicate `typedef` is a C89 ERROR** even when byte-identical (duplicate `extern`
   is fine). Two agents emitting the same struct will not compile.
4. **A declaration the file already has may be defined BELOW the insertion point.**
   `SND_CMD` (sound.c) and `CD_CMD` (cd.c) are declared mid-file. Either hoist the
   existing definition or move the dependent externs down; skipping the decl as
   "already present" produces a parse error at the top of the file.
5. **Two agents can give one function different prototypes** (`void*` vs `GAME_WORK*`).
   Return type steers caller codegen; parameter type usually does not, so keep the
   specific one — but detect it, do not let the compiler find it.

**Two functions verified in scratch but failed in-tree** — the standing warning, now
measured on real cases:
- `func_800137D4`: 172 bytes in-tree vs 188 target (4 insns short). Scratch TU had
  different types in scope than the tree file provides.
- `func_8005ECE0`: needs `jtbl_8001124C`, a jump table living in the asm rodata.
  Splicing the C orphaned the `.L8005ED30..` labels -> undefined references at LINK
  time, not compile time. **Any function whose asm mentions `jtbl_` needs a rodata
  carve first** — check before assigning it to an agent.

**Diagnostic that actually works.** Comparing an unlinked `.o` against target words
flags EVERY function (relocations are unresolved placeholders — it reported 28/28 bad,
all false). Two reliable checks instead: (a) per-function LENGTH from
`objdump -t` vs the target's instruction count — this found the real culprit
immediately; (b) `cmp` the LINKED exe. If `cmp` reports EOF/truncation, some function
is SHORT and everything after it shifted — go straight to the length check.

**cc1-27 allocates bit-fields MSB-first.** `{unsigned addr:24; len:8;}` puts addr in
bits 31..8; the real GPU packet layout needs `{len:8; addr:24;}`. libgpu.h's `P_TAG`
uses the first order and matched for the sub_scrn addPrims tails, while
Code800133D8.c needed the reversed `OT_TAG`. Both verified — pick per site, do not
assume one is universally right.

## Parallel worktrees cannot produce a matching build (2026-07-28) — UNRESOLVED

`CC_FLAGS` carries `-gcoff`, and the **absolute source path is embedded in every C object**.
A git worktree therefore builds a *different binary* from identical sources:

```
/home/X/Documents/MML-D2-26   -> moji.c.o 69420 bytes   exe 825344  (matches)
/home/X/Documents/mml-wt-wt-test -> moji.c.o 69452       exe 825516  (+172)
/home/X/Documents/MML-WT-a1   -> moji.c.o 69444          exe 825516  (+172)
```

Equalising the path LENGTH (27 chars either way) did **not** fix it — still +24 on the
object. `diff <(strings a.o) <(strings b.o)` shows exactly ONE difference: the path string
itself. Assembly objects (`32A38.s.o`) are byte-identical; only C objects differ. First
binary difference is at `0x800`, the start of code — not a trailing debug section.

**Unresolved:** why a same-length path swap changes object size by 24 bytes, and why it
reaches the stripped output at all. If the path genuinely reached the shipped exe we could
never match the retail binary from any path, so something subtler is going on.

**Practical consequence until solved: do NOT run build-gated work in a worktree.** Its
baseline does not match, so nothing it produces can be verified. Read-only analysis in a
worktree is fine. `tools/mkworktree.sh` exists and sets one up correctly in every other
respect; it prints a WARNING and exits nonzero when the baseline fails, so it will not
silently hand you a poisoned tree.

**Worth trying next:** build with debug off to see whether the delta vanishes; check
whether `-fverbose-asm` or the `.comment` section carries the command line; try a worktree
whose path is not merely the same length but sorts identically in the string table.

## 2026-07-29 (Opus, +41 batch: dispatchers + Tier A singles) — hash-gated

- **`p[0]` aliases globals but `p[1..]` does not — GCC 2.7 `MEM_IN_STRUCT_P`.**
  In `expand_expr`'s INDIRECT_REF case cc1-27 sets `MEM_IN_STRUCT_P` when the address
  is a `PLUS_EXPR`, so `p[0]` gets `in_struct=0` while `p[1]`/`p[2]`/… get `in_struct=1`.
  GCC 2.x `true_dependence` then rules "one ref in-struct with a varying address vs one
  not-in-struct with a constant address ⇒ cannot conflict", so loads of scalar externs
  hoist above the `p[1..]` stores but NOT above the `p[0]` store — batching the loads and
  sinking the stores (14 words instead of the target's strictly serialised 16).
  **Fix: give the LOADS the unsized-array spelling** (`extern u8 SYM[]; … = SYM[0];`).
  That sets `MEM_IN_STRUCT_P=1` on the load with a still-constant address, defeating the
  disambiguation the other way and pinning the order. With a constant index the addressing
  bytes are unchanged (`lui %hi` + `lbu/lhu %lo`, zero addend).
  **So the array spelling is an ALIASING knob, not only an addressing knob** — a second,
  independent reason to reach for it beyond READ FIRST §1's address-materialisation rule.
  (func_80036B00.)
- **Flag in `$v1` ⇒ two `return`s; flag in `$v0` ⇒ one accumulator.** The
  single-accumulator form (`s32 v = 0; if (f) v = X; return v;`) and the early-return form
  (`if (!f) return 0; return X;`) are NOT interchangeable, despite two drafts calling them
  "the same genus". When the accumulator loses `$v0` to the flag it costs a trailing
  `move $v0,$v1` in the `jr` delay slot. Read which register the flag load targets and
  pick the shape to match. (func_80039B80 early-return vs func_8005D938.)
- **`tools/bytecmp.sh` scored `%gp_rel` as a HARD mismatch** (its `reloc_re` had
  `%hi|%lo|jal|j|b*` but not `%gp_rel`), so every gp-relative store produced a false
  failure. Three agents hit it independently in one session and six good functions were
  nearly discarded. Fixed 2026-07-29. **Verification pattern worth reusing:** link the
  scratch `.o` at its real VRAM with `_gp = 0x80097864` (`rock_neo.ld`) plus real symbol
  values, `objcopy -O binary`, and diff against the splat comment column. That check is
  strictly stronger than bytecmp — it also validates `%hi`/`%lo` carry arithmetic, which
  bytecmp skips entirely.
- **`phase0_split.py` cannot split a function that starts exactly ON an existing segment
  boundary** — it reports `SKIP … already at a segment boundary` and does nothing. The
  conversion is still trivial by hand: change `- [OFF, asm]` to `- [OFF, c, CodeADDR]` and
  insert `- [OFF+size, asm]` after it (the rest of that raw chunk keeps building as asm).
  Recovered func_80042734 and func_80066714 this way.
- **A SKIP also leaves a PHANTOM `.c` behind** (the tool writes the file before the yaml
  insert). Delete it immediately — census counts it, the linker never references it, and
  the hash stays green because the original bytes shipped.
- **Auto-derive the template parameters from the target asm, don't transcribe them.**
  A generator that parsed each dispatcher's own asm for load type / index offset / table
  symbol got 17/18 first try, and the 18th exposed a documented-but-wrong note
  (func_800153EC is a TWO-arg call — the computed address lands in `$a1`, not `$a0`).
