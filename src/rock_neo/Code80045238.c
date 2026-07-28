#include "common.h"

extern void (*D_8008A290[])(void);

void func_80045238(u8 *o) {
    D_8008A290[o[0x8]]();
}
