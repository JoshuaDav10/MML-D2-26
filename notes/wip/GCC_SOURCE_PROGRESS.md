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
> Ground-truth dumps are BANKED (see below). Next: read `move_movables`
> (`~/src/gcc-2.7.2/loop.c:1529`) for the mechanism, cross-check against the plain-text
> verdicts in `base.c.loop`, and render the 53B40 verdict (C form that stops the
> `0x1F800070` movables merging so `0x40000000` wins the reg, OR proof it's
> C-unreachable + the alternate lever).

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
