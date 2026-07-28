#include "common.h"

extern void (*D_8008A708[])(void);

void func_80047318(u8 *o) {
    D_8008A708[o[0x8]]();
}
