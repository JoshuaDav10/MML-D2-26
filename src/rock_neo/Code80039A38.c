#include "common.h"

extern s32 D_80097D6C;
extern s32 D_80097D70;

void func_800399EC(void);

void func_80039A38(void) {
    D_80097D6C = 0;
    D_80097D70 = 0;
    func_800399EC();
}
