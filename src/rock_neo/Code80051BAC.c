#include "common.h"

extern u8 *D_80098898;

void func_80051BAC(u8 *a0) {
    if (*a0 & 3) {
        u8 **p = (u8 **)D_80098898;
        *p = a0;
        D_80098898 = (u8 *)(p + 1);
    }
    *(s16 *)(a0 + 0x7C) = 0;
}
