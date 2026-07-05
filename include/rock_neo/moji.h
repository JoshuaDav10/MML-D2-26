#ifndef ROCK_NEO_MOJI_H
#define ROCK_NEO_MOJI_H

#include "rock_neo.h"

typedef struct MOJI_TASK {
    u32 flags;    // 0x00 (MOJI_TASK0_ON etc.)
    u16 x4;       // 0x04 — zeroed as sh by MojiTaskExec
    u16 x6;
    u16 x8;
    u16 xA;
    u16 xC;
    u16 xE;
    u16 x10;
    s16 x12;
    u8* script;   // 0x14 — script/text stream pointer
    u8* stack[8]; // 0x18 — script call stack (func_80054AB4 pops via xBE)
    u16 x38;      // 0x38 — zeroed as sh by MojiTaskExec
    u8 x3A;
    u8 x3B;
    u8 x3C;
    s8 x3D;
    u8 x3E;       // read back with lbu (MojiTaskExec x3C/x70 copies)
    s8 x3F;
    s32 x40;
    u8* x44;      // 0x44 — base of a u16 jump-offset table (func_80058740)
    u8* x48;      // 0x48 — script entry pointer (script_base + offset-table entry)
    u8* stack2[8]; // 0x4C — script2 call stack (func_80054A84 pops via xC0)
    u8* script2;  // 0x6C — second script stream pointer
    u8 x70;
    u8 x71;
    s8 x72;
    s8 x73; // read back signed (func_80059530 lb)
    u8 x74[0x78 - 0x74];
    u8 x78;       // MojiTaskExec stores 0x80 (unsigned constant form)
    u8 x79[0x7C - 0x79];
    u8 x7C;       // 0x7C — index into the D_8008AB08 function table (func_80054B4C)
    u8 x7D;
    u8 x7E;
    u8 x7F;
    u8 x80[0xBC - 0x80];
    u16 xBC;      // 0xBC — cleared by opcode handlers (func_80054BB4 etc.)
    u16 xBE;      // 0xBE — script stack pointer (index into stack[])
    u16 xC0;      // 0xC0 — script2 stack pointer (index into stack2[])
    u16 xC2;      // 0xC2 — index into the x44 u16 offset table (func_80058740)
} MOJI_TASK;

/* unprototyped (K&R): callers pass `op` as a raw int (-1 stays -1);
   the ANSI definition in moji.c keeps the load-bearing u8 entry copy */
s32 MojiTaskExec();
s32 MojiTaskKill(); // 0x80053A30
s32 func_80053788();

extern u8 Moji_flag[8];  // 0x80098A58 (flag masks like MOJI_TASK0_ON view it as u32)
extern MOJI_TASK Moji_work[]; // 0x800BB6B8, stride 0xC4; slot 4 = system slot
extern u32 Moji_flag3; // 0x80098B30

#define MOJI_TASK0_ON 0x8000000
#define MOJI3_PARTS_TASK 0x10000

#endif