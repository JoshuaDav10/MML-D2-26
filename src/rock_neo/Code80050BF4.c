#include "common.h"

s32 func_80050BF4(u8 *arg0)
{
    if (*(s16 *)(arg0 + 0x2) > 0) {
        *(u8 *)0x1F8000FF = 0x10;
        return 0;
    }
    *(u8 *)0x1F8000FF = 0x20;
    return 1;
}
