#include "rock_neo.h"

#include "rock_neo/game.h"
#include "rock_neo/cd.h"
#include "rock_neo/moji.h"

extern volatile s32 idk_framecounter_maybe; // 0x80098888, lui-accessed (not sdata);
                                            // volatile: original reloads after store
extern s32 D_80098158;             // sdata ($gp)
extern s32 D_800979D8;             // sdata ($gp)
void ChangeTh(u32);
void func_8007FF70(void);
void func_8007FF80(void);
long func_8001246C();
extern s32 D_800988D8; // sdata ($gp)
extern u16 D_801F8100[];
extern s32 D_801F8110[];
extern s32 D_801F8114[];
extern u8 *D_801F811C[];
extern s32 D_801F8144[];
long PCopen(char *, long, long);
long PCread(long, u8 *, long);
void func_8007699C(s32);
extern s32 D_801F8108[];
void CloseTh(s32);
extern u16 *D_801F8300;

// clang-format off

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", main);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", idk_Init_system_maybe);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_800120A8);

void func_8001215C(u8 *x) {
    ClearOTagR((unsigned long *)(x + 0x70), 8);
    *(u16 *)(x + 0xA) = 0;
    x[0x2A] = 0;
    x[0x2B] = 0;
    x[0x2C] = 1;
    x[0x2D] = 0;
    x[0x2E] = 0;
    x[0x2F] = 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_800121AC);

void func_800121AC(u8 *);
extern u8 D_800C0C00[];
extern u8 D_800C0D68[];

void func_80012298(void) {
    func_800121AC(D_800C0C00);
    func_800121AC(D_800C0D68);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_800122D0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_80012350);

s32 vsync_cb(void) {
    idk_framecounter_maybe += 1;
    return idk_framecounter_maybe;
}

extern u16 D_800C0C26; // joy pad word, lui-accessed
extern u16 D_800C0D8E;
extern s32 D_80097864; // sdata ($gp)

void func_80012424(void) {
    if ((D_800C0C26 & 0xF00) == 0xF00 && (D_800C0D8E & 0x2000)) {
        D_80097864 ^= 1;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_8001246C);

void func_80012938(void) {
    func_8007FF70();
    D_800988D8 = OpenEvent(0xF0000010, 0x1000, 0x1000, func_8001246C);
    EnableEvent(D_800988D8);
    func_8007FF80();
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_80012988);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_80012A5C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_80012BC4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_80012C80);

void func_80012E10(s32 n, s32 fn) {
    u16 *p;

    func_8007FF70();
    p = (u16 *)(0x801F8100 + (n << 7));
    D_801F8108[n << 5] = OpenTh((long (*)())fn, D_801F8110[n << 5], D_801F8144[n << 5]);
    func_8007FF80();
    *p = 2;
}

s32 func_80012E98(s32 arg0) {
    u16 *p = D_801F8300;
    p[1] = arg0;
    p[0] = 1;
    ChangeTh(0xFF000000);
}

void func_80012ECC(void) {
    u16 **q = (u16 **)0x801F8300;
    **q = 0;
    func_8007FF70();
    CloseTh(*(s32 *)(*q + 4));
    func_8007FF80();
    ChangeTh(0xFF000000);
}

void func_80012F24(s32 n) {
    D_801F8100[n << 6] = 0;
    func_8007FF70();
    CloseTh(D_801F8108[n << 5]);
    func_8007FF80();
}

void func_80012F78(s32 arg0) {
    D_80098158 = arg0;
    D_800979D8 = 1;
    ChangeTh(0xFF000000);
}

void func_80012FA4(s32 arg0) {
    u16 *p = (u16 *)(0x801F8100 + (arg0 << 7));
    *p |= 0x40;
}

void func_80012FC8(s32 arg0) {
    u16 *p = (u16 *)(0x801F8100 + (arg0 << 7));
    *p &= 0xFFBF;
}

void func_80012FEC(s32 n, char *name) {
    s32 fd;
    u8 buf[0x10];
    u8 *p;

    fd = PCopen(name, 0, 0);
    PCread(fd, buf, 0x10);
    PCread(fd, (u8 *)(0x801F8114 + (n << 7)), 0x3C);
    PCread(fd, D_801F811C[n << 5], 0x7B4);
    /* same slot as D_801F811C[n << 5]; the distinct expression keeps cc1
       from CSEing the address, so both loads stay in $at-indexed form and
       the reloc resolves to identical bytes */
    p = ((u8 **)D_801F8114)[(n << 5) + 2];
    while (PCread(fd, p, 0x800) == 0x800) {
        p += 0x800;
    }
    func_8007699C(fd);
    FlushCache();
    func_80012E10(n, D_801F8114[n << 5]);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_800130D0);

void func_8001319C(void) {
    s32 i;

    for (i = 0; i < 8; i++) {
        (*(u16 **)0x801F8300)[i + 0x30] = 0;
    }
    for (i = 0; i < 0x10; i++) {
        /* explicit left-assoc pointer arithmetic (not [i + 0x71]) keeps
           the pointer first in the addu */
        *(*(u8 **)0x801F8300 + i + 0x71) = 0;
    }
}

void func_800131FC(void) {
    D_80098B1C = 0;
    D_80098B1D = 0;
    while (1) {
        D_80080894[D_80098B1C](&D_80098B1C);
        func_80012E98(1);
    }
}

void func_8001326C(u8* arg0) {
    unkst_80098A28* var_s0;
    u32 var_s2;

    SetDispMask(1);
    var_s0 = &D_80098A28;
    for (var_s2 = 0; var_s2 < 2; var_s2++) {
        var_s0[var_s2].unk3 = 3;
        var_s0[var_s2].unk7 = 98;
        var_s0[var_s2].unkC = 320;
        var_s0[var_s2].unk8 = 0;
        var_s0[var_s2].unkA = 0;
        var_s0[var_s2].unkE = 240;
        SetDrawMode(&D_80098A10[var_s2], 0, 0, GetTPage(2, 2, 0, 0), 0);
    }
    func_80063EF0();
    func_80053788();
    func_800665E8();
    Cd_read_comb(INIT_DAT_BIN);
    Game_work.x5B = 1;
    *(s8* )0x1F800074 = 0;
    Game_work.x10 = 0;
    Game_work.x14 = 0;
    Game_work.difficultyUnlocked[0] = 0;
    Game_work.difficultyUnlocked[1] = 0;
    Game_work.x54[0] = 0;
    Game_work.x54[1] = 0;
    Game_work.x54[4] = 0;
    Game_work.x54[5] = 0;
    Game_work.x70 = -1;
    Game_work.x72 = -1;
    Game_work.x74 = -1;
    Game_work.x76 = -1;
    *arg0 += 1;
}
