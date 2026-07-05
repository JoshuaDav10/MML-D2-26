#include "rock_neo.h"
#include "rock_neo/moji.h"
#include "rock_neo/game.h"

extern s32 D_80098AF4; // sdata ($gp)
s32 func_8005BF10(s32, s32, u8*);

u8 Moji_flag[8]; // COMMON on purpose: splat carved 0x80098A58 out of the
                 // extracted data for C to provide; gprel.py gp-rewrites
                 // refs to small .comm symbols in the census (see tool)

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80053788);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", MojiTaskExec);

void MojiTaskExec2(s32 arg0, u8 arg1) {
    MojiTaskExec(arg0, 0x80153000, arg1);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", MojiTaskKill);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800546B0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054700);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054798);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054804);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054874);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800548C4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005497C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054A04);

s32 func_80054A84(MOJI_TASK *m) {
    m->script2 = m->stack2[--m->xC0];
    return 1;
}

void func_80054AB4(MOJI_TASK *m) {
    m->script = m->stack[--m->xBE];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80054ADC);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055344);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800553A8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800553F0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055438);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800554E4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800555F4);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055A78);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055B14);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055BB0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055C1C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80055C80);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800560D0);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005619C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800561E8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056234);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056280);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800563A8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800564C8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056558);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056610);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800566CC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056778);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056820);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80056D10);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800570B0);

s32 func_80057124(MOJI_TASK *m) {
    s32 zenny = Game_work.zennyCount;
    m->script2 += 1;
    m->x40 = zenny;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057144);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057184);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005721C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800572C8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057408);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005753C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057574);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800576C4);

extern u8 D_80098830;
extern u8 *D_8008CBA4[];

void func_80057708(MOJI_TASK *m) {
    m->stack[m->xBE++] = m->script + 1;
    m->script = D_8008CBA4[D_80098830];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057744);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800577FC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057924);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800579D8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057A24);

void func_80057A94(MOJI_TASK *m) {
    m->stack[m->xBE++] = m->script + 1;
    m->script = D_8008CBA4[D_80098830];
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057AD0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057B24);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057B70);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057BB4);

s32 func_80057BFC(MOJI_TASK *m) {
    return func_8005BF10(1, D_80098AF4, m->script += 1);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057C2C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057D00);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057D60);

extern u8 D_800BE2F8[];

s32 func_80057DB8(MOJI_TASK *m) {
    u32 f = *(u32*)Moji_flag;
    u32 hi = f & ~0xFF;
    *(u32*)Moji_flag = hi;
    *(u32*)Moji_flag = hi | D_800BE2F8[f & 0xFF];
    m->script2 += 1;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057DF4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057E50);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80057FF8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005851C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005861C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058740);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058788);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800587F8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058AEC);

s32 func_80058C08(MOJI_TASK *m) {
    m->x3D = m->script2[1];
    m->script2 += 2;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058C28);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058CC8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058D64);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058EA0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80058F18);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800592FC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800594CC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059530);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005958C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059660);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059728);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_800598C8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059B44);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059D20);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_80059E24);

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

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005BC90);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/moji", func_8005BCE4);
