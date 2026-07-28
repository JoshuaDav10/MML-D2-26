#include "common.h"

extern void (*D_80116F44[])(void);
extern void (*D_80117054[])(void);
extern void (*D_80116EA8[])(void);
extern void (*D_8011700C[])(void);
extern void (*D_80116E58[])(void);
extern void (*D_80117028[])(void);
extern void (*D_80116FA0[])(void);
extern void (*D_80117040[])(void);
extern void (*D_8011701C[])(void);

/* ST03 ovl0, high-duplicate cluster (64 functions, no jump tables).
 * These bodies recur across the whole game -- func_80107F3C alone has 144
 * copies in 33 level files -- so the C here is written to be copy-pasted.
 *
 * Notes that cost iterations, keep them:
 *  - PosXYZ being 2-aligned is load-bearing: it is what makes `w->pos =
 *    w->homePos` compile to the lwl/lwr + swl/swr pair instead of lw/sw.
 *  - lh vs lhu here is NOT a signedness signal on the field. Every 16-bit field
 *    below is s16; cc1-27 emits lhu when the value is only consumed by
 *    truncating arithmetic and lh when the full value matters. `angle` appears
 *    as both.
 *  - A 16-bit field tested after arithmetic emits `sll 16` when s16 and
 *    `andi 0xffff` when u16 (func_801083DC).
 *
 * Mirror of this file lives at src/ST03B/... (identical code chunk). Keep in sync.
 */
typedef struct PosXYZ {
    s16 x;
    s16 y;
    s16 z;
    s16 unk6;
} PosXYZ;

typedef struct WORK {
    u8      pad0[0xA];
    u8      xA;                  /* 0x00A routine/step byte */
    u8      padB[0xE - 0xB];
    u8      xE;                  /* 0x00E swing amplitude (units of 8) */
    u8      padF[0x14 - 0xF];
    PosXYZ  pos;                 /* 0x014 */
    s32     posFxX;              /* 0x01C 16.16 fixed-point position */
    s32     posFxY;              /* 0x020 */
    s32     posFxZ;              /* 0x024 */
    u8      pad28[0x46 - 0x28];
    s16     x46;                 /* 0x046 */
    s16     x48;                 /* 0x048 */
    u8      pad4A[0x56 - 0x4A];
    s16     angle;               /* 0x056 */
    u8      pad58[0x64 - 0x58];
    s16     targetAngle;         /* 0x064 */
    u8      pad66[0xA7 - 0x66];
    s8      xA7;                 /* 0x0A7 signed state flag */
    u8      padA8[0x350 - 0xA8];
    s16     x350;                /* 0x350 countdown timer / turn rate */
    s16     homeAngle;           /* 0x352 */
    PosXYZ  homePos;             /* 0x354 */
    s16     x35C;                /* 0x35C direction flag */
    s16     x35E;                /* 0x35E swing counter */
    s16     x360;                /* 0x360 */
    s16     x362;                /* 0x362 second direction flag */
    s16     x364;                /* 0x364 */
    s16     x366;                /* 0x366 swing limit / target step */
} WORK;

void func_80031824(void);
s16  func_80031D5C(s16 target, s16 current, s16 step);

/* A second object genus in this cluster: actor/NPC handlers that talk to the
 * player. Kept as its own typedef because its 0x14 block is three u16 world
 * coords (not WORK's PosXYZ) and its 0x56 angle is u16 (not s16) -- and
 * signedness here is load-bearing, so merging the two would break both. */
typedef struct ACTOR_WORK {
    u8      pad0[0x9];
    u8      x9;                  /* 0x009 routine */
    u8      padA[0xC - 0xA];
    u8      xC;                  /* 0x00C */
    u8      xD;                  /* 0x00D next-routine; bit 7 = force */
    u8      padE[0x14 - 0xE];
    u16     x14;                 /* 0x014 world X */
    u16     x16;                 /* 0x016 */
    u16     x18;                 /* 0x018 world Z */
    u8      pad1A[0x56 - 0x1A];
    u16     x56;                 /* 0x056 facing angle (12-bit) */
    u8      pad58[0xAC - 0x58];
    u8      xAC;                 /* 0x0AC requested action id */
    u8      padAD[0x348 - 0xAD];
    u16     x348;                /* 0x348 flag bits */
    u8      x34A;                /* 0x34A action id to run on arrival */
    u8      x34B;                /* 0x34B subtask refcount */
    s16     x34C;                /* 0x34C target angle */
    s16     x34E;                /* 0x34E turn rate */
} ACTOR_WORK;

typedef struct PL_WORK2 {
    u8      x0;                  /* 0x000 status flag bits */
    u8      pad1[0x9 - 0x1];
    u8      x9;                  /* 0x009 routine */
    u8      xA;                  /* 0x00A sub-routine */
    u8      padB[0x14 - 0xB];
    u16     x14;                 /* 0x014 world X */
    u16     x16;
    u16     x18;                 /* 0x018 world Z */
} PL_WORK2;
extern PL_WORK2 Player_work;

typedef struct GAME_WORK2 {
    s8      routine_0;
} GAME_WORK2;
extern GAME_WORK2 Game_work;

typedef struct OFS {
    s16 x0;
    s16 x2;                      /* X offset */
    s16 x4;
    s16 x6;
    s16 x8;
    s16 xA;                      /* Z offset */
    s16 xC;
    s16 xE;
} OFS;

/* Aggregate spelling is load-bearing: as a scalar, GCC 2.7 proves it cannot
 * alias a struct store and hoists the load above it. See LESSONS. */
extern s8 D_800987B0[];

s32  func_8003A13C(void);
s32  func_80063BC8(void *, s32, s32, s32);
s16  func_80031BEC(void *, void *, s16, s16);
s16  func_80031D5C(s16, s16, s16);
void func_80030058(OFS *, s16, s16, s32, s32);
s32  MojiTaskKill();
s32  MojiTaskExec2(s32, s32);

void func_80107F3C(void) {
    func_80031824();
}

void func_80107F5C(u8 *o) {
    D_80116E58[o[0xA]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80107F98);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108008);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010807C);

void func_801080E4(WORK *p) {
    if (p->xA7 < 0) {
        p->xA = 0;
    }
}

void func_80108100(u8 *o) {
    D_80116EA8[o[0xA]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010813C);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108194);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801082F8);

void func_801083DC(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

void func_80108404(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = 1;
        w->x35C = 1 - w->x35C;
        w->x35E = w->x366 - w->x35E;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108480);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801084FC);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108560);

void func_80108648(WORK *w) {
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

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108704);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801087A8);

void func_801087EC(u8 *o) {
    D_80116F44[o[0xA]]();
}

void func_80108828(WORK *w) {
    w->x35C = 0;
    w->x35E = 0;
    w->x362 = 0;
    w->x46 = 0;
    w->x48 = 0;
    w->x366 = w->xE * 8;
    w->x360 = w->x366;
    w->xA++;
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010885C);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801089C8);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108B9C);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108CCC);

void func_80108CF4(WORK *w) {
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

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108D9C);

void func_80108E28(u8 *o) {
    D_80116FA0[o[0xA]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108E64);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108EC0);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801090E8);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109288);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801092B0);

void func_80109358(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = w->x366;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801093BC);

void func_8010940C(u8 *o) {
    D_8011700C[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109448);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109534);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109608);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801096AC);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109730);

void func_801097F4(u8 *o) {
    D_8011701C[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109830);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010991C);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801099F0);

void func_80109AB4(u8 *o) {
    D_80117028[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109AF0);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109C44);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109C94);

void func_80109CF8(u8 *o) {
    D_80117040[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109D34);

void func_80109E94(ACTOR_WORK *work) {
    PL_WORK2 *pl = &Player_work;

    if (func_8003A13C() == 0) {
        func_80063BC8(&work->x14, 0xD, 0, work->xC);
        work->x34B++;
    }
    work->x56 = (work->x56 + func_80031BEC(&work->x14, &pl->x14, work->x56, work->x34E)) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109F1C);

void func_80109FD4(ACTOR_WORK *work) {
    if (Game_work.routine_0 == 3) {
        MojiTaskKill();
        MojiTaskExec2(0, 0xDC);
        work->x34B--;
    }
}

void func_8010A02C(ACTOR_WORK *work) {
    PL_WORK2 *pl = &Player_work;
    s16 turn = func_80031D5C(work->x34C, work->x56, work->x34E);

    if ((turn == 0) || (work->xD & 0x80)) {
        if (func_8003A13C() == 0) {
            pl->x0 |= 2;
            Player_work.x9 = 0;
            Player_work.xA = 0;
            work->xAC = work->x34A;
            work->x348 |= 2;
            work->x9 = work->xD & 7;
        }
    }
    work->x56 = (work->x56 + turn) & 0xFFF;
}

void func_8010A100(u8 *o) {
    D_80117054[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A13C);

void func_8010A224(ACTOR_WORK *work) {
    work->x56 = (work->x56 + func_80031BEC(&work->x14, &Player_work.x14, work->x56, work->x34E)) & 0xFFF;
    if (D_800987B0[0] == 0) {
        work->xAC = 9;
        work->x348 &= ~4;
        work->x34B++;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A2A4);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A344);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A3AC);

s32 func_8010A44C(u8 a) {
    return a == 0x11;
}

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A45C);

INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A49C);

s32 func_8010A4EC(ACTOR_WORK *work, s16 arg1, s32 range) {
    OFS ofs;
    s32 ox;
    s32 oz;
    s32 dx;
    s32 dz;

    func_80030058(&ofs, work->x56, arg1 << 4, 0, 0);
    ox = ofs.x2;
    oz = ofs.xA;
    dx = range + (work->x14 + ox - Player_work.x14);
    dz = range + (work->x18 + oz - Player_work.x18);
    if ((u16)dx < (u16)range * 2) {
        if ((u16)dz < (u16)range * 2) {
            return 1;
        }
    }
    return 0;
}
