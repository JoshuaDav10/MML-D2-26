#include "common.h"
#include "rock_neo/game.h"
#include "rock_neo/scene.h"

extern u8 D_80098198;
extern u8 D_80098199;

extern u8 D_80098AB8[];
extern s32 D_800988E8;    // lui-accessed (not sdata)
extern s32 D_800ACDBC[];
extern s32 D_800BC740;
extern s32 D_800BC744;
extern s32 D_800BC748;
extern s32 D_800BC770;
extern s32 D_800BC774;
extern s32 D_800BC778;
extern s32 D_800BC7F0;
extern s16 D_80098208; // sdata ($gp), stored as a block of four
extern s16 D_8009820A;
extern s16 D_8009820C;
extern s16 D_8009820E;

void func_8001D854(u8 arg0) {
    if (D_80098788 == 0) {
        D_80098788 = arg0;
    }
}

s32 func_8001D878(void) {
    return D_80098AB8[0] & 0x20;
}

s32 Cd_read_sync2();
s32 func_80012E98(s32);

void func_8001D888(void) {
    while (Cd_read_sync2()) {
        func_80012E98(1);
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001D8C0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001D928);

void func_8001D974(void) {
    Game_work.x53++;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001D990);

extern u8 Sce_flag[];

s32 Sce_flag_test(s32 flagno) {
    u8 buf[8]; // unused, but the original allocates an 8-byte frame here
    s32 bit, mask;

    bit = flagno & 7;
    flagno = (u32)flagno >> 3;
    mask = 0x80 >> bit;
    return (Sce_flag[flagno] & mask) != 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", Sce_flag_on);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", Sce_flag_off);

extern u8 *D_80098B60;

s32 func_8001DD88(void) {
    u8 *p = D_80098B60;
    D_80098198 = p[0];
    D_80098199 = p[1];
    D_80098B60 = p + 2;
    // read via D_80098199-1 (same address as D_80098198): forces the lh to
    // alias the D_80098199 store so it schedules after it, as in the original
    return *(s16*)(&D_80098199 - 1);
}

s32 func_8001DDC0(u8 *arg0) {
    D_80098198 = arg0[0];
    D_80098199 = arg0[1];
    return *(s16*)&D_80098198;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001DDE4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001DE84);

void func_8001DEDC(void) {}

void func_8001DEE4(void) {
    s32 *p;
    s32 i;

    D_800988E8 = 0;
    i = 0x1F;
    p = &D_800ACDBC[0];
    for (; i >= 0; i--) {
        *p-- = 0;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001DF10);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001DFEC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E068);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E20C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E390);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E3F0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E460);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E4C4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E550);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E6C0);

void func_8001E7E4(s32 arg0, s32 arg1, s32 arg2) {
    D_800BC740 = arg0 << 16;
    D_800BC744 = arg1 << 16;
    D_800BC748 = arg2 << 16;
}

void func_8001E810(s32 arg0, s32 arg1, s32 arg2) {
    D_800BC770 = arg0 << 16;
    D_800BC774 = arg1 << 16;
    D_800BC778 = arg2 << 16;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E83C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E968);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001EAE8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001EB98);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001EC0C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F070);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F158);

void func_8001F1DC(void) {
    Scene_work.x10 = 0;
    Scene_work.x18 = 0;
    Scene_work.x1C = 0;
    Scene_work.x8 = 0;
    Scene_work.x9 = 0;
}

void func_8001F20C(u8 *arg0, s32 arg1) {
    D_800BC7F0 = arg1;
    Scene_work.xA4 = arg0;
    Scene_work.x8 = arg0[0];
    Scene_work.x9 = arg0[1];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F23C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F3C8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F580);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F5E4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F6C4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F740);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F798);

void func_8001F820(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F828);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F8DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F9AC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FA94);

void func_8001FB24(void) {
    if (Game_work.x52 == 6) {
        D_8009820E = 0x67;
        D_8009820C = 0x67;
        D_8009820A = 0x67;
        D_80098208 = 0x67;
    } else {
        D_8009820E = 0x66;
        D_8009820C = 0x66;
        D_8009820A = 0x66;
        D_80098208 = 0x66;
    }
}

extern s16 D_80098212; // sdata ($gp)

void func_8001FB54(void) {
    s8 v = Game_work.x52;
    D_80098212 = (v < 0) ? 0x6B : (v < 6) ? 0x69 : (v < 8) ? 0x6A : 0x6B;
}

extern s16 D_80098236; // sdata ($gp)

void func_8001FB8C(void) {
    s8 v = Game_work.x52;
    D_80098236 = (v < 0) ? 0x78 : (v < 2) ? 0x76 : (v < 6) ? 0x77 : 0x78;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FBC4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FC50);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FCA4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FCE4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FD3C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FD90);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FDE4);
