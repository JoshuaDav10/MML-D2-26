# Research findings — gcc 2.7.2 CSE-hoist & pointer-fold suppression

**Provenance:** Deep-research report returned against the prompt in
[`RESEARCH_BRIEF_gcc272_idioms.md`](RESEARCH_BRIEF_gcc272_idioms.md). Answers
**Q1** (constant-address CSE / loop-invariant hoist suppression = the
`func_80053B40` blocker) and **Q2** (pointer folding + branch-delay slots =
`func_8001BB4C`).

> **STATUS — CANDIDATE, NOT HASH-GATED.** Everything below is a lead, not a
> proven idiom. It is an LLM-authored research report; some cited files/paths
> (e.g. `src/main/gp_relative.c`, specific line cites) may not exist as stated.
> Nothing here goes into [`LESSONS.md`](LESSONS.md) until it is confirmed by a
> byte-for-byte `make CPP=cpp check_rock_neo_only` OK on a real MML function.
> Treat repo/file references as *starting points to verify*, not facts.

Related: portable rules in [`COMPILER_IDIOMS.md`](COMPILER_IDIOMS.md); local
forensics in [`wip/53B40_PROGRESS.md`](wip/53B40_PROGRESS.md) (Q1) and
[`wip/BB4C_PROGRESS.md`](wip/BB4C_PROGRESS.md) (Q2).

---

## Q1 — Suppress CSE of a repeated constant address (no loop-invariant hoist)

### Mechanism (per the report)
Hoisting is a two-pass process:
1. **`cse.c`** value-numbers duplicate constant loads/calcs within a block or
   flow path and consolidates identical address constants into a single pseudo.
2. **`loop.c`** then runs invariant analysis: `scan_loop` registers candidate
   expressions in a `struct movable` list; `combine_movables` merges invariants
   sharing a source; `move_movables` relocates the consolidated load into the
   loop pre-header and assigns it a saved register.

The merge in `combine_movables` is governed by:
```c
if (m->set_src == n->set_src || rtx_equal_p (m->set_src, n->set_src))
```
To suppress the merge, the C source must make `rtx_equal_p` return 0 for the
address computation, **or** insert a non-invariant side effect between uses so
the address is no longer classed as loop-invariant.

### Why the already-tried idioms fail
- **Volatile pointer cast** — forces the *dereference* to execute as written but
  does **not** mark the *address calculation* volatile; the numeric constant is
  still a pure invariant, so it's consolidated and hoisted.
- **`asm volatile("":::"memory")` barrier** — prevents caching dereferenced
  *values*, but does not clobber constant-address registers (a constant address
  calc doesn't depend on memory state).
- **Volatile local** — defeats hoisting but introduces `$sp`-relative reloads
  (`lw`) that don't match the original inline `lui/ori`.

### Candidate idioms (verify against hash)

| Idiom | C formulation | Expected asm | Mechanism |
|-------|---------------|--------------|-----------|
| **Volatile offset variable** | `extern volatile int zero_offset;`<br>`#define ADDR 0x1F800070`<br>`*(volatile u32*)(ADDR + zero_offset) = val;` | `lw $at, zero_offset` / `lui $v0,0x1F80` / `addu $at,$at,$v0` / `sw $a0,0x70($at)` | Forces re-evaluation of the addition at every site → address calc is loop-**variant**, not merged. |
| **Volatile inline-asm load** | `u32 __ptr;`<br>`__asm__ volatile("li %0,%1" : "=r"(__ptr) : "i"(addr));`<br>`(volatile u32*)__ptr;` (as macro returning the ptr) | `lui $at,0x1F80` / `ori $at,$at,0x70` / `sw $a0,0($at)` | `asm volatile` is an opaque side-effecting black box → `loop.c` won't hoist it. |
| **Double-clobber anchor (raw store)** | `#define POKE(addr,val) __asm__ volatile("sw %1,0(%0)" :: "r"(addr),"r"(val) : "memory")` | `lui $at,0x1F80` / `ori $at,$at,0x70` / `sw $a0,0($at)` | Bypasses compiler store-gen entirely; no hoisting possible. |

> Note: the volatile-offset idiom emits an extra `lw`/`addu` — it matches only if
> the original actually has that shape. For MML's 53B40 the target is a clean
> inline `lui/ori` with a *different* constant hoisted, so the inline-asm-load
> form is the closer candidate. Confirm with a diff.

---

## Q2 — Force reuse of a held base register instead of an absolute store

### Mechanism (per the report)
`combine.c`'s `try_combine` merges a pointer-register setup followed by a store.
If the base register holds a known compile-time constant, it folds
`offset($reg)` into an absolute `%lo(sym)($at)` with a fresh `lui`. Sketch of the
folding condition:
```c
if (CONSTANT_P (src) && rtx_equal_p (dest, base_reg)) {
    new_mem = find_single_use (dest);
    if (new_mem && GET_CODE (new_mem) == MEM)
        SUBST (XEXP (new_mem, 0), plus_constant (src, offset));
}
```
**Delay-slot consequence:** a folded absolute store is *two* instructions
(`lui` + `sw`), which cannot fit a single branch-delay slot. If the base stays
in a register the store is *one* instruction (`sw $v1,0($a0)`) and `reorg.c` can
migrate it into a preceding branch's delay slot — matching hand-tuned originals.

### Candidate idioms (verify against hash)

| Technique | C formulation | Expected asm | Outcome |
|-----------|---------------|--------------|---------|
| **Opaque pointer anchor** | `struct Gpu *gpu = (struct Gpu*)0x1F801810;`<br>`__asm__ volatile("" : "+r"(gpu));`<br>`gpu->gp0 = val;` | `addu $a0,$base,$zero` / `sw $v1,0($a0)` | `"+r"` marks the reg modified → breaks constant folding, enables delay-slot fill. |
| **Local register binding** | `register struct Gpu *gpu __asm__("a0") = (struct Gpu*)0x1F801810;`<br>`__asm__ volatile("" : "+r"(gpu));`<br>`gpu->gp0 = val;` | `sw $v1,0($a0)` | Pins base into a specific reg; consistent reg-relative addressing. |
| **Explicit delay-slot anchor** | `if (cond) { gpu->gp0 = val; __asm__ volatile("" : "+r"(gpu)); }` | `bne …` / `sw $v1,0($a0)` | Keeps reg-relative layout across the branch so the scheduler fills the slot. |

The reported win pattern: pass a base pointer through an empty `asm volatile("" : "+r"(p))`
**immediately after initialization**, before the run of stores.

---

## Diagnostic workflow (per the report)

RTL dumps to locate where the address is consolidated / hoisted:
```bash
cc1 -O2 -G8 -dc -dl -dg input.c -o output.s
```
- `.cse` dump (`-dc`) — a single pseudo assigned the address and shared across
  stores ⇒ CSE merged them.
- `.loop` dump (`-dl`) — constant load present in the loop **pre-header** ⇒
  treated as loop-invariant (this is our 53B40 tell).
- `.greg` dump (`-dg`) — final register assignments / spills.

maspsx flags to expose folding:
```bash
python3 tools/maspsx/maspsx.py --no-macro-inc --expand-li output.s > clean.s
```
- `--no-macro-inc` — `$at` managed exactly as cc1 emits (no implicit macro regs).
- `--expand-li` — `li` → explicit `lui/ori`, so absolute folds are visible.
- `gp_allow_offset` / `gp_allow_la` **off** — preserve literal address loads
  instead of `$gp`-relative optimization.

## Reference projects named (VERIFY — paths may be wrong)
- **sotn-decomp** (Castlevania: SOTN) — GCC 2.7.2 PSY-Q, `-O2 -G8`; scratchpad
  CSE suppression. Report cites `src/main/gp_relative.c`, `src/physics/collision.c`
  (unverified).
- **silent-hill-decomp** — GCC 2.7.2 PSY-Q `-O2 -G8`; opaque-pointer-anchor for
  GPU-register folding + delay-slot stores. Cites `src/gpu/draw.c`,
  `src/engine/main_loop.c` (unverified).
- **medievil-decomp** — GCC 2.7.2 PSY-Q `-O2 -fpcc-struct-return`; absolute-fold
  suppression on hardware writes. Cites `src/sound/driver.c`, `src/graphics/ot.c`
  (unverified).
- **maspsx** (`github.com/mkst/maspsx`) — documents cc1-vs-ASPSX quirks.
