#include "common.h"

s32 func_80071220();
void SpuSetKey(s32, u32);

extern s32 D_800821F8[];
extern s32 *D_800AD15C[];
extern s16 D_80082274;
extern s16 D_800822D6;
extern s16 D_80082218;
s32 Sce_flag_test(s32);
extern void (*Unk_stage_func_tbl[])(s32);
extern s32 D_80098958; // lui-accessed (not sdata)
extern s8 Game_work[]; // this TU only reads byte x53; full type in game.h

extern s16 D_8008222A;

void func_800198C0(void) {
    s32 a;
    s32 i;
    s16 *p;

    a = (Sce_flag_test(6) == 0 || Sce_flag_test(0x2C) != 0) ? 7 : 2;
    i = 5;
    p = &D_8008222A;
    do {
        *p-- = a;
    } while (--i >= 0);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019918);

void func_8001997C(void) {
    if (Game_work[0x53] == 1) {
        D_80082274 = 0x29;
    } else {
        D_80082274 = 0x108;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_800199A4);

extern s16 D_800822A6;

void func_800199F8(void) {
    s16 *p;
    s32 i;
    s16 v = (Game_work[0x53] == 1) ? 0x17 : 0x108;
    i = 5;
    p = &D_800822A6;
    for (; i >= 0; i--) {
        *p-- = v;
    }
}

extern s16 D_800822B0;

void func_80019A34(void) {
    s16 *p;
    s32 i;
    s16 v = (Game_work[0x53] == 1) ? 0x17 : 0x108;
    i = 4;
    p = &D_800822B0;
    for (; i >= 0; i--) {
        *p-- = v;
    }
}

void func_80019A70(void) {
    if (Sce_flag_test(0x1E1)) {
        D_800822D6 = 0x25;
    } else {
        D_800822D6 = 0x30;
    }
}

extern u16 D_800822F2;
extern u16 D_800822F4;

void func_80019AA4(void) {
    s32 v = Sce_flag_test(0x35);
    if (v != 0) v = 0x2D;
    else v = 0xFFFF;
    D_800822F2 = v;
    D_800822F4 = v;
}

void func_80019AE0(void) {
    if (Sce_flag_test(0x1E1)) {
        D_80082218 = 0x25;
    } else {
        D_80082218 = 0x30;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019B14);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019C20);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019D18);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_80019DE0);

void func_80019F94(s32 arg0) {
    D_800AD15C[0] = &D_800821F8[arg0];
}

void func_80019FB4(void) {
    void (*fn)(s32) = Unk_stage_func_tbl[Game_work[0x50]];
    D_80098958 |= 0x800;
    if (fn != 0) {
        fn(0);
    }
}

typedef struct {
    s8 x0;    // cleared on enqueue
    s8 x1;
    s16 x2;   // sound code
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
} SND_CMD; // 0x14 — sound command queue entry

extern SND_CMD *D_80098938;
extern SND_CMD D_800BE6D8; // queue-full sentinel slot

void Sound_call(s32 code, s32 arg1, s32 arg2) {
    SND_CMD *q;

    if (D_80098938 != &D_800BE6D8) {
        D_80098938->x0 = 0;
        q = D_80098938;
        q->x2 = code;
        q->x4 = arg1;
        q->x8 = arg2;
        D_80098938 = q + 1;
    }
}

void Sound_call2(s32 code, s32 *args) {
    SND_CMD *q;

    if (D_80098938 != &D_800BE6D8) {
        D_80098938->x0 = 1;
        q = D_80098938;
        q->x2 = code;
        q->x4 = args[0];
        q->x8 = args[1];
        /* q++ (not q + 1) — the in-place increment's anti-dependence stops
           cc1's scheduler hoisting the queue-pointer store above q->x8 */
        q++;
        D_80098938 = q;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A0A8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A110);

// 5-entry table at Game_work+0x1BA, stride 8; the separate `off` variable
// (not i*8) keeps the per-iteration lui/$at symbol-indexed lbu form
s32 func_8001A1FC(s32 key) {
    s32 i, off;
    i = 0;
    for (off = 0; i < 5; i++, off += 8) {
        if (((u8*)Game_work)[0x1BA + off] == key) return i;
    }
    return -1;
}

extern u8 D_80098AD0[];

s32 func_8001A238(s32 key) {
    s32 i, off;
    i = 0;
    for (off = 0; i < 5; i++, off += 8) {
        if (D_80098AD0[off] == key) return i;
    }
    return -1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A274);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A63C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A6DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A744);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A834);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A968);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AA8C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AC58);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AE6C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AEF0);

void func_8001B2F0(s16 arg0) {
    func_80071220(arg0);
}

void func_8001B314(void) {
    SpuSetKey(0, 0xFF8000);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001B33C);
