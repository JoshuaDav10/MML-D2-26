#ifndef ROCK_NEO_SCENE_H
#define ROCK_NEO_SCENE_H

#include "rock_neo.h"

typedef struct SCENE_WORK {
    u8 x0;
    u8 x1;        // Sce flag index base (func_8001DE84: Sce_flag_test(x1 + 0x380))
    u8 x2[0x8 - 0x2];
    u8 x8;         // func_8001F20C copies arg0[0] here
    u8 x9;         // func_8001F20C copies arg0[1] here
    u8 xA[0x10 - 0xA];
    s32 x10;
    u8 x14[0x18 - 0x14];
    s32 x18;
    s32 x1C;
    u8 x20[0xA4 - 0x20];
    u8 *xA4;       // func_8001F20C stores its arg0 pointer here
} SCENE_WORK;      // 0x800C4C48, size 0xA8 per splat syms

extern SCENE_WORK Scene_work;

#endif
