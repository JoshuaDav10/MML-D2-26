#include "common.h"

extern void (*D_8008A76C[])(void);

void func_80048354(u8 *o) {
    D_8008A76C[o[0x8]]();
}
