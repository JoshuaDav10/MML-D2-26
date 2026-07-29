#include "common.h"
#include "rock_neo/player.h"

extern u8 *D_80089EAC[];

s16 func_80037634(PL_WORK *arg0, s32 arg1) {
    u8 *base = D_80089EAC[arg1];
    s8 idx = arg0->weapon_data[arg1].dist_level;
    return *(s16 *)(base + idx * 8 + 4);
}
