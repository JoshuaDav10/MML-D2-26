#include "common.h"

/* EVE_WORK field offsets recovered from the ST1A asm; names from Capcom's own
   eve19.c (left uncompiled in the JP/PSP ST1E.BIN, documented on TCRF).
   Only the fields eve19 touches are typed so far. */
typedef struct EVE_WORK {
    u8 pad0[0x3];    /* 0x00 */
    u8 type;         /* 0x03 — lbu, indexes eve19_main_tbl */
    u8 pad4[0x4];    /* 0x04 */
    u8 routine_0;    /* 0x08 — lbu, indexes eve19_move_tbl */
    u8 routine_1;    /* 0x09 */
    u8 routine_2;    /* 0x0A */
    u8 padB[0x1C - 0xB];
    s32 eve_free[1]; /* 0x1C */
} EVE_WORK;

#define R0_MAIN 0
#define R0_END  1

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", StXX_eve19_move);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_end);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_00);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_01);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_02);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_05);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_06);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_09);

void eve19_main_dummy(EVE_WORK *evp) {
    evp->routine_0 = R0_END;
}

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", check_block);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", check_swing_camera);
