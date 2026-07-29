# Wave B2 — three engine functions

All three verified with `bytecmp.sh` → 0 hard mismatches. Reloc flags: 0 for all.

---

## func_8002DE04 — 7 insns, bulk asm `1DB20.s`

**Bytecmp verdict: PASS** (0 hard mismatches, 0 reloc flags)

Key discovery: both `arg0` and `arg1` must be declared `s32` (plain integers, not
pointers). The compiler then computes `arg1 = arg1 * 8 + arg0` keeping `$a1` as the
accumulating register, producing `sll $a1,$a1,3; addu $a1,$a1,$a0`. Declaring `arg0`
as a pointer (`s32 *`) caused the compiler to accumulate into `$a0` instead (`addu
$a0,$a0,$a1`), which is a hard mismatch on all three pointer-using instructions.

Reference asm extracted to `/tmp/ref_func_8002DE04.s` (trimmed to 7 instructions).

```c
typedef int s32;

void func_8002DE04(s32 arg0, s32 arg1, s32 *arg2) {
    s32 *p;
    arg1 = arg1 * 8 + arg0;
    p = (s32 *)arg1;
    p[0] = arg2[0];
    p[1] = arg2[1];
}
```

---

## func_80048CEC — 7 insns, bulk asm `393EC.s`

**Bytecmp verdict: PASS** (0 hard mismatches, 0 reloc flags)

0x1F800048 is scratchpad RAM; the function dereferences it as a pointer-to-pointer-to-s16
and returns `table[i]`. The `addu $a0,$a0,$v0` has rs=$a0 (index, after sll ×2) — this is
the index-first form, produced by the bracketed `p[i]` spelling (per LESSONS line ~571).

Reference asm extracted to `/tmp/ref_func_80048CEC.s`.

```c
typedef short s16;
typedef int s32;

s16 func_80048CEC(s32 i) {
    return (*(s16 **)0x1F800048)[i];
}
```

---

## func_800459A0 — 8 insns, per-function asm `nonmatchings/Code800459A0/func_800459A0.s`

**Bytecmp verdict: PASS** (0 hard mismatches, 0 reloc flags)

Stores `s16(-0x28)` at offset 0x2E, `s16(5)` at offset 0x34, and increments a `u8` at
offset 0x8 via `lbu/addiu/sb`. No existing header defines this struct; padding arrays are
used to hit the required offsets. Statement order follows store order (x2E first, then x34,
then x8 read-modify-write in the delay slot).

The `x8 = x8 + 1` form (not `x8++`) matches the confirmed pattern from the moji `m->x72`
family noted in lane_b_drafts.md.

Note: this function is `copies=2` — `func_8004676C` at 352A8.s+0x4C has byte-identical
shape and can use the same C body in a separate TU.

```c
typedef unsigned char u8;
typedef short s16;

typedef struct {
    u8 _0[0x8];
    u8 x8;
    u8 _9[0x2E - 0x9];
    s16 x2E;
    u8 _30[0x34 - 0x30];
    s16 x34;
} OBJ;

void func_800459A0(OBJ *o) {
    o->x2E = -0x28;
    o->x34 = 5;
    o->x8 = o->x8 + 1;
}
```

---

## Notes for landing

- `func_8002DE04` and `func_80048CEC` live in bulk `.s` files; explicit `REFERENCE.s`
  argument is required for `bytecmp.sh` (refs at `/tmp/ref_func_8002DE04.s` and
  `/tmp/ref_func_80048CEC.s`).
- `func_800459A0` has a per-function nonmatchings file; `bytecmp.sh` auto-discovers it.
- The OBJ struct for `func_800459A0` / `func_8004676C` is unidentified in any existing
  header. Either add it to a new or existing header before landing, or inline the padded
  typedef in the TU. Do NOT add it to `obj.h` without confirming the fields against the
  full object layout.
- `func_8002DE04` sits in `1DB20.s` near `func_8002DDDC` and `func_8002DE20`. The asm
  block after `func_8002DE04` is `func_8002DE20` — it is NOT included in the extracted
  reference slice.
