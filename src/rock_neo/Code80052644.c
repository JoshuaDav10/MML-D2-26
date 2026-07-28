#include "common.h"

typedef struct {
    u8  pad0[0x76];
    s8  x76;
    u8  pad77[0xC0 - 0x77];
    s32 xC0;
} OBJ52;

void func_80052644(s32 arg0, OBJ52 *o) {
    if ((o->x76 != 0) && (arg0 == o->xC0)) {
        o->x76 = 0;
        o->xC0 = 0;
    }
}
