#include "common.h"

extern void (*D_8010D414[])(void);
extern void (*D_8010D444[])(void);
extern void (*D_8010D494[])(void);
extern void (*D_8010D530[])(void);
extern void (*D_8010D58C[])(void);
void func_8001DDE4(s32);

extern void (*D_8010D608[])(void);
extern void (*D_8010D640[])(void);
extern void (*D_8010D62C[])(void);
extern void (*D_8010D82C[])(void);
extern void (*D_8010D614[])(void);
extern void (*D_8010D808[])(void);
extern void (*D_8010D81C[])(void);
extern void (*D_8010D5F8[])(void);
extern void (*D_8010D6FC[])(void);

/* ST0C ovl0 (== ST0CB == ST0CC, sha1-verified over the whole code chunk).
 * Whole text is one C segment; the 34 bodies below were already solved in ST03
 * and are byte-identical here (the body hash is over raw instruction words, so
 * identical bodies imply identical call targets). Each one was still
 * re-verified against ST0C's OWN asm before landing -- 34/34.
 * Everything else is INCLUDE_ASM, including the jump-table users.
 */

typedef struct EVE_WORK {
    u8  pad0[0x4];
    u8  x4;       /* 0x04 - "already fired" latch, set to 0xFF */
} EVE_WORK;
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
    u8      padA8[0x328 - 0xA8];
    s16     x328;                /* 0x328 stepped/clamped angle output */
    u8      pad32A[0x350 - 0x32A];
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
typedef struct GAME_WORK2 {
    s8      routine_0;
} GAME_WORK2;
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
typedef struct ENEMY_WORK {
    u8      pad0[0x8];
    u8      routine;             /* 0x008 */
    u8      pad9[0xB - 0x9];
    u8      step;                /* 0x00B */
    s8      timer;               /* 0x00C frame counter */
    u8      padD[0x38 - 0xD];
    u8      name[0x8];           /* 0x038 identifier block (passed by address) */
    u8      pad40[0x46 - 0x40];
    s16     unk46;               /* 0x046 */
    u8      pad48[0x54 - 0x48];
    s16     speed;               /* 0x054 clamped to 0x400 */
    s16     angle;               /* 0x056 facing (12-bit) */
    u8      pad58[0x66 - 0x58];
    s16     targetAngle;         /* 0x066 */
} ENEMY_WORK;
typedef struct ENTITY_FUNCS {
    void (*init)(void *);
    void (*act)(void *);
} ENTITY_FUNCS;
typedef struct ENTITY_WORK {
    u8             pad0[0x8];
    u8             routine;      /* 0x008; 0xFF = dead */
    u8             pad9[0xA4 - 0x9];
    ENTITY_FUNCS  *funcs;        /* 0x0A4 */
} ENTITY_WORK;

typedef struct EFFECT_REQ {
    u8      x0;                  /* 0x000 effect kind */
    u8      pad1;
    u8      x2;                  /* 0x002 sub-kind / flags */
    u8      x3;                  /* 0x003 parameter */
    u8      pad4[0x1C - 0x4];
    s16     x1C;                 /* 0x01C X */
    s16     x1E;                 /* 0x01E Y (-1 = use default) */
    s16     x20;                 /* 0x020 Z */
    u8      pad22[0x38 - 0x22];
    PosXYZ  pos;                 /* 0x038 */
} EFFECT_REQ;

typedef struct SCE_TBL {
    u32  num;
    u8  *list;
} SCE_TBL;

void func_8001DF10(SCE_TBL *st);
extern SCE_TBL D_8010CB40;

/* Views of the actor struct used by the ST0C step/sweep handlers. Kept as
 * separate typedefs (rather than merged into WORK/ACTOR_WORK) because only the
 * OFFSETS matter and separate views cannot shift each other's fields. */
typedef struct SWEEP_WORK {
    u8  pad0[0x9];
    u8  x9;                      /* 0x009 routine */
    u8  padA[0xC - 0xA];
    s16 xC;                      /* 0x00C */
    u8  padE[0x22 - 0xE];
    u8  x22;                     /* 0x022 */
    u8  pad23[0x38 - 0x23];
    s16 x38;                     /* 0x038 */
    s16 x3A;                     /* 0x03A */
} SWEEP_WORK;

typedef struct LATCH_WORK {
    u8  pad0[0xD];
    s8  xD;                      /* 0x00D pending-next-routine */
    u8  padE[0x168 - 0xE];
    u8  x168;
    u8  x169;
    u8  pad16A[0x448 - 0x16A];
    u8  x448;                    /* 0x448 requested action */
} LATCH_WORK;

typedef struct FLAG_WORK {
    u8  pad0[0xFF];
    s8  xFF;
    u8  pad100[0x103 - 0x100];
    s8  x103;
    u8  pad104[0x107 - 0x104];
    s8  x107;
} FLAG_WORK;

extern SCE_TBL D_8010CB48;
void func_8003786C(s32 id);
extern u8 D_800C1B60;
extern s8 D_800C356C;
extern u8 D_800C356D;
extern u8 D_800C356E;
extern u8 D_800C356F;
extern u8 D_800C3570;
extern u8 D_800C3571;

u8  pad0[0x4];
void func_80103AB0(s32 idx);
void func_80101B70(void);
s32  Sce_flag_test(s32 id);
void Sce_flag_on(s32 id);
u8  *func_80031418(void);
void func_8001EB98(u8 *p);
extern u8 D_801151DC[];
s16 x;
s16 y;
s16 z;
s16 unk6;
u8      pad0[0xA];
u8      padB[0xE - 0xB];
u8      padF[0x14 - 0xF];
u8      pad28[0x46 - 0x28];
u8      pad4A[0x56 - 0x4A];
u8      pad58[0x64 - 0x58];
u8      pad66[0xA7 - 0x66];
u8      padA8[0x350 - 0xA8];
void func_80031824(void);
s16  func_80031D5C(s16 target, s16 current, s16 step);
u8      pad0[0x9];
u8      padA[0xC - 0xA];
u8      padE[0x14 - 0xE];
u8      pad1A[0x56 - 0x1A];
u8      pad58[0xAC - 0x58];
u8      padAD[0x348 - 0xAD];
u8      pad1[0x9 - 0x1];
u8      padB[0x14 - 0xB];
u16     x16;
extern PL_WORK2 Player_work;
s8      routine_0;
extern GAME_WORK2 Game_work;
s16 x0;
s16 x4;
s16 x6;
s16 x8;
s16 xC;
s16 xE;
extern s8 D_800987B0[];
s32  func_8003A13C(void);
s32  func_80063BC8(void *, s32, s32, s32);
s16  func_80031BEC(void *, void *, s16, s16);
s16  func_80031D5C(s16, s16, s16);
void func_80030058(OFS *, s16, s16, s32, s32);
s32  MojiTaskKill();
s32  MojiTaskExec2(s32, s32);
void func_800318D8(void);
void func_80031988(void);
u8      pad0[0x8];
u8      pad9[0xB - 0x9];
u8      padD[0x38 - 0xD];
u8      pad40[0x46 - 0x40];
u8      pad48[0x54 - 0x48];
u8      pad58[0x66 - 0x58];
extern s32  rand(void);
extern void func_80032538(ENEMY_WORK *);
extern void func_800334C4(ENEMY_WORK *);
extern s32  func_80048C60(void *);
extern u8   D_800B51C4[];
void (*init)(void *);
void (*act)(void *);
u8             pad0[0x8];
u8             pad9[0xA4 - 0x9];


INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80100BB0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80100C18);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80100DE8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80100E34);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80100ED4);

void func_80100F58(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_8001DF10(&D_8010CB40);
        evp->x4 = 0xFF;
    }
}

void func_80100FA4(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80100FAC);

void func_80100FF8(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80101000);

void func_801010AC(void) {
    if (Sce_flag_test(0x28)) {
        func_8001DDE4(0x18);
    }
}

void func_801010DC(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_8001DF10(&D_8010CB48);
        func_8003786C(0x19D);
        evp->x4 = 0xFF;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80101130);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80101184);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801011D8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010122C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80101280);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801012D4);

s32 func_80101330(void) {
    Sce_flag_on(0x1FF);
    return 0;
}

void func_80101350(u8 arg0) {
    D_800C356C = -1;
    D_800C356F = arg0;
    D_800C356D = 0;
    D_800C3571 = 0xFF;
    D_800C3570 = 0xFF;
    D_800C356E = D_800C1B60;
}

void func_80101398(LATCH_WORK *work) {
    if (work->xD == 0) {
        /* Store first: writing xD++ first reuses the cached lb and emits a
         * spurious frame; store-first forces a genuine lbu reload of xD and the
         * scheduler sinks the store into its load-delay slot. See LESSONS. */
        work->x448 = 0;
        work->xD++;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801013C0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010143C);

void func_8010148C(LATCH_WORK *work) {
    if (work->xD == 0) {
        work->x448 = 0x80;
        work->x168 = 0;
        work->x169 = 0;
        work->xD++;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801014BC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010151C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801015E0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801016DC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801017A0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010189C);

void func_80101968(ENTITY_WORK *work) {
    if (work->routine != 0xFF) {
        work->funcs->act(work);
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801019A8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80101CB8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80101D04);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80101E90);

void func_80102114(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80102134);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801021A0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80102370);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80102568);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801028DC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80102A60);

void func_80102CB8(u8 *o) {
    D_8010D414[o[0x8]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80102CF4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80102EB4);

void func_80103070(void) {
    func_80031824();
}

void func_80103090(u8 *o) {
    D_8010D444[o[0xA]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801030CC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010313C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801031B0);

void func_80103218(WORK *p) {
    if (p->xA7 < 0) {
        p->xA = 0;
    }
}

void func_80103234(u8 *o) {
    D_8010D494[o[0xA]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103270);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801032C8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010342C);

void func_80103510(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

void func_80103538(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = 1;
        w->x35C = 1 - w->x35C;
        w->x35E = w->x366 - w->x35E;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801035B4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103630);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103694);

void func_8010377C(WORK *w) {
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

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103838);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801038DC);

void func_80103920(u8 *o) {
    D_8010D530[o[0xA]]();
}

void func_8010395C(WORK *w) {
    w->x35C = 0;
    w->x35E = 0;
    w->x362 = 0;
    w->x46 = 0;
    w->x48 = 0;
    w->x366 = w->xE * 8;
    w->x360 = w->x366;
    w->xA++;
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103990);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103AFC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103CD0);

void func_80103E00(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

void func_80103E28(WORK *w) {
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

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103ED0);

void func_80103F5C(u8 *o) {
    D_8010D58C[o[0xA]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103F98);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80103FF4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010421C);

void func_801043BC(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801043E4);

void func_8010448C(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = w->x366;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801044F0);

void func_80104540(u8 *o) {
    D_8010D5F8[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010457C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104668);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010473C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801047E0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104864);

void func_80104928(u8 *o) {
    D_8010D608[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104964);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104A50);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104B24);

void func_80104BE8(u8 *o) {
    D_8010D614[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104C24);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104D78);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104DC8);

void func_80104E2C(u8 *o) {
    D_8010D62C[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80104E68);

void func_80104FC8(ACTOR_WORK *work) {
    PL_WORK2 *pl = &Player_work;

    if (func_8003A13C() == 0) {
        func_80063BC8(&work->x14, 0xD, 0, work->xC);
        work->x34B++;
    }
    work->x56 = (work->x56 + func_80031BEC(&work->x14, &pl->x14, work->x56, work->x34E)) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105050);

void func_80105108(ACTOR_WORK *work) {
    if (Game_work.routine_0 == 3) {
        MojiTaskKill();
        MojiTaskExec2(0, 0xDC);
        work->x34B--;
    }
}

void func_80105160(ACTOR_WORK *work) {
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

void func_80105234(u8 *o) {
    D_8010D640[o[0x34B]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105270);

void func_80105358(ACTOR_WORK *work) {
    work->x56 = (work->x56 + func_80031BEC(&work->x14, &Player_work.x14, work->x56, work->x34E)) & 0xFFF;
    if (D_800987B0[0] == 0) {
        work->xAC = 9;
        work->x348 &= ~4;
        work->x34B++;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801053D8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105478);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801054E0);

s32 func_80105580(u8 a) {
    return a == 0x11;
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105590);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801055D0);

s32 func_80105620(ACTOR_WORK *work, s16 arg1, s32 range) {
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

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801056C8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105794);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105868);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105CA8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105E10);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105E98);

void func_80105F60(FLAG_WORK *p, u8 f) {
    if (f) {
        p->x107 = 0;
    } else {
        p->x107 = -1;
    }
}

void func_80105F80(FLAG_WORK *p, u8 f) {
    if (f) {
        p->xFF = 0;
        p->x103 = 0;
    } else {
        p->xFF = -1;
        p->x103 = -1;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80105FA8);

void func_801060D8(ENEMY_WORK *work, s32 enable) {
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

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010614C);

void func_801061BC(ENEMY_WORK *work) {
    work->unk46 += 4;
    work->speed += 0x20;
    if (work->speed > 0x400) {
        work->speed = 0x400;
    }
    func_80032538(work);
    func_800334C4(work);
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80106220);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801062BC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80106350);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801063C8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80106478);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801065E4);

void func_801068D8(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801068F8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010695C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80106E8C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80106FAC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801072D0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801073A4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80107530);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80107640);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80107838);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80107A58);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80107C1C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80107D58);

void func_80107E64(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80107E84);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108028);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801082F4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801083C4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801084C8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108690);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801086E8);

void func_80108740(WORK *work, s16 want, s16 cur) {
    s32 diff;

    diff = want - cur;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff >= 0x21) {
        work->x328 = (cur < want) ? want + 0x20 : want - 0x20;
    }
}

void func_80108784(WORK *work, s16 want, s16 cur) {
    s32 diff;

    diff = want - cur;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff >= 0x21) {
        work->x328 = (cur < want) ? want - 0x20 : want + 0x20;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801087C8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801088B8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_801089B8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108A10);

void func_80108AF0(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108B10);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108BEC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108C40);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108CDC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80108EA0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109108);

void func_80109174(WORK *work, EFFECT_REQ *req) {
    req->x0 = 7;
    req->x2 = 0x80;
    req->x3 = 0x42;
    req->pos = work->pos;
}

void func_801091B4(WORK *work, EFFECT_REQ *req) {
    req->x0 = 3;
    req->x2 = 0x83;
    req->x3 = 0;
    /* Field order (x1C then x1E) is load-bearing even though the target STORES
     * x1E first -- the scheduler does that swap; source order decides which
     * register the lhu gets. See LESSONS. */
    req->x1C = work->pos.x;
    req->x1E = -1;
    req->x20 = work->pos.z;
}

void func_801091E4(u8 *o) {
    D_8010D6FC[o[0x3]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109220);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109368);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109480);

void func_80109648(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109668);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010979C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109814);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109974);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109B1C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_80109F28);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A048);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A0B4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A15C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A2A4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A35C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A4B4);

void func_8010A570(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A590);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A5E0);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010A9E8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010AB60);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010ACA4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010ADA4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010AE10);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010AF34);

void func_8010B008(void) {
    func_80031988();
}

void func_8010B028(u8 *o) {
    D_8010D808[o[0x8]]();
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B064);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B2F4);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B358);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B43C);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B490);

void func_8010B500(EVE_WORK *evp) {
}

void func_8010B508(SWEEP_WORK *work) {
    work->x38 += 2;
    work->x3A -= 1;
    if (work->x3A == -0x64) {
        work->x9++;
    }
}

void func_8010B54C(EVE_WORK *evp) {
}

void func_8010B554(SWEEP_WORK *work) {
    work->x38 -= 2;
    if (++work->x3A == 0) {
        work->x9 = 0;
    }
}

void func_8010B580(u8 *o) {
    D_8010D81C[o[0x9]]();
}

void func_8010B5BC(EVE_WORK *evp) {
}

void func_8010B5C4(SWEEP_WORK *work) {
    work->x38 -= 2;
    work->x3A += 1;
    if (work->x3A == 0x64) {
        work->x9++;
    }
}

void func_8010B608(EVE_WORK *evp) {
}

void func_8010B610(SWEEP_WORK *work) {
    work->x38 += 2;
    work->x3A -= 1;
    if (work->x38 == 0) {
        work->x9 = 0;
    }
}

void func_8010B640(u8 *o) {
    D_8010D82C[o[0x9]]();
}

void func_8010B67C(EVE_WORK *evp) {
}

void func_8010B684(SWEEP_WORK *work) {
    work->xC += 0x11;
}

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B698);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B6D8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B7EC);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010B838);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010BAE8);

INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010BBD4);

/* PARKED: the C form needs a symbol reference (lui %hi + lhu %lo folded) to
 * reproduce the target's 7 words, but ST0C ovl0's linker inputs do not define
 * System_timer (0x1F800002) -- the original baked the address in as a literal,
 * so splat never added it to undefined_syms_auto. A raw-address deref builds the
 * address in a register instead and costs a word (8 vs 7). Needs a symbol added
 * to this chunk's linker inputs; not worth destabilising the build for one fn. */
INCLUDE_ASM("config/overlay/splat.us.ST0CB/../../../asm/ST0CB/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0", func_8010BE64);
