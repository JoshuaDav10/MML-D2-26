#include "types.h"

typedef struct {
    s32 x0, x4, x8, xC, x10, x14, x18, x1C, x20, x24, x28;
    u8 pad[0x100 - 0x2C];
    u8 data[0x800 - 0x100];
} CMD_ENT; /* 0x800 stride @ D_800B5DB0 */

typedef struct {
    s32 x0;   /* 5604 rem */
    s32 x4;   /* 5608 countdown */
    s32 x8;   /* 560C */
    s32 xC;   /* 5610 */
    s32 x10;  /* 5614 */
    s32 x14;  /* 5618 */
    s32 x18;  /* 561C */
    s32 x1C;  /* 5620 (gate byte at [0]) */
    s32 x20;  /* 5624 */
    s32 x24;  /* 5628 */
    s32 x28;  /* 562C */
    s32 x30;  /* 5634 tile-x counter */
    s32 x34;  /* 5638 tile-y counter */
} CD_WORK; /* @ D_800C5604 */

typedef struct { s16 x, y, w, h; } RECT;

typedef struct { u8 x0; u8 x1; u8 pad[2]; s32 x4; } AD_ENT; /* 8-byte @ D_80098AD0, indexed by type */

extern CMD_ENT D_800B5DB0[];
extern CD_WORK D_800C5604;
extern RECT    D_8009BE48;
extern AD_ENT  D_80098AD0[];

extern s32 D_800987A8;
extern s32 D_80098828;
extern s32 D_80098868;
extern u8  D_800988D0;
extern s32 D_80098958;
extern u8  D_80098964;
extern u8  D_8009896C;
extern s32 D_800989C4;
extern s32 D_800989C8;
extern s32 D_80098A54;
extern u8  D_80098B38[];
/* state-2 / type-7 / type-8 access the work block by ABSOLUTE name.
   Union offsets 5618/561C/5620 declared at their BYTE nature; word sites cast. */
extern s32 D_800C5614, D_800C5624, D_800C5628, D_800C5634, D_800C5638;
extern s8  D_800C5618, D_800C561C;
extern u8  D_800C5620;
extern s16 D_8009BE4A;

extern struct { u8 p[4]; s32 x4; } *D_80098A84;

extern u8 D_800AD168[];

/* Game_work as a struct: the per-type slot array at 0x1B8 (stride 8). Accessing
   by member (Game_work.s[idx].v) forces per-site absolute %hi/%lo — cc1 does NOT
   hoist a base pseudo (the (s16*)&Game_work cast form hoisted &Game_work+0x1B8
   into an extra saved reg $s7). */
typedef struct { s16 v; s8 b; u8 _p; s32 w; } GW_SLOT; /* 8 bytes @ 0x1B8 + idx*8 */
extern struct GAMEWORK { u8 _pad[0x1B8]; GW_SLOT s[16]; } Game_work;
#define GW_VAB(idx)  (Game_work.s[idx].v)   /* Game_work + 0x1B8 + idx*8, s16 */
#define GW_1BA(idx)  (Game_work.s[idx].b)   /* + 0x1BA, s8 */
#define GW_1BC(idx)  (Game_work.s[idx].w)   /* + 0x1BC, s32 */
#define GW_1DA       (*((u8 *)&Game_work + 0x1DA))

void LoadImage();
void SsVabClose(s16);
s16  SsVabOpenHeadSticky(s32, s16, s32);
s16  SsVabTransBodyPartly(void *, s32, s16);
void SsVabTransCompleted(s32);
void func_80017684(void *, u32, s32);
void func_800176A4(void *, void *, s32, u32);
void func_800176DC(void *, u32, s32);
void func_8001B33C(s32);
void func_8001CAAC(void);
void func_8001CB7C(void);
void func_8001CF98(s32);
void func_8001D324(s32);

/* p2 = &w->x8 (base+8): word index j -> byte 8+4j.
   p2[0]=x8 p2[1]=xC p2[2]=x10 p2[3]=x14 p2[4]=x18 p2[5]=x1C p2[6]=x20
   p2[7]=x24 p2[9]=x30? no: 8+4*10=0x30 -> x30=p2[10], x34=p2[11], x4=p2[-1]
   q4 = base+0x1C: q4[0]=x1C q4[-1]=x18 q4[-2]=x14 q4[-3]=x10 q4[-5]=x8 q4[-6]=x4 */

void func_8001BB4C(void) {
    CD_WORK *w;
    s32 *p2;
    s32 *q4;
    s32 one;
    RECT *r;
    CMD_ENT *e;
    s32 type;
    s32 t, wd;
    s32 x, y;
    u8 *src, *dst;

    switch (D_800989C4) {
    case 0:
        if (D_80098964 == 0) {
            func_8001CB7C();
        }
        D_800989C4 += 1;
        /* fallthrough */
    case 1:
        if (D_80098964 == 0) {
            return;
        }
        func_8001D324(0xA0);
        D_800989C4 += 1;
        /* fallthrough */
    case 2:
        if (D_80098828 != 0) {
            D_80098828 -= 1;
            return;
        }
        func_8001CF98(D_80098A84->x4);
        D_800988D0 = 0;
        D_800987A8 = 0;
        D_80098828 = 0;
        D_800989C4 += 1;
        /* fallthrough */
    case 3:
        D_80098828 += 1;
        if (D_80098828 == 0x96) {
            D_8009896C |= 2;
            return;
        }
        if (D_80098B38[D_800987A8] != 1) {
            return;
        }
        one = 1;
        w = &D_800C5604;
        p2 = &w->x8;
        q4 = &w->x1C;
        while (1) {
            switch (D_800989C8) {
            case 0:
                e = &D_800B5DB0[D_800987A8];
                type = e->x0;
                D_80098A54 = type;
                if (type == -1) {
                    D_800989C4 = 4;
                    return;
                }
                w->x0 = e->x4;
                w->x4 = e->x8;
                switch (type) {
                case 0:
                    p2[0] = e->xC;
                    p2[-1] -= 1;
                    break;
                case 1:
                    t = e->xC;
                    p2[0] = t;
                    p2[1] = e->x10;
                    p2[2] = e->x14;
                    p2[3] = e->x18;
                    p2[4] = e->x1C;
                    p2[5] = e->x20;
                    p2[6] = e->x24;
                    r = &D_8009BE48;
                    p2[11] = 0;
                    p2[10] = 0;
                    p2[7] = e->x28;
                    if (t != -1) {
                        wd = e->xC;
                        if ((u32)wd < 0x140) {
                            x = e->x10 & 0x100;
                            func_800176A4((u8 *)e + 0x100,
                                &D_800AD168[(wd + ((((e->x10 + (x >> 4)) - 0xF0) - x) << 8)) << 1],
                                (u32)(e->x18 * e->x14) >> 4 << 1,
                                wd);
                            x = e->x10 & 0x100;
                            D_80098868 |= one << (((e->x10 + (x >> 4)) - 0xF0) - x);
                        } else {
                            r->x = wd;
                            r->y = e->x10;
                            r->w = e->x14;
                            r->h = e->x18;
                            LoadImage(r, (u8 *)e + 0x100, e, wd);
                        }
                    }
                    r->w = 0x20;
                    r->h = 0x20;
                    p2[-1] -= 1;
                    break;
                case 3:
                    p2[0] = e->xC;
                    p2[1] = e->x10;
                    p2[2] = e->x14;
                    p2[11] = 0;
                    p2[10] = 0;
                    D_8009BE48.w = 0x20;
                    D_8009BE48.h = 0x20;
                    p2[3] = e->x18;
                    break;
                case 4:
                case 9:
                case 10:
                    wd = e->xC;
                    if ((u32)wd < 0x100) {
                        x = e->x10 & 0x100;
                        func_800176A4((u8 *)e + 0x100,
                            &D_800AD168[(wd + ((((e->x10 + (x >> 4)) - 0xF0) - x) << 8)) << 1],
                            (u32)(e->x18 * e->x14) >> 4 << 1,
                            wd);
                        x = e->x10 & 0x100;
                        D_80098868 |= one << (((e->x10 + (x >> 4)) - 0xF0) - x);
                    } else {
                        r = &D_8009BE48;
                        r->x = wd;
                        r->y = e->x10;
                        if (D_80098A54 != 0xA) {
                            r->w = e->x14;
                            y = e->x18;
                        } else {
                            r->w = 0x40;
                            y = (u32)(e->x14 * e->x18) >> 6;
                        }
                        r->h = y;
                        LoadImage(r, (u8 *)e + 0x100, e, wd);
                    }
                    D_800989C8 = 0;
                    goto tail;
                case 5:
                    p2[0] = e->xC;
                    p2[1] = e->x10;
                    p2[2] = e->x14;
                    p2[3] = e->x18;
                    *((u8 *)p2 + 0x14) = one;
                    p2[4] = e->xC * 0x1820 + 0x8014B000;
                    if (e->xC == 4) {
                        if (GW_1DA == e->x10) {
                            *((u8 *)p2 + 0x14) = 0;
                        }
                        if (D_80098958 & 0x800) {
                            *((u8 *)p2 + 0x14) = one;
                            func_8001B33C(0x8014B000);
                        }
                    }
                    if (D_800C5620 == one) {
                        GW_1BA(e->xC) = e->x10;
                        GW_1BC(e->xC) = e->x14;
                        if (GW_VAB(e->xC) != -1) {
                            SsVabClose(GW_VAB(e->xC));
                        }
                    }
                    p2[-1] -= 1;
                    break;
                case 7:
                    t = e->x14 + 0x8013B000;
                    D_80098AD0[e->xC].x4 = t;
                    D_800C5614 = t;
                    D_800C5618 = 0;
                    if (D_80098AD0[e->xC].x0 != e->x10) {
                        D_80098AD0[e->xC].x0 = e->x10;
                        D_800C5618 = one;
                    }
                    break;
                case 8:
                    t = e->x18 + 0x8013B000;
                    D_80098AD0[e->xC].x4 = t;
                    *(s32 *)&D_800C5618 = t;
                    D_800C561C = 0;
                    if (D_80098AD0[e->xC].x0 != e->x10) {
                        D_80098AD0[e->xC].x0 = e->x10;
                        D_80098AD0[e->xC].x1 = e->x14;
                        *(s8 *)&D_800C561C = one;
                    }
                    break;
                default:
                    break;
                }
                D_800989C8 = D_80098A54 + 1;
                goto tail;

            case 1:
                if ((u32)w->x0 >= 0x800) {
                    func_800176DC((u8 *)&D_800B5DB0[D_800987A8], w->x8, 0x40);
                    w->x0 -= 0x800;
                    w->x8 += 0x800;
                } else {
                    dst = (u8 *)w->x8;
                    src = (u8 *)&D_800B5DB0[D_800987A8];
                    if (w->x0 != 0) {
                        do {
                            *dst++ = *src++;
                            w->x0 -= 1;
                        } while (w->x0 != 0);
                    }
                }
                w->x4 -= 1;
                if (w->x4 == 0) {
                    goto tail0;
                }
                goto tail;

            case 2:
                r = &D_8009BE48;
                r->x = *(s32 *)&D_800C561C + (D_800C5634 << 5);
                D_8009BE4A = *(s32 *)&D_800C5620 + (D_800C5638 << 5);
                LoadImage(r, (u8 *)&D_800B5DB0[D_800987A8]);
                D_800C5634 += 1;
                if (D_800C5634 == ((u32)D_800C5624 >> 5)) {
                    D_800C5634 = 0;
                    D_800C5638 += 1;
                    if (D_800C5638 == ((u32)D_800C5628 >> 5)) {
                        goto tail0;
                    }
                }
                goto tail;

            case 4:
                r = &D_8009BE48;
                r->x = p2[0] + (p2[10] << 5);
                r->y = p2[1] + (p2[11] << 5);
                LoadImage(r, (u8 *)&D_800B5DB0[D_800987A8]);
                p2[10] += 1;
                if (p2[10] == p2[2]) {
                    p2[10] = 0;
                    p2[11] += 1;
                    if (p2[11] == p2[3]) {
                        goto tail0;
                    }
                }
                goto tail;

            case 6:
                if (*(u8 *)q4 == 0) {
                    q4[-6] -= 1;
                    if (q4[-6] == 0) {
                        goto tail0;
                    }
                } else {
                    x = q4[-2];
                    if (x >= 0x800) {
                        func_800176DC((u8 *)&D_800B5DB0[D_800987A8], q4[-1], 0x40);
                        q4[-1] += 0x800;
                        q4[-2] -= 0x800;
                    } else if (x != 0) {
                        if (x < 0) {
                            x += 3;
                        }
                        func_80017684((u8 *)&D_800B5DB0[D_800987A8], q4[-1], x >> 2);
                        t = q4[-5];
                        q4[-2] = 0;
                        GW_VAB(q4[-5]) =
                            SsVabOpenHeadSticky(t * 0x1820 + 0x8014B000, t, q4[-3]);
                    } else {
                        if (SsVabTransBodyPartly((u8 *)&D_800B5DB0[D_800987A8], 0x800,
                                GW_VAB(q4[-5])) == GW_VAB(q4[-5])) {
                            D_800989C8 = 0;
                        }
                        SsVabTransCompleted(1);
                    }
                }
                goto tail;

            case 8:
                if ((u32)w->x0 >= 0x800) {
                    if (w->x14 != 0) {
                        func_800176DC((u8 *)&D_800B5DB0[D_800987A8], w->x10, 0x40);
                    }
                    w->x0 -= 0x800;
                    w->x10 += 0x800;
                } else {
                    if (w->x14 != 0) {
                        dst = (u8 *)w->x10;
                        src = (u8 *)&D_800B5DB0[D_800987A8];
                        if (w->x0 != 0) {
                            do {
                                *dst++ = *src++;
                                w->x0 -= 1;
                            } while (w->x0 != 0);
                        }
                    }
                    goto tail0;
                }
                goto tail;

            case 9:
                if ((u32)w->x0 >= 0x800) {
                    if (w->x18 != 0) {
                        func_800176DC((u8 *)&D_800B5DB0[D_800987A8], w->x14, 0x40);
                    }
                    w->x0 -= 0x800;
                    w->x14 += 0x800;
                } else {
                    if (w->x18 != 0) {
                        dst = (u8 *)w->x14;
                        src = (u8 *)&D_800B5DB0[D_800987A8];
                        if (w->x0 != 0) {
                            do {
                                *dst++ = *src++;
                                w->x0 -= 1;
                            } while (w->x0 != 0);
                        }
                    }
                    goto tail0;
                }
                goto tail;

            default:
                goto tail;
            }
tail0:
            D_800989C8 = 0;
tail:
            D_80098B38[D_800987A8] = 0;
            D_800987A8 += 1;
            if (D_800987A8 == 0xA) {
                D_800987A8 = 0;
            }
            if (D_80098B38[D_800987A8] != one) {
                return;
            }
        }
    case 4:
        func_8001CAAC();
        break;
    }
}
