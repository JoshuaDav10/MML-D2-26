#include "common.h"

void func_80049470(u8 *, s32, u8);

void func_80049448(u8 *o) {
    func_80049470(o + 0x14, *(s32 *)(o + 0x78), o[1]);
}
