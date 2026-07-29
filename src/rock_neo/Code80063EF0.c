#include "common.h"

extern s32 D_800C3528[12];

void func_80063EF0(void) {
    s32 *p = D_800C3528;
    s32 i = 0;

    do {
        i++;
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p += 3;
    } while (i < 4);
}
