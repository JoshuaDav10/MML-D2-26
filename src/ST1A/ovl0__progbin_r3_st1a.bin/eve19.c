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
    u32 eve_free[1]; /* 0x1C — compared with sltiu, so unsigned */
} EVE_WORK;

typedef struct CHECK_BLOCK {
    s8 x;   /* 0x0 */
    s8 z;   /* 0x1 */
    u8 dx;  /* 0x2 */
    u8 dz;  /* 0x3 */
} CHECK_BLOCK;

typedef struct SVECTOR_ { s16 vx, vy, vz, pad; } SVECTOR_;

extern struct { u8 pad0[0x14]; SVECTOR_ pos; } Player_work[]; /* base 0x800B51B0, pos @ +0x14 = 0x800B51C4 */
extern u8 D_800A395F; /* Camera_work.routine_3 (splat name; Camera_work base not yet carved) */
void Swing_Camera2(s32 a, s32 b, s32 c, SVECTOR_ *p);

extern void (*eve19_move_tbl[])(EVE_WORK *evp);
extern void (*eve19_main_tbl[])(EVE_WORK *evp);
void Close_EVE_WORK(EVE_WORK *evp);

#define R0_MAIN 0
#define R0_END  1

void StXX_eve19_move(EVE_WORK *evp) {
    eve19_move_tbl[evp->routine_0](evp);
}

void eve19_main(EVE_WORK *evp) {
    eve19_main_tbl[evp->type](evp);
}

void eve19_end(EVE_WORK *evp) {
    Close_EVE_WORK(evp);
}

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_00);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_01);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_02);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_05);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_06);

INCLUDE_ASM("config/overlay/splat.us.ST1A/../../../asm/ST1A/ovl0__progbin_r3_st1a.bin/nonmatchings/eve19", eve19_main_area_09);

void eve19_main_dummy(EVE_WORK *evp) {
    evp->routine_0 = R0_END;
}

s32 check_block(EVE_WORK *evp, CHECK_BLOCK *cbp) {
    if (((u16)((s8)(Player_work[0].pos.vx >> 9) - cbp->x) < (u16)cbp->dx) &&
        ((u16)((s8)(Player_work[0].pos.vz >> 9) - cbp->z) < (u16)cbp->dz)) {
        return 1;
    } else {
        return 0;
    }
}

s32 check_swing_camera(EVE_WORK *evp) {
    s32 ret = 0;

    switch (evp->routine_2) {
    case 0:
        Swing_Camera2(4, 512, 1024, &Player_work[0].pos);
        evp->routine_2++;
        break;
    case 1:
        if (D_800A395F == 0) {
            if (evp->eve_free[0] >= 90) {
                ret = 1;
            } else {
                evp->routine_2 = 0;
            }
        }
        break;
    }
    evp->eve_free[0]++;
    return ret;
}
