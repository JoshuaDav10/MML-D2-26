/* func_8003EEC0 — 178 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003EEC0.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 func_80031268(u8 *, s32);                       /* extern */
s32 func_8003129C(u8 *, s32);                       /* extern */
? func_80039630(s16, ?);                            /* extern */
? func_8003F188(PL_WORK *, s32, ?);                 /* extern */
s32 func_80041044(PL_WORK *);                       /* extern */
s32 func_80041158(PL_WORK *);                       /* extern */
? func_80041EF4(PL_WORK *, ?, ?);                   /* extern */
? func_80042094(PL_WORK *, ?, ?, ?);                /* extern */
extern s32 D_80097D08;
extern s32 D_80098B18;

void func_8003EEC0(PL_WORK *arg0) {
    ? var_a2;
    s16 temp_v0;
    s16 temp_v0_2;
    s16 temp_v0_3;
    s16 temp_v0_4;
    s16 temp_v0_5;
    s16 temp_v0_6;
    s16 temp_v0_7;
    s16 temp_v1;
    s16 temp_v1_2;
    s16 temp_v1_3;
    s32 temp_s2;
    s32 var_s1;
    s32 var_v0;
    u16 temp_s1;

    if (arg0->unkA == 0) {
        arg0->unkA = 1U;
        arg0->unk44 = 0;
        arg0->unk46 = 0;
        arg0->unk48 = 0;
        arg0->x108 = 0;
        arg0->unk10A = 0;
        arg0->unk10E = 0;
    }
    temp_s2 = D_80097D08;
    if (temp_s2 != 0) {
        if (D_80098B18 & 2) {
            if (func_8003F224(arg0) != 0) {
                goto block_5;
            }
            temp_v0 = func_80031268(&arg0->xC[8], temp_s2 + 0x14) & 0xFFF;
            arg0->unk10A = temp_v0;
            var_s1 = (s16) arg0->x56 - temp_v0;
            var_v0 = var_s1 < -0x800;
            if (var_s1 >= 0x801) {
                var_s1 -= 0x1000;
                var_v0 = var_s1 < -0x800;
            }
            if (var_v0 != 0) {
                var_s1 += 0x1000;
            }
            if (!((s8) arg0->x171[3] & 0xC0)) {
                var_a2 = 1;
                goto block_19;
            }
            if (func_80041044(arg0) == 0) {
                var_a2 = 0;
block_19:
                func_8003F188(arg0, var_s1, var_a2);
            }
            temp_v1 = (s16) arg0->x108;
            temp_s1 = func_8003129C(&arg0->xC[8], temp_s2 + 0x14) - 0xC00;
            if (temp_s1 == temp_v1) {
                arg0->unk10E = 0;
            } else if ((temp_s1 - temp_v1) > 0) {
                temp_v0_2 = arg0->unk10E;
                if (temp_v0_2 < 0) {
                    arg0->unk10E = 0;
                } else {
                    temp_v0_3 = temp_v0_2 + 4;
                    arg0->unk10E = temp_v0_3;
                    if (temp_v0_3 >= 0x41) {
                        arg0->unk10E = 0x40;
                    }
                }
                temp_v1_2 = (s16) arg0->x108;
                temp_v0_4 = arg0->unk10E;
                if (temp_v0_4 < (temp_s1 - temp_v1_2)) {
                    arg0->x108 = temp_v1_2 + temp_v0_4;
                } else {
                    arg0->x108 = temp_s1;
                }
                if ((s16) arg0->x108 >= 0x1C1) {
                    arg0->x108 = 0x1C0;
                }
            } else {
                temp_v0_5 = arg0->unk10E;
                if (temp_v0_5 > 0) {
                    arg0->unk10E = 0;
                } else {
                    temp_v0_6 = temp_v0_5 - 4;
                    arg0->unk10E = temp_v0_6;
                    if (temp_v0_6 < -0x40) {
                        arg0->unk10E = -0x40;
                    }
                }
                temp_v0_7 = (s16) arg0->x108;
                temp_v1_3 = arg0->unk10E;
                if ((temp_s1 - temp_v0_7) < temp_v1_3) {
                    arg0->x108 = temp_v0_7 + temp_v1_3;
                } else {
                    arg0->x108 = temp_s1;
                }
                if ((s16) arg0->x108 < -0x1C0) {
                    arg0->x108 = -0x1C0U;
                }
            }
            func_80039630((s16) arg0->x108, 0x15E);
            return;
        }
        goto block_6;
    }
block_5:
block_6:
    func_80041EF4(arg0, 2, 0x40);
    func_80039630((s16) arg0->x108, 0x15E);
    arg0->unk10E = 0;
    if (!((s8) arg0->x171[3] & 0xC0)) {
        func_80042094(arg0, 4, 0x48, 1);
        return;
    }
    if (func_80041158(arg0) == 0) {
        func_80042094(arg0, 4, 0x48, 0);
        return;
    }
    arg0->xB4 = 0;
}
