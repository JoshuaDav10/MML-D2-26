#include "common.h"
#include "rock_neo/player.h"
#include "rock_neo/scene.h"

extern s32 D_80097D2C;
extern s32 D_80098670;
extern s32 D_80098674;

void func_80039DE8(void) {
    D_80097D2C = 0x200;
    Player_work.x44C[1] = 0;
    Scene_work.x1 = D_80098674;
    Scene_work.x0 = D_80098670;
}
