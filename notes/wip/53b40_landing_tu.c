#include "rock_neo.h"
#include "rock_neo/moji.h"
#include "rock_neo/sound.h"

extern s32 D_80098824;
extern s32 D_80098960;
extern s32 D_80098B2C;
extern u16 D_800C0C26;
extern u16 D_800C0C2A;
typedef struct DRAWCTX {
    u8 pad[0x70];
    u32 x70[3];
    u32 x7C;
} DRAWCTX;
extern DRAWCTX *D_80098934;
extern RECT D_80097F50[];
extern u16 D_80097F30[];
extern u8 D_8008AE7C[];
extern s32 (*D_8008A91C[])();
extern s32 (*D_8008AAC4[])();
extern s32 (*D_8008AAE4[])();
extern MOJI_TASK D_800BB9C8;
s32 func_8001D878(void);
void SetDrawArea(u32 *, RECT *);

void func_80053B40(void) {
    MOJI_TASK *m;
    MOJI_TASK *last;
    u32 *prim;
    u32 *pr;
    u32 **pp;
    u32 setflag;
    u32 tst;
    u32 fff;
    RECT rect;
    s32 c;
    s32 t;
    s32 op;
    u32 *pt;
    MOJI_TASK *mb;
    u32 f;
    u32 w;
    if ((*(u32*)Moji_flag & 0x400000) || D_80098824) {
        return;
    }
    m = Moji_work;
    *(u32*)Moji_flag &= 0x3043FFFF;
    D_80098B2C = 0;
    if (m >= &Moji_work[5]) {
        goto tail_env;
    }
    do {
        tst = 0x40000;
        setflag = 0x40000000;
        fff = 0xFFFFFF;
        if ((s32)m->flags < 0) {
            c = m - Moji_work;
            f = *(u32*)Moji_flag | 0x80000000;
            *(u32*)Moji_flag = f | (0x8000000 >> c);
            if ((*(u32*)Moji_flag & tst) == 0
                && (*(u32*)Moji_flag & (0x10000 >> (s16)m->x6)) == 0) {
                w = D_800C0C26;
                m->xB8 = w;
                w = D_800C0C2A;
                m->xBA = w;
            } else {
                m->xB8 = 0;
                m->xBA = 0;
            }
            if (!(m->flags & 0x8000000) && (m->xB8 & 0x1000)
                && (m->flags & tst) && func_8001D878()) {
                m->flags &= 0xFFFBFFFF;
                func_8001D494(0, 1, 0);
            }
        loop1:
            op = m->script2[0];
            if ((u32)op < 0x84) {
                m->x4 -= 1;
                if ((s16)m->x4 > 0) {
                } else {
                    if (!(m->flags & 0x8000000) && (m->xB8 & 0x1000)
                        && !(m->flags & tst)) {
                        m->x4 = (u8)m->x3F >> 1;
                    } else {
                        m->x4 = (u8)m->x3F;
                    }
                    m->script2 += 1;
                    if (!(m->flags & 0x60000000)) {
                        if (!(m->flags & tst)) {
                            Sound_call(0x84, 0, 0x64);
                        }
                        m->flags |= setflag;
                    }
                }
                if ((s16)m->x4 == 0) {
                    m->flags |= setflag;
                    goto loop1;
                }
            } else {
                if (D_8008A91C[op](m)) {
                    goto loop1;
                }
            }
            if ((u8)m->x3F == 0 && !(m->flags & 0x10000000)) {
                goto loop1;
            }
            pp = (u32 **)0x1F800070;
            pr = *pp;
            prim = pr;
            *pp = prim + 3;
            SetDrawArea(prim, &D_80097F50[(*(u8 *)0x1F800000)]);
            {
                DRAWCTX *dc = D_80098934;
                prim[0] = (prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
                dc->x70[m->x3D] =
                    (dc->x70[m->x3D] & 0xFF000000) | ((u32)prim & fff);
            }
            m->x78 = 0x80;
            m->script = m->x48;
            m->x10 = m->x8;
            m->x73 = 0;
            m->xBE = 0;
            m->x3E = m->x3C;
            m->x12 = m->xA + m->x38;
            if (m->script != m->script2) {
                do {
                    op = m->script[0];
                    if ((u32)op < 0x84) {
                        if (!(m->flags & 0x100000)) {
                            u32 *p;
                            u8 *pb;
                            pp = (u32 **)0x1F800070;
                            p = *pp;
                            pb = (u8 *)p;
                            *pp = (u32 *)((u8 *)p + 0x14);
                            pb[3] = 4;
                            pb[7] = 0x64;
                            pb[4] = m->x78;
                            pb[5] = m->x78;
                            pb[6] = m->x78;
                            *(s16 *)(pb + 8) = m->x10;
                            *(s16 *)(pb + 0xA) = m->x12;
                            *(s16 *)(pb + 0x10) = 0xC;
                            *(s16 *)(pb + 0x12) = 0xC;
                            *(s16 *)(pb + 0xE) = D_80097F30[m->x3E];
                            pb[0xC] = ((u8)(m->script[0] % 0x15)) * 0xC;
                            pb[0xD] = ((u8)(m->script[0] / 0x15)) * 0xC;
                            {
                                DRAWCTX *dc = D_80098934;
                                p[0] = (p[0] & 0xFF000000)
                                     | (dc->x70[m->x3D] & fff);
                                dc->x70[m->x3D] =
                                    (dc->x70[m->x3D] & 0xFF000000)
                                    | ((u32)p & fff);
                            }
                            m->x10 += D_8008AE7C[m->script[0]];
                        }
                        m->script += 1;
                    } else {
                        D_8008AAC4[op](m);
                    }
                } while (m->script != m->script2);
            }
        post_render:
            pr = *(u32 **)0x1F800070;
            prim = pr;
            rect.x = m->x8;
            rect.y = m->xA + ((*(u8 *)0x1F800000) << 8);
            rect.w = m->x7E * 0xC;
            rect.h = m->x7F * 0xC;
            *(u32 **)0x1F800070 = prim + 3;
            SetDrawArea(prim, &rect);
            {
                DRAWCTX *dc = D_80098934;
                prim[0] = (prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
                dc->x70[m->x3D] =
                    (dc->x70[m->x3D] & 0xFF000000) | ((u32)prim & fff);
            }
            if (m->flags & 0x4000000) {
                D_8008AAE4[m->x7C](m);
            }
        } else {
            *(u32*)Moji_flag &= ~(0x10000 >> (s16)m->x6);
        }
        last = &D_800BB9C8;
        if (m != last) {
            mb = (MOJI_TASK *)((u8 *)last - 0x310);
            D_80098B2C |= (s32)(s8)m->x71 << ((s32)(m - mb) << 3);
        }
        m->flags &= 0xAF7FFFFF;
        m++;
    } while (m < &last[1]);
tail_env:
    pt = *(u32 **)0x1F800070;
    *(u32 **)0x1F800070 = pt + 3;
    t = GetTPage(0, 0, 0x3C0, 0x100);
    SetDrawMode((DR_MODE *)pt, 0, 0, t & 0xFFFF, 0);
    {
        DRAWCTX *dc;
        u32 ww;
        ww = 0xFFFFFF;
        dc = D_80098934;
        pt[0] = (pt[0] & 0xFF000000) | (dc->x7C & ww);
        D_80098960 += 1;
        dc->x7C = (dc->x7C & 0xFF000000) | ((u32)pt & ww);
    }
}
