# ST11 Batch 1 Transplant Results

All 7 functions verified with `tools/bytecmp.sh` (exit 0, 0 hard mismatches).
Reloc flags on jal targets are expected — linker-resolved, not code mismatches.

---

## func_801089AC — PASS (exit 0, 0 hard, 0 reloc)
Source: `func_8010BD80` (ST10B) — empty EVE_WORK callback.
ASM: 2 insn (`jr $ra; nop`).

```c
#include "common.h"

typedef struct EVE_WORK {
    u8  pad0[0x4];
    u8  x4;
} EVE_WORK;

void func_801089AC(EVE_WORK *evp) {
}
```

---

## func_801089B4 — PASS (exit 0, 0 hard, 0 reloc)
Source: `func_8010BD80` (ST10B) — same empty EVE_WORK callback, different address.
ASM: 2 insn (`jr $ra; nop`).

```c
#include "common.h"

typedef struct EVE_WORK {
    u8  pad0[0x4];
    u8  x4;
} EVE_WORK;

void func_801089B4(EVE_WORK *evp) {
}
```

---

## func_80103EBC — PASS (exit 0, 0 hard, 0 reloc)
Source: `func_80104C00` (ST10B) — `return a == 0x11` equality test.
ASM: 4 insn (`andi; xori 0x11; sltiu 1; jr $ra`).

```c
#include "common.h"

s32 func_80103EBC(u8 a) {
    return a == 0x11;
}
```

---

## func_80101B54 — PASS (exit 0, 0 hard, 0 reloc)
Source: `func_80102898` (ST10B) — check `xA7 < 0`, clear `xA`.
ASM: 7 insn (`lb; bgez; sb zero,0xA(a0); jr $ra`).
Note: the WORK struct here only needs to reach offset 0xA7 — used a minimal
pad layout. The struct definition below is the minimum needed to compile correctly.

```c
#include "common.h"

typedef struct PosXYZ {
    s16 x;
    s16 y;
    s16 z;
    s16 unk6;
} PosXYZ;
typedef struct WORK {
    u8      pad0[0xA];
    u8      xA;
    u8      padB[0xE - 0xB];
    u8      xE;
    u8      padF[0x14 - 0xF];
    PosXYZ  pos;
    s32     posFxX;
    s32     posFxY;
    s32     posFxZ;
    u8      pad28[0xA7 - 0x28];
    s8      xA7;
} WORK;

void func_80101B54(WORK *p) {
    if (p->xA7 < 0) {
        p->xA = 0;
    }
}
```

---

## func_80100A50 — PASS (exit 0, 0 hard, 1 reloc)
Source: `func_80108C48` (ST10B) — thin wrapper calling `func_80031824`.
ASM: 8 insn (standard jal wrapper). Reloc flag on `jal func_80031824` is expected.

```c
#include "common.h"

void func_80031824(void);

void func_80100A50(void) {
    func_80031824();
}
```

---

## func_801019AC — PASS (exit 0, 0 hard, 1 reloc)
Source: `func_80108C48` (ST10B) — same thin wrapper, different address.
ASM: 8 insn. Reloc flag on `jal func_80031824` is expected.

```c
#include "common.h"

void func_80031824(void);

void func_801019AC(void) {
    func_80031824();
}
```

---

## func_80105214 — PASS (exit 0, 0 hard, 1 reloc)
Source: `func_80108C48` (ST10B) — same thin wrapper, different address.
ASM: 8 insn. Reloc flag on `jal func_80031824` is expected.

```c
#include "common.h"

void func_80031824(void);

void func_80105214(void) {
    func_80031824();
}
```

---

## Summary

| Target          | Insns | Source       | bytecmp | Hard | Reloc |
|-----------------|-------|--------------|---------|------|-------|
| func_801089AC   | 2     | func_8010BD80| PASS    | 0    | 0     |
| func_801089B4   | 2     | func_8010BD80| PASS    | 0    | 0     |
| func_80103EBC   | 4     | func_80104C00| PASS    | 0    | 0     |
| func_80101B54   | 7     | func_80102898| PASS    | 0    | 0     |
| func_80100A50   | 8     | func_80108C48| PASS    | 0    | 1(jal)|
| func_801019AC   | 8     | func_80108C48| PASS    | 0    | 1(jal)|
| func_80105214   | 8     | func_80108C48| PASS    | 0    | 1(jal)|

All 7 verified. Ready for transplant into the ST11 .c file.
