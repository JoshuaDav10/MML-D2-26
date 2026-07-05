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
- **OPEN PROBLEM — equality leg in a ternary chain (func_8001FCA4)**: cc1
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
  goto, arm swaps all canonicalize identically). Skip siblings with this
  shape until a compiler-level explanation is found.
