#include "common.h"

extern void (*D_8008D7C4[])(void);

void func_8005C978(u8 *o) {
    D_8008D7C4[o[0x4]]();
}
