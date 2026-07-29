#include "common.h"


typedef struct {
    char pad0[8];
    u8 x8;
    char pad9[0x25];
    s16 x2E;
    char pad30[4];
    u16 x34;
} OBJ;

void func_8004676C(OBJ *o) {
    o->x2E = -0x28;
    o->x34 = 5;
    o->x8 = o->x8 + 1;
}
