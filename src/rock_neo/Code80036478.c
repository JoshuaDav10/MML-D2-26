#include "common.h"

void func_80036478(u8 *p, s8 a, u32 b) {
    p[6] = a;
    p[0xF] = b >> 4;
    p[7] = 0;
}
