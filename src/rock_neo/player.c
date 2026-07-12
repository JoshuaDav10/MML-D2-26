#include "common.h"
#include "rock_neo/player.h"
#include "rock_neo/game.h"
#include "rock_neo/scene.h"
#include "rock_neo/sound.h"

s32 func_800406A8(PL_WORK*);
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

void func_8003EE68(PL_WORK* pl) {
    pl->x9 = 7;
    pl->xA = 4;
    pl->x75 = 1;
    pl->x74 = 1;
    Pl00_shot_enable_on(pl);
    func_80041DDC(pl, 0x12, 0, 1);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003EEC0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F188);

s32 func_8003F224(PL_WORK *pl) {
    u16 k = pl->x11C;
    s32 g;

    if (Game_work.x54[2] != 0) goto one;
    /* the shared return-1 label keeps the tail a branch (separate returns
       collapse to a setcc/sltiu) — same trick as scene func_8001DE84 */
    g = ((u8 *)&Game_work)[0x83];
    if (g == 0) {
        if (pl->x13E & k) {
            return 1;
        }
    }
    if (g != 1) {
        return 0;
    }
    if (pl->x140 & k) {
        return 0;
    }
one:
    return 1;
}

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

void func_8003FDA8(PL_WORK* pl) {
    u16 k = pl->x11C; /* preloaded so the lhu schedules above the first branch */
    if (*(u8*)&pl->xA < 3) {
        if ((k & pl->x134) == 0) {
            pl->x44A[0] = 0;
            Sound_call(0x8F, 0, 0);
            if (func_800406A8(pl) == 0) {
                pl->x9 = 0;
                pl->xA = 0;
            }
        }
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003FE1C);

void func_8003FFA8(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003FFB0);

s32 func_80040818(PL_WORK*);
s32 func_80040710(PL_WORK*);
void func_80040B68(PL_WORK*, s32);

void func_800400B8(PL_WORK* pl) {
    if (func_80040818(pl) == 0) {
        if (*(s8*)&pl->x74 == 0 && *(u8*)&pl->xA == 1) {
            if (func_80040710(pl) != 0) {
                return;
            }
        }
        if (*(s8*)&pl->x112 != 2) {
            func_80040B68(pl, 2);
        }
    }
}

void func_80040130(void) {}

void func_80040138(void) {}

void func_80040140(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 2) {
        func_80040764();
    }
}

void func_80040170(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040178);

void func_800394AC(void);

void func_80040224(PL_WORK *pl) {
    u16 k = pl->x11C;

    func_80040B68(pl, 0);
    if (((u8 *)&Game_work)[0x83] != 1) {
        if (!(pl->x140 & k)) {
            pl->x9 = 0;
            pl->xA = 0;
            func_800394AC();
            pl->x108 = 0;
        }
    }
}

void func_80040294(void) {}

void func_8004029C(void) {}

void func_800402A4(void) {}

void func_800402AC(void) {}

void func_800402B4(void) {}

void func_800402BC(void) {}

s32 func_800402C4(PL_WORK* pl) {
    if (pl->x11C & pl->x124) {
        if (pl->x11C & pl->x134) {
            pl->x9 = 1;
            pl->xA = 0;
            return 1;
        }
    }
    return 0;
}

s32 func_8004030C(PL_WORK* pl, s32 arg1) {
    u16 k;

    if (Scene_work.x0 != 0 || arg1 == 0) {
        k = pl->x11C;
    } else {
        k = pl->x11E;
    }
    if (k & pl->x126) {
        if (k & pl->x134) {
            pl->x9 = 1;
            pl->xA = 0x100;
            return 1;
        }
    }
    return 0;
}

s32 func_80040380(PL_WORK* pl) {
    u8 t;

    if ((pl->x11C & pl->x124) && !(pl->x11C & pl->x134)) {
        t = pl->x112;
        pl->x9 = 2;
        pl->xA = 0;
        pl->x112 = 0;
        pl->x113 = t;
        return 1;
    }
    return 0;
}

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

s32 func_80040710(PL_WORK* pl) {
    u16 m0 = pl->x128;
    u16 m1 = pl->x12A;
    u16 k = pl->x11C & (m0 | m1);

    if (k == m0) {
        pl->x9 = 9;
        pl->xA = 0;
        return 1;
    }
    if (k == m1) {
        pl->x9 = 9;
        pl->xA = 0x100;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040764);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040818);

s32 func_80040AEC(PL_WORK* pl) {
    u16 k = pl->x11C;

    if ((((u8*)&Game_work)[0x83] == 1) || (k & pl->x140)) {
        pl->x9 = 0xE;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

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

s32 func_80041A44(PL_WORK* pl, s32 k) {
    s32 v;
    if (*(s8*)&pl->x74 != 0) v = 0xC;
    else if (k & 0x41000000) v = 0xC;
    else if (*(s16*)&pl->life <= 0) v = 0xC;
    else if (pl->x9 == 0xB && *(u8*)&pl->xA < 2) return 0xB; /* v assigned only after the compares — keeps v dead there so it lands in $v0 */
    else v = 0xB;
    pl->x9 = v;
    *(u8*)&pl->xA = 0;
    return v;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041AB0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041B8C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041DDC);

void func_8002FEA4(PL_WORK *, s16, s16, s16);

void func_80041E90(PL_WORK *pl, s16 x, s16 y, s16 z) {
    pl->x56 -= pl->x116;
    func_8002FEA4(pl, x, y, z);
    pl->x56 += pl->x116;
}

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
