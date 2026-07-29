#include "common.h"

s32 func_8002FE74(u32 arg0)
{
    if (arg0 < 0x20) {
        arg0 = arg0 & 0x10;
    } else {
        arg0 = arg0 & 0xE0;
        if (arg0 == 0xA0) {
            arg0 = 0x80;
        }
    }
    return arg0;
}
