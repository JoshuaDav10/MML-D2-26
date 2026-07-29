#include "common.h"

/* ST11 ovl0 (== ST11B, sha1-verified over the whole code chunk).
 * Whole text is one C segment. Bodies below were already solved in ST10B and
 * are byte-identical here; each re-verified against ST11's OWN asm before
 * landing. Typedefs are ST10B's proven set, not re-derived -- cc1 silently
 * accepts an unknown struct member, so a re-derived layout can delete a body.
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


INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801000B4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010011C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100284);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801002D0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100370);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801003C4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100428);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010047C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801004D0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100524);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100578);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801005F4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100640);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801007CC);

void func_80100A50(void) {
    func_80031824();
}
INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100A70);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100ADC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100CAC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80100EA4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101218);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010139C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801015F4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101630);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801017F0);

void func_801019AC(void) {
    func_80031824();
}
INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801019CC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101A08);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101A78);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101AEC);

void func_80101B54(WORK *p) {
    if (p->xA7 < 0) {
        p->xA = 0;
    }
}
INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101B70);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101BAC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101C04);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101D68);

void func_80101E4C(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101E74);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101EF0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101F6C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80101FD0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801020B8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102174);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102218);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010225C);

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

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801022CC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102438);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010260C);

void func_8010273C(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102764);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010280C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102898);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801028D4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102930);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102B58);

void func_80102CF8(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102D20);

void func_80102DC8(WORK *w) {
    s16 turn = func_80031D5C(w->targetAngle, w->angle, 0x40);

    if (turn == 0) {
        w->xA = w->x366;
    }
    w->angle = (w->angle + turn) & 0xFFF;
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102E2C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102E7C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102EB8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80102FA4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103078);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010311C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801031A0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103264);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801032A0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010338C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103460);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103524);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103560);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801036B4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103704);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103768);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801037A4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103904);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010398C);

void func_80103A44(ACTOR_WORK *work) {
    if (Game_work.routine_0 == 3) {
        MojiTaskKill();
        MojiTaskExec2(0, 0xDC);
        work->x34B--;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103A9C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103B70);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103BAC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103C94);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103D14);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103DB4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103E1C);

s32 func_80103EBC(u8 a) {
    return a == 0x11;
}
INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103ECC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103F0C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80103F5C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104004);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801040D0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801041A4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801045E4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010474C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801047D4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010489C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801048BC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801048E4);

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

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104A88);

void func_80104AF8(ENEMY_WORK *work) {
    work->unk46 += 4;
    work->speed += 0x20;
    if (work->speed > 0x400) {
        work->speed = 0x400;
    }
    func_80032538(work);
    func_800334C4(work);
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104B5C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104BF8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104C8C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104D04);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104DB4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80104F20);

void func_80105214(void) {
    func_80031824();
}
INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80105234);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80105298);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801057C8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801058E8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80105C0C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80105CE0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80105E6C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80105F7C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106174);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106394);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106558);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106694);

void func_801067A0(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801067C0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106964);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106C30);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106D00);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106E04);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80106FCC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107024);

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

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107104);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801071A0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107364);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801075CC);

void func_80107638(WORK *work, EFFECT_REQ *req) {
    req->x0 = 7;
    req->x2 = 0x80;
    req->x3 = 0x42;
    req->pos = work->pos;
}

void func_80107678(WORK *work, EFFECT_REQ *req) {
    req->x0 = 3;
    req->x2 = 0x83;
    req->x3 = 0;
    req->x1C = work->pos.x;
    req->x1E = -1;
    req->x20 = work->pos.z;
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801076A8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801076F8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107B00);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107C78);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107DBC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107EBC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80107EF8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108188);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801081EC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801082D0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108324);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108394);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801083D0);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108688);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801086F4);

void func_8010881C(ENEMY_WORK *work) {
    if (func_80048C60(work->name) == func_80048C60(D_800B51C4)) {
        if (!Sce_flag_test(0x7B1)) {
            work->routine++;
            work->step = 0;
        }
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108888);

void func_801089AC(EVE_WORK *evp) {
}
void func_801089B4(EVE_WORK *evp) {
}
INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801089BC);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108A08);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108CB8);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80108DA4);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80109034);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_80109050);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_8010908C);

INCLUDE_ASM("config/overlay/splat.us.ST11/../../../asm/ST11/ovl0__progbin_r3_st11.bin/nonmatchings/Code801000B4", func_801090A8);
