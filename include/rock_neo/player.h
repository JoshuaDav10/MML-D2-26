#ifndef ROCK_NEO_PLAYER_H
#define ROCK_NEO_PLAYER_H

#include "rock_neo.h"

typedef struct WPN_DATA {
    s8 attack_level;
    s8 bullet_level;
    s8 dist_level;
    s8 repeat_level;
    s8 unk[0x4];
} WPN_DATA;

extern WPN_DATA Rock_buster_parts_tbl[];

typedef struct PL_WORK {
    u8 x0[0x9];
    u8 x9;        // routine/state id (func_800406A8 family sets 5/7/8)
    u16 xA;       // sub-state counter (low byte checked by func_80040140)
    u8 xC[0x56 - 0xC];
    u16 x56;      // func_80041E90 -=/+= x116 around func_8002FEA4
    u8 x58[0x6E - 0x58];
    u16 life;
    u8 x70[0x74 - 0x70];
    u8 x74;       // set to 1 with x75 by func_8003EE68
    u8 x75;
    u8 x76[0xB4 - 0x76];
    u16 xB4;      // cleared by func_80042044 when keys don't select the action
    u8 xB6[0xBD - 0xB6];
    u8 xBD;
    u8 xBE[0x10C - 0xBE];
    u16 x10C;     // func_80041EF4 clears when no key match
    u8 x10E[0x110 - 0x10E];
    s8 x110;
    u8 x111;
    u8 x112;      // func_80040380 swaps into x113 and zeroes
    u8 x113;
    u8 x114[0x116 - 0x114];
    u16 x116;     // func_80041E90 offset applied to x56
    u8 x118[0x11C - 0x118];
    u16 x11C;     // key/input mask (func_80040B34 tests vs x13E)
    u16 x11E;     // key/input mask (func_800406A8 family tests vs x138)
    u8 x120[0x124 - 0x120];
    u16 x124;     // key/input mask (func_800402C4 tests vs x11C)
    u16 x126;     // key/input mask (func_80041EF4 alt of x124)
    u16 x128;     // key/input mask (func_800405F4 ORs with x12A, tests vs x11C)
    u16 x12A;     // key/input mask
    u8 x12C[0x134 - 0x12C];
    u16 x134;     // key/input mask (func_800402C4 tests vs x11C)
    u8 x136[0x138 - 0x136];
    u16 x138;
    u8 x13A[0x13E - 0x13A];
    u16 x13E;
    u16 x140;     // key/input mask (func_80040AEC tests vs x11C)
    u8 x142[0x170 - 0x142];
    s8 weapon_right_no;
    u8 x171[0x449 - 0x171];
    u8 x449;
    u8 x44A[0x450 - 0x44A];
    u8 rb_parts_equip_data[4];
    u8 rb_parts_sort_data[0x454 + 0x20 - 0x454];
    u8 x474[0x474 + 0x80 - 0x474];
    WPN_DATA weapon_data[2];
} PL_WORK;
extern PL_WORK Player_work;

unknown_t Pl00_shot_enable_off(PL_WORK*);
unknown_t Pl00_shot_enable_on(PL_WORK*);

#define PL_KEY_ITEM_06 0x526

typedef struct _unkstruc_800C3558 {
    u8 x0[0x14];
    s8 x14;
    s8 x15;
    u8 x16, x17;
} _unkstruc_800C3558;
unknown_t func_80015EE8(PL_WORK*, _unkstruc_800C3558*, unknown_t);
extern _unkstruc_800C3558 D_800C3558;

#endif
