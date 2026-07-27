#define ACCEPT_REORDERING_BULLSHIT
#include "common.h"

#include "rock_neo/cd.h"
#include "rock_neo/game.h"
#include "rock_neo/joy.h"
#include "rock_neo/moji.h"
#include "rock_neo/obj.h"
#include "rock_neo/player.h"
#include "rock_neo/sce.h"
#include "rock_neo/sound.h"
#include "rock_neo/sub_scrn.h"

/* --- decls: parallel grind wave 2, 2026-07-26 --- */
extern u8 D_8008DAF4[];

void func_8005EC34(void) {
    Sub_screen_back_ground_set();
    D_8008DBB0[D_800A38F0.routine_0](&D_800A38F0);
}

s32 func_8005EC80(s32* arg0) {
    MojiTaskKill();
    MojiTaskExec(0, D_8008CB94, -1);
    func_80063FC0(0, 0x20006); // sus second parameter, looks like an enum
    Sub_screen_basic_param_set();
    *arg0 = 1;
    return 0;
}

// clang-format off

 // https://decomp.me/scratch/2QuPj
INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005ECE0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005ED9C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005F210);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005F3D0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005F608);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005F9E8);

void func_8005FAE4(SUB_SCREEN_WORK* subp) {
    switch (((s8*)subp)[2]) {
    case 0:
        if (Sce_flag_test(D_8008DAF4[((u8*)subp)[4]] + 0x520)) {
            Sound_call(SE_DECISION, 1, 0);
            MojiTaskExec(0, 0x801F2000, ((u8*)subp)[5]);
        } else {
            Sound_call(SE_CANCEL, 1, 0);
        }
        ((s8*)subp)[2]++;
        break;
    case 1:
        if (!((*(u32*)0x80098A58) & MOJI_TASK0_ON)) {
            ((s8*)subp)[1] = 2;
            ((s8*)subp)[2] = 0;
        }
        break;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005FBB4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005FDE4);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_8005FFBC);

// clang-format on

s32 func_800600CC(SUB_SCREEN_WORK* subp) {
    switch (subp->routine_1) {
    case 0: {
        MojiTaskKill();
        Game_logo_kill(-1);
        Cd_read_comb(205);
        subp->routine_1++;
        break;
    }
    case 1: {
        if (Cd_read_sync2() != 0) {
            break;
        }
        func_8001D7AC(22);
        MojiTaskExec(0, 0x801F2000, 0x1D); // ???
        func_80063FC0(2, 0x20007);
        func_800605DC();
        subp->routine_1++;
        break;
    }
    case 2: {
        /* raw-address derefs of Moji_flag (0x80098A58) / Moji_flag3
           (0x80098B30): this function needs the 2-insn lui form while the
           rest of the TU (and the gp census) uses %gp_rel — the constant
           deref assembles to the same bytes as the original's lui/lw */
        if (((*(u32 *)0x80098A58) & 0x480000FF) == 0x48000002) {
            Cd_read_comb(EXIT_SUB_BIN);
            subp->routine_1++;
            break;
        }
        if (!((*(u32 *)0x80098A58) & 0x8000000)) {
            if (!((*(u32 *)0x80098B30) & 0x10000)) {
                if ((*(u32 *)0x80098B30) & 0x80000) {
                    func_80060248(subp);
                }
            } else {
                if ((*(u32 *)0x80098B30) & 0x40000) {
                    func_80060248(subp);
                }
            }
        }
        func_800605DC();
        Sub_screen_shift_check(subp);
        break;
    }
    case 3: {
        if (Cd_read_sync2() == 0) {
            *(u32*)&subp->routine_0 = 0;
        }
        break;
    }
    }
    return 0;
}

// clang-format off

void func_80060248(SUB_SCREEN_WORK* s) {
    /* byte-offset form: several of these slots fall inside pad arrays of
       the current SUB_SCREEN_WORK typing */
    u8 a = ((u8 *)s)[0x12];
    u8 b = ((u8 *)s)[0x15];
    u8 c = ((u8 *)s)[0x1A];
    u8 d = ((u8 *)s)[0x1D];
    u8 e = ((u8 *)s)[0x22];
    u8 f = ((u8 *)s)[0x25];
    u8 g = ((u8 *)s)[0x2A];
    u8 h = ((u8 *)s)[0x2D];
    u8 i = ((u8 *)s)[0x32];
    u8 j = ((u8 *)s)[0x35];

    ((u8 *)s)[0x6] = 3;
    ((u8 *)s)[0x10] = a;
    ((u8 *)s)[0x13] = b;
    ((u8 *)s)[0x18] = c;
    ((u8 *)s)[0x1B] = d;
    ((u8 *)s)[0x20] = e;
    ((u8 *)s)[0x23] = f;
    ((u8 *)s)[0x28] = g;
    ((u8 *)s)[0x2B] = h;
    ((u8 *)s)[0x30] = i;
    ((u8 *)s)[0x33] = j;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_800602A4);

void func_800602A4(SUB_SCREEN_WORK*, s32);

void func_800605DC(void) {
    Sub_screen_rb_parts_calc(&D_800A38F0);
    func_800602A4(&D_800A38F0, 0);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_80060618);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_80060B00);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_80060C70);

// clang-format on

#ifndef ACCEPT_REORDERING_BULLSHIT
// clang-format off
INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", func_80060DB8);
#else
unknown_t func_80060DB8(SUB_SCREEN_WORK* subp) {
    PL_WORK* pp = &Player_work;

    if (((subp->weapon_no != pp->weapon_right_no) ||
         (subp->x8 != *(u16*)&pp->rb_parts_equip_data[0])) ||
        (subp->rb_parts_no_2 != pp->rb_parts_equip_data[2])) {

        Pl00_shot_enable_off(pp);
        Pl00_shot_enable_on(pp);

        if (subp->weapon_no != pp->weapon_right_no)
            Obj_work_flag_change(WORK_PL_SHL, WORK_KILL);

        pp->x110 = 0;
    }
    return (1);
}
#endif

s32 Sub_screen_cancel_check(void) {
    u32 joy_trg;

    // joy_trg = Joy1.trg;
    joy_trg = D_800C0C2A;

    // if( (joy_trg & (JOY_CANCEL | JOY_ST)) ){
    if ((joy_trg & 0x1008) != 0) {
        Sound_call(SE_CANCEL, 1, 0);

        MojiTaskKill();
        Game_logo_kill(-1);
        Cd_read_comb(EXIT_SUB_BIN);

        return (1);
    } else {
        return (0);
    }
}

#ifndef ACCEPT_REORDERING_BULLSHIT

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_shift_check);
#else
void Sub_screen_shift_check(SUB_SCREEN_WORK* subp) {
    u32 joy_trg;

    joy_trg = D_800C0C2A;

    if (joy_trg & (JOY_L1 | JOY_R1)) {
        Sound_call(SE_DECISION, 1, 0);

        MojiTaskKill();
        Game_logo_kill(-1);
        if (joy_trg & JOY_L1) {
            if ((--subp->routine_0) < 0x02)
                subp->routine_0 = 0x06;
        } else {
            if ((++subp->routine_0) > 0x06)
                subp->routine_0 = 0x02;
        }
        subp->routine_1 = 0;
        subp->routine_2 = 0;
    }
}
#endif

// clang-format off

void Sub_screen_sort_attack(void) {
    s32 d0, d1;
    u8 sort_src, sort_dist;

    PL_WORK* pp;

    pp = &Player_work;

    for (d0 = 0; d0 < 0x1f; d0++) {
        for (d1 = 0; d1 < 0x1f - d0; d1++) {
            if (!(sort_dist = pp->rb_parts_sort_data[d1 + 1]))
                continue;
            if ((!(sort_src = pp->rb_parts_sort_data[d1])) ||
                (Rock_buster_parts_tbl[sort_src - 1].attack_level <
                 Rock_buster_parts_tbl[sort_dist - 1].attack_level))
                Sub_screen_sort_sub(pp, d1, d1 + 1);
        }
    }
}

void Sub_screen_sort_energy(void) {
    s32 d0, d1;
    u8 sort_src, sort_dist;

    PL_WORK* pp;

    pp = &Player_work;

    for (d0 = 0; d0 < 0x1f; d0++) {
        for (d1 = 0; d1 < 0x1f - d0; d1++) {
            if (!(sort_dist = pp->rb_parts_sort_data[d1 + 1]))
                continue;
            if ((!(sort_src = pp->rb_parts_sort_data[d1])) ||
                (Rock_buster_parts_tbl[sort_src - 1].bullet_level <
                 Rock_buster_parts_tbl[sort_dist - 1].bullet_level))
                Sub_screen_sort_sub(pp, d1, d1 + 1);
        }
    }
}

void Sub_screen_sort_range(void) {
    s32 d0, d1;
    u8 sort_src, sort_dist;

    PL_WORK* pp;

    pp = &Player_work;

    for (d0 = 0; d0 < 0x1f; d0++) {
        for (d1 = 0; d1 < 0x1f - d0; d1++) {
            if (!(sort_dist = pp->rb_parts_sort_data[d1 + 1]))
                continue;
            if ((!(sort_src = pp->rb_parts_sort_data[d1])) ||
                (Rock_buster_parts_tbl[sort_src - 1].dist_level <
                 Rock_buster_parts_tbl[sort_dist - 1].dist_level))
                Sub_screen_sort_sub(pp, d1, d1 + 1);
        }
    }
}

void Sub_screen_sort_rapid(void) {
    s32 d0, d1;
    u8 sort_src, sort_dist;

    PL_WORK* pp;

    pp = &Player_work;

    for (d0 = 0; d0 < 0x1f; d0++) {
        for (d1 = 0; d1 < 0x1f - d0; d1++) {
            if (!(sort_dist = pp->rb_parts_sort_data[d1 + 1]))
                continue;
            if ((!(sort_src = pp->rb_parts_sort_data[d1])) ||
                (Rock_buster_parts_tbl[sort_src - 1].repeat_level <
                 Rock_buster_parts_tbl[sort_dist - 1].repeat_level))
                Sub_screen_sort_sub(pp, d1, d1 + 1);
        }
    }
}

#ifndef ACCEPT_REORDERING_BULLSHIT
void Sub_screen_sort_sub(PL_WORK *pw, s32 arg1, s32 arg2) {
    u8 tmp = pw->rb_parts_sort_data[arg1];
    pw->rb_parts_sort_data[arg1] = pw->rb_parts_sort_data[arg2];
    pw->rb_parts_sort_data[arg2] = tmp;
}
#else
void Sub_screen_sort_sub(PL_WORK* pp, s32 d0, s32 d1) {
    u8 sort_buff;

    sort_buff = pp->rb_parts_sort_data[d0];
    pp->rb_parts_sort_data[d0] = pp->rb_parts_sort_data[d1];
    pp->rb_parts_sort_data[d1] = sort_buff;
}
#endif

#ifndef ACCEPT_REORDERING_BULLSHIT
INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_rb_parts_set);
#else
void Sub_screen_rb_parts_set(void) {
    s32 d0, d1, d2;

    PL_WORK* pp;

    pp = &Player_work;

    if (Sce_flag_test(PL_KEY_ITEM_06))
        d0 = 3;
    else
        d0 = 2;

    pp->weapon_data[1].attack_level = 0x00;
    pp->weapon_data[1].bullet_level = 0x00;
    pp->weapon_data[1].dist_level = 0x00;
    pp->weapon_data[1].repeat_level = 0x00;

    for (d1 = 0; d1 < d0; d1++) {
        if (!(d2 = pp->rb_parts_equip_data[d1]))
            continue;
        pp->weapon_data[1].attack_level +=
            Rock_buster_parts_tbl[d2 - 1].attack_level;
        pp->weapon_data[1].bullet_level +=
            Rock_buster_parts_tbl[d2 - 1].bullet_level;
        pp->weapon_data[1].dist_level +=
            Rock_buster_parts_tbl[d2 - 1].dist_level;
        pp->weapon_data[1].repeat_level +=
            Rock_buster_parts_tbl[d2 - 1].repeat_level;
    }
    if (pp->weapon_data[1].attack_level > 0x07)
        pp->weapon_data[1].attack_level = 0x07;

    if (pp->weapon_data[1].bullet_level > 0x07)
        pp->weapon_data[1].bullet_level = 0x07;

    if (pp->weapon_data[1].dist_level > 0x07)
        pp->weapon_data[1].dist_level = 0x07;

    if (pp->weapon_data[1].repeat_level > 0x07)
        pp->weapon_data[1].repeat_level = 0x07;
}
#endif

// clang-format off

#if defined(USE_OG_COMPILER) && !defined(ACCEPT_REORDERING_BULLSHIT) /* draft compiles 6 insns short (0x1AC vs 0x1C4) — not matched yet */
void Sub_screen_rb_parts_calc(SUB_SCREEN_WORK* subp) {
    s32 d0, d1, d2;

    PL_WORK* pp;

    pp = &Player_work;

    if (Sce_flag_test(PL_KEY_ITEM_06))
        d0 = 3;
    else
        d0 = 2;

    subp->attack_end_0 = subp->attack_end_1 = 0x00;
    subp->bullet_end_0 = subp->bullet_end_1 = 0x00;
    subp->dist_end_0 = subp->dist_end_1 = 0x00;
    subp->repeat_end_0 = subp->repeat_end_1 = 0x00;

    if ((*(u32 *)Moji_flag) & MOJI_TASK0_ON) {
        for (d1 = 0; d1 < d0; d1++) {
            if (!(d2 = pp->rb_parts_equip_data[d1]))
                continue;
            Sub_screen_rb_parts_calc_sub00(subp, d2 - 1);
            Sub_screen_rb_parts_calc_sub01(subp, d2 - 1);
        }
    } else {
        if (Moji_flag3 & MOJI3_PARTS_TASK) {
            for (d1 = 0; d1 < d0; d1++) {
                if ((d2 = pp->rb_parts_equip_data[d1]))
                    Sub_screen_rb_parts_calc_sub00(subp, d2 - 1);

                if (d1 == GET_SELECT_NO(1))
                    continue;
                if (d2)
                    Sub_screen_rb_parts_calc_sub01(subp, d2 - 1);
            }
        } else {
            for (d1 = 0; d1 < d0; d1++) {
                //				if( (d1 != GET_SELECT_NO(1)) &&
                //										(d2 =
                // pp->rb_parts_equip_data[d1])
                //)
                if ((d2 = pp->rb_parts_equip_data[d1]))
                    Sub_screen_rb_parts_calc_sub00(subp, d2 - 1);

                if (d1 == GET_SELECT_NO(1))
                    d2 = GET_PARTS_NO(1);
                if (d2)
                    Sub_screen_rb_parts_calc_sub01(subp, d2 - 1);
            }
        }
    }
}
#else
INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_rb_parts_calc);
#endif

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_rb_parts_calc_sub00);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_rb_parts_calc_sub01);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_status_calc);

void Sub_screen_gauge_set(s32 x0, s32 y0, s32 power, s32 flag) {
    POLY_FT4 *ft4_buff_ptr1, *ft4_buff_ptr2;

    ft4_buff_ptr1 = ft4_buff_ptr2 = PRIM_PTR(POLY_FT4);

    *(u32*)ft4_buff_ptr2 = ((u32)(ft4_buff_ptr2 + 1) & 0x00ffffff) | 0x09000000;
    *(u32*)&ft4_buff_ptr2->r0 = 0x2c808080;

    ft4_buff_ptr2->x0 = ft4_buff_ptr2->x2 = x0;
    ft4_buff_ptr2->x1 = ft4_buff_ptr2->x3 = x0 + power + 0x06;
    ft4_buff_ptr2->y0 = ft4_buff_ptr2->y1 = y0 + flag * 0x02;
    ft4_buff_ptr2->y2 = ft4_buff_ptr2->y3 = y0 + flag * 0x02 + 0x05;

    ft4_buff_ptr2->tpage = 0x0c;
    ft4_buff_ptr2->clut = 0xfa0c >> 2;

    *(u16*)&ft4_buff_ptr2->u0 = 0x60 | ((0x90 + flag * 0x08) << 8);
    *(u16*)&ft4_buff_ptr2->u1 = 0x61 | ((0x90 + flag * 0x08) << 8);
    *(u16*)&ft4_buff_ptr2->u2 = 0x60 | ((0x95 + flag * 0x08) << 8);
    *(u16*)&ft4_buff_ptr2->u3 = 0x61 | ((0x95 + flag * 0x08) << 8);

    ft4_buff_ptr2++;

    *(u32*)ft4_buff_ptr2 = ((u32)(ft4_buff_ptr2 + 1) & 0x00ffffff) | 0x09000000;
    *(u32*)&ft4_buff_ptr2->r0 = 0x2c808080;

    ft4_buff_ptr2->x0 = ft4_buff_ptr2->x2 = x0 + power + 0x06;
    ft4_buff_ptr2->x1 = ft4_buff_ptr2->x3 = x0 + power + 0x0a;
    ft4_buff_ptr2->y0 = ft4_buff_ptr2->y1 = y0 + flag * 0x02;
    ft4_buff_ptr2->y2 = ft4_buff_ptr2->y3 = y0 + flag * 0x02 + 0x05;

    ft4_buff_ptr2->tpage = 0x0c;
    ft4_buff_ptr2->clut = 0xfa0c >> 2;

    *(u16*)&ft4_buff_ptr2->u0 = 0x6f | ((0x90 + flag * 0x08) << 8);
    *(u16*)&ft4_buff_ptr2->u1 = 0x73 | ((0x90 + flag * 0x08) << 8);
    *(u16*)&ft4_buff_ptr2->u2 = 0x6f | ((0x95 + flag * 0x08) << 8);
    *(u16*)&ft4_buff_ptr2->u3 = 0x73 | ((0x95 + flag * 0x08) << 8);

    ft4_buff_ptr2++;

    if ((!flag) && (((y0 != 0x46) && (power == 0x0a * 7)) ||
                    ((y0 == 0x46) && (power == 0x0a * 4)))) {
        *(u32*)ft4_buff_ptr2 = ((u32)(ft4_buff_ptr2 + 1) & 0x00ffffff) | 0x04000000;
        *(u32*)&ft4_buff_ptr2->r0 = 0x64808080;
        ((SPRT*)ft4_buff_ptr2)->x0 = x0 + 0x0a + (power == 0x0a * 7) * 0x0e;
        ((SPRT*)ft4_buff_ptr2)->y0 = y0;
        *(u32*)&(((SPRT*)ft4_buff_ptr2)->u0) = 0x28 | (0xf0 << 8) | ((0xfa08 >> 2) << 16);
        *(u32*)&(((SPRT*)ft4_buff_ptr2)->w) = 0x00060018;

        ft4_buff_ptr2++;
    }
    addPrims(&D_80098934->x70[2], ft4_buff_ptr1, ft4_buff_ptr2 - 1);
    PRIM_PTR(POLY_FT4) = ft4_buff_ptr2;
}

void Sub_screen_gauge_set2(s32 x0, s32 y0, s32 power, s32 infi) {
    POLY_FT4 *ft4_buff_ptr1, *ft4_buff_ptr2;

    ft4_buff_ptr1 = ft4_buff_ptr2 = PRIM_PTR(POLY_FT4);

    *(u32*)ft4_buff_ptr2 = ((u32)(ft4_buff_ptr2 + 1) & 0x00ffffff) | 0x09000000;
    *(u32*)&ft4_buff_ptr2->r0 = 0x2c808080;

    ft4_buff_ptr2->x0 = ft4_buff_ptr2->x2 = x0;
    ft4_buff_ptr2->x1 = ft4_buff_ptr2->x3 = x0 + power;
    ft4_buff_ptr2->y0 = ft4_buff_ptr2->y1 = y0;
    ft4_buff_ptr2->y2 = ft4_buff_ptr2->y3 = y0 + 0x08;

    ft4_buff_ptr2->tpage = 0x0c;
    ft4_buff_ptr2->clut = 0xfa0c >> 2;

    *(u16*)&ft4_buff_ptr2->u0 = 0x48 | (0x90 << 8);
    *(u16*)&ft4_buff_ptr2->u1 = 0x49 | (0x90 << 8);
    *(u16*)&ft4_buff_ptr2->u2 = 0x48 | (0x98 << 8);
    *(u16*)&ft4_buff_ptr2->u3 = 0x49 | (0x98 << 8);

    ft4_buff_ptr2++;

    *(u32*)ft4_buff_ptr2 = ((u32)(ft4_buff_ptr2 + 1) & 0x00ffffff) | 0x09000000;
    *(u32*)&ft4_buff_ptr2->r0 = 0x2c808080;

    ft4_buff_ptr2->x0 = ft4_buff_ptr2->x2 = x0 + power;
    ft4_buff_ptr2->x1 = ft4_buff_ptr2->x3 = x0 + power + 0x04;
    ft4_buff_ptr2->y0 = ft4_buff_ptr2->y1 = y0;
    ft4_buff_ptr2->y2 = ft4_buff_ptr2->y3 = y0 + 0x08;

    ft4_buff_ptr2->tpage = 0x0c;
    ft4_buff_ptr2->clut = 0xfa0c >> 2;

    *(u16*)&ft4_buff_ptr2->u0 = 0x4c | (0x90 << 8);
    *(u16*)&ft4_buff_ptr2->u1 = 0x50 | (0x90 << 8);
    *(u16*)&ft4_buff_ptr2->u2 = 0x4c | (0x98 << 8);
    *(u16*)&ft4_buff_ptr2->u3 = 0x50 | (0x98 << 8);

    ft4_buff_ptr2++;

    if ((infi) && (power == 88)) {
        *(u32*)ft4_buff_ptr2 = ((u32)(ft4_buff_ptr2 + 1) & 0x00ffffff) | 0x04000000;
        *(u32*)&ft4_buff_ptr2->r0 = 0x64808080;
        ((SPRT*)ft4_buff_ptr2)->x0 = x0 + 0x18;
        ((SPRT*)ft4_buff_ptr2)->y0 = y0;
        *(u32*)&(((SPRT*)ft4_buff_ptr2)->u0) = 0x28 | (0xf8 << 8) | ((0xfa08 >> 2) << 16);
        *(u32*)&(((SPRT*)ft4_buff_ptr2)->w) = 0x00080028;

        ft4_buff_ptr2++;
    }
    addPrims(&D_80098934->x70[2], ft4_buff_ptr1, ft4_buff_ptr2 - 1);
    PRIM_PTR(POLY_FT4) = ft4_buff_ptr2;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_basic_param_set);

SPRT* Sub_screen_basic_param_set_sub(SPRT* ptr, u32 pos, s32 num, s32 flag) {
    s32 d0, d1;

    for (d0 = (flag & 0xFF) - 1; d0 >= 0x00; d0--) {
        d1 = num % 10;
        if (((d0 == (flag & 0xFF) - 1) && (!(flag & 0x0200))) || (num) ||
            (!(flag & 0x0100))) {
            *(u32*)ptr = ((u32)(ptr + 1) & 0x00FFFFFF) | 0x04000000;
            *(u32*)&ptr->r0 = 0x64808080;
            *(u32*)&ptr->x0 = d0 * 0x07 + pos;
            *(u32*)&ptr->w = 0x00080008;
            *(u32*)&ptr->u0 = (d1 * 0x08 + 0x60) | (0xB0 << 8) | (0xF9D4 << 14);
            ptr++;
        }
        num /= 10;
    }
    return ptr;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_status_param_set);

SPRT* Sub_screen_status_param_set_sub(SPRT* ptr, u32 pos, s32 num, s32 flag) {
    s32 d0, d1;

    for (d0 = (flag & 0xFF) - 1; d0 >= 0x00; d0--) {
        d1 = num % 10;
        if (((d0 == (flag & 0xFF) - 1) && (!(flag & 0x0200))) || (num) ||
            (!(flag & 0x0100))) {
            *(u32*)ptr = ((u32)(ptr + 1) & 0x00FFFFFF) | 0x04000000;
            *(u32*)&ptr->r0 = 0x64808080;
            *(u32*)&ptr->x0 = d0 * 0x07 + pos;
            *(u32*)&ptr->w = 0x00080006;
            *(u32*)&ptr->u0 = (d1 * 0x08) | (0xC0 << 8) | (0xFA08 << 14);
            ptr++;
        }
        num /= 10;
    }
    return ptr;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_back_ground_set);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Sub_screen_sound_reinit);

extern s32 Map_screen_work; // sdata ($gp)
extern u8 D_800987F4[4];    // word-cleared, then byte flag set (lui-accessed)

void Map_screen_init(void) {
    Map_screen_work = 0;
    *(u32*)D_800987F4 = 0;
    if (Game_work.x52 < 9) {
        D_800987F4[0] = 0;
    } else {
        D_800987F4[0] = 1;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", Map_screen_task);

void map_screen_set(void) {
    POLY_FT4* ft4_buff_ptr;

    ft4_buff_ptr = PRIM_PTR_INC(POLY_FT4);

    *(u32*)ft4_buff_ptr = 0x09000000;
    *(u32*)&ft4_buff_ptr->r0 = 0x2C808080;

    ft4_buff_ptr->x0 = ft4_buff_ptr->x2 = 32;
    ft4_buff_ptr->x1 = ft4_buff_ptr->x3 = 287;
    ft4_buff_ptr->y0 = ft4_buff_ptr->y1 = 0;
    ft4_buff_ptr->y2 = ft4_buff_ptr->y3 = 240;

    ft4_buff_ptr->tpage = (0x0C) | (0x01 << 7);
    ft4_buff_ptr->clut = (0xFA00 >> 2);

    *(u16*)&ft4_buff_ptr->u0 = 0x00 | (0x00 << 8);
    *(u16*)&ft4_buff_ptr->u1 = 0xFF | (0x00 << 8);
    *(u16*)&ft4_buff_ptr->u2 = 0x00 | (0xF0 << 8);
    *(u16*)&ft4_buff_ptr->u3 = 0xFF | (0xF0 << 8);

    AddPrim(&D_80098934->x70[2], ft4_buff_ptr);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/sub_scrn", map_cursor_set);

// clang-format on
