# wave_b5 — func_80047BD8 / func_80047C64 / func_800485C4

## Family verdict

All three are identical-structure 8-instruction thin wrappers:
`addiu $sp,-0x18 / sw $ra / jal CALLEE / nop / lw $ra / addiu $sp,+0x18 / jr $ra / nop`

This is **family=324** (lane_b_drafts.md §A31–A37), copies=43 per the draft.
No argument shuffle, no delay-slot store — pure pass-through.

func_80047BD8 and func_800485C4 both call func_80031988.
func_80047C64 calls func_80047CD8 instead.

Reference asm: 383D8.s (bulk asm chunk, `[0x383D8, asm]` in splat yaml) for the first two;
38B90.s for func_800485C4.

---

## func_80047BD8

Bytecmp verdict: **PASS** — 0 hard mismatches, 1 reloc flag (jal address, resolves at link).

```c
void func_80031988(void *);

void func_80047BD8(void *o) {
    func_80031988(o);
}
```

---

## func_80047C64

Bytecmp verdict: **PASS** — 0 hard mismatches, 1 reloc flag (jal address, resolves at link).

```c
void func_80047CD8(void *);

void func_80047C64(void *o) {
    func_80047CD8(o);
}
```

---

## func_800485C4

Bytecmp verdict: **PASS** — 0 hard mismatches, 1 reloc flag (jal address, resolves at link).

```c
void func_80031988(void *);

void func_800485C4(void *o) {
    func_80031988(o);
}
```

---

## Notes for integration

- `func_80031988` is already declared in several overlay TUs as `void func_80031988(void);`
  (K&R-no-args form). The `void *` parameter here is consistent with how the family is
  described in lane_b_drafts.md — both forms emit identical bytes (argument in $a0 either
  way). Prefer `void *` if the destination TU uses it; use `void` if that file already
  has the K&R form to avoid a conflicting prototype.
- `func_80047CD8` is not yet in any src TU; declare as `void func_80047CD8(void *);` or
  `void func_80047CD8(void);` — same byte outcome. No symbol resolution concern; both
  callees exist in the asm/ tree and will be linked from the object they currently live in.
- The reloc flags are expected: `jal` targets are link-time relocations, not hard mismatches.
  bytecmp correctly does not fail on them.
- Scratch objects used unique paths (BYTECMP_OBJ=/tmp/bc_b5w_*_<pid>.o) per the LESSONS
  concurrency rule; no cross-contamination.
