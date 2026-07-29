#include "common.h"

extern u8 D_800BBD85[];
extern u8 D_800BBD81;

void func_8005D4F8(void)
{
    if (D_800BBD85[0] == 0) {
        D_800BBD85[0] = 2;
        D_800BBD81 = 0x1C;
    }
}
