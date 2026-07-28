#include "common.h"

extern void (*D_8008A0C4[])(void);

void func_80042734(s8 *o) {
    D_8008A0C4[o[0x171]]();
}
