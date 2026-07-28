#include "common.h"

extern void (*D_8008A208[])(void);

void func_80043F34(u8 *o) {
    D_8008A208[o[0x8]]();
}
