#include "common.h"


void func_8002DE04(s32 arg0, s32 arg1, s32 *arg2) {
    s32 *p;
    arg1 = arg1 * 8 + arg0;
    p = (s32 *)arg1;
    p[0] = arg2[0];
    p[1] = arg2[1];
}
