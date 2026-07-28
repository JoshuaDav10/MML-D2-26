#include "common.h"

extern void (*D_8008A778[])(void);

void func_800485E4(u8 *o) {
    D_8008A778[o[0x4]]();
}
