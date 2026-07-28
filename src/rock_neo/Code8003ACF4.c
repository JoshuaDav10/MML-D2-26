#include "common.h"

extern s32 D_8015C008[];

void func_8003ACF4(s32 i, s32 v) {
    D_8015C008[i * 3] = v;
}
