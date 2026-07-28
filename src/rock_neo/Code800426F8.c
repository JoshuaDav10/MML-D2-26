#include "common.h"

extern void (*D_8008A08C[])(void);

void func_800426F8(s8 *o) {
    D_8008A08C[o[0x170]]();
}
