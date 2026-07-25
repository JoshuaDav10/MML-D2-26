
asm(".include \"macro.inc\"\n");;
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
extern void InitHeap(unsigned long *, unsigned long);
extern long Load(char *, struct EXEC *);
extern long Exec(struct EXEC *, long, char **);
extern void _bu_init(void);
extern long open(char *, unsigned long);
extern long lseek(long, long, long);
extern long read(long, void *, long);
extern long write(long, void *, long);
extern long close(long);
extern long format(char *);
extern struct DIRENTRY *firstfile(char *, struct DIRENTRY *);
extern struct DIRENTRY *nextfile(struct DIRENTRY *);
extern long erase(char *);
extern long Krom2RawAdd(unsigned long);
extern void ChangeClearPAD(long);
extern void StopPAD(void);
extern void FlushCache(void);
extern void DeliverEvent(unsigned long, unsigned long);
extern long TestEvent(long);
extern long OpenEvent(unsigned long, long, long, long (*func)());
extern long EnableEvent(long);
extern void _96_remove(void);
extern void exit();
extern void puts(char *);
extern char *strcat(char *, char *);
extern char *strcpy(char *, char *);
extern int strlen(char *);
extern void *memcpy(unsigned char *, unsigned char *, int);
extern void *memset(unsigned char *, unsigned char, int);
extern int rand(void);
extern void srand(unsigned int);
extern void *malloc(size_t);
extern void free(void *);
int printf(char *, ...);
typedef struct 
{
  short x;
  short y;
  short w;
  short h;
} RECT;
typedef struct 
{
  unsigned long tag;
  unsigned long code[15];
} DR_ENV;
typedef struct 
{
  unsigned long *tag;
  unsigned long code[2];
} DR_MODE;
typedef struct 
{
  unsigned long tag;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  unsigned char u0;
  unsigned char v0;
  unsigned short clut;
  unsigned char r1;
  unsigned char g1;
  unsigned char b1;
  unsigned char p1;
  short x1;
  short y1;
  unsigned char u1;
  unsigned char v1;
  unsigned short tpage;
  unsigned char r2;
  unsigned char g2;
  unsigned char b2;
  unsigned char p2;
  short x2;
  short y2;
  unsigned char u2;
  unsigned char v2;
  unsigned short pad2;
} POLY_GT3;
typedef struct 
{
  unsigned long tag;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  unsigned char r1;
  unsigned char g1;
  unsigned char b1;
  unsigned char pad1;
  short x1;
  short y1;
  unsigned char r2;
  unsigned char g2;
  unsigned char b2;
  unsigned char pad2;
  short x2;
  short y2;
  unsigned char r3;
  unsigned char g3;
  unsigned char b3;
  unsigned char pad3;
  short x3;
  short y3;
} POLY_G4;
typedef struct 
{
  unsigned long tag;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  unsigned char u0;
  unsigned char v0;
  unsigned short clut;
  unsigned char r1;
  unsigned char g1;
  unsigned char b1;
  unsigned char p1;
  short x1;
  short y1;
  unsigned char u1;
  unsigned char v1;
  unsigned short tpage;
  unsigned char r2;
  unsigned char g2;
  unsigned char b2;
  unsigned char p2;
  short x2;
  short y2;
  unsigned char u2;
  unsigned char v2;
  unsigned short pad2;
  unsigned char r3;
  unsigned char g3;
  unsigned char b3;
  unsigned char p3;
  short x3;
  short y3;
  unsigned char u3;
  unsigned char v3;
  unsigned short pad3;
} POLY_GT4;
typedef struct 
{
  unsigned long tag;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  unsigned char u0;
  unsigned char v0;
  unsigned short clut;
  short w;
  short h;
} SPRT;
typedef struct 
{
  unsigned long tag;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  unsigned char u0;
  unsigned char v0;
  unsigned short clut;
} SPRT_16;
typedef struct 
{
  unsigned long tag;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  unsigned char u0;
  unsigned char v0;
  unsigned short clut;
} SPRT_8;
typedef struct 
{
  RECT clip;
  short ofs[2];
  RECT tw;
  unsigned short tpage;
  unsigned char dtd;
  unsigned char dfe;
  unsigned char isbg;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  DR_ENV dr_env;
} DRAWENV;
typedef struct 
{
  RECT disp;
  RECT screen;
  unsigned char isinter;
  unsigned char isrgb24;
  unsigned char pad0;
  unsigned char pad1;
} DISPENV;
extern void SetShadeTex(void *p, int tge);
extern void SetPolyGT3(POLY_GT3 *p);
extern void SetPolyG4(POLY_G4 *p);
extern void SetPolyGT4(POLY_GT4 *p);
extern void SetSprt(SPRT *p);
extern void SetSprt16(SPRT_16 *p);
extern void SetSprt8(SPRT_8 *p);
extern int ResetGraph(int mode);
extern int SetGraphReverse(int mode);
extern int SetGraphQueue(int mode);
extern unsigned long DrawSyncCallback(void (*func)());
extern void SetDispMask(int mask);
extern int DrawSync(int mode);
extern int ClearImage(RECT *rect, unsigned char r, unsigned char g, unsigned char b);
extern int LoadImage(RECT *rect, unsigned long *p);
extern int StoreImage(RECT *rect, unsigned long *p);
extern int MoveImage(RECT *rect, int x, int y);
extern unsigned long *ClearOTag(unsigned long *ot, int n);
extern unsigned long *ClearOTagR(unsigned long *ot, int n);
extern void DrawOTag(unsigned long *p);
extern DRAWENV *PutDrawEnv(DRAWENV *env);
extern DISPENV *PutDispEnv(DISPENV *env);
void AddPrim(void *, void *);
u16 GetClut(int, int);
u16 GetTPage(int, int, int, int);
typedef struct 
{
  unsigned char tag[4];
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  unsigned char u0;
  unsigned char v0;
  unsigned short clut;
  short x1;
  short y1;
  unsigned char u1;
  unsigned char v1;
  unsigned short tpage;
  short x2;
  short y2;
  unsigned char u2;
  unsigned char v2;
  unsigned short pad2;
  short x3;
  short y3;
  unsigned char u3;
  unsigned char v3;
  unsigned short pad3;
} POLY_FT4;
typedef struct 
{
  unsigned long *tag;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
  unsigned char code;
  short x0;
  short y0;
  short x1;
  short y1;
  short x2;
  short y2;
  short x3;
  short y3;
} POLY_F4;
void SetDrawMode(DR_MODE *p, int dfe, int dtd, int tpage, RECT *tw);
unknown_t func_80063FC0(unknown_t, unknown_t);
unknown_t func_8001D7AC(unknown_t);
unknown_t func_80012E98(unknown_t);
unknown_t func_80031AA4();
unknown_t func_80016D64();
unknown_t Sce_flag_on(unknown_t);
unknown_t func_8001F23C();
extern unknown_t D_8008CB94;
extern s32 D_80098B2C;
unknown_t func_8001F3C8();
unknown_t func_80038648();
extern u8 D_800C356F[];
unknown_t func_80017C30(unknown_t);
unknown_t func_8001B2F0(unknown_t);
unknown_t func_8005DF3C();
unknown_t func_8005DF4C();
unknown_t func_80063EF0();
unknown_t func_8001B314();
unknown_t func_8001B33C();
unknown_t func_8001D494(unknown_t, unknown_t, unknown_t);
unknown_t Sce_flag_off(unknown_t);
unknown_t func_80063BA8();
unknown_t func_800665E8();
extern u16 D_80098910[];
extern DR_MODE D_80098A10[];
extern s16 D_80098A70;
extern s16 D_80098A72;
extern u8 D_80098788;
typedef struct unkst_80098A28
{
  s8 xe0[3];
  s8 unk3;
  s8 unk4[3];
  s8 unk7;
  s16 unk8;
  s16 unkA;
  s16 unkC;
  s16 unkE;
} unkst_80098A28;
extern unkst_80098A28 D_80098A28;
extern void (*D_80080894[])(s8 *);
extern s8 D_80098B1C;
extern s8 D_80098B1D;
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
  s8 x3F;
  s32 x40;
  u8 *x44;
  u8 *x48;
  u8 *stack2[8];
  u8 *script2;
  u8 x70;
  u8 x71;
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
s32 MojiTaskExec();
s32 MojiTaskKill();
s32 func_80053788();
extern u8 Moji_flag[8];
extern MOJI_TASK Moji_work[];
extern u32 Moji_flag3;
void Game_logo_kill(unknown_t);
typedef struct 
{
  u8 pad[4];
  u16 vals[6];
} GW84_ENTRY;
typedef struct GAME_WORK
{
  s8 routine_0;
  s8 x1;
  u8 x2[0x4 - 0x2];
  s32 x4;
  s16 x8;
  s16 xA;
  s32 time;
  s32 x10;
  s32 x14;
  u16 difficultyUnlocked[2];
  s32 zennyCount;
  u8 x20[0x3C - 0x20];
  void *x3C;
  u8 x40[0x10];
  s8 stage_no;
  s8 area_no;
  s8 x52;
  s8 x53;
  s8 x54[0x5A - 0x54];
  s8 x5A;
  s8 x5B;
  s8 x5C[0x60 - 0x5C];
  u16 x60;
  u8 x62[0x68 - 0x62];
  s16 x68;
  s16 x6A;
  s16 x6C;
  u8 x6E[0x70 - 0x6E];
  s16 x70;
  s16 x72;
  s16 x74;
  s16 x76;
  s8 x78[0x7C - 0x78];
  u8 x7C;
  u8 x7D;
  s8 x7E[0x81 - 0x7E];
  s8 x81;
  u8 x82;
  s8 x83;
  GW84_ENTRY x84_tbl[1];
} GAME_WORK;
typedef struct 
{
  u8 tag[4];
  u8 unk4;
  u8 unk5;
  u8 unk6;
  u8 code;
  s16 unk8;
  s16 unkA;
  s16 unkC;
  s16 unkE;
  s16 unk10;
  s16 unk12;
  s16 unk14;
  s16 unk16;
  s8 gap[16];
} UNK_PRIM_1;
typedef struct 
{
  int x0;
  s8 x4[0x70 - 0x4];
  u32 x70[3];
  u32 x7C;
} UnkStruc_80098934;
extern UnkStruc_80098934 *D_80098934;
extern GAME_WORK Game_work;
extern void (*Game_main_tbl[])(GAME_WORK *);
void func_80016BC0();
void func_80016BF4();
void func_800155A4(void);
void func_80015634(GAME_WORK *);
void func_80015734(GAME_WORK *);
void func_80015840(GAME_WORK *);
void func_800158F0(GAME_WORK *);
void func_80015FE8(GAME_WORK *);
void func_80016160(GAME_WORK *);
void func_800164B4(GAME_WORK *);
void func_80016528(GAME_WORK *);
void func_8001663C(GAME_WORK *);
void func_80016798(GAME_WORK *);
void func_800169AC(GAME_WORK *);
void Sound_call(unknown_t, s32, s32);
s32 Sce_flag_test(unknown_t);
extern s32 D_80098AF4;
extern s32 D_80098824;
extern s32 D_80098960;
s32 Cd_read_comb();
void func_8001D7E4();
s32 func_8005BF10(s32, s32, u8 *);
s32 func_8005DA78(s32);
s32 func_8005DAEC(s32);
s32 func_8005DA1C(s32);
extern u8 *D_8008CE5C[];
extern u8 *D_8008CE10[];
extern u8 D_800989D4;
extern u8 *D_8008D0D4[];
extern u8 *D_8008CCA4[];
extern u8 *D_8008CACC[];
extern s8 D_800BE2F7[];
s32 func_80039E18(void);
extern u8 D_800BE2F8[];
extern u8 D_800BE3DB;
extern u8 D_8009899C;
extern u8 D_80098930;
extern u8 D_80098B6C;
extern u16 D_8008FD3C[];
extern u8 *D_8008BAA4[];
extern u8 *D_8008CE58;
s32 func_8004327C(s32);
extern struct 
{
  u8 _p[0x450];
  u8 remap450[4];
  u8 remap454[0x1A0];
} Player_work;
extern u16 D_80098912[];
s32 func_8005ACA8(MOJI_TASK *);
u8 Moji_flag[8];
s32 func_80053788(void);
s32 MojiTaskExec(no, script_base, op);
void MojiTaskExec2(s32 arg0, u8 arg1);
s32 MojiTaskKill(void);
void func_80053AA4(u8 no);
extern s32 D_80098B2C;
extern u16 D_800C0C26;
extern u16 D_800C0C2A;
typedef UnkStruc_80098934 DRAWCTX;
extern RECT D_80097F50[];
extern u16 D_80097F30[];
extern u8 D_8008AE7C[];
extern s32 (*D_8008A91C[])();
extern s32 (*D_8008AAC4[])();
extern s32 (*D_8008AAE4[])();
extern MOJI_TASK D_800BB9C8;
void SetDrawArea(u32 *, RECT *);
void func_80053B40(void)
{
  int new_var;
  s32 new_var2;
  MOJI_TASK *m;
  u8 (*new_var6)[];
  MOJI_TASK *last;
  MOJI_TASK *new_var5;
  u32 new_var8;
  u32 *prim;
  u32 *pr;
  u32 **pp;
  u32 setflag;
  int new_var9;
  u32 tst;
  char new_var10;
  RECT rect;
  int new_var3;
  u32 fff;
  u32 fossil[2];
  unsigned char c;
  s32 t;
  s32 op;
  MOJI_TASK *mv;
  int new_var4;
  u32 *pt;
  MOJI_TASK *mb;
  MOJI_TASK *new_var7;
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
    new_var7 = m;
    tst = 0x40000;
    setflag = 0x40000000;
    fff = 0xFFFFFF;
    (void)&fossil;
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
        w = (m->xBA = D_800C0C2A);
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
            m->x4 = ((u8) m->x3F) >> 1;
          }
          else
          {
            m->x4 = (u8) m->x3F;
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
        if (D_8008A91C[op](new_var7))
      {
        goto loop1;
      }
      new_var4 = 0x10000000;
      if ((((u8) m->x3F) == 0) && (!(m->flags & new_var4)))
      {
        goto loop1;
      }
      pp = 0x1F800070;
      pr = *pp;
      prim = pr;
      *pp = prim + 3;
      SetDrawArea(prim, &D_80097F50[*((u8 *) 0x1F800000)]);
      new_var9 = 0x80;
      {
        DRAWCTX *dc = D_80098934;
        if (Moji_flag)
        {
          prim[0] = (prim[0] & 0xFF000000) | (dc->x70[m->x3D] & fff);
          dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) prim) & fff);
        }
        else
        {
          prim[0] = (prim[0] & 0xFF000000) | ((*dc).x70[m->x3D] & fff);
          dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) prim) & fff);
        }
      }
      m->x78 = new_var9;
      m->script = m->x48;
      m->x10 = m->x8;
      m->x73 = 0;
      c = m->xA;
      m->x12 = c + m->x38;
      m->xBE = 0;
      m->x3E = m->x3C;
      if (m->script != m->script2)
      {
        do
        {
          op = m->script[0];
          if ((((u32) op) + 1) < (0x84 + 1))
          {
            if (!(m->flags & 0x100000))
            {
              u32 *p;
              u8 *pb;
              pp = (u32 **) 0x1F800070;
              p = *pp;
              pb = (u8 *) p;
              *((u32 **) 0x1F800070) = (u32 *) (((u8 *) p) + 0x14);
              ((u8 *) p)[3] = 4;
              ((u8 *) p)[new_var = 7] = 0x64;
              ((u8 *) p)[4] = m->x78;
              mv = m;
              ((u8 *) p)[5] = m->x78;
              ((u8 *) p)[6] = m->x78;
              *((s16 *) (((u8 *) p) + 8)) = m->x10;
              new_var3 = 0x12;
              *((s16 *) (((u8 *) p) + 0xA)) = m->x12;
              *((s16 *) (((u8 *) p) + 0x10)) = 0xC;
              *((s16 *) (((u8 *) p) + new_var3)) = 0xC;
              *((s16 *) (pb + 0xE)) = D_80097F30[m->x3E];
              f = p[0];
              pb[0xC] = ((u8) (m->script[0] % 0x15)) * 0xC;
              new_var5 = m;
              pb[0xD] = ((u8) (m->script[0] / 0x15)) * 0xC;
              {
                DRAWCTX *dc = D_80098934;
                p[0] = (f & 0xFF000000) | (dc->x70[m->x3D] & fff);
                dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (((u32) p) & fff);
              }
              mv->x10 += (*(new_var6 = &D_8008AE7C))[new_var5->script[0]];
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
          prim[0] = (prim[0] & 0xFF000000) | ((*dc).x70[m->x3D] & fff);
          new_var8 = (u32) prim;
          dc->x70[m->x3D] = (dc->x70[m->x3D] & 0xFF000000) | (new_var8 & fff);
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
      D_80098B2C |= ((s32) ((s8) m->x71)) << (((s32) (m - mb)) << 3);
    }
    m->flags &= 0xAF7FFFFF;
    m++;
  }
  while (m < (&last[1]));
  tail_env:
  pt = *((u32 **) 0x1F800070);

  *((u32 **) 0x1F800070) = pt + 3;
  t = GetTPage(0, 0, 0x3C0, 0x100);
  SetDrawMode((DR_MODE *) pt, 0, 0, t & 0xFFFF, 0);
  {
    DRAWCTX *dc;
    u32 ww;
    ww = 0xFFFFFF;
    dc = D_80098934;
    pt[0] = (pt[0] & 0xFF000000) | (dc->x7C & ww);
    new_var10 = ((u32) pt) & ww;
    D_80098960 += 1;
    dc->x7C = (dc->x7C & 0xFF000000) | new_var10;
  }
}

void func_800542FC(MOJI_TASK *m);
void func_80054310(MOJI_TASK *m);
void func_80054324(MOJI_TASK *m);
void func_80054338(MOJI_TASK *m);
void func_8005434C(MOJI_TASK *m);
void func_80054360(MOJI_TASK *m);
void func_80054374(MOJI_TASK *m);
void func_80054388(MOJI_TASK *m);
void func_8005439C(MOJI_TASK *m);
s32 func_800543B0(MOJI_TASK *m);
s32 func_800543C8(MOJI_TASK *m);
s32 func_800543E0(MOJI_TASK *m);
s32 func_800543F8(MOJI_TASK *m);
s32 func_80054410(u8 *p);
s32 func_80054424(u8 *p);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80054450\n\t.ent\tfunc_80054450\nfunc_80054450:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80054450.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80054450");;
s32 func_8005457C(MOJI_TASK *m);
void func_8005459C(MOJI_TASK *m);
s32 func_800545C8(MOJI_TASK *m);
void func_80054694(MOJI_TASK *m);
s32 func_800546B0(MOJI_TASK *m);
s32 func_80054700(MOJI_TASK *m);
s32 func_80054798(MOJI_TASK *m);
s32 func_80054804(MOJI_TASK *m);
s32 func_80054874(MOJI_TASK *m);
s32 func_8001D58C(s32);
s32 func_8001D878(void);
s32 func_800548C4(MOJI_TASK *m);
s32 func_8005497C(MOJI_TASK *m);
void func_80054A04(MOJI_TASK *m);
s32 func_80054A84(MOJI_TASK *m);
void func_80054AB4(MOJI_TASK *m);
extern s32 (*D_8008AAC0[])();
s32 func_80054ADC(MOJI_TASK *m);
extern s32 (*D_8008AB08[])();
void func_80054B4C(MOJI_TASK *m);
s32 func_80054B88(MOJI_TASK *m);
s32 func_80054BAC(void);
s32 func_80054BB4(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80054BEC\n\t.ent\tfunc_80054BEC\nfunc_80054BEC:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80054BEC.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80054BEC");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80054E54\n\t.ent\tfunc_80054E54\nfunc_80054E54:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80054E54.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80054E54");;
void func_80055304(MOJI_TASK *m);
s32 func_8005531C(MOJI_TASK *m);
s32 func_80055344(MOJI_TASK *m);
s32 func_800553A8(MOJI_TASK *m);
s32 func_800553F0(MOJI_TASK *m);
s32 func_80055438(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_800554E4\n\t.ent\tfunc_800554E4\nfunc_800554E4:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_800554E4.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_800554E4");;
void func_800555F4(MOJI_TASK *m);
s32 func_8005563C(MOJI_TASK *m);
s32 func_80055660(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80055688\n\t.ent\tfunc_80055688\nfunc_80055688:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80055688.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80055688");;
s32 func_800557B8(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80055868\n\t.ent\tfunc_80055868\nfunc_80055868:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80055868.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80055868");;
s32 func_80055A78(MOJI_TASK *m);
s32 func_80055B14(MOJI_TASK *m);
s32 func_80055BB0(MOJI_TASK *m);
s32 func_80055C1C(MOJI_TASK *m);
extern u8 *D_8008D0D4[];
s32 func_80055C80(MOJI_TASK *m);
s32 func_80055CC4(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80055CFC\n\t.ent\tfunc_80055CFC\nfunc_80055CFC:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80055CFC.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80055CFC");;
void func_800560D0(MOJI_TASK *m);
s32 func_80056128(MOJI_TASK *m);
void func_80056148(MOJI_TASK *m);
void func_80056180(MOJI_TASK *m);
s32 func_8005619C(MOJI_TASK *m);
s32 func_800561E8(MOJI_TASK *m);
s32 func_80056234(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80056280\n\t.ent\tfunc_80056280\nfunc_80056280:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80056280.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80056280");;
extern u8 D_80098850;
extern u8 D_80098830;
void func_800563A8(MOJI_TASK *m);
void func_800564C8(MOJI_TASK *m);
void func_80056558(MOJI_TASK *m);
void func_80056610(MOJI_TASK *m);
void func_800566CC(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80056778\n\t.ent\tfunc_80056778\nfunc_80056778:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80056778.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80056778");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80056820\n\t.ent\tfunc_80056820\nfunc_80056820:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80056820.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80056820");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80056D10\n\t.ent\tfunc_80056D10\nfunc_80056D10:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80056D10.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80056D10");;
s32 func_800570B0(MOJI_TASK *m);
s32 func_80057124(MOJI_TASK *m);
void func_80057144(MOJI_TASK *m);
s32 func_80057184(MOJI_TASK *m);
s32 func_8005721C(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_800572C8\n\t.ent\tfunc_800572C8\nfunc_800572C8:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_800572C8.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_800572C8");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80057408\n\t.ent\tfunc_80057408\nfunc_80057408:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80057408.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80057408");;
void func_8005753C(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80057574\n\t.ent\tfunc_80057574\nfunc_80057574:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80057574.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80057574");;
extern u8 D_80098830;
extern u8 *D_8008CBA4[];
s32 func_800576C4(MOJI_TASK *m);
void func_80057708(MOJI_TASK *m);
s32 func_80057744(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_800577FC\n\t.ent\tfunc_800577FC\nfunc_800577FC:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_800577FC.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_800577FC");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80057924\n\t.ent\tfunc_80057924\nfunc_80057924:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80057924.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80057924");;
void func_800579D8(MOJI_TASK *m);
s32 func_80057A24(MOJI_TASK *m);
void func_80057A94(MOJI_TASK *m);
s32 func_80057AD0(MOJI_TASK *m);
void func_80057B24(MOJI_TASK *m);
s32 func_80043294(s32, s32, s32);
s32 func_80057B70(MOJI_TASK *m);
s32 func_80057BB4(MOJI_TASK *m);
s32 func_80057BFC(MOJI_TASK *m);
s32 func_80057C2C(MOJI_TASK *m);
s32 func_80057D00(MOJI_TASK *m);
void func_80057D60(MOJI_TASK *m);
extern u8 D_800BE2F8[];
s32 func_80057DB8(MOJI_TASK *m);
s32 func_80057DF4(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80057E50\n\t.ent\tfunc_80057E50\nfunc_80057E50:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80057E50.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80057E50");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80057FF8\n\t.ent\tfunc_80057FF8\nfunc_80057FF8:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80057FF8.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80057FF8");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005851C\n\t.ent\tfunc_8005851C\nfunc_8005851C:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005851C.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005851C");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005861C\n\t.ent\tfunc_8005861C\nfunc_8005861C:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005861C.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005861C");;
void func_80058740(MOJI_TASK *m);
s32 func_80058788(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_800587F8\n\t.ent\tfunc_800587F8\nfunc_800587F8:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_800587F8.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_800587F8");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80058AEC\n\t.ent\tfunc_80058AEC\nfunc_80058AEC:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80058AEC.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80058AEC");;
s32 func_80058C08(MOJI_TASK *m);
s32 func_80058C28(MOJI_TASK *m);
void func_80058CC8(MOJI_TASK *m);
void func_80058D64(MOJI_TASK *m);
extern u8 D_800BE377;
extern u8 D_8009899C;
s32 func_80058DB4(MOJI_TASK *m);
extern s32 D_800989A8;
extern u8 D_800BE420[];
void func_80058DEC(MOJI_TASK *m);
extern u8 D_80098851;
s32 func_80058EA0(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80058F18\n\t.ent\tfunc_80058F18\nfunc_80058F18:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80058F18.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80058F18");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_800592FC\n\t.ent\tfunc_800592FC\nfunc_800592FC:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_800592FC.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_800592FC");;
s32 func_800594CC(MOJI_TASK *m);
void func_80059530(MOJI_TASK *m);
extern u8 *D_8008CBA0[];
s32 func_8005958C(MOJI_TASK *m);
void func_80059660(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80059728\n\t.ent\tfunc_80059728\nfunc_80059728:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80059728.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80059728");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_800598C8\n\t.ent\tfunc_800598C8\nfunc_800598C8:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_800598C8.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_800598C8");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80059B44\n\t.ent\tfunc_80059B44\nfunc_80059B44:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80059B44.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80059B44");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80059D20\n\t.ent\tfunc_80059D20\nfunc_80059D20:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80059D20.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80059D20");;
void func_80059E24(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_80059E74\n\t.ent\tfunc_80059E74\nfunc_80059E74:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_80059E74.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_80059E74");;
s32 func_8005A598(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005A634\n\t.ent\tfunc_8005A634\nfunc_8005A634:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005A634.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005A634");;
void func_8005A858(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005A874\n\t.ent\tfunc_8005A874\nfunc_8005A874:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005A874.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005A874");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005AB78\n\t.ent\tfunc_8005AB78\nfunc_8005AB78:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005AB78.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005AB78");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005ACA8\n\t.ent\tfunc_8005ACA8\nfunc_8005ACA8:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005ACA8.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005ACA8");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005B454\n\t.ent\tfunc_8005B454\nfunc_8005B454:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005B454.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005B454");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005B5D8\n\t.ent\tfunc_8005B5D8\nfunc_8005B5D8:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005B5D8.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005B5D8");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005B85C\n\t.ent\tfunc_8005B85C\nfunc_8005B85C:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005B85C.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005B85C");;
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005B98C\n\t.ent\tfunc_8005B98C\nfunc_8005B98C:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005B98C.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005B98C");;
void func_8005BC90(MOJI_TASK *m);
asm(".section .text\n\t.align\t2\n\t.globl\tfunc_8005BCE4\n\t.ent\tfunc_8005BCE4\nfunc_8005BCE4:\n.include \"config/../asm/rock_neo/nonmatchings/moji/func_8005BCE4.s\"\n\t.set reorder\n\t.set at\n\t.end\tfunc_8005BCE4");;
