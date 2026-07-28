#include "common.h"

extern void (*D_8008A0CC[])(void);

void func_800433C0(u8 *o) {
    D_8008A0CC[o[0x8]]();
}
