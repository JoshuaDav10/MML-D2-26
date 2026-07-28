#include "common.h"

extern u8  D_800BEAD8[];
extern u8 *D_8009893C;
extern s8  D_800987D4;
extern s8  D_800987DC;

void func_8002037C(void) {
    D_800BEAD8[0] = 0;
    D_8009893C = D_800BEAD8;
    D_800987D4 = 0;
    D_800987DC = 0;
}
