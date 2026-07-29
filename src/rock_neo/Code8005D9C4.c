#include "common.h"

extern u8 D_800BBD87[];
void func_8005D528(void);

void func_8005D9C4(void) {
    D_800BBD87[0] = 0;
    func_8005D528();
}
