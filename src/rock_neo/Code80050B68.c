#include "common.h"

typedef struct { u8 pad0[2]; s16 x2; } OBJ50;   /* or use the real struct once typed */

s32 func_80050B68(OBJ50 *p) {
    return p->x2 != 0;
}
