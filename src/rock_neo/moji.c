#include "rock_neo.h"
#include "rock_neo/moji.h"
#include "rock_neo/game.h"
#include "rock_neo/sound.h"

extern s32 D_80098AF4; // sdata ($gp)
extern s32 D_80098824; // lui-accessed word, not in gp census
s32 func_8005BF10(s32, s32, u8*);
s32 func_8005DA78(s32);
s32 func_8005DAEC(s32);
s32 func_8005DA1C(s32);
extern u8 *D_8008CE5C[];
extern u8 *D_8008CE10[];
extern u8 D_800989D4; // sdata ($gp)
extern u8 *D_8008D0D4[];
extern u8 *D_8008CCA4[];
extern u8 *D_8008CACC[];
extern s8 D_800BE2F7[];
s32 func_80039E18(void); // s32 (not void) is load-bearing in func_80055C1C:
                         // reserving $v0 across the call moves script2 to $v1
                         // and lets `return 1` fill the load-delay slot
extern u8 D_800BE2F8[];
extern u8 D_8009899C; // sdata ($gp)
extern u8 D_80098930; // sdata ($gp)
extern u8 D_80098B6C; // sdata ($gp)
extern u16 D_8008FD3C[];
s32 func_8004327C(s32);
// byte-view of Player_work (0x800B51B0, size 0x5F4); the script CALL handlers
// index u8 remap tables inside it (at +0x450 and +0x454). Member access forces
// per-site %hi/%lo(Player_work+off) rather than a hoisted base pointer.
extern struct {
    u8 _p[0x450];
    u8 remap450[4];    // +0x450
    u8 remap454[0x1A0]; // +0x454
} Player_work;
extern u16 D_80098912[];
s32 func_8005ACA8(MOJI_TASK*);

u8 Moji_flag[8]; // COMMON on purpose: splat carved 0x80098A58 out of the
                 // extracted data for C to provide; gprel.py gp-rewrites
                 // refs to small .comm symbols in the census (see tool)

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80053788);

s32 MojiTaskExec(no, script_base, op)
s32 no;
u8 *script_base;
u8 op;
{
    MOJI_TASK *m;
    u8 *s;
    u32 f;
    u32 mask; // NOTE: the u8 `op` param and this named mask local are
              // register-allocation-load-bearing: `no` vs the 0x40000
              // constant tie at priority 12/88 == 3/22 and the u8 param's
              // entry copy breaks the tie the original way (s3/s2)

    if ((*(u32 *)Moji_flag & 0x400000) || D_80098824) {
        return 0;
    }
    m = &Moji_work[no];
    mask = 0x40000;
    if (m->flags & mask) {
        func_8001D494(0, 1, 0);
    }
    if (no != 4) {
        if (Moji_work[4].flags & mask) {
            func_8001D494(0, 1, 0);
        }
        Moji_work[4].flags = 0;
        Moji_work[4].xC2 = 0xFF;
    }
    m->flags = 0x80000000;
    m->x3F = 1;
    m->x3E = 0;
    m->xC2 = op;
    m->x4 = 0;
    m->x8 = 0;
    m->xA = 0;
    m->xC = 0;
    m->xE = 0;
    m->x3C = m->x3E;
    if (op != 0xFF) {
        s = script_base + *(u16 *)(script_base + op * 2);
        m->x44 = script_base;
        m->script2 = m->x48 = s;
    } else {
        m->script2 = m->x48 = script_base;
        m->x44 = 0;
    }
    m->x3A = 2;
    m->x3D = 3;
    m->x71 = 0;
    m->x72 = 0;
    m->x73 = 0;
    m->x7D = 0;
    m->x7E = 0;
    m->x7F = 0;
    m->x3B = 0;
    m->x78 = 0x80;
    m->x70 = m->x3E;
    f = *(u32 *)Moji_flag | 0x80000000;
    m->xC0 = 0;
    m->xBE = 0;
    m->x38 = 0;
    m->xBC = 0;
    m->script = m->script2;
    *(u32 *)Moji_flag = f | (0x8000000 >> no);
    return 1;
}

void MojiTaskExec2(s32 arg0, u8 arg1) {
    MojiTaskExec(arg0, (u8*)0x80153000, arg1);
}

s32 MojiTaskKill(void) {
    MOJI_TASK *m;
    u32 acc = 0;

    for (m = Moji_work; m < &Moji_work[5]; m++) {
        u32 f = m->flags;
        m->flags = 0;
        m->xC2 = 0xFF;
        acc |= f;
    }
    if (acc & 0x40000) {
        func_8001D494(0, 1, 0);
    }
    *(u32 *)Moji_flag = 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80053AA4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80053B40);

void func_800542FC(MOJI_TASK *m) {
    m->script += 1;
}

void func_80054310(MOJI_TASK *m) {
    m->script += 2;
}

void func_80054324(MOJI_TASK *m) {
    m->script += 3;
}

void func_80054338(MOJI_TASK *m) {
    m->script += 4;
}

void func_8005434C(MOJI_TASK *m) {
    m->script += 5;
}

void func_80054360(MOJI_TASK *m) {
    m->script += 6;
}

void func_80054374(MOJI_TASK *m) {
    m->script += 7;
}

void func_80054388(MOJI_TASK *m) {
    m->script += 9;
}

void func_8005439C(MOJI_TASK *m) {
    m->script += 11;
}

s32 func_800543B0(MOJI_TASK *m) {
    m->script2 += 1;
    return 1;
}

s32 func_800543C8(MOJI_TASK *m) {
    m->script2 += 2;
    return 1;
}

s32 func_800543E0(MOJI_TASK *m) {
    m->script2 += 3;
    return 1;
}

s32 func_800543F8(MOJI_TASK *m) {
    m->script2 += 5;
    return 1;
}

s32 func_80054410(u8 *p) {
    return p[0] | (p[1] << 8);
}

s32 func_80054424(u8 *p) {
    return p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054450);

s32 func_8005457C(MOJI_TASK *m) {
    m->x3B += 1;
    m->script2 += 1;
    return 1;
}

void func_8005459C(MOJI_TASK *m) {
    m->x10 = m->x8;
    m->x12 += 0xC;
    m->x3E = m->x70;
    m->script += 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800545C8);

void func_80054694(MOJI_TASK *m) {
    m->x3E = m->script[1];
    m->script += 2;
}

s32 func_800546B0(MOJI_TASK *m) {
    s32 v = func_80054410(m->script2 + 1);
    m->x3F = v;
    m->x4 = (u8)v;
    m->script2 += 3;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054700);

s32 func_80054798(MOJI_TASK *m) {
    m->x8 = func_80054410(m->script2 + 1);
    m->xA = func_80054410(m->script2 + 3);
    m->x7E = m->script2[5];
    m->x7F = m->script2[6];
    m->script2 += 7;
    return 1;
}

s32 func_80054804(MOJI_TASK *m) {
    u8 *s = m->script2;
    u32 f;

    m->flags |= 0x10000000;
    f = Moji_work[s[1]].flags;
    if ((s32)f >= 0 || (f & 0x20000)) {
        m->script2 = s + 2;
        return 1;
    }
    return 0;
}

s32 func_80054874(MOJI_TASK *m) {
    Sound_call((u16)func_80054410(m->script2 + 1), 0, 0);
    m->script2 += 3;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800548C4);

s32 func_8005497C(MOJI_TASK *m) {
    u16 sp = m->xC0;
    m->xC0 = sp + 1;
    m->stack2[sp] = m->script2 + 2;
    if (m->x44 != 0) {
        m->script2 = m->x44 + ((u16 *)m->x44)[m->script2[1]];
    } else {
        m->script2 = D_8008CACC[m->script2[1]];
    }
    return 1;
}

void func_80054A04(MOJI_TASK *m) {
    u16 sp = m->xBE;
    m->xBE = sp + 1;
    m->stack[sp] = m->script + 2;
    if (m->x44 != 0) {
        m->script = m->x44 + ((u16 *)m->x44)[m->script[1]];
    } else {
        m->script = D_8008CACC[m->script[1]];
    }
}

s32 func_80054A84(MOJI_TASK *m) {
    m->script2 = m->stack2[--m->xC0];
    return 1;
}

void func_80054AB4(MOJI_TASK *m) {
    m->script = m->stack[--m->xBE];
}

extern s32 (*D_8008AAC0[])();

s32 func_80054ADC(MOJI_TASK *m) {
    u32 f = m->flags;
    u8 c;

    if (f & 0x2000000) {
        m->flags = f & ~0x2000000;
        m->x7D = 0;
    }
    c = m->script2[1];
    m->x7C = c;
    D_8008AAC0[c]();
    return 0;
}

extern s32 (*D_8008AB08[])();

void func_80054B4C(MOJI_TASK *m) {
    D_8008AB08[m->x7C]();
}

s32 func_80054B88(MOJI_TASK *m) {
    m->flags |= MOJI_TASK0_ON;
    m->script2 += 1;
    return 1;
}

s32 func_80054BAC(void) {
    return 0;
}

s32 func_80054BB4(MOJI_TASK *m) {
    u8 c = m->x72 + 1;
    u32 f = m->flags | 0x400800;
    u8 *s = m->script2 + 3;
    m->xBC = 0;
    m->flags = f;
    m->x72 = c;
    m->script2 = s;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054BEC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054E54);

void func_80055304(MOJI_TASK *m) {
    m->script += m->x72 + 2;
}

s32 func_8005531C(MOJI_TASK *m) {
    m->flags &= ~MOJI_TASK0_ON;
    m->script2 += 1;
    return 1;
}

s32 func_80055344(MOJI_TASK *m) {
    u8 *s = m->script2;
    u8 *base = m->x44;
    u8 c1 = s[1];
    u8 c2 = s[2];

    m->script2 = s + 3;
    if (base != 0) {
        MojiTaskExec((s8)c1, base, c2);
    } else {
        MojiTaskExec((s8)c1, D_8008CACC[c2], 0xFF);
    }
    return 1;
}

s32 func_800553A8(MOJI_TASK *m) {
    Sce_flag_on((u16)func_80054410(m->script2 + 1));
    m->script2 += 3;
    return 1;
}

s32 func_800553F0(MOJI_TASK *m) {
    Sce_flag_off((u16)func_80054410(m->script2 + 1));
    m->script2 += 3;
    return 1;
}

s32 func_80055438(MOJI_TASK *m) {
    u8 b;
    u8 *base;

    if (Sce_flag_test((u16)func_80054410(m->script2 + 1))) {
        b = m->script2[3];
    } else {
        b = m->script2[4];
    }
    if (b == 0xFF) {
        m->script2 += 5;
    } else {
        m->xC2 = b;
        base = m->x44;
        m->x48 = m->script2 = base + ((u16 *)base)[b];
    }
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800554E4);

void func_800555F4(MOJI_TASK *m) {
    m->x10 += func_80054410(m->script + 1);
    m->script += 3;
}

s32 func_8005563C(MOJI_TASK *m) {
    m->flags |= 0x20000000;
    m->script2 += 1;
    return 1;
}

s32 func_80055660(MOJI_TASK *m) {
    m->flags &= ~0x20000000;
    m->script2 += 1;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055688);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800557B8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055868);

s32 func_80055A78(MOJI_TASK *m) {
    u8 *s = m->script2;
    u8 b;
    u8 *base;

    if (Game_work.stage_no == s[1] && Game_work.area_no == s[2]) {
        b = s[3];
    } else {
        b = m->script2[4];
    }
    if (b == 0xFF) {
        m->script2 += 5;
    } else {
        m->xC2 = b;
        base = m->x44;
        m->x48 = m->script2 = base + ((u16 *)base)[b];
    }
    return 1;
}

s32 func_80055B14(MOJI_TASK *m) {
    if ((u16)func_80054410(m->script2 + 1)) {
        Game_logo_kill((s16)func_80054410(m->script2 + 3));
    } else {
        s32 arg = func_80054410(m->script2 + 3) & 0xFFFF;
        func_80063FC0(arg, func_80054424(m->script2 + 5));
    }
    m->script2 += 9;
    return 1;
}

s32 func_80055BB0(MOJI_TASK *m) {
    u32 f = m->flags;

    if (f & 0x4000000) {
        m->flags = f | 0x100000;
        D_8008AAC0[m->x7C]();
        return 0;
    }
    m->script2 += 1;
    return 1;
}

s32 func_80055C1C(MOJI_TASK *m) {
    if (m->flags & 0x10000) {
        func_80039E18();
        m->flags &= ~0x10000;
    }
    m->script2 += 1;
    return 1;
}

extern u8 *D_8008D0D4[];

s32 func_80055C80(MOJI_TASK *m) {
    u8 *p = m->script2;
    *D_8008D0D4[p[1]] = p[2];
    m->script2 += 3;
    return 1;
}

s32 func_80055CC4(MOJI_TASK *m) {
    u8 c = m->x72 + 1;
    u32 f = m->flags | 0x404000;
    u8 *s = m->script2 + 7;
    m->xBC = 0;
    m->flags = f;
    m->x72 = c;
    m->script2 = s;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055CFC);

void func_800560D0(MOJI_TASK *m) {
    u8 *p = m->script;
    if (D_8008D0D4[p[1]][0] != p[2]) {
        m->x78 = 0x40;
    } else {
        m->x78 = 0x80;
    }
    m->script += 3;
}

s32 func_80056128(MOJI_TASK *m) {
    m->x3B += 1;
    m->script2 += 2;
    return 1;
}

void func_80056148(MOJI_TASK *m) {
    m->x10 = m->x8;
    m->x12 += m->script[1];
    m->x3E = m->x70;
    m->script += 2;
}

void func_80056180(MOJI_TASK *m) {
    m->x78 = m->script[1];
    m->script += 2;
}

s32 func_8005619C(MOJI_TASK *m) {
    if (func_8005DA78(1)) {
        m->script2 += 1;
        return 1;
    }
    return 0;
}

s32 func_800561E8(MOJI_TASK *m) {
    if (func_8005DAEC(1)) {
        m->script2 += 1;
        return 1;
    }
    return 0;
}

s32 func_80056234(MOJI_TASK *m) {
    if (func_8005DA1C(1)) {
        m->script2 += 1;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056280);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800563A8);

void func_800564C8(MOJI_TASK *m) {
    u8 *base = m->x44;

    if (base != 0) {
        MojiTaskExec(m->script[1], base, (u8)(m->script[2] + m->x71));
    } else {
        MojiTaskExec(m->script[1], D_8008CACC[m->script[2] + (s8)m->x71], 0xFF);
    }
    m->script += 3;
}

void func_80056558(MOJI_TASK *m) {
    u8 *base = m->x44;

    if (base != 0) {
        MojiTaskExec(m->script[1], base,
                     (u8)(m->script[2] + D_800BE2F8[(s8)m->x71 + D_8009899C]));
    } else {
        MojiTaskExec(m->script[1],
                     D_8008CACC[m->script[2] + (s8)D_800BE2F8[(s8)m->x71 + D_8009899C]],
                     0xFF);
    }
    m->script += 3;
}

void func_80056610(MOJI_TASK *m) {
    u8 *base = m->x44;

    if (base != 0) {
        MojiTaskExec(m->script[1], base,
                     (u8)(m->script[2] +
                          Player_work.remap454[(s8)m->x71 + D_8009899C] - 1));
    } else {
        MojiTaskExec(m->script[1],
                     D_8008CACC[m->script[2] +
                                Player_work.remap454[(s8)m->x71 + D_8009899C] - 1],
                     0xFF);
    }
    m->script += 3;
}

void func_800566CC(MOJI_TASK *m) {
    u8 *base = m->x44;

    if (base != 0) {
        MojiTaskExec(m->script[1], base,
                     (u8)(m->script[2] + Player_work.remap450[(s8)m->x71] - 1));
    } else {
        MojiTaskExec(m->script[1],
                     D_8008CACC[m->script[2] + Player_work.remap450[(s8)m->x71] - 1],
                     0xFF);
    }
    m->script += 3;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056778);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056820);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056D10);

s32 func_800570B0(MOJI_TASK *m) {
    u8 *s = m->script2;
    u8 c1 = s[2];
    u8 c2 = s[3];
    u8 key = s[1]; /* key/flag after c1/c2: init order pins key=$a0, flag=$v1 */
    u8 flag = Moji_flag[0];

    m->script2 = s + 4;
    if (flag == key) {
        u8 *base = m->x44;
        if (base != 0) {
            MojiTaskExec((s8)c1, base, c2);
        } else {
            MojiTaskExec((s8)c1, D_8008CACC[c2], 0xFF);
        }
    }
    return 1;
}

s32 func_80057124(MOJI_TASK *m) {
    s32 zenny = Game_work.zennyCount;
    m->script2 += 1;
    m->x40 = zenny;
    return 1;
}

void func_80057144(MOJI_TASK *m) {
    m->x40 = Game_work.zennyCount;
    func_8005BF10(0, m->x40, m->script += 1);
}

s32 func_80057184(MOJI_TASK *m) {
    u8 *s = m->script2;
    u8 idx = s[1];
    u8 b2 = s[2];
    u8 b3 = s[3];
    u8 b4 = s[4];

    m->script2 = s + 5;
    if (*D_8008D0D4[idx] == b2) {
        u8 *base = m->x44;
        if (base != 0) {
            MojiTaskExec((s8)b3, base, b4);
        } else {
            MojiTaskExec((s8)b3, D_8008CACC[b4], 0xFF);
        }
    }
    return 1;
}

s32 func_8005721C(MOJI_TASK *m) {
    D_80098930 = D_800BE2F8[D_8009899C + (s8)m->x71] + m->script2[1];
    if (Sce_flag_test(0x2EE) == 0 && func_8004327C(-D_8008FD3C[D_80098930])) {
        D_80098B6C = 1;
    } else {
        D_80098B6C = 0;
    }
    m->script2 += 2;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800572C8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057408);

void func_8005753C(MOJI_TASK *m) {
    u8 *p = m->script;
    if (p[0] == 0xD5) {
        m->script = p + (p[1] + 2);
    } else {
        m->script = p + 3;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057574);

extern u8 D_80098830;
extern u8 *D_8008CBA4[];

s32 func_800576C4(MOJI_TASK *m) {
    m->stack2[m->xC0++] = m->script2 + 1;
    m->script2 = D_8008CBA4[D_80098830];
    return 1;
}

void func_80057708(MOJI_TASK *m) {
    m->stack[m->xBE++] = m->script + 1;
    m->script = D_8008CBA4[D_80098830];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057744);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800577FC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057924);

void func_800579D8(MOJI_TASK *m) {
    func_8005BF10(1, (Game_work.x7D - Game_work.x7C) * 10, m->script += 1);
}

s32 func_80057A24(MOJI_TASK *m) {
    m->stack2[m->xC0++] = m->script2 + 1;
    Sce_flag_on(D_80098830 | 0x500);
    m->script2 = D_8008CBA4[D_80098830];
    return 1;
}

void func_80057A94(MOJI_TASK *m) {
    m->stack[m->xBE++] = m->script + 1;
    m->script = D_8008CBA4[D_80098830];
}

s32 func_80057AD0(MOJI_TASK *m) {
    m->stack2[m->xC0++] = m->script2 + 2;
    m->script2 = D_8008CBA4[m->script2[1]];
    return 1;
}

void func_80057B24(MOJI_TASK *m) {
    m->stack[m->xBE++] = m->script + 2;
    m->script = D_8008CBA4[m->script[1]];
}

// func_80043294 is called for effect only, but declaring it s32 (its real
// return type) keeps $v0 reserved across the call so script2 lands in $v1
// and the `return 1` fills the load-delay slot — matches the original.
s32 func_80043294(s32, s32, s32);

s32 func_80057B70(MOJI_TASK *m) {
    func_80043294(D_80098AF4, 0, 0);
    m->script2 += 1;
    return 1;
}

s32 func_80057BB4(MOJI_TASK *m) {
    func_80043294(-D_80098AF4, 0, 0);
    m->script2 += 1;
    return 1;
}

s32 func_80057BFC(MOJI_TASK *m) {
    return func_8005BF10(1, D_80098AF4, m->script += 1);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057C2C);

s32 func_80057D00(MOJI_TASK *m) {
    m->stack2[m->xC0++] = m->script2 + 2;
    m->script2 = D_8008CE10[D_800BE2F8[m->script2[1]]];
    return 1;
}

void func_80057D60(MOJI_TASK *m) {
    m->stack[m->xBE++] = m->script + 2;
    m->script = D_8008CE10[D_800BE2F8[m->script[1]]];
}

extern u8 D_800BE2F8[];

s32 func_80057DB8(MOJI_TASK *m) {
    u32 f = *(u32*)Moji_flag;
    u32 hi = f & ~0xFF;
    *(u32*)Moji_flag = hi;
    *(u32*)Moji_flag = hi | D_800BE2F8[f & 0xFF];
    m->script2 += 1;
    return 1;
}

s32 func_80057DF4(MOJI_TASK *m) {
    u32 i; /* sltiu in the original */

    for (i = 0; i < 3; i++) {
        /* the cast keeps &x84_tbl (GW+0x84) as the materialized base with
           vals' +4 in the lhu offset; direct .x84_tbl[..].vals[i] folds the
           +4 into the symbol addend (different bytes) */
        m->script2[i + 1] = ((GW84_ENTRY *)&Game_work.x84_tbl)[D_800989D4].vals[i];
    }
    m->script2 += 1;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057E50);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057FF8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005851C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005861C);

void func_80058740(MOJI_TASK *m) {
    u8 *base;
    m->stack[m->xBE++] = m->script + 5;
    base = m->x44;
    m->script = base + *(u16*)(base + m->xC2 * 2);
}

s32 func_80058788(MOJI_TASK *m) {
    u32 f = m->flags | 0x400800; /* f-before-n order pins the lui pair order */
    s32 n = m - Moji_work;
    u8 c = m->x72 + 1;
    u8 *s = m->script2 + 5;

    m->xBC = 0;
    m->flags = f;
    m->x72 = c;
    m->script2 = s;
    Moji_flag3 &= ~(0x20000 << n);
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800587F8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058AEC);

s32 func_80058C08(MOJI_TASK *m) {
    m->x3D = m->script2[1];
    m->script2 += 2;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058C28);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058CC8);

void func_80058D64(MOJI_TASK *m) {
    m->x10 = func_80054410(m->script + 1);
    m->x12 = func_80054410(m->script + 3);
    m->script += 5;
}

extern u8 D_800BE377;
extern u8 D_8009899C;

s32 func_80058DB4(MOJI_TASK *m) {
    u8 c;
    s32 i;
    u8 *p;

    c = 0xFF;
    i = 0x7F;
    p = &D_800BE377;
    D_8009899C = 0;
    m->script2 += 1;
    for (; i >= 0; i--) { *p-- = c; }
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058DEC);

extern u8 D_80098851;

s32 func_80058EA0(MOJI_TASK *m) {
    u32 f = m->flags | 0x402800; /* f-before-n order pins the lui pair order */
    s32 n = m - Moji_work;
    u8 c = m->x72 + 1;
    u8 *s = m->script2 + 5;

    m->xBC = 0;
    m->flags = f;
    m->x72 = c;
    m->script2 = s;
    D_80098851 = 0xFF;
    Moji_flag3 &= ~(0x20000 << n);
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058F18);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800592FC);

s32 func_800594CC(MOJI_TASK *m) {
    m->stack2[m->xC0++] = m->script2 + 1;
    m->script2 = D_8008CCA4[D_800BE2F7[D_8009899C + m->x72]];
    return 1;
}

void func_80059530(MOJI_TASK *m) {
    m->stack[m->xBE++] = m->script + 1;
    m->script = D_8008CCA4[D_800BE2F7[D_8009899C + m->x73]];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005958C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059660);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059728);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800598C8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059B44);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059D20);

void func_80059E24(MOJI_TASK *m) {
    s32 i = m->script[1];
    m->stack[m->xBE++] = m->script + 2;
    m->script = D_8008CE5C[D_800BE2F8[i]];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059E74);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005A598);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005A634);

void func_8005A858(MOJI_TASK *m) {
    m->x10 += 0xC;
    m->script += 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005A874);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005AB78);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005ACA8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005B454);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005B5D8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005B85C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005B98C);

void func_8005BC90(MOJI_TASK *m) {
    if (D_80098912[0] == 1) {
        m->x7C = 0;
    } else {
        m->x7C = 4;
    }
    func_8005ACA8(m);
    m->x7C = 7;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005BCE4);
