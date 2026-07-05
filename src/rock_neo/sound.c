#include "common.h"

s32 func_80071220();
void SpuSetKey(s32, u32);

extern s32 D_800821F8[];
extern s32 *D_800AD15C[];
extern s16 D_80082274;
extern s16 D_800822D6;
extern s16 D_80082218;
s32 Sce_flag_test(s32);
extern s8 Game_work[]; // this TU only reads byte x53; full type in game.h

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_800198C0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019918);

void func_8001997C(void) {
    if (Game_work[0x53] == 1) {
        D_80082274 = 0x29;
    } else {
        D_80082274 = 0x108;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_800199A4);

extern s16 D_800822A6;

void func_800199F8(void) {
    s16 *p;
    s32 i;
    s16 v = (Game_work[0x53] == 1) ? 0x17 : 0x108;
    i = 5;
    p = &D_800822A6;
    for (; i >= 0; i--) {
        *p-- = v;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019A34);

void func_80019A70(void) {
    if (Sce_flag_test(0x1E1)) {
        D_800822D6 = 0x25;
    } else {
        D_800822D6 = 0x30;
    }
}

extern u16 D_800822F2;
extern u16 D_800822F4;

void func_80019AA4(void) {
    s32 v = Sce_flag_test(0x35);
    if (v != 0) v = 0x2D;
    else v = 0xFFFF;
    D_800822F2 = v;
    D_800822F4 = v;
}

void func_80019AE0(void) {
    if (Sce_flag_test(0x1E1)) {
        D_80082218 = 0x25;
    } else {
        D_80082218 = 0x30;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019B14);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019C20);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019D18);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019DE0);

void func_80019F94(s32 arg0) {
    D_800AD15C[0] = &D_800821F8[arg0];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019FB4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", Sound_call);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", Sound_call2);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A0A8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A110);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A1FC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A238);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A274);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A63C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A6DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A744);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A834);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A968);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AA8C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AC58);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AE6C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AEF0);

void func_8001B2F0(s16 arg0) {
    func_80071220(arg0);
}

void func_8001B314(void) {
    SpuSetKey(0, 0xFF8000);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001B33C);
