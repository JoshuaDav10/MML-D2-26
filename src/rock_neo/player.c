#include "common.h"
#include "rock_neo/player.h"

void func_8003BE6C(PL_WORK*, s32);
s32 func_80041DDC(PL_WORK*, s32, s32, s32);
void func_80040764(void);

void func_8003BE40(s32 arg0) {
    func_8003BE6C(&Player_work, arg0);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003BE6C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C07C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C2D8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C418);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C4EC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C7B0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C88C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003CB20);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003CBF8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003CD4C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003D124);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003D1E0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003D6DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003D7D4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003D960);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003DA34);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003DC5C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E068);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E224);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E4A8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E6B0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E860);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003EB08);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003EE68);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003EEC0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F188);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F224);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F288);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F3E8);

void func_8003F498(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x33, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

void func_8003F4E8(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x34, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

void func_8003F538(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x35, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

void func_8003F588(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x36, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F5D8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F7F0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F97C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003FDA8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003FE1C);

void func_8003FFA8(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003FFB0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800400B8);

void func_80040130(void) {}

void func_80040138(void) {}

void func_80040140(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 2) {
        func_80040764();
    }
}

void func_80040170(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040178);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040224);

void func_80040294(void) {}

void func_8004029C(void) {}

void func_800402A4(void) {}

void func_800402AC(void) {}

void func_800402B4(void) {}

void func_800402BC(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800402C4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8004030C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040380);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800403D4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040468);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800404FC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040574);

s32 func_800405F4(PL_WORK* pl) {
    if (pl->x11C & (pl->x128 | pl->x12A)) {
        pl->x9 = 4;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040630);

s32 func_800406A8(PL_WORK* pl) {
    if (pl->x11E & pl->x138) {
        pl->x9 = 7;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

s32 func_800406DC(PL_WORK* pl) {
    if (pl->x11E & pl->x138) {
        pl->x9 = 8;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040710);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040764);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040818);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040AEC);

s32 func_80040B34(PL_WORK* pl) {
    if (pl->x11C & pl->x13E) {
        pl->x9 = 5;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040B68);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040E00);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040E9C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040FA8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041044);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800410D4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041158);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800411DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800414F0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8004174C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041A44);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041AB0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041B8C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041DDC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041E90);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041EF4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041F54);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80042044);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80042094);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80042154);

s32 func_80042208(void) {
    if (*(s16*)&Player_work.life >= 0) {
        Player_work.x449 = 1;
        return 1;
    }
    return 0;
}
