#include "common.h"

extern void (*D_8008FD20[])(void);

s32 func_80066714(s8 *o) {
    D_8008FD20[o[0x5]]();
    return 0;
}
