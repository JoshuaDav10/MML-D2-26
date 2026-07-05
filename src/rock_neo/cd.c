#include "common.h"

extern u8 *unknown_Cd_strucptr[];
extern u8 D_800A3A40[];

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B3E4);

void func_8001B4C4(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B4CC);

void func_8001B63C(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B644);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B6FC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B7B4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B858);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B89C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001BB4C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001C7F0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001C824);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001C95C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CAAC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CB30);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CB7C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CC08);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CC7C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CD60);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CF98);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D078);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D254);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D2BC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D324);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D394);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D414);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", Cd_read_comb);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D468);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D494);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D58C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D648);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D6D8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D7AC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D7E4);

s32 Cd_read_sync2(void) {
    return unknown_Cd_strucptr[0] != D_800A3A40;
}
