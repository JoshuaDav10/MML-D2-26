#include "common.h"
#include "rock_neo/game.h"

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

void func_8001D854(u8 arg0) {
    if (D_80098788 == 0) {
        D_80098788 = arg0;
    }
}

s32 func_8001D878(void) {
    return D_80098AB8[0] & 0x20;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001D888);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001D8C0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001D928);

void func_8001D974(void) {
    Game_work.x53++;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001D990);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", Sce_flag_test);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", Sce_flag_on);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", Sce_flag_off);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001DD88);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F1DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F20C);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FB24);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FB54);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FB8C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FBC4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FC50);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FCA4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FCE4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FD3C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FD90);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001FDE4);
