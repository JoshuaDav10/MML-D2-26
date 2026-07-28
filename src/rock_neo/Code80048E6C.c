#include "common.h"

s32 func_80048EA4(u8 *, s32, s32, u8);

s32 func_80048E6C(u8 *o) {
    return func_80048EA4(o + 0x14, *(s32 *)(o + 0x78),
                         *(s16 *)(o + 0x16) - *(s16 *)(o + 0x2E), o[1]);
}
