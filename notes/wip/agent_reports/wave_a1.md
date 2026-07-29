# wave_a1 deliverables

All three functions are confirmed `jr $ra; nop` (2 instructions). Each is an
empty function — no struct fields or externs needed beyond `#include "common.h"`.

---

## func_800394A4

asm: `asm/rock_neo/29CA4.s` (standalone `asm` segment, no parent .c owns it)

```c
void func_800394A4(void) {
}
```

bytecmp: **PASS** — 2/2 words, 0 hard mismatches, 0 reloc flags

---

## func_80044934

asm: `asm/rock_neo/35134.s` (standalone `asm` segment, no parent .c owns it)

```c
void func_80044934(void) {
}
```

bytecmp: **PASS** — 2/2 words, 0 hard mismatches, 0 reloc flags

---

## func_80044AA8

asm: `asm/rock_neo/352A8.s` (standalone `asm` segment, no parent .c owns it)

```c
void func_80044AA8(void) {
}
```

bytecmp: **PASS** — 2/2 words, 0 hard mismatches, 0 reloc flags

---

## Notes

- All three are in standalone `asm` splat segments; no existing .c file wraps
  them. Each needs its own new .c TU (e.g. `src/rock_neo/Code800394A4.c`,
  `src/rock_neo/Code80044934.c`, `src/rock_neo/Code80044AA8.c`) and a
  corresponding splat yaml change to switch `[0x29CA4, asm]` → `[0x29CA4, c,
  Code800394A4]`, likewise for the other two offsets.
- The reference .s lines I extracted had only 2 instruction words each —
  bytecmp saw no ambiguity.
