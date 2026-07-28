#include "common.h"

extern void (*D_8008D7C0[])(void);

void func_8005C868(u8 *o) {
    D_8008D7C0[o[0x4]]();
}
