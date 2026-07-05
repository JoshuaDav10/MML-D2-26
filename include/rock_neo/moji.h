#ifndef ROCK_NEO_MOJI_H
#define ROCK_NEO_MOJI_H

#include "rock_neo.h"

typedef struct MOJI_TASK {
    u32 flags;    // 0x00 (MOJI_TASK0_ON etc.)
    u8 x4[0x10 - 0x4];
    u16 x10;
    s16 x12;
    u8* script;   // 0x14 — script/text stream pointer
    u8 x18[0x3B - 0x18];
    u8 x3B;
    u8 x3C;
    s8 x3D;
    s8 x3E;
    s8 x3F;
    s32 x40;
    u8 x44[0x6C - 0x44];
    u8* script2;  // 0x6C — second script stream pointer
    s16 x70;
    s8 x72;
    u8 x73[0x78 - 0x73];
    s8 x78;
} MOJI_TASK;

unknown_t MojiTaskExec(unknown_t, unknown_t, unknown_t);
s32 MojiTaskKill(); // 0x80053A30
s32 func_80053788();

extern u8 Moji_flag[8];  // 0x80098A58 (flag masks like MOJI_TASK0_ON view it as u32)
extern u32 Moji_flag3; // 0x80098B30

#define MOJI_TASK0_ON 0x8000000
#define MOJI3_PARTS_TASK 0x10000

#endif