#include "common.h"

extern void (*D_8008A250[])(void);

void func_80044AB0(u8 *o) {
    D_8008A250[o[0x8]]();
}
