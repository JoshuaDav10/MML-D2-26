# ST11 Batch 4 — transplant report

All 7 functions verified via `tools/bytecmp.sh`. Every function: **0 hard mismatches**.
Only RELOC flags (jal targets, %hi/%lo relocations) which resolve at link time — expected and harmless per project convention.

---

## func_80101E74 (31 insn) — PASS (exit 0, 0 hard, 1 reloc)
Source: func_80102BB8 in ST10B. Fields identical (targetAngle@0x64, angle@0x56, x35C@0x35C, x35E@0x35E, x366@0x366).

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
    u8      pad28[0x46 - 0x28];
    s16     x46;
    s16     x48;
    u8      pad4A[0x56 - 0x4A];
    s16     angle;
    u8      pad58[0x64 - 0x58];
    s16     targetAngle;
    u8      pad66[0xA7 - 0x66];
    s8      xA7;
    u8      padA8[0x328 - 0xA8];
    s16     x328;
    u8      pad32A[0x350 - 0x32A];
    s16     x350;
    s16     homeAngle;
    PosXYZ  homePos;
    s16     x35C;
    s16     x35E;
    s16     x360;
    s16     x362;
    s16     x364;
    s16     x366;
} WORK;

s16 func_80031D5C(s16 target, s16 current, s16 step);

void func_80101E74(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = 1;
        w->x35C = 1 - w->x35C;
        w->x35E = w->x366 - w->x35E;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}
```

---

## func_80103C94 (32 insn) — PASS (exit 0, 0 hard, 5 reloc)
Source: func_801049D8 in ST10B. Uses D_800B51C4 (= &Player_work.x14, accessed as a standalone symbol in this TU via lui/addiu pair). D_800987B0 same address.

```c
#include "common.h"

typedef struct ACTOR_WORK {
    u8      pad0[0x9];
    u8      x9;
    u8      padA[0xC - 0xA];
    u8      xC;
    u8      xD;
    u8      padE[0x14 - 0xE];
    u16     x14;
    u16     x16;
    u16     x18;
    u8      pad1A[0x56 - 0x1A];
    u16     x56;
    u8      pad58[0xAC - 0x58];
    u8      xAC;
    u8      padAD[0x348 - 0xAD];
    u16     x348;
    u8      x34A;
    u8      x34B;
    s16     x34C;
    s16     x34E;
} ACTOR_WORK;

extern u8 D_800B51C4[];
extern s8 D_800987B0[];
s16 func_80031BEC(void *, void *, s16, s16);

void func_80103C94(ACTOR_WORK *work) {
    work->x56 = (work->x56 + func_80031BEC(&work->x14, D_800B51C4, work->x56, work->x34E)) & 0xFFF;
    if (D_800987B0[0] == 0) {
        work->xAC = 9;
        work->x348 &= ~4;
        work->x34B++;
    }
}
```

---

## func_80103904 (34 insn) — PASS (exit 0, 0 hard, 5 reloc)
Source: func_80104648 in ST10B. Identical — Player_work resolves to same address in this overlay.

```c
#include "common.h"

typedef struct ACTOR_WORK {
    u8      pad0[0x9];
    u8      x9;
    u8      padA[0xC - 0xA];
    u8      xC;
    u8      xD;
    u8      padE[0x14 - 0xE];
    u16     x14;
    u16     x16;
    u16     x18;
    u8      pad1A[0x56 - 0x1A];
    u16     x56;
    u8      pad58[0xAC - 0x58];
    u8      xAC;
    u8      padAD[0x348 - 0xAD];
    u16     x348;
    u8      x34A;
    u8      x34B;
    s16     x34C;
    s16     x34E;
} ACTOR_WORK;
typedef struct PL_WORK2 {
    u8      x0;
    u8      pad1[0x9 - 0x1];
    u8      x9;
    u8      xA;
    u8      padB[0x14 - 0xB];
    u16     x14;
    u16     x16;
    u16     x18;
} PL_WORK2;

extern PL_WORK2 Player_work;
s32 func_8003A13C(void);
s32 func_80063BC8(void *, s32, s32, s32);
s16 func_80031BEC(void *, void *, s16, s16);

void func_80103904(ACTOR_WORK *work) {
    PL_WORK2 *pl = &Player_work;

    if (func_8003A13C() == 0) {
        func_80063BC8(&work->x14, 0xD, 0, work->xC);
        work->x34B++;
    }
    work->x56 = (work->x56 + func_80031BEC(&work->x14, &pl->x14, work->x56, work->x34E)) & 0xFFF;
}
```

---

## func_80102764 (42 insn) — PASS (exit 0, 0 hard, 1 reloc)
Source: func_801034A8 in ST10B. Identical field layout (x364@0x364, angle@0x56, x350@0x350, x35E@0x35E, x35C@0x35C, x362@0x362, x360@0x360, x366@0x366).

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
    u8      pad28[0x46 - 0x28];
    s16     x46;
    s16     x48;
    u8      pad4A[0x56 - 0x4A];
    s16     angle;
    u8      pad58[0x64 - 0x58];
    s16     targetAngle;
    u8      pad66[0xA7 - 0x66];
    s8      xA7;
    u8      padA8[0x328 - 0xA8];
    s16     x328;
    u8      pad32A[0x350 - 0x32A];
    s16     x350;
    s16     homeAngle;
    PosXYZ  homePos;
    s16     x35C;
    s16     x35E;
    s16     x360;
    s16     x362;
    s16     x364;
    s16     x366;
} WORK;

s16 func_80031D5C(s16 target, s16 current, s16 step);

void func_80102764(WORK *w) {
    s16 turn = func_80031D5C(w->x364, w->angle, w->x350);

    if (turn == 0) {
        if (w->x35E != 0) {
            w->x35C = 1 - w->x35C;
        }
        w->xA = 1;
        w->x362 = 1 - w->x362;
        w->x360 = w->x366 - w->x360 + 1;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}
```

---

## func_80103F5C (42 insn) — PASS (exit 0, 0 hard, 5 reloc)
Source: func_80104CA0 in ST10B. Uses D_800B51C4 and D_800B51C8 as u16 scalars (Player_work.x14 and .x18 accessed directly as global symbols in this TU — lui/lhu pairs, not via struct base).

```c
#include "common.h"

typedef struct OFS {
    s16 x0;
    s16 x2;
    s16 x4;
    s16 x6;
    s16 x8;
    s16 xA;
    s16 xC;
    s16 xE;
} OFS;
typedef struct ACTOR_WORK {
    u8      pad0[0x9];
    u8      x9;
    u8      padA[0xC - 0xA];
    u8      xC;
    u8      xD;
    u8      padE[0x14 - 0xE];
    u16     x14;
    u16     x16;
    u16     x18;
    u8      pad1A[0x56 - 0x1A];
    u16     x56;
    u8      pad58[0xAC - 0x58];
    u8      xAC;
    u8      padAD[0x348 - 0xAD];
    u16     x348;
    u8      x34A;
    u8      x34B;
    s16     x34C;
    s16     x34E;
} ACTOR_WORK;

extern u16 D_800B51C4;
extern u16 D_800B51C8;
void func_80030058(OFS *, s16, s16, s32, s32);

s32 func_80103F5C(ACTOR_WORK *work, s16 arg1, s32 range) {
    OFS ofs;
    s32 ox;
    s32 oz;
    s32 dx;
    s32 dz;

    func_80030058(&ofs, work->x56, arg1 << 4, 0, 0);
    ox = ofs.x2;
    oz = ofs.xA;
    dx = range + (work->x14 + ox - D_800B51C4);
    dz = range + (work->x18 + oz - D_800B51C8);
    if ((u16)dx < (u16)range * 2) {
        if ((u16)dz < (u16)range * 2) {
            return 1;
        }
    }
    return 0;
}
```

---

## func_801020B8 (47 insn) — PASS (exit 0, 0 hard, 1 reloc)
Source: func_80102DFC in ST10B. Identical — same WORK struct fields, same logic.

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
    u8      pad28[0x46 - 0x28];
    s16     x46;
    s16     x48;
    u8      pad4A[0x56 - 0x4A];
    s16     angle;
    u8      pad58[0x64 - 0x58];
    s16     targetAngle;
    u8      pad66[0xA7 - 0x66];
    s8      xA7;
    u8      padA8[0x328 - 0xA8];
    s16     x328;
    u8      pad32A[0x350 - 0x32A];
    s16     x350;
    s16     homeAngle;
    PosXYZ  homePos;
    s16     x35C;
    s16     x35E;
    s16     x360;
    s16     x362;
    s16     x364;
    s16     x366;
} WORK;

s16 func_80031D5C(s16 target, s16 current, s16 step);

void func_801020B8(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    w->angle = (w->angle + turn) & 0xFFF;
    if (turn == 0) {
        w->xA = 1;
        if (w->x35C != 0) {
            w->pos = w->homePos;
            w->posFxX = w->pos.x << 16;
            w->posFxY = w->pos.y << 16;
            w->posFxZ = w->pos.z << 16;
            w->angle = w->homeAngle;
        }
        w->x35C = 1 - w->x35C;
    }
}
```

---

## func_80103A9C (53 insn) — PASS (exit 0, 0 hard, 8 reloc)
Source: func_801047E0 in ST10B. Uses D_800B51B9 and D_800B51BA (Player_work.x9 and .xA stored as zero via lui/$at pairs). Player_work same address.

```c
#include "common.h"

typedef struct ACTOR_WORK {
    u8      pad0[0x9];
    u8      x9;
    u8      padA[0xC - 0xA];
    u8      xC;
    u8      xD;
    u8      padE[0x14 - 0xE];
    u16     x14;
    u16     x16;
    u16     x18;
    u8      pad1A[0x56 - 0x1A];
    u16     x56;
    u8      pad58[0xAC - 0x58];
    u8      xAC;
    u8      padAD[0x348 - 0xAD];
    u16     x348;
    u8      x34A;
    u8      x34B;
    s16     x34C;
    s16     x34E;
} ACTOR_WORK;
typedef struct PL_WORK2 {
    u8      x0;
    u8      pad1[0x9 - 0x1];
    u8      x9;
    u8      xA;
    u8      padB[0x14 - 0xB];
    u16     x14;
    u16     x16;
    u16     x18;
} PL_WORK2;

extern PL_WORK2 Player_work;
extern u8 D_800B51B9;
extern u8 D_800B51BA;
s32 func_8003A13C(void);
s16 func_80031D5C(s16, s16, s16);

void func_80103A9C(ACTOR_WORK *work) {
    PL_WORK2 *pl = &Player_work;
    s16 turn = func_80031D5C(work->x34C, work->x56, work->x34E);

    if ((turn == 0) || (work->xD & 0x80)) {
        if (func_8003A13C() == 0) {
            pl->x0 |= 2;
            D_800B51B9 = 0;
            D_800B51BA = 0;
            work->xAC = work->x34A;
            work->x348 |= 2;
            work->x9 = work->xD & 7;
        }
    }
    work->x56 = (work->x56 + turn) & 0xFFF;
}
```

---

## Summary

| Function | Insns | Source | Hard mismatches | Reloc flags | Verdict |
|---|---|---|---|---|---|
| func_80101E74 | 31 | func_80102BB8 | 0 | 1 | PASS |
| func_80103C94 | 32 | func_801049D8 | 0 | 5 | PASS |
| func_80103904 | 34 | func_80104648 | 0 | 5 | PASS |
| func_80102764 | 42 | func_801034A8 | 0 | 1 | PASS |
| func_80103F5C | 42 | func_80104CA0 | 0 | 5 | PASS |
| func_801020B8 | 47 | func_80102DFC | 0 | 1 | PASS |
| func_80103A9C | 53 | func_801047E0 | 0 | 8 | PASS |

All 7 verified. Reloc flags only — all resolve at link time.

### Key symbol notes
- `D_800B51C4` = `Player_work.x14` (Player_work at 0x800B51B0; +0x14 = 0x800B51C4). In func_80103C94 used as `u8[]` (address passed to func_80031BEC). In func_80103F5C used as `u16` (lhu direct load).
- `D_800B51C8` = `Player_work.x18` — same principle.
- `D_800B51B9`/`D_800B51BA` = `Player_work.x9`/`Player_work.xA` — cleared via lui/$at sb $zero pairs. Declared as standalone `extern u8` to get the lui-pair addressing.
- `D_800987B0` — same global in ST10B and ST11 (shared read-only data).
