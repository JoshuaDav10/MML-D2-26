#include "common.h"
#include "rock_neo/game.h"
#include "rock_neo/scene.h"

extern u8 D_80098198;
extern u8 D_80098199;
extern u8 D_800C356E[];
extern s16 *D_800893BC[];

extern u8 D_80098AB8[];
extern s32 D_800BE438[];
extern s32 D_800989F8; // lui-accessed (not sdata)
extern s32 D_800989FC; // lui-accessed (not sdata)
extern s32 D_80098A00; // lui-accessed (not sdata)
extern s32 D_800988E8;    // lui-accessed (not sdata)
extern s32 D_800ACDBC[];
extern s32 D_800BC740;
extern s32 D_800BC744;
extern s32 D_800BC748;
extern s32 D_800BC770;
extern s32 D_800BC774;
extern s32 D_800BC778;
extern s32 D_800BC7F0;
extern s16 D_8009827C; // sdata ($gp)
extern s16 D_8009827E;
extern s16 D_8009828C;
extern s16 D_8009828E;
extern s16 D_80098208; // sdata ($gp), stored as a block of four
extern s16 D_8009820A;
extern s16 D_8009820C;
extern s16 D_8009820E;
extern s16 D_8009825A;
extern s16 D_8009824C;
extern s16 D_800982FA;
extern s16 D_800982FC;
extern s16 D_800982FE;
extern s16 D_80098276;
extern s16 D_800981D0;
extern s16 D_800981D2;
extern s16 D_800981E2;

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

extern u8 Sce_flag[];
extern u8 D_800B6130[];
void func_80067E18(void);
void func_80067E4C(void);
void func_800176DC(u8 *, u8 *, s32);

void func_8001D8C0(s32 f) {
    s32 *p;
    s32 i;

    if (f == 0) {
        p = (s32 *)Sce_flag;
        for (i = 0x3F; i >= 0; i--) {
            *p++ = 0;
        }
        func_80067E18();
        func_80067E4C();
    } else {
        func_800176DC(D_800B6130, Sce_flag, 8);
    }
}

void func_8001D928(s32 a) {
    s32 *p;
    s32 i;

    if (a == 0) {
        p = D_800BE438;
        for (i = 0xF; i >= 0; i--) {
            *p++ = 0;
        }
    }
    *(s32 *)&Scene_work = 0;
    D_800989F8 = 0;
    D_800989FC = 0;
    D_80098A00 = 0;
}

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

extern u16 D_800C0C2A; // Joy1.trg, lui-accessed

s32 func_8001DE84(void) {
    if (Game_work.x5A == 0) {
        if (Sce_flag_test(Scene_work.x1 + 0x380) == 0) goto zero;
    }
    /* the goto (shared return-0 label) is load-bearing: separate
       `return 0;`s let cc1 collapse the tail to a setcc (sltu) */
    if (D_800C0C2A & 8) {
        return 1;
    }
zero:
    return 0;
}

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

extern u8 *D_800ACD40[];
u8 *func_8001EAE8(); /* unprototyped: func_8001EB98 must call it with no arg setup */

void func_8001DFEC(u8 **st, s32 n) {
    u8 *e = (u8 *)(n * 0x14 + (s32)st[1]); /* integer add keeps the scaled index in $rs */
    s32 k = e[0];

    func_8001EAE8(e);
    D_800ACD40[k] = Scene_work.x24[k];
    D_800988E8 |= 0x80000000 >> k;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E068);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001E20C);

void func_8001E390(s32 n) {
    u32 i;

    for (i = 0x2E0; i < 0x2EC; i++) {
        if (n == i) {
            Sce_flag_on(i);
        } else {
            Sce_flag_off(i);
        }
    }
}

extern u16 D_800891B4[];
void func_800179FC();

void func_8001E3F0(s16 n) {
    u16 *p = &Game_work.x60;
    s16 v = *p + D_800891B4[n];
    *p = v;
    if (v < 0) {
        *p = 0;
    } else if (v >= 0x100) {
        *p = 0xFF;
    }
    func_800179FC();
}

void func_8001E460(s32 n, s32 on, s32 b) {
    if (on != 0) {
        Sce_flag_on(n + 0x7E0);
        if (b != 0) {
            Sce_flag_on(n + 0x7C0);
            return;
        }
    } else {
        Sce_flag_off(n + 0x7E0);
    }
    Sce_flag_off(n + 0x7C0);
}

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

void func_8001EB98(u8 *p) {
    u8 pad[8]; /* dead frame space, present in the original */
    u8 *save = Scene_work.x24[p[0]];
    u8 *q = func_8001EAE8();
    u32 t;

    t = q[6] & 0xDF;
    q[6] = t;
    t = p[0]; /* reload+shift stay in one register only via reassignment */
    t <<= 2;
    *(u8 **)((u8 *)Scene_work.x24 + t) = save;
}

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

void func_8002FBF4(void);
void Cd_read_comb(s32);

void func_8001F580(u16 no, s32 flag, s32 flag2) {
    u16 keep = no; /* the distinct u16 copy is load-bearing: s32 keep gets
                      merged back into no's register (one callee-save short) */

    if (flag != 0) {
        func_8002FBF4();
    }
    if (flag2 != 0) {
        Game_work.x6A = keep;
    }
    Cd_read_comb((s16)no);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F5E4);

void func_8001F6C4(void) {
    s16 *p = D_800893BC[D_800C356E[0]];
    s32 a = p[0];
    s32 f = D_800C356F[0];
    if (Game_work.x68 == a) {
        a = p[f + 1];
        if (a < 0) {
            return;
        }
        if (Game_work.x6A == a) {
            return;
        }
    }
    func_8001D7AC(a);
}

extern void (*D_8008980C[])(void);
void func_8001F798(s32);

void func_8001F740(void) {
    s8 *p = &Game_work.stage_no;

    func_8001F798(*p);
    D_8008980C[*p]();
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F798);

void func_8001F820(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/scene", func_8001F828);

/* Jump-table function (jtbl_800108C0). Case 0's flag test written as a
   || ternary: an if/else there makes cc1 precompute the else-value before
   the branch, which lengthens `out`'s live range across a Sce_flag_test
   $v0 return, forcing `out` into $v1 (wrong). The ternary keeps `out` in
   $v0 and the ==-leg un-inverted. See LESSONS 2026-07-06. */
void func_8001F8DC(void) {
    s8 v = Game_work.x52;
    s16 out;
    switch (v) {
    case 0:
        if (Sce_flag_test(6) == 0) goto d0_3C;
        if (Sce_flag_test(0x59) != 0) goto d0_3C;
        D_800981D0 = 0x3D;
        return;
    d0_3C:
        D_800981D0 = 0x3C;
        return;
    case 1:
        D_800981D0 = 0x3C;
        out = 0x3F;
        break;
    case 2:
    case 3:
        D_800981D0 = 0x3C;
        out = 0x40;
        break;
    case 4:
    case 5:
        D_800981D0 = 0x3C;
        out = 0x41;
        break;
    case 0xB:
        D_800981D0 = 0x3C;
        out = 0x43;
        break;
    default:
        D_800981D0 = 0x3C;
        out = 0x42;
        break;
    }
    D_800981D2 = out;
}

/* Jump-table function (jtbl_800108F0). Case 0 uses the || ternary trick
   (see func_8001F8DC) so `out` stays in $v0 across the whole function. */
void func_8001F9AC(void) {
    s8 v;
    s16 out;
    if (Sce_flag_test(0x204) != 0 && Sce_flag_test(0x206) == 0) {
        out = 0x4F;
    } else if (Sce_flag_test(0x212) != 0 && Sce_flag_test(0x213) == 0) {
        out = 0x4E;
    } else if (Sce_flag_test(0x200) != 0) {
        out = 0x4C;
    } else {
        v = Game_work.x52;
        switch (v) {
        case 0:
            out = (Sce_flag_test(2) == 0 || Sce_flag_test(1) != 0) ? 0x4A : 0x4B;
            break;
        case 1:
            out = 0x4D;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            out = 0x50;
            break;
        case 6:
        case 7:
            out = 0x51;
            break;
        case 0xB:
            out = 0x53;
            break;
        default:
            out = 0x52;
            break;
        }
    }
    D_800981E2 = out;
}

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

void func_8001FC50(void) {
    s8 v = Game_work.x52;
    s16 out;
    switch (v) { /* switch keeps the case tree + un-cross-jumped 0x81 legs */
    case 1:
    case 2:
    case 3:
        if (Sce_flag_test(0x30) == 0) {
            out = 0x82;
            break;
        }
        /* fallthrough */
    case 0:
    default:
        out = 0x81;
        break;
    }
    D_8009824C = out;
}

void func_8001FCA4(void) {
    s8 v = Game_work.x52;
    s16 out;
    switch (v) { /* irregular — switch keeps ==5 leg beqz+j shape; if/else inverts */
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        out = 0x85;
        break;
    case 5:
        out = 0x86;
        break;
    default:
        out = 0x87;
        break;
    }
    D_8009825A = out;
}

/* Dense switch: cc1 emits the jump table into .rodata; the linker script
   places scene.c.o(.rodata) at 0x80010920, carved out of the extracted
   rodata (800/1140 split). */
void func_8001FCE4(void) {
    s8 v = Game_work.x52;
    s16 out;
    switch (v) {
    case 0:
        out = 0x8E;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        out = 0x8F;
        break;
    case 6:
    case 7:
        out = 0x90;
        break;
    default:
        out = 0x91;
        break;
    }
    D_80098276 = out;
}

void func_8001FD3C(void) {
    s8 x;

    if (Game_work.x52 == 1) {
        D_8009827C = 0x93;
    } else {
        D_8009827C = 0x94;
    }
    x = Game_work.x52;
    D_8009827E = (x < 0) ? 0x97 : (x < 2) ? 0x95 : (x < 5) ? 0x96 : 0x97;
}

void func_8001FD90(void) {
    s8 x;

    if (Game_work.x52 == 1) {
        D_8009828C = 0x9D;
    } else {
        D_8009828C = 0x9E;
    }
    x = Game_work.x52;
    D_8009828E = (x < 0) ? 0xA1 : (x < 2) ? 0x9F : (x < 5) ? 0xA0 : 0xA1;
}

void func_8001FDE4(void) {
    s8 v = Game_work.x52;
    s16 out;
    switch (v) {
    case 0:
    case 1:
    case 2:
    case 3:
        out = 0xC3;
        break;
    case 4:
        out = 0xC4;
        break;
    case 0xB:
        out = 0xC4;
        break;
    default:
        return;
    }
    D_800982FE = out;
    D_800982FC = out;
    D_800982FA = out;
}
