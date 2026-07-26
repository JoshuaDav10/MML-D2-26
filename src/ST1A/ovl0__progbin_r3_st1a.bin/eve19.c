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
s32 check_block(EVE_WORK *evp, CHECK_BLOCK *cbp);
s32 check_swing_camera(EVE_WORK *evp);

extern u32 Moji_flag;                 /* engine, 0x80098A58 */
extern u16 D_800C1C8A[];              /* eve19_check_flag — ARRAY extern: target
                                         materializes the address (lui+addiu), which per
                                         LESSONS means unsized array, not scalar */
extern u16 D_800C1C8C;                /* st1a_console_flag */
extern u8  D_800B526D[];              /* Player_work[0].status_flag (base+0xBD) */
s32  Sce_flag_test(s32 id);
void Sce_flag_on(s32 id);
void Sound_call(s32 req_no, s32 pos_xy, s32 pos_z);
s32  MojiTaskExec2(s32 task_no, s32 mess_no);

#define eve19_check_flag   D_800C1C8A[0]
#define st1a_console_flag  D_800C1C8C
#define pl_status_flag     D_800B526D[0]

#define MOJI_TASK0_ON 0x8000000
#define MOJI_TASK4_ON 0x800000
#define PL_STATUS_CONTROL_OFF 0x40

#define OPEN_SUB_GATE_FLAG    0x56
#define JUNO_ENTER_DEMO_FLAG  0x35
#define JUNO_BOX_CHECK_FLAG   0x604
#define ST1A_DIFFRACTER_FLAG  0x6D

#define FLG_EXE_EV43  0x01
#define FLG_MES_B7F   0x04
#define FLG_MES_B5F   0x08
#define FLG_MES_B3F   0x10
#define FLG_MES_B1F   0x20
#define FLG_OPEN_SUB  0x40

#define MES_OPEN_SUB_0 0x01
#define MES_B5F        0x06
#define MES_B1F        0x07
#define MES_B3F        0x08
#define MES_B8F        0x1B
#define MES_OPEN_SUB_1 0x1D

#define SE_MAIN_CHIKA_YURE      0x208
#define SE_MAIN_CHIKA_YURE_STOP 0x209

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

void eve19_main_area_00(EVE_WORK *evp) {
    switch (evp->routine_1) {
    case 0:
        if (Sce_flag_test(OPEN_SUB_GATE_FLAG)) {
            evp->routine_0 = R0_END;
        } else {
            evp->routine_1++;
        }
        break;
    case 1:
        if (eve19_check_flag & FLG_OPEN_SUB) {
            evp->routine_1++;
        }
        break;
    case 2:
        if (!(Moji_flag & MOJI_TASK0_ON)) {
            MojiTaskExec2(0, MES_OPEN_SUB_0);
            eve19_check_flag &= ~FLG_OPEN_SUB;
            pl_status_flag |= PL_STATUS_CONTROL_OFF;
            evp->routine_1++;
        }
        break;
    case 3:
        if (!(Moji_flag & MOJI_TASK0_ON)) {
            if (Sce_flag_test(OPEN_SUB_GATE_FLAG)) {
                Sound_call(SE_MAIN_CHIKA_YURE, 0, 0);
                evp->routine_1++;
                evp->routine_2 = 0;
                evp->eve_free[0] = 0;
            } else {
                pl_status_flag &= ~PL_STATUS_CONTROL_OFF;
                evp->routine_1 = 1;
            }
        }
        break;
    case 4:
        if (check_swing_camera(evp)) {
            Sound_call(SE_MAIN_CHIKA_YURE_STOP, 0, 0);
            evp->routine_1++;
        }
        break;
    case 5:
        if (!(Moji_flag & MOJI_TASK0_ON)) {
            st1a_console_flag = 1;
            MojiTaskExec2(0, MES_OPEN_SUB_1);
            evp->routine_1++;
        }
        break;
    case 6:
        if (!(Moji_flag & MOJI_TASK0_ON)) {
            pl_status_flag &= ~PL_STATUS_CONTROL_OFF;
            evp->routine_0 = R0_END;
        }
        break;
    }
}

void eve19_main_area_01(EVE_WORK *evp) {
    CHECK_BLOCK check_block_data = {0xfe, 0xfe, 0x04, 0x01};

    if (Sce_flag_test(JUNO_ENTER_DEMO_FLAG)) {
        eve19_check_flag |= FLG_EXE_EV43;
        evp->routine_0 = R0_END;
        return;
    }
    if (check_block(evp, &check_block_data)) {
        Sce_flag_on(JUNO_BOX_CHECK_FLAG);
        eve19_check_flag |= FLG_EXE_EV43;
        evp->routine_0 = R0_END;
    }
}

void eve19_main_area_02(EVE_WORK *evp) {
    CHECK_BLOCK check_block_data = {0x0a, 0xfa, 0x03, 0x01};

    switch (evp->routine_1) {
    case 0:
        if (Sce_flag_test(ST1A_DIFFRACTER_FLAG)) {
            evp->routine_0 = R0_END;
            return;
        }
        if (check_block(evp, &check_block_data)) {
            evp->routine_1++;
        }
        break;
    case 1:
        if (!(Moji_flag & MOJI_TASK0_ON)) {
            if (check_block(evp, &check_block_data)) {
                evp->routine_1++;
            } else {
                evp->routine_1 = 0;
            }
        }
        break;
    case 2:
        MojiTaskExec2(0, MES_B8F);
        pl_status_flag |= PL_STATUS_CONTROL_OFF;
        Sce_flag_on(ST1A_DIFFRACTER_FLAG);
        evp->routine_1++;
        break;
    case 3:
        if (!(Moji_flag & MOJI_TASK0_ON)) {
            pl_status_flag &= (~PL_STATUS_CONTROL_OFF);
            evp->routine_0 = R0_END;
        }
        break;
    }
}

void eve19_main_area_05(EVE_WORK *evp) {
    CHECK_BLOCK check_block_data = {0xfb, 0xea, 0x01, 0x01};

    switch (evp->routine_1) {
    case 0:
        if ((eve19_check_flag & FLG_MES_B5F) || Sce_flag_test(OPEN_SUB_GATE_FLAG)) {
            eve19_check_flag |= FLG_MES_B5F;
            evp->routine_0 = R0_END;
            return;
        }
        if (check_block(evp, &check_block_data)) {
            evp->routine_1++;
        }
        break;
    case 1:
        if (!(Moji_flag & MOJI_TASK4_ON)) {
            if (check_block(evp, &check_block_data)) {
                evp->routine_1++;
            } else {
                evp->routine_1 = 0;
            }
        }
        break;
    case 2:
        MojiTaskExec2(4, MES_B5F);
        eve19_check_flag |= FLG_MES_B5F;
        evp->routine_1++;
        break;
    case 3:
        if (!(Moji_flag & MOJI_TASK4_ON)) {
            evp->routine_0 = R0_END;
        }
        break;
    }
}

void eve19_main_area_06(EVE_WORK *evp) {
    CHECK_BLOCK check_block_data = {0x01, 0xfa, 0x01, 0x01};

    switch (evp->routine_1) {
    case 0:
        if ((eve19_check_flag & FLG_MES_B3F) || Sce_flag_test(OPEN_SUB_GATE_FLAG)) {
            eve19_check_flag |= FLG_MES_B3F;
            evp->routine_0 = R0_END;
            return;
        }
        if (check_block(evp, &check_block_data)) {
            evp->routine_1++;
        }
        break;
    case 1:
        if (!(Moji_flag & MOJI_TASK4_ON)) {
            if (check_block(evp, &check_block_data)) {
                evp->routine_1++;
            } else {
                evp->routine_1 = 0;
            }
        }
        break;
    case 2:
        MojiTaskExec2(4, MES_B3F);
        eve19_check_flag |= FLG_MES_B3F;
        evp->routine_1++;
        break;
    case 3:
        if (!(Moji_flag & MOJI_TASK4_ON)) {
            evp->routine_0 = R0_END;
        }
        break;
    }
}

void eve19_main_area_09(EVE_WORK *evp) {
    CHECK_BLOCK check_block_data = {0xfb, 0x24, 0x01, 0x01};

    switch (evp->routine_1) {
    case 0:
        if (eve19_check_flag & FLG_MES_B1F) {
            evp->routine_0 = R0_END;
            return;
        } else if (Sce_flag_test(OPEN_SUB_GATE_FLAG)) {
            eve19_check_flag |= (FLG_MES_B1F | FLG_MES_B7F);
            evp->routine_0 = R0_END;
            return;
        }
        if (check_block(evp, &check_block_data)) {
            evp->routine_1++;
        }
        break;
    case 1:
        if (!(Moji_flag & MOJI_TASK4_ON)) {
            if (check_block(evp, &check_block_data)) {
                evp->routine_1++;
            } else {
                evp->routine_1 = 0;
            }
        }
        break;
    case 2:
        MojiTaskExec2(4, MES_B1F);
        eve19_check_flag |= FLG_MES_B1F;
        evp->routine_1++;
        break;
    case 3:
        if (!(Moji_flag & MOJI_TASK4_ON)) {
            evp->routine_0 = R0_END;
        }
        break;
    }
}

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
