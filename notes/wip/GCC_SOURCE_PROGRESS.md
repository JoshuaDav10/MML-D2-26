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
