#include "common.h"

extern void (*D_8008A220[])(void);

void func_8004495C(u8 *o) {
    D_8008A220[o[0x8]]();
}
