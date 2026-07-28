#include "common.h"

extern s8  D_800BBE2C;
extern s8  D_800BBE38;
extern s8  D_800BBE39;
extern s16 D_800BBE44;
extern s16 D_800BBE46;

void func_8005D844(s16 arg0, s16 arg1, s8 arg2) {
    D_800BBE2C = 3;
    D_800BBE44 = arg0;
    D_800BBE46 = arg1;
    D_800BBE38 = arg2;
    D_800BBE39 = 1;
}
