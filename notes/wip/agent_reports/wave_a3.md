# Wave A3 — Three Engine Functions

## func_8005C224 (3 insn)

**Bytecmp: PASS** — 0 hard mismatches, 1 reloc flag (expected: %gp_rel offset unresolvable by bytecmp).

Source file target: new `src/rock_neo/Code8005C224.c` (pattern matches Code8005C218.c and Code8005C230.c neighbors).

```c
extern s32 D_80098AF4;

void func_8005C224(s32 arg0) {
    D_80098AF4 = arg0;
}
```

Reasoning: `sw $a0, %gp_rel(D_80098AF4)($gp)` — gp-relative store of a 32-bit register → plain scalar `extern s32`, setter pattern identical to neighbors Code8005C218 (s8) and Code8005C230 (s32).

---

## func_8005EBC4 (4 insn)

**Bytecmp: PASS** — 0 hard mismatches, 2 reloc flags (expected: %hi/%lo relocations unresolvable by bytecmp).

Source file target: `src/rock_neo/Code8005EBC4.c` (already split, currently INCLUDE_ASM stub).

`D_800BBD94` is already declared `extern s16` in neighboring TUs (Code8005EBA4.c, Code8005D938.c) — do not redeclare if the target file inherits from a header; but the file currently has no extern, so include it.

```c
extern s16 D_800BBD94;

s16 func_8005EBC4(void) {
    return D_800BBD94;
}
```

Reasoning: `lui $v0, %hi(D_800BBD94)` + `lh $v0, %lo(D_800BBD94)($v0)` — direct `%lo(SYM)($r)` access without an intermediate `addiu` → plain scalar (LESSONS §2). `lh` = signed halfword → `s16`. Returns `$v0` → getter returning `s16`. The neighboring setter (Code8005EBA4.c) confirms `s16` and the scalar spelling.

---

## func_80036A78 (4 insn)

**Bytecmp: PASS** — 0 hard mismatches, 0 reloc flags (pure constant address, no symbol relocations).

Source file target: new `src/rock_neo/Code80036A78.c` (lives in a pure-asm segment `[0x26CB0, asm]`; needs splitting before use).

```c
void func_80036A78(s16 arg0) {
    *(s16*)0x1F800076 = arg0;
}
```

Reasoning: `lui $at, (0x1F800076 >> 16)` + `sh $a0, (0x1F800076 & 0xFFFF)($at)` — raw PSX scratchpad RAM address (0x1F800000 range), not a symbol. `sh` = store halfword → cast to `s16*`. Pattern matches existing raw-address writes in the codebase (main.c `*(s8*)0x1F800074 = 0`, etc.). Bytecmp confirmed clean: constant address folds directly, no reloc ambiguity.
