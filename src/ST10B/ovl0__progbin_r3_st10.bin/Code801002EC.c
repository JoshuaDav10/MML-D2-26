#include "common.h"

/* ST10 ovl0 (== ST10B, sha1-verified over the whole code chunk).
 * Whole text is one C segment. The bodies below were already solved in
 * ST03/ST0C and are byte-identical here; each was re-verified against
 * ST10's OWN asm before landing (42/42). Everything else is INCLUDE_ASM.
 */

typedef struct EVE_WORK {
    u8  pad0[0x4];
    u8  x4;       /* 0x04 - "already fired" latch, set to 0xFF */
} EVE_WORK;
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
typedef struct BOSS_WORK {
    u8      pad0[0x8];
    u8      routine;             /* 0x008 top-level state */
    u8      step;                /* 0x009 sub-state */
    u8      phase;               /* 0x00A intro/spawn phase counter */
    u8      padB[0x14 - 0xB];
    s16     pos[4];              /* 0x014 position (passed by address) */
    u8      pad1C[0x46 - 0x1C];
    s16     pitch;               /* 0x046 */
    u8      pad48[0x56 - 0x48];
    s16     angle;               /* 0x056 facing (12-bit) */
    u8      pad58[0x66 - 0x58];
    s16     targetPitch;         /* 0x066 */
    u8      pad68[0xAC - 0x68];
    u8      flags;               /* 0x0AC */
    u8      padAD[0x348 - 0xAD];
    u16     timer;               /* 0x348 frame counter (u16: andi+sltiu) */
    s16     targetAngle;         /* 0x34A */
    s16     wobble;              /* 0x34C odd 1..0x7F */
} BOSS_WORK;
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

void func_80103AB0(s32 idx);
void func_80101B70(void);
s32  Sce_flag_test(s32 id);
void Sce_flag_on(s32 id);
u8  *func_80031418(void);
void func_8001EB98(u8 *p);
extern u8 D_801151DC[];
extern void (*D_80116F44[])(void);
extern void (*D_80117054[])(void);
extern void (*D_80116EA8[])(void);
extern void (*D_8011700C[])(void);
extern void (*D_80116E58[])(void);
extern void (*D_80117028[])(void);
extern void (*D_80116FA0[])(void);
extern void (*D_80117040[])(void);
extern void (*D_8011701C[])(void);
void func_80031824(void);
s16  func_80031D5C(s16 target, s16 current, s16 step);
extern PL_WORK2 Player_work;
extern GAME_WORK2 Game_work;
extern s8 D_800987B0[];
s32  func_8003A13C(void);
s32  func_80063BC8(void *, s32, s32, s32);
s16  func_80031BEC(void *, void *, s16, s16);
s16  func_80031D5C(s16, s16, s16);
void func_80030058(OFS *, s16, s16, s32, s32);
s32  MojiTaskKill();
s32  MojiTaskExec2(s32, s32);
extern void (*D_80117104[])(void);
extern void (*D_80117140[])(void);
extern void (*D_80117460[])(void);
extern void (*D_8011734C[])(void);
extern void (*D_801170E0[])(void);
extern void (*D_801171D8[])(void);
void func_800318D8(void);
void func_80031988(void);
extern s32  rand(void);
extern void func_80032538(ENEMY_WORK *);
extern void func_800334C4(ENEMY_WORK *);
extern s32  func_80048C60(void *);
extern s32  Sce_flag_test(s32);
extern u8   D_800B51C4[];
extern void *func_80049C24(void);
extern void  func_80037BB4(BOSS_WORK *);
extern void  func_80037738(void *, s32);
extern void  Sce_flag_on(s32);
extern void  Sound_call2(s32, s16 *);
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
void func_8001DF10(SCE_TBL *st);
extern SCE_TBL D_8010CB40;
extern SCE_TBL D_8010CB48;
void func_8003786C(s32 id);
extern u8 D_800C1B60;
extern s8 D_800C356C;
extern u8 D_800C356D;
extern u8 D_800C356E;
extern u8 D_800C356F;
extern u8 D_800C3570;
extern u8 D_800C3571;

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801002EC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100354);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801006B0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801006FC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010079C);

void func_80100838(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100840);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100894);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801008E8);

void func_8010093C(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100944);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100990);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801009DC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100A28);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100A44);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100A90);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100ADC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100B38);

s32 func_80100B94(void) {
    Sce_flag_on(0x1FF);
    return 0;
}

s32 func_80100BB4(void) {
    Sce_flag_on(0x1FF);
    return 0;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100BD4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100C10);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100E38);

void func_80100E60(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80100E68);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80101068);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801010C8);

void func_80101178(ENTITY_WORK *work) {
    if (work->routine != 0xFF) {
        work->funcs->act(work);
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801011B8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80101338);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80101384);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80101510);

void func_80101794(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801017B4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80101820);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801019F0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80101BE8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80101F5C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801020E0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102338);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102374);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102534);

void func_801026F0(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102710);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010274C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801027BC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102830);

void func_80102898(WORK *p) {
    if (p->xA7 < 0) {
        p->xA = 0;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801028B4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801028F0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102948);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102AAC);

void func_80102B90(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

void func_80102BB8(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = 1;
        w->x35C = 1 - w->x35C;
        w->x35E = w->x366 - w->x35E;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102C34);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102CB0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102D14);

void func_80102DFC(WORK *w) {
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

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102EB8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102F5C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80102FA0);

void func_80102FDC(WORK *w) {
    w->x35C = 0;
    w->x35E = 0;
    w->x362 = 0;
    w->x46 = 0;
    w->x48 = 0;
    w->x366 = w->xE * 8;
    w->x360 = w->x366;
    w->xA++;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103010);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010317C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103350);

void func_80103480(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

void func_801034A8(WORK *w) {
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

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103550);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801035DC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103618);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103674);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010389C);

void func_80103A3C(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103A64);

void func_80103B0C(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = w->x366;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103B70);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103BC0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103BFC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103CE8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103DBC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103E60);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103EE4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103FA8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80103FE4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801040D0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801041A4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104268);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801042A4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801043F8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104448);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801044AC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801044E8);

void func_80104648(ACTOR_WORK *work) {
    PL_WORK2 *pl = &Player_work;

    if (func_8003A13C() == 0) {
        func_80063BC8(&work->x14, 0xD, 0, work->xC);
        work->x34B++;
    }
    work->x56 = (work->x56 + func_80031BEC(&work->x14, &pl->x14, work->x56, work->x34E)) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801046D0);

void func_80104788(ACTOR_WORK *work) {
    if (Game_work.routine_0 == 3) {
        MojiTaskKill();
        MojiTaskExec2(0, 0xDC);
        work->x34B--;
    }
}

void func_801047E0(ACTOR_WORK *work) {
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

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801048B4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801048F0);

void func_801049D8(ACTOR_WORK *work) {
    work->x56 = (work->x56 + func_80031BEC(&work->x14, &Player_work.x14, work->x56, work->x34E)) & 0xFFF;
    if (D_800987B0[0] == 0) {
        work->xAC = 9;
        work->x348 &= ~4;
        work->x34B++;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104A58);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104AF8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104B60);

s32 func_80104C00(u8 a) {
    return a == 0x11;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104C10);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104C50);

s32 func_80104CA0(ACTOR_WORK *work, s16 arg1, s32 range) {
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

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104D48);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104D84);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80104EC4);

void func_80105068(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105088);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801051A0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105360);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801053EC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801054B8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010558C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801059CC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105B34);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105BBC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105C84);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105CA4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105CCC);

void func_80105DFC(ENEMY_WORK *work, s32 enable) {
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

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105E70);

void func_80105EE0(ENEMY_WORK *work) {
    work->unk46 += 4;
    work->speed += 0x20;
    if (work->speed > 0x400) {
        work->speed = 0x400;
    }
    func_80032538(work);
    func_800334C4(work);
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105F44);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80105FE0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80106074);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801060EC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010619C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80106308);

void func_801065FC(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010661C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80106680);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80106BB0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80106CD0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80106FF4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801070C8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80107254);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80107364);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010755C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010777C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80107940);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80107A7C);

void func_80107B88(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80107BA8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80107D4C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80108018);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801080E8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801081EC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801083B4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010840C);

void func_80108464(WORK *work, s16 want, s16 cur) {
    s32 diff;

    diff = want - cur;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff >= 0x21) {
        work->x328 = (cur < want) ? want + 0x20 : want - 0x20;
    }
}

void func_801084A8(WORK *work, s16 want, s16 cur) {
    s32 diff;

    diff = want - cur;
    if (diff < 0) {
        diff = -diff;
    }
    if (diff >= 0x21) {
        work->x328 = (cur < want) ? want - 0x20 : want + 0x20;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801084EC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801085DC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801086DC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80108734);

void func_80108814(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80108834);

void func_80108910(BOSS_WORK *work) {
    work->wobble = (rand() & 0x7F) | 1;
    work->pitch = (rand() & 0xF) + 8;
    work->targetPitch = (rand() & 0xF) - 8;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80108964);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80108AC8);

void func_80108C48(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80108C68);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80108FC4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80109074);

void func_8010924C(BOSS_WORK *work) {
    work->routine = 1;
    work->step = 0;
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010925C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80109488);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801096CC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80109748);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80109800);

void func_8010988C(BOSS_WORK *work, s32 enable) {
    if (enable) {
        work->timer++;
        if (work->timer >= 0x3D) {
            work->timer = 0;
            /* Flat, left-to-right, constant in the MIDDLE (see LESSONS). */
            work->targetAngle = (work->angle + 0x7D0 + (rand() & 0x3F)) & 0xFFF;
            /* Must come LAST: placed first, the `li 0x14` is delay-slot-safe and
             * gets stolen into the guarding bnez, costing a word. */
            work->step = 0x14;
        }
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_801098FC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80109A6C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80109AA8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_80109E50);

void func_8010A2C8(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A2D0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A2F4);

void func_8010A324(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A32C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A380);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A3A0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A3C0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A468);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A518);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A584);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A664);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A700);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010A8C4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010AB2C);

void func_8010AB98(WORK *work, EFFECT_REQ *req) {
    req->x0 = 7;
    req->x2 = 0x80;
    req->x3 = 0x42;
    req->pos = work->pos;
}

void func_8010ABD8(WORK *work, EFFECT_REQ *req) {
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

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010AC08);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010ACA4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010AD74);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010AE98);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010AF4C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010AF9C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010B3A4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010B51C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010B660);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010B760);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010B79C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010BA54);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010BAC0);

void func_8010BBE8(ENEMY_WORK *work) {
    if (func_80048C60(work->name) == func_80048C60(D_800B51C4)) {
        if (!Sce_flag_test(0x7B1)) {
            work->routine++;
            work->step = 0;
        }
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010BC54);

void func_8010BD78(EVE_WORK *evp) {
}

void func_8010BD80(EVE_WORK *evp) {
}

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010BD88);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010BE48);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010BFA4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010C05C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010C098);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010C1E8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010C4F0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CA70);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CABC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CB24);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CBD0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CC10);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CC4C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CCFC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CDCC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CE48);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CE84);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010CF28);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D108);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D2AC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D384);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D410);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D608);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D690);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D758);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D818);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D8BC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010D8FC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010DAE8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010DB64);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010DBD0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010DC1C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010DECC);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010DFB8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E248);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E264);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E2D0);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E3C8);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E504);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E628);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E768);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E890);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010E940);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010EA6C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010EB28);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010EB64);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010EDF4);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010EE58);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010EF3C);

INCLUDE_ASM("config/overlay/splat.us.ST10B/../../../asm/ST10B/ovl0__progbin_r3_st10.bin/nonmatchings/Code801002EC", func_8010EF90);
