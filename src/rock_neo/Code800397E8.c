#include "common.h"
#include "rock_neo/player.h"

extern s32 D_800A396C;
extern s32 D_80097D2C;
extern s32 D_800A3964;
extern s8 D_800A3962;

void func_800397E8(void) {
    D_800A396C = 0x30;
    D_80097D2C = 0x200;
    D_800A3964 = 0x200;
    D_800A3962 = -1;
    Player_work.x44C[1] = 0;
}
