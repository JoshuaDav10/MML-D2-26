#include "common.h"

extern void func_80030628(void *, s32, s32, s32, s32);

void func_80030604(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_80030628(arg0, arg1, arg2, arg3, *(s16 *)((u8 *)arg0 + 0x56));
}
