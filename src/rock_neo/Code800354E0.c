#include "common.h"

extern u32 D_80098798;

void func_800354E0(s32 arg0) {
    D_80098798 &= ~(0x80000000 >> arg0);
}
