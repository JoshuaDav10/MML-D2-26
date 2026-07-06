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
