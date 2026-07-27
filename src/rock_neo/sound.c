#include "common.h"

/* --- decls: parallel grind wave 2, 2026-07-26 --- */
/* func_80019D18 */
extern u16 *D_80082320[]; /* 0x80082320: per-stage u16 sound-code table ptrs */
/* 1-byte alias of Game_work: Game_work is large, so with -G8 its address is
   "expensive" and cc1 CSEs &Game_work+0x50 into a callee-saved register.
   The alias makes the address cheap, so cc1 emits independent bare refs and
   GAS -G0 expands them to the target's lui/%lo(Game_work+0x50) form.
   Game_work has no %gp_rel( hit in the extracted asm, so gprel.py drops the
   resulting `.extern Game_work,1` (no COMMON leak) and does not gp-rewrite. */
extern u8 Game_work_b __asm__("Game_work");

#define GWB(off, type) (*(type *)((u8 *)&Game_work_b + (off)))
/* both defined later in this file (lines ~172 / ~306); forward-declare */
void Sound_call(s32 code, s32 arg1, s32 arg2);

void func_8001B33C(void);

extern u8 D_80098AFD[];


extern u8 D_8008241C[];


void func_8001AEF0(void);

/* --- func_8001A744: SPU key-on from a VAB tone attribute --- */
typedef struct {
    s16 left;
    s16 right;
} SpuVolume;

typedef struct {
    u32 voice;
    u32 mask;
    SpuVolume volume;
    SpuVolume volmode;
    SpuVolume volumex;
    u16 pitch;
    u16 note;
    u16 sample_note;
    s16 envx;
    u32 addr;
    u32 loop_addr;
    s32 a_mode;
    s32 s_mode;
    s32 r_mode;
    u16 ar;
    u16 dr;
    u16 sr;
    u16 rr;
    u16 sl;
    u16 adsr1;
    u16 adsr2;
} SpuVoiceAttr; /* 0x40 — PSYQ libspu voice attributes */

typedef struct {
    u8 prior, mode, vol, pan;
    u8 center, shift, min, max;
    u8 vibW, vibT, porW, porT;
    u8 pbmin, pbmax, reserved1, reserved2;
    u16 adsr1;
    u16 adsr2;
    s16 prog;
    s16 vag;
    s16 reserved[4];
} VagAtr; /* 0x20 — PSYQ libsnd tone attribute (matches caller's 0x20 stride) */

typedef struct {
    u8 x0;
    u8 x1;
    u8 x2; /* low nibble -> D_80098B48[ch] */
    u8 x3; /* low 5 bits -> SPU voice number */
} SND_VOICE; /* 0x4 — entry of D_8008241C */

extern u8 D_80098B48[];

void func_8001A274(void *, u16, s16 *, s16 *, s16);

s32 func_8001A6DC(s32, s32);

s32 SsUtGetVagAddr(s16, s16);

void SpuSetKeyOnWithAttr(SpuVoiceAttr *);

extern u16 *D_80082320[];

extern u8 D_800C356E;

extern u8 D_800C356F;

void func_8001D854(u8);

/* --- decls: parallel grind wave 1, 2026-07-26 --- */
/* func_8001AE6C */
extern u8 D_80098840;   /* fade step; written by func_80019DE0 (still INCLUDE_ASM) */
/* func_8001B33C */
void func_80070114(s16);
void func_8007013C(s16, u8);

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
extern s16 D_8008222C;
extern s16 D_80082278;

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

void func_80019918(void) {
    s32 v;
    if (Sce_flag_test(0x204) != 0) {
        if (Sce_flag_test(0x206) == 0) {
            v = 0x39;
            goto done;
        }
    }
    if (Sce_flag_test(0x212) == 0) {
        v = 0x7;
        goto done;
    }
    v = Sce_flag_test(0x213);
    if (v != 0) v = 0x7;
    else v = 0x39;
done:
    D_8008222C = v;
}

void func_8001997C(void) {
    if (Game_work[0x53] == 1) {
        D_80082274 = 0x29;
    } else {
        D_80082274 = 0x108;
    }
}

void func_800199A4(void) {
    s8 v = Game_work[0x53];
    s16 out;
    switch (v) {
    case 1:
    case 2:
    case 3:
        if (Sce_flag_test(0x30) == 0) {
            out = 0x16;
            break;
        }
        /* fallthrough */
    default:
        out = 0x108;
        break;
    }
    D_80082278 = out;
}

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

void func_80019C20(void) {
    void (*fn)(s32);
    u16 *p;
    u16 cur;
    s32 chg;

    fn = Unk_stage_func_tbl[D_800C356E];
    if (fn != 0) {
        fn(1);
    }

    chg = 0;
    p = D_80082320[D_800C356E];
    if (p == 0) {
        chg = 1;
    } else {
        cur = p[D_800C356F];
        if (cur == 0xFFFF) {
            chg = 1;
        } else {
            p = D_80082320[Game_work[0x50]];
            if (p[Game_work[0x51]] != cur) chg = 1;
        }
    }
    if (chg != 0) {
        func_8001D854(0x10);
        D_80098958 |= 0x800;
    }
}

void func_80019D18(void) {
    void (*fn)(s32);
    u16 *p;
    u16 code;
    s32 ok;

    fn = Unk_stage_func_tbl[GWB(0x50, s8)];
    if (fn != 0) {
        fn(0);
    }

    p = D_80082320[GWB(0x50, s8)];
    code = 0;
    ok = 0;
    if (p != 0) {
        code = p[GWB(0x51, s8)];
        ok = code != 0xFFFF;
    }
    if (ok) {
        Sound_call(code, 0, 0);
    } else {
        func_8001B33C();
    }
    D_80098958 &= ~0x800;
}

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

/* moved below the SND_CMD typedef (it is defined mid-file, not at the top) */
extern SND_CMD D_800BE4F8[];
extern void (*D_80082C64[])(SND_CMD *);

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

extern s16 D_80098AF8[]; // seq table, stride 8: {s16 seq, s8 flag, u8 x3, u8 vol, ...}
extern s8 D_80098AFA[];
extern u8 D_80098AFC[];
s16 SsIsEos(s16, s16);

s32 func_8001A0A8(s32 n) {
    s32 ret = 0;

    if (D_80098AFA[n << 3] != -1) {
        s16 seq = D_80098AF8[n << 2];
        if (seq != -1) {
            ret = SsIsEos(seq, D_80098AFC[n << 3]);
        }
    }
    return ret;
}

// Sound frame tick: poll the 4 seq slots, then drain the command queue by
// dispatching each entry through a 4-entry handler table selected by the top
// 2 bits of a per-sound-code attribute byte.
// Two IVs (i and off) keep the D_80098AFD store in the symbol-indexed
// lui/$at + addu + sb form (COMPILER_IDIOMS s4: one IV would pointer-march).
void func_8001A110(void) {
    SND_CMD *q;
    s32 i, off;

    if (D_80098958 != 0) {
        func_8001AEF0();
        for (i = 0, off = 0; i < 4; i++, off += 8) {
            D_80098AFD[off] = func_8001A0A8(i);
        }
        q = D_800BE4F8;
        if (D_80098938 != q) {
            do {
                D_80082C64[D_8008241C[q->x2 * 4] >> 6](q);
                q++;
            } while (q != D_80098938);
        }
        D_80098938 = D_800BE4F8;
    }
}

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

extern s32 D_80082C70[];
extern s32 D_80082CA0[];

s32 func_8001A63C(s32 arg0) {
    s32 a1 = 0x1000;
    s32 v0;
    if (arg0 >= 0) {
        v0 = D_80082C70[arg0 % 12];
        a1 <<= arg0 / 12;
    } else {
        arg0 = -arg0;
        v0 = D_80082CA0[arg0 % 12];
        a1 >>= arg0 / 12;
    }
    a1 = a1 * v0;
    return a1 >> 16;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A6DC);

void func_8001A744(s32 vabId, SND_VOICE *v, VagAtr *tone, void *src) {
    SpuVoiceAttr attr;
    s32 ch;

    ch = v->x3 & 0x1F;
    func_8001A274(src, tone->vol, &attr.volume.left, &attr.volume.right,
                  ((s16 *)&Game_work[0x1E0])[ch]);
    attr.voice = 1 << ch;
    attr.mask = 0x60093;
    attr.addr = SsUtGetVagAddr(vabId, tone->vag);
    attr.pitch = func_8001A6DC(tone->min - tone->center, tone->shift);
    attr.adsr1 = tone->adsr1;
    attr.adsr2 = tone->adsr2;
    D_80098B48[ch] = v->x2 & 0xF;
    SpuSetKeyOnWithAttr(&attr);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A834);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001A968);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AA8C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AC58);

s32 func_8001AE6C(s32 arg0, s16 *arg1) {
    s32 ret = 0;

    if (arg0 & 3) {
        ret = 1;
        if (arg0 & 1) {
            *arg1 -= D_80098840;
            if (*arg1 <= 0) {
                *arg1 = 0;
                ret = 0;
            }
        } else {
            *arg1 += D_80098840;
            if (*arg1 >= 0x7F) {
                *arg1 = 0x7F;
                ret = 0;
            }
        }
    }
    return ret;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sound", func_8001AEF0);

void func_8001B2F0(s16 arg0) {
    func_80071220(arg0);
}

void func_8001B314(void) {
    SpuSetKey(0, 0xFF8000);
}

void func_8001B33C(void) {
    s32 i;
    s16 *seqp;
    u8 *volp;
    s16 seq;
    s8 f;

    seqp = D_80098AF8;
    for (i = 0; i < 4; i++, seqp += 4) {
        volp = &D_80098AFC[i * 8];
        seq = *seqp;
        if (seq != -1) {
            f = ((s8 *)volp)[-2];
            if (f != -1) {
                if (f == 0) {
                    func_80070114(seq);
                } else {
                    func_8007013C(seq, *volp);
                }
            }
        }
    }
}
