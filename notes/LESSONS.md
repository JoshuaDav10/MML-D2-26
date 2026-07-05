# Decomp Lessons — accumulated compiler & workflow knowledge

Running log of what's been *proven against the hash* (not theorized). Every
future session should read this before matching. Add to it whenever a diff
iteration teaches something; this file is how the project gets smarter.

## How knowledge persists between sessions
- `CLAUDE.md` — auto-loaded rules (build commands, gotchas, branch policy).
- This file — compiler idioms and matching tactics, growing per session.
- `notes/DOCUMENT_INDEX.md` — index to the external reference corpus
  (MIPS R3000 manual, PSXSPX hardware spec, PSYQ SDK docs) at
  `notes/reference/`. Index-driven lookup only; never whole-file pulls.
- `include/rock_neo/*.h` — struct layouts are *executable knowledge*: once a
  field is typed (e.g. MOJI_TASK.script), every later function gets it free.
- `progress.md` / `activity.md` — counters and per-session narrative.
- Git history on `dev` — each commit message records what matched and why.

## cc1-27 (GCC 2.7.2) codegen facts — verified byte-for-byte

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
