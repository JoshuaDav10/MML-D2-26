#ifndef ROCK_NEO_MOJI_H
#define ROCK_NEO_MOJI_H

#include "rock_neo.h"

typedef struct MOJI_TASK {
    u32 flags;    // 0x00 (MOJI_TASK0_ON etc.)
    u8 x4[0x8 - 0x4];
    u16 x8;
    u8 xA[0x10 - 0xA];
    u16 x10;
    s16 x12;
    u8* script;   // 0x14 — script/text stream pointer
    u8* stack[8]; // 0x18 — script call stack (func_80054AB4 pops via xBE)
    u8 x38[0x3B - 0x38];
    u8 x3B;
    u8 x3C;
    s8 x3D;
    s8 x3E;
    s8 x3F;
    s32 x40;
    u8 x44[0x4C - 0x44];
    u8* stack2[8]; // 0x4C — script2 call stack (func_80054A84 pops via xC0)
    u8* script2;  // 0x6C — second script stream pointer
    u8 x70;
    u8 x71;
    s8 x72;
    u8 x73[0x78 - 0x73];
    s8 x78;
    u8 x79[0xBE - 0x79];
    u16 xBE;      // 0xBE — script stack pointer (index into stack[])
    u16 xC0;      // 0xC0 — script2 stack pointer (index into stack2[])
} MOJI_TASK;

unknown_t MojiTaskExec(unknown_t, unknown_t, unknown_t);
s32 MojiTaskKill(); // 0x80053A30
s32 func_80053788();

extern u8 Moji_flag[8];  // 0x80098A58 (flag masks like MOJI_TASK0_ON view it as u32)
extern u32 Moji_flag3; // 0x80098B30

#define MOJI_TASK0_ON 0x8000000
#define MOJI3_PARTS_TASK 0x10000

#endif