# GCC Source Expedition — Phase 0 checkpoint (2026-07-12)

## ⚠️ VERDICT: cc1-27 is NOT stock GCC 2.7.2 — it is an SN Systems (PSY-Q) patched build

- `./bin/cc1-27 --version` (invalid option, but banner prints via stdin compile):
  `GNU C 2.7.2.SN32.3.7 Build 0001 [AL 1.1, MM 40] BSD Mips compiled by GNU C`
- `strings` markers:
  - `2.7.2.SN32.3.7 Build 0001` — SN Systems version suffix (PSY-Q toolchain lineage)
  - `/home/aaron/sn-2.7.2` — build path of a modern host recompile of the SN 2.7.2 source tree
  - `N_PSYM` present
- Binary itself: modern x86-64 ELF PIE (Linux), with debug info — i.e. the SN-patched
  GCC 2.7.2 source recompiled for a 2026 host, not the original DOS/Win32 PSY-Q exe.
- **Implication:** the public FSF gcc-2.7.2 source is a close BASELINE, not an exact
  match. SN32.3.7 carries Sony/SN patches; expect divergence in some passes. Treat
  public-source reading as "probably identical unless proven otherwise per-pass",
  and prefer diffing against an sn-gcc source tree if one can be located later.

## Source obtained: YES

- Location (outside repo): `~/src/gcc-2.7.2/` (i.e. `/home/X/src/gcc-2.7.2/`)
- Archive: `~/src/gcc-2.7.2.tar.bz2` from
  `https://gcc.gnu.org/pub/gcc/old-releases/gcc-2/gcc-2.7.2.tar.bz2`
  (the `.tar.gz` at `ftp.gnu.org/gnu/gcc/` 404s; old releases live on the
  gcc.gnu.org mirror, bz2 only)

## Target files located

- `/home/X/src/gcc-2.7.2/loop.c`
- `/home/X/src/gcc-2.7.2/global.c`
- `/home/X/src/gcc-2.7.2/cse.c`
- `/home/X/src/gcc-2.7.2/jump.c`
- `move_movables` confirmed in loop.c: declared line 267, called line 966,
  defined line 1529.

---

# Phase 1 — move_movables / 53B40 (IN PROGRESS)

## RESUME FROM (single-line pointer — read this first each run)
> Phase 1 ANSWERED (teeth 2-4): 53B40 constants are C-reachable. Fixes verified by
> compile: (1) reused `pp` local kills the 0x1F800070 hoist; (2) pre-loop
> `setflag = 0x40000000` local wins a callee-saved reg; (3) use extern symbol
> D_800BB9C8 (= Moji_work+0x310) instead of &Moji_work[4]/[5]. Next: apply the three
> edits to tools/decomp-permuter/mml_53B40/base.c + fix the entry-check spelling so
> the BB9C8 address stops hoisting ($fp→$s7), then permute/diff to full match.

## Ratchet tooth 1 (2026-07-14, Opus prep) — dumps banked, decision text is READABLE

The 53B40 candidate is `tools/decomp-permuter/mml_53B40/base.c` (moji.c ships 53B40 as
an INCLUDE_ASM stub, so the shipped-TU dump does NOT contain it — must dump the candidate).

**Dumps generated and staged** in the session scratchpad:
`/tmp/claude-1000/-home-X-Documents-MML-D2-26/4176ee0f-5f43-4ff2-a970-32eada8f0e13/scratchpad/`
- `base.c.loop` (87KB) — loop pass, contains the movable decision text + the constants
- `base.c.greg` (123KB) — global register allocation dump
- `base.i` — preprocessed candidate (cpp output)

**Exact regen command** (cheap — rerun if scratchpad is gone; this is the durable ratchet):
```
cd <repo>; source .venv/bin/activate
CPPF="-Iinclude -undef -Wall -lang-c -fno-builtin -Dmips -D__GNUC__=2 -D__OPTIMIZE__ -D__mips__ -D__mips -Dpsx -D__psx__ -D__psx -D_PSYQ -D__EXTENSIONS__ -D_MIPSEL -D_LANGUAGE_C -DLANGUAGE_C -DHACKS"
CCF="-mcpu=3000 -quiet -w -O2 -funsigned-char -fpeephole -ffunction-cse -fpcc-struct-return -fcommon -fverbose-asm -fgnu-linker -mgas -msoft-float -G8 -gcoff"
cpp $CPPF tools/decomp-permuter/mml_53B40/base.c > base.i
./bin/cc1-27 $CCF -dL -dg -dumpbase base.c base.i -o /dev/null
```
(Flags pulled verbatim from `make CPP=cpp -n build/src/rock_neo/moji.c.o`.)

**Key finding — the loop pass narrates its movable decisions in plain text:**
- `Insn 549: possible biv, reg 242, const = 528482416` — `0x1F800070` flagged as a
  candidate basic induction variable.
- `Insn N: regno R (life L), move-insn savings S not desirable` — hoist rejected.
- `Insn N: regno R (life L), move-insn savings S  moved to M` — hoist accepted.
- `Insn N: regno R (life L), done move-insn matches K` — **movable MERGED into earlier
  identical insn K** = the value-merge the 53B40 analysis suspected, now visible.

So the pilot no longer needs a `-dL` invocation figured out from scratch — it needs the
`move_movables` source read to explain WHY these verdicts fire, then the 53B40 verdict.
Also propagated to `notes/LESSONS.md` (2026-07-14 section) — SN-patch fact + -dL recipe.

## Ratchet tooth 2 (2026-07-14, Fable) — move_movables/combine_movables rule EXTRACTED

Source: FSF `~/src/gcc-2.7.2/loop.c` (baseline; SN divergence not yet observed in this pass).

1. **Movable creation** (scan_loop, ~line 770-796): each loop-invariant `SET (reg) (src)`
   with reg set once becomes a movable with:
   - `savings = n_times_used[regno]` (number of uses of that pseudo in the loop)
   - `lifetime = luid(last_use) - luid(first_use)`
2. **Merge = `matches K`** (combine_movables, line 1232-1288): M1 merges into earlier M iff
   - `n_times_used == 1` for BOTH regs (each set once), M1 not global, not partial,
   - dest modes equal (or both MODE_INT with M's width >= M1's),
   - `rtx_equal_for_loop_p (m->set_src, m1->set_src)` — **identical source RTX**.
   Consequence: two pseudos each loaded from the same CONST_INT inside one loop ALWAYS
   merge (constants are always rtx_equal). Merge does `m->lifetime += m1->lifetime;
   m->savings += m1->savings; m1->done = 1; m1->match = m;` — all uses of M1's reg are
   rewritten via reg_map to M's reg (move_movables line 1934-1948). This is the value-merge:
   ONE register carries the constant for the whole (summed) lifetime.
3. **Desirability = `moved to` vs `not desirable`** (move_movables line 1630):
   moved iff `already_moved[regno] || (threshold * savings * lifetime) >= insn_count`.
   - `threshold = (loop_has_call ? 1 : 2) * (1 + n_non_fixed_regs)` (loop.c:532)
   - `threshold -= 3` after EACH moved insn (lines 1719/1904) — order matters, earlier
     movables in insn order get a fatter threshold.
   - `insn_count *= 2` if `moved_once[regno]` (reg already hoisted from another loop);
     dump prints "halved since already moved".
4. **`possible biv, const = C`** (record_biv, dump at line 4313): a bare `reg = CONST_INT`
   in the loop is recorded as a degenerate biv (mult_val=0, inc_val=C). This is why
   528482416 (0x1F800070) shows as "possible biv" — bookkeeping for strength_reduce,
   not itself the hoist decision.

Next: cross-check these predicates against base.c.loop verdict lines for
528482416 (0x1F800070) and 1073741824 (0x40000000).

## Ratchet tooth 3 (2026-07-14, Fable) — dump cross-check: formula CONFIRMED, target 0x40000000 hoist is LOOP-UNREACHABLE

Dump verdicts (base.c.loop, outer loop 61..1036, "399 real insns", loop has calls) are
fully consistent with the FSF formula `moved iff threshold*savings*lifetime >= insn_count`.
No SN divergence observed in this pass.

**Threshold bounding from verdict pairs** (T0 = initial threshold, -3 per move,
and note `insn_count *= 2` at "halved since already moved" MUTATES the local param —
it persists for all LATER movables in the chain, loop.c:1611):
- insn 88 (savings 3, life 3) not desirable → 9*T0 < 399 → T0 <= 44
- insn 104 (savings 4, life 4) moved       → 16*T0 >= 399 → T0 >= 25
- insn 982 (savings 2, life 25) not desirable, after 5 moves (-15) and one doubling
  (insn_count 798) → 50*(T0-15) < 798 → T0 <= 30
→ T0 ∈ [25,30] (= 1 + n_non_fixed_regs, loop_has_call=1). Plausible MIPS n_non_fixed ~24-29.

**The two constants:**
- 0x1F800070 (528482416): three movables (insns 392,549,808; lives 3,3,24) MERGE
  (549/808 print "matches 392") → combined savings 3, life 30 → product 90*T >> 399
  → moved to 1129 → greg gives it $20/$s4. TARGET instead rematerializes lui/ori at
  4 sites and does NOT hoist.
- 0x40000000 (1073741824): two movables (insns 340,360; adjacent single-use temps,
  life 1 each) merge → savings 2, life 2 → product 4*T <= 120 < 399 → "not desirable"
  → inline li. TARGET hoists into $s7 (prologue lui, 2 uses at 44584/445A4).

**Hoist-slot audit:** current build and target agree on 5 of 6 callee-saved constants
($s2+0x71, 0x40000, 0xFF000000, 0xFFFFFF, 0x86186187); the ONLY swap is
0x1F800070 (current) vs 0x40000000 (target).

**Decisive inference:** the target's $s7=0x40000000 has exactly 2 uses; product 4*T
can never reach 399 for any T0 <= 100 → move_movables could NOT have hoisted it in the
original compile either. Therefore in the original C, 0x40000000 was NOT a loop movable:
it was a pseudo set OUTSIDE the loop — i.e. a local variable initialized to 0x40000000
before the loop (`u32 x = 0x40000000;` idiom), register-allocated $s7 by greg directly.
Corollary: with that pseudo occupying a callee-saved slot, only 6 slots exist for 7
candidates; the loop-hoisted 0x1F800070 pseudo (REG_EQUIV const) should LOSE a hard reg
and be rematerialized by reload as inline lui/ori — exactly the target pattern.
Next (tooth 4): test that C form against the dumps/asm.

## Ratchet tooth 4 (2026-07-14, Fable) — VERDICT: C-REACHABLE. Both constants fixed at source level (verified by compile)

**Corrections to tooth 2** (source re-read): `n_times_used` is a bcopy of `n_times_set`
(loop.c:597) — the `==1` merge precondition and `m->savings` init count SETS, not uses.
Also `force_movables` (loop.c:1223) has the famous 2.7.2 bug `m1->savings += m1->savings`
(self-doubling; explains dump savings of 2/4 on single-set regs).

**Why blocking the merge is sufficient** (I was wrong earlier that insn 808 alone would
still move): unmerged, insn 808 (life 24) is evaluated at ITS chain position — after the
inner-loop re-move (insn 1121) has PERMANENTLY doubled insn_count to 798, and after ~4
moves have decayed threshold by 12. 24*(T0-12) <= 24*18 = 432 < 798 → not desirable.
The merge is what evaluates the combined movable at insn 392's EARLY position (product
90*T vs 399). Threshold cascade note: each hoist you remove makes later movables MORE
desirable (-3 decay not taken), so fixes must be applied together.

**Verified C forms** (compiled with the tooth-1 recipe; asm inspected):
1. `0x1F800070` — route every deref through ONE reused local pointer:
   `u32 * volatile *pp;` then at EACH site `pp = (u32 * volatile *)0x1F800070;
   prim = *pp; *pp = prim + 3;` etc. (4 sites incl. tail). The shared local makes
   `n_times_set[pp] == 4 != 1` → scan_loop never creates a movable → no merge, no hoist;
   cse is EBB-local so each site remats `lui/ori` inline — EXACTLY the target pattern
   at all 4 sites (verified in scratchpad test2.s/test3.s).
   Contrast: the anonymous `(*(u32 * volatile *)0x1F800070)` spelling gives each site a
   FRESH single-set compiler temp → three mergeable movables → hoisted. The volatile
   only protects the MEMORY access, not the address constant.
2. `0x40000000` — pre-loop local: `setflag = 0x40000000;` before the loop, both
   `m->flags |= setflag;` sites use it. Not a loop movable (set outside) → greg gives it
   a callee-saved reg directly. Proven loop-unreachable in tooth 3 (product 4*T <= 120).
   Currently lands in $fp instead of $s7 because one competitor hoist remains (below).
3. Bonus discovery (target asm 449D4-449EC, 44A30): the original does NOT write
   `&Moji_work[4]`/`&Moji_work[5]`. It references a SEPARATE extern symbol `D_800BB9C8`
   (= Moji_work+0x310): `if (m != &D_800BB9C8)`, `t = ((u8*)m - ((u8*)&D_800BB9C8 -
   0x310))/0xC4`, loop end `m < &(&D_800BB9C8)[1]` (sltu vs $a1+0xC4 with $a1 = lui/addiu
   %hi/%lo(D_800BB9C8) inline). The `+784` constant movable never existed in the original.
   Remaining delta in test3: the BB9C8 address expr still hoists ($23 = entry-check reg
   - 196) because my entry-check spelling shares the pseudo with the loop-end check —
   spelling work for the permuter thread (entry check in target is `m+0x3D4`-based,
   i.e. Moji_work-relative, DIFFERENT from the BB9C8-based loop-end check).

**Match-or-park:** not yet a byte match (register assignment $fp vs $s7 pending the
BB9C8/entry-check spelling), but the compiler-wall question is ANSWERED: no toolchain
patch, no maspsx lever, no accept-as-asm needed. This is ordinary C-form matching now.
Test files: scratchpad test1.c/test2.c/test3.c (+ .s); regen trivial from base.c with
the three edits above.

## Ratchet tooth 5 (2026-07-14, Opus foreground) — levers RE-VERIFIED + permuter re-seeded

Reproduced the three levers from scratch (worker's scratch test files were lost with
its session). All three confirmed against the target asm via `tools/bytecmp.sh`:
- **setflag** (`u32 setflag=0x40000000;` pre-loop): 0x40000000 now hoists into a
  callee-saved reg (`lui $fp,0x4000`) instead of inline `li` — the intended effect.
- **reused-pp** (`u32 * volatile *pp;` reused at all 4 sites): 0x1F800070 stays inline
  as `lui/ori` at exactly 4 sites (verified — the 6 `lui 0x1f80` are 4×0x70 + 2×0x00,
  matching target which also has 2× the 0x1F800000 byte-read).
- **D_800BB9C8**: ground-truthed the tail from the asm — target's entry check
  `beq $s2,$a1` and loop-end `addiu $v0,$a1,0xC4` BOTH reuse `$a1=&D_800BB9C8`, while
  the INITIAL guard stays Moji_work-relative (`addiu $v0,$s2,0x3D4` = &Moji_work[5]).
  Draft: keep line-107 guard as `&Moji_work[5]`; do-while continuation as
  `m < &(&D_800BB9C8)[1]`; `t = ((u8*)m - ((u8*)&D_800BB9C8 - 0x310))/0xC4`.

**THE gap that stalled prior permuter runs:** the committed permuter `base.c` (dated
Jul 12) was LEVER-LESS — every one of the ~dozens of `output-*` runs searched from the
wrong structure, which is why none converged. Now FIXED: `tools/decomp-permuter/mml_53B40/base.c`
seeded with all three levers (also persisted to the tracked `notes/wip/53b40_draft.c`
since the permuter dir is gitignored). Permuter re-launched from the correct structure.

**Sole residual:** allocno priority — 0x40000000 lands in `$fp`, target wants `$s7`
(Moji_work base takes $s7 in the draft; target holds it in $s2). Draft is +10 insns
over target (505 vs 495), entirely the $fp/$s7 cascade. Next lever if permuter stalls:
the -dg allocno-priority arithmetic (LESSONS "MojiTaskExec" recipe) to bump the
0x40000000 pseudo's priority above $fp's allocation slot.

## Ratchet tooth 6 (2026-07-19, Fable) — $s7 SOLVED + 6 more structural teeth; 426→~100 unaligned

**THE $s7/$fp SWAP IS FIXED.** -dg forensics on the lever'd draft: the intruder was
pseudo 417 = `&D_800BB9C8` hoisted by move_movables (insn 1131) — it ranked 8th in
allocno order and took $s7; setflag(75) fell to $fp. Draft ranking matched the
target EXACTLY through $s6 otherwise. Kill 417's hoist → setflag lands in $s7 with
no other perturbation. All verified via bytecmp + fresh dumps (draft now emits
`li $23,0x40000000` = lui $s7).

**Landed levers (all in notes/wip/53b40_draft.c, structurally verified):**
1. **Tail `last` local**: `last = &D_800BB9C8;` per-iteration inside the loop tail;
   entry-check `m != last`, `mb = (MOJI_TASK*)((u8*)last - 0x310); t = m - mb;`,
   loop-end `m < &last[1]`. Movable rejected (savings 1, late chain position) →
   per-iteration `lui/addiu $a1` exactly like target. The separate `mb` statement
   pins `addiu $v0,$a1,-0x310; subu` (writing the expression inline reassociates to
   (m+0x310)-last — wrong).
2. **Pointer-subtraction divisions**: target's magic constant is 0x1A1F58D1 with
   `mflo; sra 2` = gcc EXACT division (196*M ≡ 4 mod 2^32) from MOJI_TASK* pointer
   subtraction. `c = m - Moji_work;` and `t = m - mb;` — NOT the byte-diff/0xC4
   signed-division form (0x5397829D, mfhi, sign-fix — 2 insns fatter each site).
3. **SetDrawMode takes 5 args** (PSYQ: p,dfe,dtd,tpage,tw): `SetDrawMode(pt,0,0,
   t&0xFFFF,0)` — the 5th arg is `sw $zero,0x10($sp)` in the jal slot and grows the
   outgoing-args area to 0x18 → rect moves to 0x18($sp) → frame 0x50 region. Fixed.
4. **DRAWCTX shape**: tag slots are `D_80098934->x70[m->x3D]` — u32 x70[3] array at
   +0x70 indexed by x3D (sll 2), NOT stride-0x80 struct indexing. Struct is
   `{u8 pad[0x70]; u32 x70[3]; u32 x7C;}` with a LOCAL `dc = D_80098934;` per site
   (pointer loaded once, held across the prim[0] store; x3D and the slot reload).
5. **loop1 layout**: `if ((u32)op < 0x84) {small-op arm} else {dispatch}` puts the
   dispatch block at the end (target layout). Inner glyph loop wants the opposite
   sense (`>= 0x84` fall-through) — still 1 branch-sense off (bnez vs beqz, minor).
6. **Separate `op` variable** for the two script-byte loops (not `c`) — c stays the
   div quotient ($a0 family), op gets $v1 like target.
7. **OR-chain split** (MojiTaskExec idiom): `f = *(u32*)Moji_flag | 0x80000000;`
   then `*(u32*)Moji_flag = f | (0x8000000 >> c);` — kills the reassociation.
8. **xB8/xBA serialization**: one shared temp `w` for both lhu→sh pairs (WAW on the
   single pseudo forces load,nop,sh,load,sh like target; two temps let sched pair
   the loads).
9. **(s16)m->x6** casts at both `0x10000 >> x6` sites → signed lh like target.
10. **pp hybrid** (replaces the volatile device): ONE `u32 **pp` var assigned at
    the render + glyph sites ONLY (2 sets → multi-set → no movable, combine can't
    fold); post_render and tail_env use the ANONYMOUS `*(u32**)0x1F800070` form
    (post_render's lone movable fails threshold; tail is outside the loop).
    This reproduces the target's per-site regs ($v1/$v1/$a2/$v1). Anonymous at ALL
    sites re-hoists (verified — $fp comes back); volatile pins the *pp store too
    early (target has it in the jal delay slot).
11. **tail_env uses its own pointer var** (`pt`) — loop `prim` conflicts with the
    $s0 giv so it sits in $s1; the tail pseudo doesn't conflict → $s0 like target.

**Metric**: structural-unaligned (new tools: scratchpad sdiff.py aligning opcode+reg
streams) 495-ref vs draft: start ~190 → now ~100; words 495 vs 492 (draft 3 short).
bytecmp hard mismatches are cascade-noise (~407) — most of the body is
register-name/scheduling identical after alignment.

**OPEN (the remaining ~100, in blocks):**
a. Preheader order: target = giv($s0 addiu, in beqz slot), 0x40000($s5),
   0x40000000($s7 - the setflag STATEMENT insn), 0xFFFFFF($s3), 0xFF000000($s4),
   0x86186187($s6). Draft = setflag(slot), s5, s4, s3, s6, giv. Two puzzles: giv
   emitted FIRST (before movables — strength_reduce emits after move_movables, so
   sched/dbr must have moved it), and FFFFFF-before-FF000000 movable order (first-
   use order should be FF000000 first given `(prim[0]&0xFF000000)|(slot&0xFFFFFF)`
   — maybe the original wrote the operands SWAPPED; probe pending).
b. `prim = *pp` load lands DIRECTLY in $s1 in draft; target does `lw $v0` + `addu
   $s1,$v0` (copy) at render AND post_render (both $v0). Multi-set intermediate var
   + copy spelling did NOT survive (cse copy-propagates it away). Target also has
   8 DEAD stack bytes (0x20-0x27 never touched) = 2 reload spill slots → the copies
   are likely RELOAD/pressure artifacts (orig compile spilled where draft doesn't).
   Draft frame 0x48 vs target 0x50 is exactly these 8 bytes.
c. D494 call block: `m->flags &= 0xFFFBFFFF` store should land in the jal delay
   slot (target) — draft emits it 2 insns early.
d. Sound-region `j loop1` should carry `sw $v0,0($s2)` in its slot — draft leaves
   the sw before the j (nop in slot).
e. Member-store cluster (x78/script/x10/x73/xBE/x3E/x12): statement order now
   matches ref store order but sched pulls x3E's sb early + x3C's lbu early;
   ref keeps x12's sh in the beq slot.
f. Glyph pb-store cluster: +0x8/+0xA vs +0x10/+0x12 store order differs.
g. mflo timing at tail div (ref fills mult latency with lb x71 + lw gp; draft
   mflo's immediately).
h. Small branch-sense flip in the inner loop head (beqz vs bnez to the far block).

**Tools**: scratchpad sdiff.py (structural aligner, mipsel-elf-objdump vs splat .s)
— rebuild it from this file's description if lost; it made every tooth above cheap.

## Ratchet tooth 7 (2026-07-19, Fable cont.) — preheader SOLVED: three body-top locals

- **Inner-loop SEMANTIC BUG found and fixed**: original renders glyphs for op < 0x84
  and calls D_8008AAC4 for >= 0x84 — every prior draft had the arms inverted
  (physically same layout, ONE branch word differs: beqz vs bnez). All prior "logic
  verified" claims were off by this word.
- **Preheader EXACT MATCH achieved** (slot=giv addiu $s0,$s2,0x71; li $s5 0x40000;
  li $s7 0x40000000; li/ori $s3 0xFFFFFF; li $s4 0xFF000000; li/ori $s6 0x86186187):
  1. `setflag = 0x40000000;` must be INSIDE the do-body top (unconditional), NOT
     pre-loop: as a body-top single-set invariant it is hoisted by move_movables;
     pre-loop it sits before the guard branch and dbr steals it for the delay slot
     (ref slot = the giv init, which dbr picks when NOTHING sits between the guard
     sltu and the branch).
  2. Conditional-arm sets are NEVER hoisted (verified P1/P2 probes + dump: an
     in-arm `setflag=` compiles to inline li ×2). Only body-top works.
  3. Movable emission order = in-loop first-set order ⇒ the ORDER of the three
     body-top statements dictates preheader li order: `tst = 0x40000;` then
     `setflag = 0x40000000;` then `fff = 0xFFFFFF;` (+ anonymous 0xFF000000 and
     0x86186187 from their first sites) reproduces the target exactly.
     The four `& 0x40000` sites use `tst`; the six in-loop `& 0xFFFFFF` sites use
     `fff`; tail_env keeps ANONYMOUS masks (target reloads fresh lui/ori there).
  4. The mask-operand order inside the tag statements stays UNSWAPPED
     (`(prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff)`) — site and-order and
     preheader li-order are decoupled by the fff local (swapping operands fixes one
     and breaks the other; the third local fixes both).
- dc = per-site BLOCK-locals (`{DRAWCTX *dc = D_80098934; ...}`) — one function-wide
  dc is a single pseudo = one reg everywhere; target uses different regs per site.
- Structural gap now 91/495 words (from 426 mismatches at session start). Draft is
  v5 = notes/wip/53b40_draft.c. Remaining blocks: 0x80000000-slot choreography,
  D494 sw-in-jal-slot, sw-in-j-slot at the x4==0 tail, prim load-copy (reload
  artifact, 2 sites), member-store cluster scheduling, glyph pb cluster order,
  mflo latency fill at tail div, tail_env mask/reg naming, glyph dc reg naming.

## Ratchet tooth 8 (2026-07-19, Fable cont.) — two more semantic fixes; 56/495; permuter re-launched on fixed infra

- **SEMANTIC FIX #2**: the original DISCARDS func_8001D494's result —
  `m->flags &= 0xFFFBFFFF; func_8001D494(0,1,0);` (the &= store sits in the jal
  delay slot; there is NO post-call store). The draft's
  `m->flags = func_8001D494(...)` emitted a phantom sw. m2c-era carry-over bug.
- Tail div: fold `t` — `D_80098B2C |= (s32)m->x71 << ((s32)(m - mb) << 3);`
  (62→56; the separate `t = m - mb;` statement pinned mflo before the lb/lw pair;
  folded, sched fills the mult latency like the target).
- tail_env: `u32 ww = 0xFFFFFF;` BLOCK-local (fresh var, NOT the function-wide
  `w` — reusing w linked pseudos across sites and moved the xB8/xBA block to $a2)
  + separate `pt` var → pt lands in $s0 like target.
- sdiff.py normalizer now canonicalizes li/move aliases (real gap was 62, not 87).
- **Permuter re-seeded** with the 56-word base (score 2595 vs stalled 9745) after
  fixing the compile.sh cwd/gprel-census bug (see PERMUTER_GUIDE.md). Running
  -j8 --best-only --stop-on-zero.
- Remaining hand-clusters (~56 words): prim load-copy artifact at render+post
  (`lw $v0; addu $s1,$v0` — cse copy-props every spelling tried; suspected
  reload/pressure artifact tied to the target's 8 dead stack bytes),
  member-store cluster reg-birth (script-reload $a2 vs $a1), pb-cluster
  x10/x12 store order (false-dep from reg reuse), glyph/tail dc + mask reg
  mirrors, 0x80000000-vs-0x10000 delay-slot choice at loop top.

## Ratchet tooth 9 (2026-07-19/20, Fable cont.) — TOOLCHAIN BUG FOUND: macro.inc li drops low halves

- Preparing the moji.c landing exposed an environment split: the same draft body
  compiled via tree headers lost the `ori $s6,0x6187` of the 0x86186187 magic —
  `lui $s6,0x8618` only. Bisection: cc1 emits the full `li $22,-2045222521` in
  BOTH environments; maspsx/gprel/patchasm pass it through; **GAS truncates it —
  but only when `.include "macro.inc"` is in the TU** (injected by include_asm.h
  into every in-tree TU). macro.inc OVERRIDES GAS's builtin `li` with a macro
  whose big-constant branches emit `lui %hi(num)` ONLY — the low half is
  silently dropped. Every existing match survived because no cc1 output had
  emitted a big-constant li with nonzero low half until 53B40's magic divisor.
- FIX (include/macro.inc): new branches for `(num & 0xFFFF) != 0` emit
  `lui (num>>16)&0xFFFF; ori num&0xFFFF` (raw shift, NOT %hi — %hi carry-rounds
  when low >= 0x8000). All previously-exercised branches byte-identical.
  Full audit_count.sh re-run after the change (hash + 205 overlays + census).
- Landing TU (scratchpad landing.c) verified at FULL PARITY with the standalone
  draft: tree adaptations are (u8) casts on the two m->x3F reads, (s8) cast on
  the m->x71 read (tree types u8 x71 / s8 x3F are opposite the draft's),
  `(DR_MODE *)pt` for SetDrawMode, local `void SetDrawArea(u32 *, RECT *);`
  decl (libgpu.h lacks it), DRAWCTX typedef + externs added to moji.c.

## Ratchet tooth 10 (2026-07-25, Opus) — STACK-SLOT ORDERING lever: 75 -> 70

**Measured state at session start** (all three rulers agree, ~2 apart as expected):
- `tools/score53b40.sh notes/wip/53b40_draft_permbest.c` = **75** positional rows
- `CPP=cpp tools/bytecmp.sh func_80053B40 <draft>` = 73 hard, **495/495 word parity**
- true in-tree (splice via `tools/land53b40.sh` + `./diff.py`) = **68** differing rows
  (in-tree is lowest because ~5 scratch mismatches are `%gp_rel` artifacts that
  resolve when linked — confirmed: idx 1/23/26/46/54 are all `gp_rel(Moji_flag)`).

**CONFIRMED SOLVED (do not re-investigate):** the frame fossil. In-tree the prologue
is byte-identical — `addiu $sp,-0x50` and every callee-save offset (0x48/0x44/0x40/
0x3C/0x38/0x34/0x30/0x2C/0x28) matches the target exactly. The `$s7`/`$fp` swap and
the frame size are DONE (teeth 6-9 work held up).

**NEW LEVER FOUND — local declaration order controls stack-slot assignment.**
Symptom: our `rect` sat at `0x20(sp)`, target's at `0x18(sp)` (5 mismatches:
`addiu a1,sp,0x18` + the four `sh v?,0x18/0x1a/0x1c/0x1e(sp)` stores). Cause: the
permuter-introduced `volatile unsigned short new_var11;` is declared BEFORE
`RECT rect;` and, being volatile, is forced to memory — it claimed slot `0x18` and
pushed `rect` up to `0x20`. Fix: **move `RECT rect;` ABOVE
`volatile unsigned short new_var11;`** in the declaration list. gcc 2.7 assigns
stack slots in declaration order, so rect reclaims 0x18. Score 75 -> 70, parity held.
(Any placement of rect earlier than new_var11 gives 70 — it is the ORDER that matters,
not the exact position.)

**Re-confirmed load-bearing (do not "clean up"):** `volatile unsigned short
new_var11` cannot be removed or retyped. Dropping `volatile` -> 494 words/333 rows;
retyping to `u32` (restoring real `m->flags & 0x100000` semantics) -> 497 words/338.
Note it is semantically a no-op as written (`0x100000` truncated into a u16 is always
0, so `if (!new_var11)` is always true) — but it holds the instruction stream
together. This matches the tooth-4/5 finding that the search's "weirdness" IS the
solution taking shape.

**Remaining 68 (in-tree), biggest cluster = the RENDER-SITE COPY (~10 rows).**
Target: `lw $v0,0($v1)` … `addu $s1,$v0,$zero` (a real live-range-split copy into the
callee-saved reg), then `addiu $v0,$s1,0xC` computes the advance FROM `$s1`. Ours
folds it to `lw $s1,0($v1)` direct — one instruction SHORT here, which shifts every
following row in the cluster. Levers TRIED AND FAILED this session (all neutral at 70
or worse — do not repeat):
  - redundant conditional def `if (m) { prim = pr; } else { prim = pr; }` (the lever
    that cracked E4C4/62C6C) -> 70, no effect
  - keep `pr` live after the copy (`pr = prim;`) -> 70, no effect
  - hoist the RECT table base into a local before the copy -> 496 words / 349
  - move the `*pp = prim + 3` store after the call -> 497 words / 487
The copy is a reload/live-range-split artifact, not a source-level construct; next
attempt should come from a `-dg`/reload dump showing WHY the original split the range
(suspect: the load result was allocated a caller-saved reg because its live range
started before the arg-setup block).

**Next step:** permuter re-seeded with the 70-row base (parity-safe). Remaining
clusters after the render copy: the `0x80000000`/`0x8000000` constant materialization
ORDER at 443dc-44400, and an `s1`/`s2` mirror at 445c4.

## Tooth 11 (2026-07-25, Opus) — RTL/stack forensics: the volatile is PADDING, not a fix

Read the actual `-dg` greg dump + compared stack-slot usage. Three findings that
reframe the endgame; the next session should start from HERE, not from more C-form
guessing at the render site.

### 1. Our render-site RTL, exactly (greg dump, insns 397-422)
```
insn 397: (set (reg/v:SI 17 s1) (mem:SI (reg/v:SI 3 v1)))   ; lw  s1,0(v1)  <-- DIRECT
insn 403: (set (reg:SI 2 v0) (plus (reg/v:SI 17 s1) 12))    ; addiu v0,s1,0xC
insn 405: (set (mem:SI (reg/v:SI 3 v1)) (reg:SI 2 v0))      ; sw  v0,0(v1)  (v1 DIES here)
insn 411: lbu v0,0x1F800000 ; 420: addu a0,s1 ; 414: sll ; 416/422: a1 = &D_80097F50[i]
```
Target instead: `lw $v0` … `lui/addiu $a1` … `addu $s1,$v0,$zero` (COPY) … `addiu $v0,$s1,0xC`
… `jal` … `sw $v0,0($v1)` **in the delay slot** (so `$v1`/pp stays live to the end).
Ours coalesces pr+prim into one pseudo (s1) and sinks the store EARLY, killing v1.

### 2. STACK LAYOUT NOW MATCHES EXCEPT ONE SLOT (this is the fossil, precisely)
Touched sp-offsets — ours vs target:
- ours:   0x10, 0x18,0x1A,0x1C,0x1E (rect), **0x20**, 0x28..0x44 (callee saves)
- target: 0x10, 0x18,0x1A,0x1C,0x1E (rect),  ---  , 0x28..0x44
Frame is 0x50 in BOTH; rect is at 0x18 in BOTH (tooth 10 fix). The ONLY difference:
**we touch 0x20; the target reserves 0x20-0x27 and never touches it.** That is the
"register-pressure fossil" — 2 words reserved for pseudos whose accesses reload
optimized away. We occupy 0x20 with the volatile `new_var11` (its `sh`/`lhu` pair).

### 3. THE KEY INSIGHT — `volatile unsigned short new_var11` is INSTRUCTION PADDING
It is semantically dead (`m->flags & 0x100000` truncated into a u16 is always 0, so
`if (!new_var11)` is always true). Removing it gives **494 words (-1)**, i.e. we are
currently at the correct 495 only because ~2 fake instructions from the volatile are
standing in for ~2 genuinely MISSING real instructions elsewhere — **one of which is
the render-site copy `addu $s1,$v0,$zero`.**

=> The endgame is NOT "make the permuter grind" and NOT "find a C spelling for the
copy in isolation". It is: **find the real missing instructions (render copy + the
0x80000000 materialization order), add them, and DELETE the volatile padding in the
same step.** Parity must be evaluated on the combined edit — each half alone breaks
495 and looks like a regression (that is why every isolated attempt has failed).

### Levers tried this session at the render site — ALL FAILED, do not repeat
| lever | result |
|---|---|
| redundant cond def `if (m){prim=pr;}else{prim=pr;}` (the E4C4/62C6C winner) | 70, no effect |
| `pr = prim;` after the copy (keep pr live) | 70, no effect |
| hoist `RECT *rp = D_80097F50;` before the copy | 496 words / 349 |
| move `*pp = prim+3` after the call | 497 / 487 |
| interleave (add) `new_var9 = 0x80;` between load and copy | 497 / 491 |
| interleave (add) `new_var10 = m->x3C;` | 495 / 78 |
| MOVE `new_var9 = 0x80;` up into the load/copy gap | 497 / 491 |
| hoist `0xFF000000` into a local (add reg pressure) | 494 / 99 |
| drop `volatile` / retype new_var11 to u32 | 494 / 333, 497 / 338 |

Note the post_render twin of this copy WAS solved by the interleave trick
(`pr = *X; rect.x = m->x8; prim = pr;`) — the render site resists it because the
statements available to interleave there are not free (each adds an insn).

### Tooth 11 addendum — the frame size is ALSO an accident of the padding
Dropping `volatile` doesn't just lose an instruction: **the frame shrinks 0x50 -> 0x48**
(`addiu $sp,-0xB8FF` vs `-0xB0FF`) and every callee-save offset shifts down 8. So the
volatile's stack slot is the ONLY reason our frame is 0x50. We currently match the
target's frame size and word count **for the wrong reasons** — padding, not structure.

The target's 0x50 frame comes from its allocator genuinely SPILLING 2 pseudos
(reserved 0x20-0x27, accesses later optimized away). Attempts to reproduce that
pressure honestly, both FAILED by *removing* instructions instead of adding pressure:
- hoist `0xFF000000` to a long-lived local -> 494 words / 99
- hoist the 3 per-site `DRAWCTX *dc` block-locals to one function-scope pseudo
  -> 492 words / 402   (confirms the tooth-9 finding that per-site dc locals are required)

**Bottom line for the next session:** the ~70 residual rows are NOT 70 independent
nits. They are the visible surface of ONE fact — our compile allocates registers
differently from the original (no spills, so no live-range-split copy, so a different
schedule). Chasing individual rows or permuting the padded base cannot converge.
The productive question is: *what makes gcc-2.7 spill exactly 2 pseudos here?*
Answer that (via `-dg`/reload analysis of allocno pressure at the render site, or by
finding the source shape that genuinely raises pressure without deleting insns) and
the copy, the frame, and most of the residual should fall out together.
