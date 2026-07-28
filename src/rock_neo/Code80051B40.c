#include "common.h"

extern u8 D_800BC058[];
extern u8 D_800BC360[];
extern u8 *D_80098894;
extern u8 *D_80098898;

void func_80051B40(void) {
    D_80098894 = D_800BC058;
    D_80098898 = D_800BC360;
}
