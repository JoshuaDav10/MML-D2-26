# wave_a6 — three engine functions

---

## func_8005D4D4 (5 insn, asm: `4DCD4.s`)

**Asm recap:**
```
addiu $v0, $zero, 0x2
lui   $at, %hi(D_800BBD84)
sb    $v0, %lo(D_800BBD84)($at)
jr    $ra
nop
```

`lui/%lo` addressing → per LESSONS: **array extern**.  
`sb` → `s8` element width.  
Return type: void (no return value register used).

```c
extern s8 D_800BBD84[];

void func_8005D4D4(void) {
    D_800BBD84[0] = 2;
}
```

**Bytecmp verdict: PASS** — 0 hard mismatches, 2 reloc flags (lui/%lo, expected).  
Both the array (`D_800BBD84[]`) and scalar (`D_800BBD84`) extern spellings compile
identically here; the array form is used to stay consistent with the LESSONS rule.

---

## func_80066750 (5 insn, asm: `56F50.s`) ⚠️ sw+sb at same offset

**Asm recap:**
```
addiu $v0, $zero, 0x1
sh    $zero, 0x0($a0)
sw    $zero, 0x4($a0)      ← clears word at offset 4
jr    $ra
 sb   $zero, 0x4($a0)      ← (delay slot) stores byte 0 at offset 4 again
```

The `sw` at +4 is immediately followed (in the delay slot) by `sb` at +4. Both
store zero, so the memory state is the same either way — but the compiler emitted
two separate stores because the C source had two separate assignments targeting
offset +4 through different declared types: one `s32` field (→ `sw`) and one
`u8`/pointer-cast byte (→ `sb`).

The simplest source that reproduces this exactly is a pointer cast to the first
byte of the word field, after zeroing the word:

```c
typedef struct { s16 x0; u8 pad2[2]; s32 x4; } OBJ66;

s32 func_80066750(OBJ66 *p) {
    p->x0 = 0;
    p->x4 = 0;
    *(u8 *)&p->x4 = 0;
    return 1;
}
```

A union variant (`union { s32 word; u8 byte; }` at offset 4) also compiles to
identical bytes — both approaches bytecmp-pass. The cast form is preferred
because it requires no extra typedef.

**Bytecmp verdict: PASS** — 0 hard mismatches, 0 reloc flags (all addresses are
struct-relative offsets; no lui/%lo or gp-rel).  
This is a true PASS with no reloc ambiguity.

**Struct note:** `OBJ66` is a new typedef introduced here. If a real struct
already covers this object in `include/rock_neo/`, replace `OBJ66` and the field
names; the byte output is identical as long as the offsets and widths match.
`pad2[2]` may be real members — this function only cares about +0 (s16) and +4
(s32 + u8 alias).

---

## func_8003A13C (6 insn, asm: `2A618.s` and `22538.s`)

**Asm recap:**
```
lui  $v0, %hi(D_800A3962)
lb   $v0, %lo(D_800A3962)($v0)
nop
nor  $v0, $zero, $v0        ← bitwise NOT: $v0 = ~$v0
jr   $ra
 srl $v0, $v0, 31           ← (delay slot) extract bit 31 of ~val
```

`lui/%lo` → array extern. `lb` → signed byte (`s8`).

The `nor`+`srl 31` pattern: `~val >> 31` equals 1 when bit 31 of the original
value is 0 (i.e., when the signed byte is ≥ 0), and 0 when bit 31 is 1 (value < 0).
This is `return (val >= 0)`. The `nop` between `lb` and `nor` is the load-delay slot
filler — GCC 2.7 does not schedule the `nor` into it because the `lb` result is
needed immediately.

```c
extern s8 D_800A3962[];

s32 func_8003A13C(void) {
    return D_800A3962[0] >= 0;
}
```

**Bytecmp verdict: PASS** — 0 hard mismatches, 2 reloc flags (lui/%lo, expected).

---

## Summary table

| Function       | insns | verdict           | reloc flags | hard mismatches |
|---------------|-------|-------------------|-------------|-----------------|
| func_8005D4D4 | 5     | PASS              | 2 (lui/%lo) | 0               |
| func_80066750 | 5     | PASS (true, no reloc) | 0       | 0               |
| func_8003A13C | 6     | PASS              | 2 (lui/%lo) | 0               |
