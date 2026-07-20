
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;
typedef unsigned long long uint64_t;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef int unknown_t;
typedef struct MOJI_TASK
{
  u32 flags;
  u16 x4;
  u16 x6;
  u16 x8;
  u16 xA;
  u16 xC;
  u16 xE;
  u16 x10;
  s16 x12;
  u8 *script;
  u8 *stack[8];
  u16 x38;
  u8 x3A;
  u8 x3B;
  u8 x3C;
  s8 x3D;
  u8 x3E;
  u8 x3F;
  s32 x40;
  u8 *x44;
  u8 *x48;
  u8 *stack2[8];
  u8 *script2;
  u8 x70;
  s8 x71;
  s8 x72;
  s8 x73;
  u8 x74[0x78 - 0x74];
  u8 x78;
  u8 x79[0x7C - 0x79];
  u8 x7C;
  u8 x7D;
  u8 x7E;
  u8 x7F;
  u8 x80[0xB8 - 0x80];
  u16 xB8;
  u16 xBA;
  u16 xBC;
  u16 xBE;
  u16 xC0;
  u16 xC2;
} MOJI_TASK;
typedef struct DRAWCTX
{
  u8 pad[0x70];
  u32 x70[3];
  u32 x7C;
} DRAWCTX;
typedef struct PRIM
{
  u32 tag;
  u8 x3;
} PRIM;
typedef struct 
{
  s16 x;
  s16 y;
  s16 w;
  s16 h;
} RECT;
extern u8 Moji_flag[8];
extern MOJI_TASK Moji_work[];
extern MOJI_TASK D_800BB9C8;
extern s32 D_80098824;
extern s32 D_80098B2C;
extern s32 D_80098960;
extern u16 D_800C0C26;
extern u16 D_800C0C2A;
extern DRAWCTX *D_80098934;
extern RECT D_80097F50[];
extern u16 D_80097F30[];
extern u8 D_8008AE7C[];
extern s32 (*D_8008A91C[])();
extern s32 (*D_8008AAC4[])();
extern s32 (*D_8008AAE4[])();
extern s32 func_8001D878();
extern s32 func_8001D494(s32, s32, s32);
extern void Sound_call(s32, s32, s32);
extern void SetDrawArea(u32 *, RECT *);
extern void SetDrawMode(u32 *, s32, s32, s32, void *);
extern s32 GetTPage(s32, s32, s32, s32);
void func_80053B40(void)
{
  int new_var;
  s32 new_var2;
  MOJI_TASK *m;
  MOJI_TASK *last;
  MOJI_TASK *new_var5;
  u32 *prim;
  u32 *pr;
  u32 **pp;
  u32 setflag;
  u32 tst;
  int new_var3;
  u32 fff;
  RECT rect;
  s32 c;
  s32 t;
  s32 op;
  MOJI_TASK *mv;
  int new_var4;
  u32 *pt;
  MOJI_TASK *mb;
  u32 f;
  u32 w;
  if (((*((u32 *) Moji_flag)) & 0x400000) || D_80098824)
  {
    return;
  }
  m = Moji_work;
  *((u32 *) Moji_flag) &= 0x3043FFFF;
  D_80098B2C = 0;
  if (m >= (&Moji_work[5]))
  {
    goto tail_env;
  }
  do
  {
    tst = 0x40000;
    setflag = 0x40000000;
    fff = 0xFFFFFF;
    if (((s32) m->flags) < 0)
    {
      c = m - Moji_work;
      new_var2 = c;
      f = (*((u32 *) Moji_flag)) | 0x80000000;
      *((u32 *) Moji_flag) = f | (0x8000000 >> new_var2);
      if ((((*((u32 *) Moji_flag)) & tst) == 0) && (((*((u32 *) Moji_flag)) & (0x10000 >> ((s16) m->x6))) == 0))
      {
        w = D_800C0C26;
        m->xB8 = w;
        w = D_800C0C2A;
        m->xBA = w;
      }
      else
      {
        m->xB8 = 0;
        m->xBA = 0;
      }
      if ((((!(m->flags & 0x8000000)) && (m->xB8 & 0x1000)) && (m->flags & tst)) && func_8001D878())
      {
        m->flags &= 0xFFFBFFFF;
        func_8001D494(0, 1, 0);
      }
      loop1:
      op = m->script2[0];

      if (((u32) op) < 0x84)
      {
        m->x4 -= 1;
        if (((s16) m->x4) > 0)
        {
        }
        else
        {
          if (((!(m->flags & 0x8000000)) && (m->xB8 & 0x1000)) && (!(m->flags & tst)))
          {
            m->x4 = m->x3F >> 1;
          }
          else
          {
            m->x4 = m->x3F;
          }
          m->script2 += 1;
          if (!(m->flags & 0x60000000))
          {
            if (!(m->flags & tst))
            {
              Sound_call(0x84, 0, 0x64);
            }
            m->flags |= setflag;
          }
        }
        if (((s16) m->x4) == 0)
        {
          m->flags |= setflag;
          goto loop1;
        }
      }
      else
        if (D_8008A91C[op](m))
      {
        goto loop1;
      }
      new_var4 = 0x10000000;
      if ((m->x3F == 0) && (!(m->flags & new_var4)))
      {
        goto loop1;
      }
      pp = (u32 **) 0x1F800070;
      pr = *pp;
      prim = pr;
      *pp = prim + 3;
      SetDrawArea(prim, &D_80097F50[*((u8 *) 0x1F800000)]);
      {
        DRAWCTX *dc = D_80098934;
        if (Moji_flag)
        {
          prim[0] = (prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
          dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) prim) & fff);
        }
        else
        {
          prim[0] = (prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
          dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) prim) & fff);
        }
      }
      m->x78 = 0x80;
      m->script = m->x48;
      m->x10 = m->x8;
      m->x73 = 0;
      m->x12 = m->xA + m->x38;
      m->xBE = 0;
      m->x3E = m->x3C;
      if (m->script != m->script2)
      {
        do
        {
          op = m->script[0];
          if (((u32) op) < 0x84)
          {
            if (!(m->flags & 0x100000))
            {
              u32 *p;
              u8 *pb;
              pp = (u32 **) 0x1F800070;
              p = *pp;
              pb = (u8 *) p;
              *pp = (u32 *) (((u8 *) p) + 0x14);
              pb[3] = 4;
              pb[new_var = 7] = 0x64;
              pb[4] = m->x78;
              mv = m;
              pb[5] = m->x78;
              pb[6] = m->x78;
              *((s16 *) (pb + 8)) = m->x10;
              new_var3 = 0x12;
              *((s16 *) (pb + 0xA)) = m->x12;
              *((s16 *) (pb + 0x10)) = 0xC;
              *((s16 *) (pb + new_var3)) = 0xC;
              *((s16 *) (pb + 0xE)) = D_80097F30[m->x3E];
              pb[0xC] = ((u8) (m->script[0] % 0x15)) * 0xC;
              new_var5 = m;
              pb[0xD] = ((u8) (m->script[0] / 0x15)) * 0xC;
              {
                DRAWCTX *dc = D_80098934;
                p[0] = (p[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
                dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) p) & fff);
              }
              mv->x10 += D_8008AE7C[new_var5->script[0]];
            }
            m->script += 1;
          }
          else
          {
            D_8008AAC4[op](m);
          }
        }
        while (m->script != m->script2);
      }
      post_render:
      pr = *((u32 **) 0x1F800070);

      rect.x = m->x8;
      rect.y = m->xA + ((*((u8 *) 0x1F800000)) << 8);
      prim = pr;
      rect.w = m->x7E * 0xC;
      rect.h = m->x7F * 0xC;
      *((u32 **) 0x1F800070) = prim + 3;
      SetDrawArea(prim, &rect);
      {
        DRAWCTX *dc = D_80098934;
        if (tst)
        {
          prim[0] = (prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
          dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) prim) & fff);
        }
        else
        {
          prim[0] = (prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
          dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) prim) & fff);
        }
      }
      if (m->flags & 0x4000000)
      {
        D_8008AAE4[m->x7C](m);
      }
    }
    else
    {
      *((u32 *) Moji_flag) &= ~(0x10000 >> ((s16) m->x6));
    }
    last = &D_800BB9C8;
    if (m != last)
    {
      mb = (MOJI_TASK *) (((u8 *) last) - 0x310);
      D_80098B2C |= ((s32) m->x71) << (((s32) (m - mb)) << 3);
    }
    m->flags &= 0xAF7FFFFF;
    m++;
  }
  while (m < (&last[1]));
  tail_env:
  pt = *((u32 **) 0x1F800070);

  *((u32 **) 0x1F800070) = pt + 3;
  t = GetTPage(0, 0, 0x3C0, 0x100);
  SetDrawMode(pt, 0, 0, t & 0xFFFF, 0);
  {
    DRAWCTX *dc;
    u32 ww;
    ww = 0xFFFFFF;
    dc = D_80098934;
    pt[0] = (pt[0] & 0xFF000000) | (dc->x7C & ww);
    D_80098960 += 1;
    dc->x7C = (dc->x7C & 0xFF000000) | (((u32) pt) & ww);
  }
}
