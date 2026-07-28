#include "common.h"

extern void (*D_80082104[])(u8 *, u8 *);

void func_800153EC(u8 *o) {
    D_80082104[o[0x8]](o, o + 0x348);
}
