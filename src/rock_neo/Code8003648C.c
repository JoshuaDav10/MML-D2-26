#include "common.h"

extern void func_800364B0(s32, void *, s16);

void func_8003648C(s32 arg0, void *arg1) {
    func_800364B0(arg0, arg1, *(s16 *)((u8 *)arg1 + 0x56));
}
