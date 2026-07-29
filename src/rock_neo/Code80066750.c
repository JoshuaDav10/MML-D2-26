#include "common.h"

typedef struct { s16 x0; u8 pad2[2]; s32 x4; } OBJ66;

s32 func_80066750(OBJ66 *p) {
    p->x0 = 0;
    p->x4 = 0;
    *(u8 *)&p->x4 = 0;
    return 1;
}
