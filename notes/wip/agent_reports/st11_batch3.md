# ST11 Batch 3 — Transplant results

All 7 functions verified with `tools/bytecmp.sh`. **0 hard mismatches on all 7.**
Reloc flags are jal/lui/%lo — link-time only, not instruction-encoding issues.

---

## func_8010707C (17 insn) — PASS (0 hard, 0 reloc)
Source: func_80108464 in ST10B. Straight rename, WORK.x328 field same offset.

```c
#include "common.h"

typedef struct WORK {
    u8  pad0[0x328];
    s16 x328;
} WORK;

void func_8010707C(WORK *work, s16 want, s16 cur) {
    s32 diff;

    diff = want - cur;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff >= 0x21) {
        work->x328 = (cur < want) ? want + 0x20 : want - 0x20;
    }
}
```

---

## func_801070C0 (17 insn) — PASS (0 hard, 0 reloc)
Source: func_801084A8 in ST10B. Same as above but ternary arms swapped.

```c
#include "common.h"

typedef struct WORK {
    u8  pad0[0x328];
    s16 x328;
} WORK;

void func_801070C0(WORK *work, s16 want, s16 cur) {
    s32 diff;

    diff = want - cur;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff >= 0x21) {
        work->x328 = (cur < want) ? want - 0x20 : want + 0x20;
    }
}
```

---

## func_80102DC8 (25 insn) — PASS (0 hard, 1 reloc)
Source: func_80103B0C in ST10B. Reloc: jal func_80031D5C (link-time).
ST11 asm uses identical field offsets: targetAngle=0x64, angle=0x56, xA=0x0A, x366=0x366.

```c
#include "common.h"

typedef struct WORK {
    u8  pad0[0xA];
    u8  xA;
    u8  padB[0x56 - 0xB];
    s16 angle;
    u8  pad58[0x64 - 0x58];
    s16 targetAngle;
    u8  pad66[0x366 - 0x66];
    s16 x366;
} WORK;

s16 func_80031D5C(s16 target, s16 current, s16 step);

void func_80102DC8(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = w->x366;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}
```

---

## func_80103A44 (22 insn) — PASS (0 hard, 4 reloc)
Source: func_80104788 in ST10B. Relocs: lui/lo Game_work, jal MojiTaskKill, jal MojiTaskExec2.

```c
#include "common.h"

typedef struct ACTOR_WORK {
    u8  pad0[0x34B];
    u8  x34B;
} ACTOR_WORK;

typedef struct GAME_WORK2 {
    s8 routine_0;
} GAME_WORK2;

extern GAME_WORK2 Game_work;
s32 MojiTaskKill();
s32 MojiTaskExec2(s32, s32);

void func_80103A44(ACTOR_WORK *work) {
    if (Game_work.routine_0 == 3) {
        MojiTaskKill();
        MojiTaskExec2(0, 0xDC);
        work->x34B--;
    }
}
```

---

## func_80104AF8 (25 insn) — PASS (0 hard, 2 reloc)
Source: func_80105EE0 in ST10B. Relocs: jal func_80032538, jal func_800334C4.
Field offsets unk46=0x46, speed=0x54 confirmed from ST11 asm.

```c
#include "common.h"

typedef struct ENEMY_WORK {
    u8  pad0[0x46];
    s16 unk46;
    u8  pad48[0x54 - 0x48];
    s16 speed;
} ENEMY_WORK;

extern void func_80032538(ENEMY_WORK *);
extern void func_800334C4(ENEMY_WORK *);

void func_80104AF8(ENEMY_WORK *work) {
    work->unk46 += 4;
    work->speed += 0x20;
    if (work->speed > 0x400) {
        work->speed = 0x400;
    }
    func_80032538(work);
    func_800334C4(work);
}
```

---

## func_8010881C (27 insn) — PASS (0 hard, 5 reloc)
Source: func_8010BBE8 in ST10B. Relocs: jal func_80048C60 (x2), lui/lo D_800B51C4, jal Sce_flag_test.
Field layout: name at 0x38, routine at 0x8, step at 0xB — identical in ST11 asm.

```c
#include "common.h"

typedef struct ENEMY_WORK {
    u8  pad0[0x8];
    u8  routine;
    u8  pad9[0xB - 0x9];
    u8  step;
    u8  pad38[0x38 - 0xC];
    u8  name[0x8];
} ENEMY_WORK;

extern s32  func_80048C60(void *);
extern s32  Sce_flag_test(s32);
extern u8   D_800B51C4[];

void func_8010881C(ENEMY_WORK *work) {
    if (func_80048C60(work->name) == func_80048C60(D_800B51C4)) {
        if (!Sce_flag_test(0x7B1)) {
            work->routine++;
            work->step = 0;
        }
    }
}
```

---

## func_80104A14 (29 insn) — PASS (0 hard, 1 reloc)
Source: func_80105DFC in ST10B. Reloc: jal rand.
Note: `work->routine = 8` is the last store in the block (critical for LESSONS flat-addiu
ordering); ST11 asm confirms: sb at 0x8 after the andi. Field layout identical.

```c
#include "common.h"

typedef struct ENEMY_WORK {
    u8  pad0[0x8];
    u8  routine;
    u8  pad9[0xB - 0x9];
    u8  step;
    s8  timer;
    u8  padD[0x56 - 0xD];
    s16 angle;
    u8  pad58[0x66 - 0x58];
    s16 targetAngle;
} ENEMY_WORK;

extern s32 rand(void);

void func_80104A14(ENEMY_WORK *work, s32 enable) {
    if (enable) {
        work->timer++;
        if (work->timer >= 0x3D) {
            work->timer = 0;
            /* The three summands must stay FLAT and left-to-right with the
             * constant in the middle -- parenthesising the rand term attaches
             * the addiu to the angle register instead. See LESSONS. */
            work->targetAngle = (work->angle + 0x7D0 + (rand() & 0x3F)) & 0xFFF;
            work->routine = 8;
        }
    }
}
```

---

## Summary

| Target | Source | Insn | Hard | Reloc | Verdict |
|---|---|---|---|---|---|
| func_8010707C | func_80108464 | 17 | 0 | 0 | PASS |
| func_801070C0 | func_801084A8 | 17 | 0 | 0 | PASS |
| func_80102DC8 | func_80103B0C | 25 | 0 | 1 | PASS |
| func_80103A44 | func_80104788 | 22 | 0 | 4 | PASS |
| func_80104AF8 | func_80105EE0 | 25 | 0 | 2 | PASS |
| func_8010881C | func_8010BBE8 | 27 | 0 | 5 | PASS |
| func_80104A14 | func_80105DFC | 29 | 0 | 1 | PASS |

All 7 verified. Zero hard mismatches. All reloc flags are jal/lui/%lo (link-time addresses,
not instruction encoding). Safe to land.
