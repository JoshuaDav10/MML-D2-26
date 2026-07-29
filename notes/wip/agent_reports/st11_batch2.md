# ST11 Batch 2 — Transplant Results

All 7 functions verified with `tools/bytecmp.sh`. Every one exits 0 (0 hard mismatches).

---

## func_801067A0  (8 insn) <- func_80108C48  PASS

```c
#include "common.h"

void func_80031824(void);

void func_801067A0(void) {
    func_80031824();
}
```

bytecmp: 0 hard mismatch(es), 1 reloc flag(s) (jal target — expected).

---

## func_80101E4C  (10 insn) <- func_80103A3C  PASS

```c
#include "common.h"

typedef struct WORK {
    u8  pad0[0xA];
    u8  xA;
    u8  padB[0x350 - 0xB];
    s16 x350;
} WORK;

void func_80101E4C(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}
```

bytecmp: 0 hard mismatch(es), 0 reloc flag(s).

---

## func_8010273C  (10 insn) <- func_80103A3C  PASS

```c
#include "common.h"

typedef struct WORK {
    u8  pad0[0xA];
    u8  xA;
    u8  padB[0x350 - 0xB];
    s16 x350;
} WORK;

void func_8010273C(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}
```

bytecmp: 0 hard mismatch(es), 0 reloc flag(s).

---

## func_80102CF8  (10 insn) <- func_80103A3C  PASS

```c
#include "common.h"

typedef struct WORK {
    u8  pad0[0xA];
    u8  xA;
    u8  padB[0x350 - 0xB];
    s16 x350;
} WORK;

void func_80102CF8(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}
```

bytecmp: 0 hard mismatch(es), 0 reloc flag(s).

---

## func_80107678  (12 insn) <- func_8010ABD8  PASS

The asm reads `0x14($a0)` into $v1 and `0x18($a0)` into $v0, matching
`work->pos.x` (offset 0x14) and `work->pos.z` (offset 0x18). The COMMENT in
the ST10B source about field ORDER (x1C before x1E is load-bearing) applies
here too — the scheduler stores x1E first in the output, but C source order
decides register allocation.

```c
#include "common.h"

typedef struct PosXYZ {
    s16 x;
    s16 y;
    s16 z;
    s16 unk6;
} PosXYZ;

typedef struct WORK {
    u8     pad0[0x14];
    PosXYZ pos;   /* 0x014: x=0x14, y=0x16, z=0x18 */
} WORK;

typedef struct EFFECT_REQ {
    u8  x0;
    u8  pad1;
    u8  x2;
    u8  x3;
    u8  pad4[0x1C - 0x4];
    s16 x1C;
    s16 x1E;
    s16 x20;
} EFFECT_REQ;

void func_80107678(WORK *work, EFFECT_REQ *req) {
    req->x0 = 3;
    req->x2 = 0x83;
    req->x3 = 0;
    req->x1C = work->pos.x;
    req->x1E = -1;
    req->x20 = work->pos.z;
}
```

bytecmp: 0 hard mismatch(es), 0 reloc flag(s).

---

## func_80102298  (13 insn) <- func_80102FDC  PASS

```c
#include "common.h"

typedef struct WORK {
    u8  pad0[0xA];
    u8  xA;
    u8  padB[0xE - 0xB];
    u8  xE;
    u8  padF[0x46 - 0xF];
    s16 x46;
    s16 x48;
    u8  pad4A[0x350 - 0x4A];
    s16 x350;
    u8  pad352[0x35C - 0x352];
    s16 x35C;
    s16 x35E;
    s16 x360;
    s16 x362;
    s16 x364;
    s16 x366;
} WORK;

void func_80102298(WORK *w) {
    w->x35C = 0;
    w->x35E = 0;
    w->x362 = 0;
    w->x46 = 0;
    w->x48 = 0;
    w->x366 = w->xE * 8;
    w->x360 = w->x366;
    w->xA++;
}
```

bytecmp: 0 hard mismatch(es), 0 reloc flag(s).

---

## func_80107638  (16 insn) <- func_8010AB98  PASS

The asm uses lwl/lwr + swl/swr pairs, which is the GCC 2.7.2 `struct =` copy idiom
for unaligned/odd-sized structs. `req->pos = work->pos` generates exactly this.
EFFECT_REQ.pos is at offset 0x38.

```c
#include "common.h"

typedef struct PosXYZ {
    s16 x;
    s16 y;
    s16 z;
    s16 unk6;
} PosXYZ;

typedef struct WORK {
    u8     pad0[0x14];
    PosXYZ pos;   /* 0x014 */
} WORK;

typedef struct EFFECT_REQ {
    u8     x0;
    u8     pad1;
    u8     x2;
    u8     x3;
    u8     pad4[0x38 - 0x4];
    PosXYZ pos;   /* 0x038 */
} EFFECT_REQ;

void func_80107638(WORK *work, EFFECT_REQ *req) {
    req->x0 = 7;
    req->x2 = 0x80;
    req->x3 = 0x42;
    req->pos = work->pos;
}
```

bytecmp: 0 hard mismatch(es), 0 reloc flag(s).

---

## Summary

| Target         | Source         | Insns | bytecmp result |
|----------------|----------------|-------|----------------|
| func_801067A0  | func_80108C48  | 8     | PASS (0 hard, 1 reloc) |
| func_80101E4C  | func_80103A3C  | 10    | PASS (0 hard, 0 reloc) |
| func_8010273C  | func_80103A3C  | 10    | PASS (0 hard, 0 reloc) |
| func_80102CF8  | func_80103A3C  | 10    | PASS (0 hard, 0 reloc) |
| func_80107678  | func_8010ABD8  | 12    | PASS (0 hard, 0 reloc) |
| func_80102298  | func_80102FDC  | 13    | PASS (0 hard, 0 reloc) |
| func_80107638  | func_8010AB98  | 16    | PASS (0 hard, 0 reloc) |

All 7 verified. No drafts — every one passed bytecmp before this report was written.
