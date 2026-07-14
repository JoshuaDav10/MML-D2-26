# PSX / PSY-Q gcc-2.7.2 Matching-Decomp Cookbook (portable)

**Scope:** transferable compiler idioms for producing *byte-for-byte matching* C
against **gcc 2.7.2** as shipped in the Sony PSY-Q SDK (`cc1`, MIPS I / R3000,
`-O2 -G8`), driving the PSX assembler (ASPSX/`aspsx`) or a GAS emulation of it.

Every entry here was **proven against a real hash** on the Mega Man Legends
(PSX, US) decomp — but the *rules* are properties of the compiler, not the game,
so they port to any PSY-Q gcc-2.7.2 title (many PSX games). Game-specific facts
(struct offsets, symbol names) were deliberately stripped; MML's raw provenance
with concrete symbols/functions lives in `notes/LESSONS.md`, cross-referenced by
section name.

> **How to use:** when a diff shows a specific *shape* of mismatch, find the
> mechanism below, apply the C knob, re-verify. Reach for the `-d*` dumps
> (§11) the moment two callee-saved registers swap or a load duplicates — the
> dumps turn guessing into arithmetic.

---

## 0. Methodology (this is the most portable thing here)

- **Only "matched" after a hash/`cmp` pass on a CLEAN rebuild.** Object files
  are often build *intermediates*; converting a stub to C without forcing a
  rebuild can leave the old object linked and the hash passes on the STUB. Always
  force the recompile+relink before believing a match.
- **Liveness mutation test (anti-hallucination):** deliberately break a matched
  function (`+1`→`+2`), rebuild — the check MUST fail; revert — passes. If
  breaking the C did not break the build, the C was never compiled in.
- **A failed compile can hide behind a green hash:** a piped `cpp|cc1|maspsx|as`
  toolchain will assemble a *truncated* stream into a partial `.o` with a fresh
  mtime while `make` later aborts — the stale exe still passes. GREP THE BUILD
  OUTPUT for `error:` yourself before trusting the check.
- **A normalizing asm-differ (register-renaming, reorder-commuting) can show a
  function as CLEAN while the real hash still FAILS** — it hides swapped setup
  loads, size shifts, and register mirrors it "normalizes" away. Gate on the
  byte/hash check, never on the pretty differ alone.
- **The diff isolates which function; then reason about the shape:** register
  mirror → reorder temps / flip decl order; extra-or-missing `nop` → aliasing or
  scheduling; branch-displacement cascade → an instruction-count deficit
  *upstream* in the same function (find the missing/extra insn, not 400 "bugs").
- **Brute-force orderings in a scratch TU, not in-tree:** pipe candidate C
  through the real `cpp|cc1|maspsx` and diff the instruction stream. 4 variants
  per compile, seconds each, decisive, zero tree churn.

---

## 1. Global addressing & relocation (`-G8`, gp vs lui)

- **cc1 does NOT choose gp vs lui** — it emits a bare memory operand plus
  `.extern sym,SIZE` for small scalars and delegates to the assembler. Under
  `-G0` GAS every bare ref becomes `lui/$at` (2 insns, recomputed per access).
  A separate pass must rewrite gp-accessed small symbols to `%gp_rel(sym)($gp)`
  and DROP the `.extern` (else GAS emits a COMMON that the linker mis-places,
  silently shifting the whole data segment). *(MML: `tools/gprel.py`.)*
- **Declaration style is the per-TU addressing control:**
  - `extern T sym;` → gp candidate (1-insn `$gp` access).
  - `extern T sym[];` + `sym[0]` at uses → forced `lui/$at`.
  - pointer global via lui: `extern T *sym[];`, use `sym[0]`.
- **Same symbol, different addressing per TU is normal.** The original mixes
  modes; when a diff shows gp-vs-lui disagreement, flip the *declaration in that
  TU*, not the tooling.
- **gp-vs-lui can even differ PER FUNCTION in one TU:** use a raw-address deref
  (`*(u32*)0xADDR`) for the lui-form site — the assembler expands a bare
  constant to identical `lui/lw` bytes and the gp pass ignores it.
- **Scalar vs array decl also decides base-register CSE:** a function that only
  *reads* a pointer global once matches the `T *sym[]` form; one that
  read-modify-writes it (read, advance, store back) needs a plain scalar pointer
  `extern T *sym;` (bare `lw`/`sw` per access) or the array form CSEs the base
  into a register (+insns).
- **Symptom table:** whole-binary pointer shift after adding an extern ⇒ COMMON
  leak (bad decl); `relocation truncated R_MIPS_GPREL16` ⇒ a symbol gp-rewritten
  out of gp range (declare it as an array). A 4-byte whole-data shift can ALSO
  be a function that compiled one instruction short — check the function's own
  diff before hunting extern decls.
- **Assembler `$at`-expansion operand order can be per-symbol** (`addu $at,REG,$at`
  vs `addu $at,$at,REG`) and is NOT controllable from C — it's an assembler
  macro. If a function mismatches ONLY in an `addu $at` operand order, patch the
  assembler's per-symbol table; don't hunt C shapes. *(MML: `maspx`
  `ASPSX_REGFIRST_SYM_PREFIXES`.)*
- **Raw-constant pointers use `lui/ori`; symbols use `lui/%hi`.** For an address
  whose low half ≥ 0x8000, a symbol decl assembles to `%hi/%lo` with a negative
  low offset (different bytes) — use a raw `(T*)0xADDR` cast to force `lui/ori`.

---

## 2. Register allocation (global.c / local-alloc)

- **Callee-saved mirroring is often an allocno-priority TIE, and the tie is
  ARITHMETIC, not luck.** gcc 2.7.2 orders pseudos by
  `floor_log2(n_refs) * n_refs / live_length` (scaled, integer-truncated); ties
  break toward the LOWER pseudo number (params allocated first). Two pseudos with
  equal priority fight for a saved reg. Diagnose with `-dl` (`.lreg`:
  `Register N used X times across Y insns`) — compute the priority for each and
  see who wins.
  - **Knob:** add ONE instruction to the live length of the pseudo that should
    LOSE. A byte-sized param declared `u8` gets a QImode entry-copy that length­
    ens its range by 1 and drops its priority below a tied constant. The
    perturbing insn must survive CSE (statement splits get folded away first).
- **A value pseudo held live across a call-return branch is denied `$v0`.**
  If an `if(call())a;else b;` PRECOMPUTES one arm before the branch, that arm's
  pseudo lives across the insn where the call result occupies `$v0`, gets
  `conflicts: 2` in `-dg` (`.greg`), lands in `$v1`, and EVERY value store in the
  function mirrors. Rewrite so the value is not defined until after the branch
  (e.g. a `||`-guarded ternary) to keep it in `$v0`. Diagnosis: `-dg` →
  `;; N conflicts: … 2 …` (2 == `$v0`).
- **Register-birth / liveness reorder:** when a call result must live across a
  second call whose arg derives from a still-live param (`a=f(n); b=f(n+1);`),
  cc1 can schedule the `n+1` arg setup above the `a=$v0` copy, making the ranges
  disjoint so they share a saved reg and the function comes up short a
  save/restore pair. Statement splits, named temps, and K&R protos do NOT stop
  it — it's a scheduling property; park until an idiom appears.
- **Cheap knobs before deep surgery:** local *declaration order* and *init order*
  drive which pseudo gets which hard reg (`i` before `p` ⇒ `i=$v1,p=$v0`;
  reverse mirrors them).

---

## 3. CSE & value numbering (cse.c)

- **cc1 CSEs an identical constant *address* into one pseudo** — even across
  several use sites, and even with `volatile` on the pointer. This is what makes
  a constant get promoted to a loop-invariant (see §4). `volatile` does NOT stop
  address-CSE (it stops *load* re-use, a different thing).
- **Break address-CSE with a NEIGHBOR symbol + addend.** Two reads of
  `sym[k]` make cc1 materialize `sym+k` into a register. Write the second as
  `((T*)other_sym)[k']` where `other_sym + k'` == the same address — different
  RTL ⇒ no CSE ⇒ both stay in symbol-indexed `$at` form, and `%lo(other+k')`
  relocates to the same bytes. Works for STORES too (a neighbor-symbol store
  also breaks an anchor-CSE against a `&struct` call argument).
- **Merge duplicate loads with a local; split them with a neighbor symbol.**
  Two occurrences of the same expression → one load if you assign to a local `v`
  and use `v` in both spots; two loads if you want them independent (write via
  different symbols/addends).
- **Constant-index array vs computed address:** `((u8*)sym)[k]` (constant `k`)
  keeps per-element `%hi/%lo(sym+k)` — no base reg; casting arithmetic
  `*(u16*)((u8*)sym+off)` materializes the base into a register (+insn). A
  remap table *inside* a big struct: declare a byte-view struct with the array
  field at that offset and index the MEMBER, so you get `%hi/%lo(Struct+off)+idx`
  rather than a runtime base compute.
- **A callee's declared RETURN TYPE steers the CALLER's allocation** even if the
  result is unused: declaring a void-looking callee `s32` reserves `$v0` across
  the call, so a following statement lands in `$v1` and a `return K` fills the
  load-delay slot. If a leaf has a spurious nop after a post-call reload, type
  the callee non-void.

---

## 4. Loop optimization (loop.c: `move_movables`, strength reduction)

- **Loop-invariant hoist — the EXACT rule** (read from FSF loop.c, confirmed
  against the real cc1-27 `-dL` dump; 53B40 provenance, 2026-07-14). A
  loop-invariant `SET(reg)(src)` with `reg` set once becomes a *movable* with
  `savings = n_times_used[regno]` (uses in the loop) and `lifetime =
  luid(last_use) − luid(first_use)`. It is hoisted to the preheader iff:
  ```
  already_moved[regno]  ||  threshold * savings * lifetime  >=  insn_count
  ```
  where `threshold = (loop_has_call ? 1 : 2) * (1 + n_non_fixed_regs)` (MIPS: ≈25–30),
  **decays −3 after each movable it hoists** (so earlier movables in insn order get
  a fatter budget), and `insn_count` **doubles permanently** the moment any reg has
  already been moved (dump prints "halved since already moved") — that mutation
  persists for ALL later movables in the chain. So the old "≈3 threshold" was wrong;
  it's this product test.
- **The CSE-merge is `combine_movables` (`matches K` in the dump).** Movable M1
  merges into an earlier M iff both regs are set once AND
  `rtx_equal_for_loop_p(m->src, m1->src)` — **identical source RTX**. Two pseudos
  loaded from the SAME `CONST_INT` in one loop therefore ALWAYS merge; the merge
  SUMS their savings+lifetime and evaluates the combined movable at the earliest
  site. This is why a constant used 3× (lives 3+3+24) sails over the threshold and
  hoists into a callee-saved reg, starving a rival 2-use constant that can't.
- **The two levers to stop a hoist from C** (both compile-verified on 53B40):
  1. *Kill the merge* → route every deref through ONE reused local
     (`pp = (T*)ADDR;` at each site). A multi-set pseudo is never a movable, so the
     constant rematerializes inline (`lui/ori`) at every site — matches the target.
  2. *A rival constant with too few uses simply cannot be loop-hoisted* — if the
     target keeps it in a callee-saved reg, it must be a **pre-loop C local**
     (`x = 0x40000000;` before the loop), not a loop-body constant.
  *(This resolved MML's 53B40 / BB4C genus: it is C-REACHABLE, not a toolchain wall.
  Regenerate the decision dump with the `-dL` recipe in LESSONS.md 2026-07-14.)*
- **Strength reduction turns array indexing into a marching pointer.** Deriving
  an offset from the loop counter (`tbl[i*8]`) makes cc1 emit `la` once + `addu
  p,8` (pointer march). Keeping TWO induction variables in the source (`for(i=0;
  i<n; i++, off+=8) tbl[off]`) preserves the original's per-iteration
  `lui/$at + addu + load` symbol-indexed form. Two IVs = symbol-indexed; one =
  pointer-march.

---

## 5. Instruction scheduling & delay slots (sched1 / RA / sched2)

- **cc1 schedules on pseudos, allocates, then schedules AGAIN** — so emission
  order rarely equals statement order. Match your *statement order* to the
  *first-use order in the asm*, then let the scheduler interleave. Two statements'
  loads routinely hoist together with the stores sinking (one into a delay slot).
- **Don't fight interleaving** — when 2-3 hand reorderings fail, brute-force all
  orderings in a scratch TU (§0) and diff.
- **Anti-dependence pins store order.** A trailing `q->field = x; global = q+1;`
  lets the scheduler hoist the global store above the field store (they look
  independent). Writing `q++; global = q;` makes the increment clobber `q`'s own
  register — the anti-dependence pins the order.
- **Aliasing forbids hoisting a load above a store it might alias.** If the asm
  loads a global BEFORE a store through a pointer, read the global into a local
  temp first. Passing a struct FIELD (not a local/global) as a call arg
  reproduces a reload-after-store (the aliasing store forbids forwarding).
- **RMW-as-argument passes the incremented value with no reload:**
  `f(a, b, p->field += 1)` hands the RMW register straight to the call; args
  evaluate right-to-left so the RMW's load comes first.
- **Compute-into-locals AT the load site keeps values live across stores** (three
  temps live at once, const in an arg reg). Writing the arithmetic back at the
  STORE site instead lets cc1 collapse each into a sequential RMW — different
  schedule.

---

## 6. Jump optimization / branch canonicalization (jump.c)

- **`switch` defeats ternary/if jump-canonicalization on equality legs.** cc1
  tree-folds `(v==K)?A:B` and inverts the branch (`bne`→…); a `switch` on a
  byte-sized value keeps the `==K` leg as an UN-inverted `beq`/`beqz` with the
  body out of line. For a parked "can't get the un-inverted beq" function, try a
  switch FIRST: load the field into a small local, switch on it, group fallthrough
  cases onto one body, equality leg its own case, `default` the last value,
  assign a local, single store after the switch.
- **case-TREE vs range-collapse is steerable by an explicit `case`.** A switch
  over only cases 1..3 range-collapses to one `slti/blez` bounds check; adding an
  explicit `case 0:` sharing the default body forces the balanced tree
  (`beqz`/`bltz`/`slti 4`) — three separate exits, matching an original that has
  them.
- **Fallthrough-into-default keeps a shared constant leg un-inverted:** an
  `if(call()==0){out=K1;break;}` followed by fallthrough into `default:out=K2;`
  emits `bnez`→(default `li` in the delay slot) — the exact original shape; the
  ternary form gets inverted+inlined.
- **Duplicate constant case bodies are NOT cross-jumped when written as separate
  cases** — write them separately if the original has two `li`s.
- **goto-shared-return defeats the setcc/`sltu` tail collapse.** A clean
  `if(x)return 1;return 0;` diamond collapses to `sltu $v0,$0,$v0`. The original's
  `bnez`-with-`li 1`-in-delay + fallthrough-0 shape comes from an EARLIER leg also
  jumping to the same return-0 (`goto zero; … zero: return 0;`) — the extra
  inbound edge blocks the store-flag conversion. Separate `return 0;` statements
  do NOT work.
- **Branch SENSE is load-bearing, not just logically-equivalent.** `bgez` that
  jumps TO the return-1 block is `if(x>=0){…;return 1;} return 0;`; a `bltz` to
  return-0 is different codegen for the same C meaning. Read the sense off the
  asm.
- **if/else ARM ORDER = branch layout.** `if(X>0)A;else B;` makes A the
  fall-through (cc1 emits the inverse test). If the asm branches TO the ">0" block
  and falls through to the other arm, invert the source to `if(X<=0)B;else A;`.
- **`||` vs `&&` phrasing of a two-test guard changes branch senses + arm
  placement.** Logically-identical `if(A||!B)x;else y;` and `if(!A&&B)y;else x;`
  emit different branch senses; pick the one whose second test branches to the
  small arm with an un-inverted `bnez` while the big arm falls through.

---

## 7. Type-driven codegen

- **`u8`/`s8` param masking:** a `u8` param gets re-masked (`andi $x,$x,0xff`) at
  entry or use even though callers pass a byte; an int-typed (`s32`) param is
  compared/used RAW. Raw compare in asm ⇒ int-typed param; masked-at-use ⇒ `u8`.
- **Signedness of a load follows the cast at the USE site:** `(s8)field` → `lb`,
  plain `u8` field → `lbu`. One function can read the same field both ways
  (`(s8)f` as a signed index, `u8 c=f+1` for an unsigned increment).
- **Where a `& 0xFFFF` mask lands is set by the C type of the holder:**
  `s32 v = f() & 0xFFFF; g(v);` masks at the STORE; `u16 v = f(); g(v);` defers
  the mask to the CALL site.
- **K&R decl + ANSI def for default-promoted args:** a negative constant through a
  narrow ANSI prototype (`u8 op`) truncates at the CALL site (`li $a2,0xFF` vs
  `-1`). Declare the function K&R (`s32 f();`) so callers pass promoted ints raw,
  but keep the DEFINITION's narrow param (its entry copy can be allocation-load-
  bearing). C89 requires the DEFINITION also be old-style when params promote —
  `s32 f(a,b,op) s32 a; T* b; u8 op; {…}` yields byte-identical `.text` to ANSI.

---

## 8. Expression-shape micro-knobs (cheap, try first on a single-insn mirror)

- **Parenthesization steers `addu` operand order:** `p + (p[1] + 2)` adds the
  constant to the byte BEFORE adding the pointer; `p + p[1] + 2` mirrors it.
- **`p[i+K]` vs `*(p+i+K)`:** bracketed → `addu index,base` (index first);
  explicit left-assoc pointer arith → `addu base,…` (pointer first).
- **`and` operand order follows source operand order:** `x & k` puts `k` (older
  live reg) first; `k & x` mirrors it.
- **Chained assignment stores inner-first:** `a = b = X;` stores `b` then `a`
  from one register, no reload (separate statements would reload via aliasing).
  Order the chain so the required-first store is the inner one.
- **Idiomatic swap first:** `t=x[a]; x[a]=x[b]; x[b]=t;` matches; "load both,
  store both" mirrors registers. Write the natural form; get creative only when
  the diff demands.
- **Field-by-field copy vs struct copy in shift loops:** a whole-struct
  `*dst = dst[1]` strength-reduces to ONE induction var (short); copying the
  fields individually keeps the original's two-cursor shape.

---

## 9. Aliasing & `volatile`

- **`volatile` reproduces a reload-after-store** (a callback counter stored then
  reloaded for its return value). Plain extern lets cc1 reuse the register (one
  `lw` short). A "redundant" load after a store of the same global ⇒ think
  `volatile`, not weird source.
- **`volatile` does NOT fix address-CSE or a 2.7 branch reorder** — for those use
  the neighbor-symbol / expression-shape knobs (§3, §6).
- **Local-array-forces-frame:** a leaf with `addiu $sp,-N` and no frame stores
  wants a dead `char buf[N];` local — cc1 reserves frame space for a local array
  even when all real work is in registers. A short `s16` local (HImode) can
  simultaneously buy an 8-byte frame temp AND an `lh + addu`-copy pattern.

---

## 10. Data / rodata & jump tables

- **Dense `switch` jump tables emit inline** as `.rdata/.align 3/.word $Lcase…`
  inside the function; the block survives the pipeline and GAS maps it to
  `.rodata`. If the code matches, the table relocs match for free.
- **A TU's tables land in ONE `.rodata` chunk in FUNCTION order** — to carve a
  multi-table TU the matched table functions' original tables must be CONTIGUOUS
  in rodata (or matched in address-contiguous groups). Check 8-alignment at carve
  points (cc1's `.align 3` fights a non-8-aligned split).
- **cc1 picks table vs `beq` ladder purely by case density** (≈8 dense → table;
  ≈3-5 sparse → ladder). Same source template drives both.
- **A `gp_rel` store can show as a scratch-TU "hard mismatch" that is a FALSE
  POSITIVE** (only the unresolved `%gp_rel` offset differs; the base bytes match).
  When the only remaining diffs are gp_rel stores, it will resolve at link — land
  it.

---

## 11. The diagnostic dumps (turn guessing into arithmetic)

`cc1 … -d<x>` writes `gccdump.<pass>`. Pipe the SAME `cpp` output the build uses.
- `-dl` → `.lreg`: per-pseudo `used X times across Y insns` + local RTL. Use for
  the allocno-priority arithmetic (§2).
- `-dg` → `.greg`: global allocation ORDER + dispositions (`regno → hard reg`) +
  `conflicts:` lists. Use for callee-saved swaps and `$v0` conflicts (§2).
- `-dL` → `.loop`: `move_movables` decisions — which invariants were "moved to
  preheader" vs "not desirable", and the savings/threshold (§4).
- `-dj` / `-dc` → `.jump` / `.cse`: what `combine`/CSE eliminated (e.g. a
  `zero_extend` present in `.jump`, gone after `combine` = an `andi` that was
  legitimately elided because `nonzero_bits` proved the source ≤ mask). Use to
  know when a "missing andi" is unrecoverable without changing the reaching def.

Reach for these the instant the usual knobs (decl/init order, ternary/if flip,
locals) fail to move a register or a load count.

---

## 12. Known-hard PARKED genera (need model-escalation or gcc-source study)

These resisted every trial-and-error C shape; the deterministic path is to read
the relevant gcc-2.7.2 pass source and construct the input, not to guess:
- **Constant-address CSE → loop-invariant hoist** (§3+§4): stop N identical
  constant-address computations from merging so a rival constant wins the saved
  reg. `volatile`, distinct pointer types, memory-clobber asm, split temps all
  fail (gcc merges identical constant addresses under -O2).
- **Held-pointer reuse vs absolute store ("addressing-crux")**: getting cc1 to
  reuse a pointer already live in a reg for a struct store instead of
  re-emitting an absolute `%hi/%lo` — cc1 keeps constant-folding the pointer form
  back to absolute.
- **Cross-block delay-slot store via a base register ("jump-canonicalization"
  residue)**: the original slips a store into a branch-delay slot by holding an
  address in a base reg across the branch.
- **Register-birth reorder** (§2): call-result and param ranges made disjoint by
  arg-setup scheduling.
- **Surviving `andi 0xFFFF`** (§11 `-dc`): combine's `nonzero_bits` proves every
  reaching def ≤ 0xFFFF and elides the mask; needs a def combine can't trace.

---

*Maintenance: when a new idiom is proven against the hash in `LESSONS.md`, if it
is a property of the COMPILER (not this game's data), distill it here under the
right mechanism. Keep game symbols out — this file is meant to leave with you.*
