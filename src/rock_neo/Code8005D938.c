#include "common.h"

extern u8  D_800BBD87;
extern s16 D_800BBD94;

s32 func_8005D938(void) {
    if (D_800BBD94 > 0) {
        return 0;
    }
    return D_800BBD87;
}
