#include "common.h"

extern void (*D_8008A724[])(void);

void func_80047D9C(u8 *o) {
    D_8008A724[o[0x8]]();
}
