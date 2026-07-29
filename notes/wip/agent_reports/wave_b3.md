# wave_b3 — three engine functions

**Generated:** 2026-07-29. Bytecmp run against unique scratch objects.

---

## Family identification

All three are **8-insn pass-through wrappers** calling `func_80031988`:

```
addiu $sp, -0x18   ; sw $ra, 0x10($sp) ; jal func_80031988 ; <delay-slot>
lw $ra, 0x10($sp)  ; addiu $sp, 0x18   ; jr $ra            ; nop
```

This is the 323/324-member family documented in `notes/wip/lane_b_drafts.md`
sections A31–A37. The drafts for func_8004493C and func_8004557C were already
present there; func_80044A80 (delay-slot store variant) was also pre-drafted
(§A37 note). All three pass bytecmp with 0 hard mismatches.

**These are one family.** Family size in the asm: 33 `jal func_80031988` sites
across the rock_neo asm corpus (counted by grep). The solved template is directly
copyable for any other member with a `nop` delay slot; members with a store in
the delay slot need the store prepended in C (the delay slot executes before
control transfers, so it comes first in source order).

---

## func_8004493C

**Source:** `asm/rock_neo/3513C.s` line 12  
**Insns:** 8  
**Delay slot:** `nop` — pure pass-through  
**Bytecmp:** EXIT 0 — 0 hard mismatches, 1 reloc flag (jal, expected)

```c
void func_80031988(u8 *);

void func_8004493C(u8 *o) {
    func_80031988(o);
}
```

---

## func_80044A80

**Source:** `asm/rock_neo/35198.s` line 79  
**Insns:** 8  
**Delay slot:** `sb $zero, 0x0($a0)` — store byte 0 at `o[0]` executes before the call  
**Bytecmp:** EXIT 0 — 0 hard mismatches, 1 reloc flag (jal, expected)

```c
void func_80031988(u8 *);

void func_80044A80(u8 *o) {
    o[0] = 0;
    func_80031988(o);
}
```

Note: `$a0` is unchanged between entry and the `jal`, so `o` still reaches
`func_80031988` as its argument. The `sb` is in the delay slot — it runs
before the call — hence it appears first in C source order.

---

## func_8004557C

**Source:** `asm/rock_neo/35A74.s` line 232  
**Insns:** 8  
**Delay slot:** `nop` — pure pass-through, identical shape to func_8004493C  
**Bytecmp:** EXIT 0 — 0 hard mismatches, 1 reloc flag (jal, expected)

```c
void func_80031988(u8 *);

void func_8004557C(u8 *o) {
    func_80031988(o);
}
```

---

## Notes for landing

- All three can share one TU or go in separate TUs — they have no data dependencies.
- The `void func_80031988(u8 *);` forward declaration is needed in each TU; if a
  real header already declares it with a compatible signature, use that instead.
- The `u8 *` parameter type is workable but not authoritative — the instruction
  words are identical for any pointer type since `$a0` just passes through.
  If the real object type is known (e.g. a struct pointer), substitute it freely.
- The delay-slot `sb $zero, 0x0($a0)` in func_80044A80 forces the store first in
  C. If the real struct has a named field at offset 0, replace `o[0] = 0` with
  `o->fieldname = 0`.
- No symbols are accessed via `%gp_rel` or `lui/%lo` in any of these three
  functions — no extern declarations for data symbols are needed.
- `func_80031988` itself is still RAW_ASM (27 insns, 244 callers per
  `notes/wip/DEP_PRIORITY.md`). Its symbol resolves at link time from the asm
  object — no risk of a dangling reference for these three TUs.
