#include "common.h"

void func_8004594C(u8 *arg0)
{
    u8 temp_v0;
    temp_v0 = *(arg0 + 0xB) - 1;
    *(arg0 + 0xB) = temp_v0;
    if (!(temp_v0 & 0xFF)) {
        *(arg0 + 0x8) = (u8)(*(arg0 + 0x8) + 1);
    }
}
