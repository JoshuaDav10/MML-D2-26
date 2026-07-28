#include "common.h"

extern void (*D_8008FD20[])(void);

s32 func_800666D8(s8 *o) {
    D_8008FD20[o[0x5]]();
    return 0;
}
