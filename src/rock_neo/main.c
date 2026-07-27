#include "rock_neo.h"

#include "rock_neo/game.h"
#include "rock_neo/cd.h"
#include "rock_neo/moji.h"

/* --- decls: parallel grind wave 2, 2026-07-26 --- */
/* func_80012BC4 */
/* 9-entry thread-API vector table (func_80012E10 .. func_800130D0), copied
   wholesale to 0x801F8000.  lui/%hi + addiu/%lo + 0(reg) => unsized array. */
extern u32 D_80080870[];

extern u16 D_800C0D8E_a[] __asm__("D_800C0D8E");

void func_800122D0(s32);

/* --- decls: parallel grind wave 1, 2026-07-26 --- */
/* func_800120A8 */
/* Double-buffer pair at 0x800C0C48, stride 0x90 (it lives at +0x48 inside the
   0x168-stride screen struct that starts at D_800C0C00; D_800C0D68, already
   extern'd in main.c, is the next one).  DISPENV 0x14 + DRAWENV 0x5C + OT 0x20
   = 0x90.  Layout confirmed by func_8001215C, whose writes land exactly on
   disp.screen.y (+0xA) and draw.dtd/dfe/isbg/r0/g0/b0 (+0x2A..+0x2F). */
typedef struct DB {
    DISPENV disp;
    DRAWENV draw;
    unsigned long ot[8];
} DB;

extern DB D_800C0C48[2];

void SetDefDrawEnv(DRAWENV* env, int x, int y, int w, int h);
void SetDefDispEnv(DISPENV* env, int x, int y, int w, int h);
void func_8001215C(u8* x); /* defined at main.c:36, i.e. BELOW this call site */

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

void func_800120A8(void) {
    SetDefDrawEnv(&D_800C0C48[0].draw, 0, 0, 320, 240);
    SetDefDispEnv(&D_800C0C48[0].disp, 0, 256, 320, 240);
    SetDefDrawEnv(&D_800C0C48[1].draw, 0, 256, 320, 240);
    SetDefDispEnv(&D_800C0C48[1].disp, 0, 0, 320, 240);
    func_8001215C((u8 *)&D_800C0C48[0]);
    func_8001215C((u8 *)&D_800C0C48[1]);
}

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

void func_800121AC(u8 *x) {
    s32 t;

    if (x[0] == 0xFF) {
        x[0x24] = 0xFF;
        x[0x25] = 0;
        *(u16 *)(x + 0x26) = 0;
        *(s32 *)(x + 0x30) = 0;
        *(s32 *)(x + 0x28) = 0;
        return;
    }

    x[0x24] = x[1] >> 4;
    *(u16 *)(x + 0x28) = *(u16 *)(x + 0x26);
    *(s32 *)(x + 0x30) = *(s32 *)(x + 4);
    *(u16 *)(x + 0x26) = ~*(u16 *)(x + 2);
    *(u16 *)(x + 0x2A) = *(u16 *)(x + 0x26) & ~*(u16 *)(x + 0x28);
    *(u16 *)(x + 0x2C) = *(u16 *)(x + 0x28) & ~*(u16 *)(x + 0x26);
    func_800122D0((s32)x);

    if ((x[0x25] & 3) == 1) {
        x[0x36] = 1;
    }
    if ((x[0x25] & 3) == 2) {
        x[0x36] = 0;
    }
    t = (x[0x25] & 1) << 1;
    x[0x25] = (x[0x25] & 0xFD) | t;
}

void func_800121AC(u8 *);
extern u8 D_800C0C00[];
extern u8 D_800C0D68[];

void func_80012298(void) {
    func_800121AC(D_800C0C00);
    func_800121AC(D_800C0D68);
}

void func_80012350(s32, s32, s32);

void func_800122D0(s32 p) {
    func_80012350(p, 0x10, 0);
    func_80012350(p, 0x40, 1);
    func_80012350(p, 0x80, 2);
    func_80012350(p, 0x20, 3);
    func_80012350(p, 0x400, 4);
    func_80012350(p, 0x800, 5);
}

void func_80012350(s32 p, s32 mask, s32 idx) {
    u8 v;
    u8 c;

    if ((*(u16 *)(p + 0x2E) & mask) == 0 && (*(u16 *)(p + 0x2A) & mask) == mask) {
        v = *(u8 *)(p + idx + 0x38);
        *(u8 *)(p + idx + 0x40) = 0;
        if (v == 0) {
            *(u8 *)(p + idx + 0x38) = 1;
        } else {
            *(u8 *)(p + idx + 0x38) = 2;
            *(u16 *)(p + 0x2E) |= mask;
        }
    } else {
        v = *(u8 *)(p + idx + 0x38);
        if (v != 0) {
            if (v != 2) {
                c = *(u8 *)(p + idx + 0x40);
                *(u8 *)(p + idx + 0x40) = c + 1;
                if (c < 6) {
                    return;
                }
            }
            *(u8 *)(p + idx + 0x38) = 0;
            *(u16 *)(p + 0x2E) &= ~mask;
        }
    }
}

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

/* Debug memory-viewer address stepper. The pad modifier bits pick a step
   size; D-pad left/right (0x10 / 0x40) move *p by step*8. The new address is
   then range-checked against main RAM (0x80000000..0x801FFF80) or scratchpad
   (0x1F800000..0x1F800380), matching the sign of the old address, and rolled
   back when it leaves the region. */
void func_80012988(s32 *p) {
    u8 unused[8];
    s32 orig;
    s32 step;
    u16 keys;
    u32 lo;

    keys = D_800C0D8E_a[0];
    orig = *p;
    step = 1;
    if (keys & 0x8000) {
        step = 0x10;
    }
    if (keys & 0x4000) {
        step = 0x100;
    }
    if (keys & 0x2000) {
        step = 0x1000;
    }
    if (keys & 0x10) {
        *p = orig - (step * 8);
    }
    if (D_800C0D8E_a[0] & 0x40) {
        *p += step * 8;
    }
    lo = 0x7FFFFFFF;
    if (orig < 0) {
        if ((u32)*p > lo) {
            if ((u32)*p <= 0x801FFF80) {
                return;
            }
        }
    } else {
        lo = 0x1F7FFFFF;
        if ((u32)*p > lo) {
            if ((u32)*p <= 0x1F800380) {
                return;
            }
        }
    }
    *p = orig;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/main", func_80012A5C);

/* Thread-slot table init: 4 slots of 0x80 at 0x801F8100.
   ktab = the PSX kernel "table of tables" at 0x100; ktab[4] (= *(u32*)0x110)
   is the TCB table base, TCBs are 0xC0 bytes, +0x78 is reg[gp] and +0x94 is
   reg[SR].  Slot i gets TCB[i+1], a 0x400 stack at 0x801FEC00+0x400*i, the
   current gp, and SR = 0x40000404 (CU2 | IM2 | IEp).
   q is the 0x801F8170 (= slot+0x70) cursor the original addresses the slot
   through; its accesses must be volatile -- that is what keeps them off a
   strength-reduced giv (so all four land on one +0x70-based register) and
   what forces the +0x0C reload right after the +0x0C store.  Raw constant
   pointers (not the D_801F81xx externs) are required for the lui/ori pairs. */
void func_80012BC4(void) {
    u32 *ktab = (u32 *)0x100;
    u32 gp;
    s32 i;
    u8 *p;
    volatile u8 *q;
    u8 *stack;
    u32 off;
    u32 sr;
    u32 tcb;
    u32 *d;

    gp = *(u32 *)(ktab[4] + 0x78);
    i = 0;
    sr = 0x40000404;
    q = (volatile u8 *)0x801F8170;
    stack = (u8 *)0x801FEC00;
    off = 0xC0;
    p = (u8 *)0x801F8100;
    for (; i < 4; i++) {
        *(u16 *)p = 0;
        tcb = ktab[4] + off;
        *(volatile u32 *)(q - 0x60) = (u32)stack;
        *(volatile u32 *)(q - 0x2C) = gp;
        *(volatile u32 *)(q - 0x64) = tcb;
        *(u32 *)(*(volatile u32 *)(q - 0x64) + 0x94) = sr;
        *q = 0;
        stack += 0x400;
        off += 0xC0;
        p += 0x80;
        q += 0x80;
    }

    d = (u32 *)0x801F8000;
    for (i = 0; i < 9; i++) {
        *d++ = D_80080870[i];
    }

    D_801F8300 = (u16 *)0x801F8100;
}

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

void func_800130D0(char *name) {
    s32 fd;
    u8 buf[0x10];
    u8 *p;
    u8 **q;

    fd = PCopen(name, 0, 0);
    PCread(fd, buf, 0x10);
    /* the raw-constant pointer (lui/ori) must be materialized HERE, not in the
       declaration's initializer -- an initializer hoists the lui/ori pair into
       the prologue ahead of the PCopen argument setup (12 hard mismatches). */
    q = (u8 **)0x801F8300;
    PCread(fd, *q + 0x14, 0x3C);
    PCread(fd, ((u8 **)*q)[7], 0x7B4);
    p = ((u8 **)*q)[7];
    while (PCread(fd, p, 0x800) == 0x800) {
        p += 0x800;
    }
    func_8007699C(fd);
    FlushCache();
    /* symbol form (%hi/%lo), unlike the raw constant above -- same split as
       func_80012E98 vs func_80012ECC in this file. */
    func_80012F78(*(s32 *)((u8 *)D_801F8300 + 0x14));
}

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
