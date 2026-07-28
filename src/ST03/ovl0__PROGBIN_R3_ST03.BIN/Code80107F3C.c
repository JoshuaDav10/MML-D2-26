#include "common.h"

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

void func_80107F3C(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80107F5C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80107F98);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108008);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010807C);

void func_801080E4(WORK *p) {
    if (p->xA7 < 0) {
        p->xA = 0;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108100);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010813C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108194);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801082F8);

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

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108480);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801084FC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108560);

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

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108704);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801087A8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801087EC);

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

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010885C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801089C8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108B9C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108CCC);

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

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108D9C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108E28);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108E64);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108EC0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801090E8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109288);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801092B0);

void func_80109358(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = w->x366;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801093BC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010940C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109448);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109534);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109608);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801096AC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109730);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801097F4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109830);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010991C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801099F0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109AB4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109AF0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109C44);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109C94);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109CF8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109D34);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109E94);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109F1C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109FD4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A02C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A100);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A13C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A224);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A2A4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A344);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A3AC);

s32 func_8010A44C(u8 a) {
    return a == 0x11;
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A45C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A49C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A4EC);
