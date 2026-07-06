#include "types.h"

/* ---- scratch-TU-local MOJI_TASK mirror (matches include/rock_neo/moji.h) ---- */
typedef struct MOJI_TASK {
    u32 flags;    // 0x00
    u16 x4;       // 0x04
    u16 x6;       // 0x06
    u16 x8;       // 0x08
    u16 xA;       // 0x0A
    u16 xC;       // 0x0C
    u16 xE;       // 0x0E
    u16 x10;      // 0x10
    s16 x12;      // 0x12
    u8* script;   // 0x14
    u8* stack[8]; // 0x18
    u16 x38;      // 0x38
    u8 x3A;       // 0x3A
    u8 x3B;       // 0x3B
    u8 x3C;       // 0x3C
    s8 x3D;       // 0x3D
    u8 x3E;       // 0x3E
    u8 x3F;       // 0x3F (read lbu everywhere)
    s32 x40;      // 0x40
    u8* x44;      // 0x44
    u8* x48;      // 0x48
    u8* stack2[8];// 0x4C
    u8* script2;  // 0x6C
    u8 x70;       // 0x70
    s8 x71;       // 0x71
    s8 x72;       // 0x72
    s8 x73;       // 0x73
    u8 x74[0x78 - 0x74];
    u8 x78;       // 0x78
    u8 x79[0x7C - 0x79];
    u8 x7C;       // 0x7C
    u8 x7D;       // 0x7D
    u8 x7E;       // 0x7E
    u8 x7F;       // 0x7F
    u8 x80[0xB8 - 0x80];
    u16 xB8;      // 0xB8
    u16 xBA;      // 0xBA
    u16 xBC;      // 0xBC
    u16 xBE;      // 0xBE
    u16 xC0;      // 0xC0
    u16 xC2;      // 0xC2
} MOJI_TASK;

/* draw-context (D_80098934 -> array); element ->x70 (OT tag) and ->x7C */
typedef struct DRAWCTX {
    u8 pad[0x70];
    u32 x70;
    u8 pad2[0x7C - 0x74];
    u32 x7C;
} DRAWCTX;

/* GPU primitive scratchpad at 0x1F800070 (SPRT-ish) */
typedef struct PRIM {
    u32 tag;      // 0x00
    u8  x3;       /* NOTE: byte writes at 0x3..0x7 below use raw offsets */
} PRIM;

typedef struct { s16 x, y, w, h; } RECT;

extern u32 Moji_flag[]; /* 0x80098A58 (as u32) */
extern MOJI_TASK Moji_work[]; /* 0x800BB6B8 */
extern s32 D_80098824;
extern s32 D_80098B2C;
extern s32 D_80098960;
extern u16 D_800C0C26;
extern u16 D_800C0C2A;
extern DRAWCTX *D_80098934;
extern RECT D_80097F50[]; /* SetDrawArea rect base, indexed [byte] stride 8 */
extern u16 D_80097F30[];
extern u8 D_8008AE7C[];

extern s32 (*D_8008A91C[])();
extern s32 (*D_8008AAC4[])();
extern s32 (*D_8008AAE4[])();

extern s32 func_8001D878();
extern s32 func_8001D494(s32, s32, s32);
extern void Sound_call(s32, s32, s32);
extern void SetDrawArea(u32 *, RECT *);
extern void SetDrawMode(u32 *, s32, s32, s32);
extern s32  GetTPage(s32, s32, s32, s32);

#define D_800BB9C8 (&Moji_work[4])
#define OTPTR (*(u32 * volatile *)0x1F800070) /* the current primitive fill pointer */
#define VRAM_ROW (*(u8 *)0x1F800000)    /* draw-area page selector */

void func_80053B40(void) {
    MOJI_TASK *m;
    u32 *prim;
    RECT rect;
    s32 c;
    s32 t;

    if ((Moji_flag[0] & 0x400000) || D_80098824) {
        return;
    }
    m = Moji_work;
    Moji_flag[0] &= 0x3043FFFF;
    D_80098B2C = 0;
    if (m >= &Moji_work[5]) {
        goto tail_env;
    }

    do {
        if ((s32)m->flags < 0) {
            /* --- active slot --- */
            c = ((s32)((u8 *)m - (u8 *)Moji_work) / 0xC4);
            Moji_flag[0] = (Moji_flag[0] | 0x80000000) | (0x8000000 >> c);

            if ((Moji_flag[0] & 0x40000) == 0
                && (Moji_flag[0] & (0x10000 >> m->x6)) == 0) {
                m->xB8 = D_800C0C26;
                m->xBA = D_800C0C2A;
            } else {
                m->xB8 = 0;
                m->xBA = 0;
            }

            if (!(m->flags & 0x8000000) && (m->xB8 & 0x1000)
                && (m->flags & 0x40000) && func_8001D878()) {
                m->flags &= 0xFFFBFFFF;
                m->flags = func_8001D494(0, 1, 0);
            }

            /* --- loop 1: advance/timing over script2 --- */
        loop1:
            c = m->script2[0];
            if ((u32)c >= 0x84) {
                /* control opcode */
                if (D_8008A91C[c](m)) {
                    goto loop1;
                }
            } else {
                m->x4 -= 1;
                if ((s16)m->x4 > 0) {
                    /* still counting down */
                } else {
                    if (!(m->flags & 0x8000000) && (m->xB8 & 0x1000)
                        && !(m->flags & 0x40000)) {
                        m->x4 = m->x3F >> 1;
                    } else {
                        m->x4 = m->x3F;
                    }
                    m->script2 += 1;
                    if (!(m->flags & 0x60000000)) {
                        if (!(m->flags & 0x40000)) {
                            Sound_call(0x84, 0, 0x64);
                        }
                        m->flags |= 0x40000000;
                    }
                }
                if ((s16)m->x4 == 0) {
                    m->flags |= 0x40000000;
                    goto loop1;
                }
            }
            /* fell out of loop1: decide whether to render */
            if (m->x3F == 0 && !(m->flags & 0x10000000)) {
                goto loop1;
            }

            /* --- render block --- */
            prim = OTPTR;
            OTPTR = prim + 3;   /* +0xC bytes */
            SetDrawArea(prim, &D_80097F50[VRAM_ROW]);

            prim[0] = (prim[0] & 0xFF000000) | (D_80098934[m->x3D].x70 & 0xFFFFFF);
            D_80098934[m->x3D].x70 =
                (D_80098934[m->x3D].x70 & 0xFF000000) | ((u32)prim & 0xFFFFFF);

            m->script = m->x48;
            m->x78 = 0x80;
            m->x10 = m->x8;
            m->x73 = 0;
            m->xBE = 0;
            m->x3E = m->x3C;
            m->x12 = m->xA + m->x38;

            if (m->script != m->script2) {
                do {
                    c = m->script[0];
                    if ((u32)c >= 0x84) {
                        /* glyph: build SPRT */
                        if (!(m->flags & 0x100000)) {
                            u32 *p = OTPTR;
                            u8 *pb = (u8 *)p;
                            OTPTR = (u32 *)((u8 *)p + 0x14);
                            pb[3] = 4;
                            pb[7] = 0x64;
                            pb[4] = m->x78;
                            pb[5] = m->x78;
                            pb[6] = m->x78;
                            *(s16 *)(pb + 8) = m->x10;
                            *(s16 *)(pb + 0x10) = 0xC;
                            *(s16 *)(pb + 0x12) = 0xC;
                            *(s16 *)(pb + 0xA) = m->x12;
                            *(s16 *)(pb + 0xE) = D_80097F30[m->x3E];
                            /* pb[0xC] = (ch % 0x15) * 0xC (ch reloaded each time) */
                            pb[0xC] = ((u8)(m->script[0] % 0x15)) * 0xC;
                            pb[0xD] = ((u8)(m->script[0] / 0x15)) * 0xC;
                            p[0] = (p[0] & 0xFF000000)
                                 | (D_80098934[m->x3D].x70 & 0xFFFFFF);
                            D_80098934[m->x3D].x70 =
                                (D_80098934[m->x3D].x70 & 0xFF000000)
                                | ((u32)p & 0xFFFFFF);
                            m->x10 += D_8008AE7C[m->script[0]];
                        }
                        m->script += 1;
                    } else {
                        D_8008AAC4[c](m);
                    }
                } while (m->script != m->script2);
            }

            /* --- post-render: second SetDrawArea + post dispatch --- */
        post_render:
            prim = OTPTR;
            rect.x = m->x8;
            rect.y = m->xA + (VRAM_ROW << 8);
            rect.w = m->x7E * 0xC;
            rect.h = m->x7F * 0xC;
            OTPTR = prim + 3;
            SetDrawArea(prim, &rect);

            prim[0] = (prim[0] & 0xFF000000) | (D_80098934[m->x3D].x70 & 0xFFFFFF);
            D_80098934[m->x3D].x70 =
                (D_80098934[m->x3D].x70 & 0xFF000000) | ((u32)prim & 0xFFFFFF);

            if (m->flags & 0x4000000) {
                D_8008AAE4[m->x7C](m);
            }
        } else {
            /* --- inactive: clear bit --- */
            Moji_flag[0] &= ~(0x10000 >> m->x6);
        }

        /* slot-4-skip: pack x71 low byte into D_80098B2C */
        if (m != D_800BB9C8) {
            t = ((s32)((u8 *)m - (u8 *)D_800BB9C8) / 0xC4);
            D_80098B2C |= (s32)m->x71 << (t << 3);
        }
        m->flags &= 0xAF7FFFFF;
        m++;
    } while (m < &Moji_work[5]);

tail_env:
    prim = OTPTR;
    OTPTR = prim + 3;
    t = GetTPage(0, 0, 0x3C0, 0x100);
    SetDrawMode(prim, 0, 0, t & 0xFFFF);
    prim[0] = (prim[0] & 0xFF000000) | (D_80098934->x7C & 0xFFFFFF);
    D_80098960 += 1;
    D_80098934->x7C = (D_80098934->x7C & 0xFF000000) | ((u32)prim & 0xFFFFFF);
}
