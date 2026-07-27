#include "common.h"
#include "rock_neo/game.h"

/* --- decls: parallel grind wave 2, 2026-07-26 --- */
/* Double-buffered 0x1000-entry reverse ordering tables (2 * 0x4000 bytes).
   func_8001246C does ClearOTagR(D_800A4D40 + (buf << 14), 0x1000) on each. */
extern u32 D_800A4D40[2][0x1000];

extern u8 D_800979DC[];

extern u8 D_800979EC[];

void func_800175D8(void);


void func_80016FD4(void);

void func_80038370(void*, void*);

void func_800175FC(void);

void func_80028DE0(void);

/* PSY-Q's P_TAG is {unsigned addr:24; unsigned len:8;}, but cc1-27 allocates
   bit-fields MSB-first, so that spelling puts addr in bits 31..8 and reads the
   companion byte from offset 3 (lbu/srl/sll/or). Reversing the two fields is
   what lands addr in bits 23..0 (the real GPU packet layout) and is what emits
   the lw / and 0xFF000000 / and 0x00FFFFFF / or / sw tail this function needs.
   include/psxsdk/libgpu.h's P_TAG (and its getaddr/setaddr/addPrims) still has
   the unreversed order, so it is deliberately not used here. */
typedef struct {
    unsigned len : 8;
    unsigned addr : 24;
} OT_TAG;

#define OT_GETADDR(p) ((unsigned long)((OT_TAG*)(p))->addr)
#define OT_SETADDR(p, v) (((OT_TAG*)(p))->addr = (unsigned long)(v))
#define OT_ADDPRIMS(ot, p0, p1) (OT_SETADDR(p1, OT_GETADDR(ot)), OT_SETADDR(ot, p0))
#define OT_LENGTH 0x1000

extern s32 D_80098810;

extern s32 D_80098864;

extern u8 D_800808A4[];

extern u8 D_800808A8[];

extern u8 D_800808D0[];

void func_8002E7A8(s32);

void func_8003697C(void);

void func_8002CFA4(void);

void func_800371A0(u8*, u8*);

void func_80015428(GAME_WORK*);

// clang-format off

// game.c views Code800133D8_work through CODE_800133D8_WORK (s16 x0); here it
// is zeroed as three consecutive words, so a local array view is used instead.
extern s32 Code800133D8_work[3];
void func_80012F78(void (*)());
void func_80013420();

void func_800133D8(void) {
    Code800133D8_work[0] = 0;
    Code800133D8_work[1] = 0;
    Code800133D8_work[2] = 0;
    func_80012F78(func_80013420);
}

void func_80013418(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013420);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013578);

void func_800135DC(GAME_WORK* arg0) {
    switch (arg0->x1) {
    case 0:
        if (D_80098A72 == 0) {
            D_80098810 = 1;
            D_80098864 = 0;
            func_8002E7A8(1);
            func_8003697C();
            func_8002CFA4();
            func_800371A0(D_800808D0, D_800808A8);
            *(u8**)0x1F800058 = D_800808A4;
            arg0->x1++;
        }
        break;
    case 1:
        D_80098810 = 0;
        arg0->xA = 0;
        func_80015428(arg0);
        func_80017C30(2);
        arg0->routine_0 = 2;
        arg0->x1 = 1;
        break;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_800136B8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_800137D4);

void func_80013890(u8 *arg0) {
    arg0[0] = 0;
    arg0[1] = 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_8001389C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013CA0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013E0C);

void func_80013F60(GAME_WORK* arg0) {
    if (D_80098A72 == 0) {
        arg0->x1++;
    }
}

void func_80013F8C(GAME_WORK* arg0) {
    if (D_80098A72 == 0) {
        arg0->routine_0++;
        arg0->x1 = 0;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013FB8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80014394);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_8001456C);
