#include "common.h"

extern void (*D_8008A258[])(void);

void func_80044C58(u8 *o) {
    D_8008A258[o[0x8]]();
}
