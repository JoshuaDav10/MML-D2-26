#include "common.h"

typedef struct {
    s32 cmd;   // 0x0 — command id (1, 4, 6 seen so far)
    s32 arg0;  // 0x4
    s32 arg1;  // 0x8
    s32 xC;    // 0xC
} CD_CMD;      // 0x10 — Cd command queue entry

extern CD_CMD *unknown_Cd_strucptr;
extern s32 D_800989C4;
void CdSyncCallback(s32);
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

void func_8001C7F0(void) {
    D_800989C4 += 1;
    CdSyncCallback(0);
}

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

void func_8001D414(s32 arg0, s32 arg1) {
    CD_CMD *p = unknown_Cd_strucptr;
    p->cmd = 4;
    p->arg0 = arg0;
    p->arg1 = arg1;
    unknown_Cd_strucptr = p + 1;
}

void Cd_read_comb(s32 arg0) {
    CD_CMD *p = unknown_Cd_strucptr;
    p->cmd = 6;
    p->arg0 = arg0;
    unknown_Cd_strucptr = p + 1;
}

void func_8001D468(s32 arg0, s32 arg1) {
    CD_CMD *p = unknown_Cd_strucptr;
    p->cmd = 1;
    p->arg0 = arg0;
    p->arg1 = arg1;
    unknown_Cd_strucptr = p + 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D494);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D58C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D648);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D6D8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D7AC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D7E4);

s32 Cd_read_sync2(void) {
    return unknown_Cd_strucptr != (CD_CMD *)D_800A3A40;
}
