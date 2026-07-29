# Wave A4 — func_8005C23C, func_8005EBB4, func_8005EBD4

All three are 4-instruction global setter/getter bodies.
Addressing note: all symbols use `lui/%lo` (never `%gp_rel`) in the target asm,
so they require array-style externs (`extern T SYM[]`) to force lui/$at addressing.

---

## func_8005C23C

ASM: `asm/rock_neo/4CA3C.s`  
`sw $a0` → D_800989EC via lui/%lo → word store → s32 setter.

```c
extern s32 D_800989EC[];

void func_8005C23C(s32 arg0) {
    D_800989EC[0] = arg0;
}
```

**bytecmp verdict: PASS** — 0 hard mismatches, 2 reloc flags (%hi/%lo, expected).

---

## func_8005EBB4

ASM: `asm/rock_neo/4F3B4.s`  
`sh $a0` → D_800BBD96 via lui/%lo → halfword store → s16 setter.

Note: `Code8005EBA4.c` (the adjacent TU) declares `extern s16 D_800BBD96;` (scalar,
gp-relative). This TU is separate; the array declaration here is correct for the
lui/$at addressing the target uses.

```c
extern s16 D_800BBD96[];

void func_8005EBB4(s16 arg0) {
    D_800BBD96[0] = arg0;
}
```

**bytecmp verdict: PASS** — 0 hard mismatches, 2 reloc flags (%hi/%lo, expected).

---

## func_8005EBD4

ASM: `asm/rock_neo/4F3D4.s` (first 4 instructions only — file also contains
func_8005EBE4 which is a separate nonmatching stub).  
`lh $v0` ← D_800BBD96 via lui/%lo → signed halfword load → s16 getter.

Bytecmp was run with a trimmed reference covering only the 4 instructions of
func_8005EBD4 (not the appended func_8005EBE4 body in the same .s file).

```c
extern s16 D_800BBD96[];

s16 func_8005EBD4(void) {
    return D_800BBD96[0];
}
```

**bytecmp verdict: PASS** — 0 hard mismatches, 2 reloc flags (%hi/%lo, expected).
