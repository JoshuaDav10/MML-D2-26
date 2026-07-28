#include "common.h"

extern void (*D_8008A358[])(void);

void func_8004569C(u8 *o) {
    D_8008A358[o[0x8]]();
}
