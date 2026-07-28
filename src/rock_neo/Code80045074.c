#include "common.h"

extern void (*D_8008A288[])(void);

void func_80045074(u8 *o) {
    D_8008A288[o[0x8]]();
}
