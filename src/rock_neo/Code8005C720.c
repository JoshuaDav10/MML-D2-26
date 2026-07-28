#include "common.h"

extern void (*D_8008D7B4[])(void);

void func_8005C720(u8 *o) {
    D_8008D7B4[o[0x4]]();
}
