/* Sub_screen_rb_parts_calc — 113 asm insns
 * TU: src/rock_neo/sub_scrn.c
 * asm: asm/rock_neo/nonmatchings/sub_scrn/Sub_screen_rb_parts_calc.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? Sub_screen_rb_parts_calc_sub00(void *, s32);      /* extern */
? Sub_screen_rb_parts_calc_sub01(void *, s32);      /* extern */
extern u8 D_80098B2D;
extern u8 D_80098B31;

void Sub_screen_rb_parts_calc(void *arg0) {
    PL_WORK *var_v0;
    PL_WORK *var_v0_2;
    PL_WORK *var_v0_3;
    s32 temp_s0_2;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_3;
    s32 var_s3;
    u8 temp_s0;
    u8 temp_s0_3;
    u8 var_s0;

    var_s3 = 2;
    if (Sce_flag_test(0x526) != 0) {
        var_s3 = 3;
    }
    arg0->unk14 = 0;
    arg0->unk11 = 0;
    arg0->unk1C = 0;
    arg0->unk19 = 0;
    arg0->unk24 = 0;
    arg0->unk21 = 0;
    arg0->unk2C = 0;
    arg0->unk29 = 0;
    if (*Moji_flag & 0x08000000) {
        var_s1 = 0;
        if (var_s3 != 0) {
            var_v0 = &Player_work;
            do {
                temp_s0 = var_v0->rb_parts_equip_data[0];
                if (temp_s0 != 0) {
                    temp_s0_2 = temp_s0 - 1;
                    Sub_screen_rb_parts_calc_sub00(arg0, temp_s0_2);
                    Sub_screen_rb_parts_calc_sub01(arg0, temp_s0_2);
                }
                var_s1 += 1;
                var_v0 = (PL_WORK *) &Player_work.x0[var_s1];
            } while (var_s1 < var_s3);
        }
    } else if (Moji_flag3 & 0x10000) {
        var_s1_2 = 0;
        if (var_s3 != 0) {
            var_v0_2 = &Player_work;
            do {
                temp_s0_3 = var_v0_2->rb_parts_equip_data[0];
                if (temp_s0_3 != 0) {
                    Sub_screen_rb_parts_calc_sub00(arg0, temp_s0_3 - 1);
                }
                if ((var_s1_2 != D_80098B2D) && (temp_s0_3 != 0)) {
                    Sub_screen_rb_parts_calc_sub01(arg0, temp_s0_3 - 1);
                }
                var_s1_2 += 1;
                var_v0_2 = (PL_WORK *) &Player_work.x0[var_s1_2];
            } while (var_s1_2 < var_s3);
        }
    } else {
        var_s1_3 = 0;
        if (var_s3 != 0) {
            var_v0_3 = &Player_work;
            do {
                var_s0 = var_v0_3->rb_parts_equip_data[0];
                if (var_s0 != 0) {
                    Sub_screen_rb_parts_calc_sub00(arg0, var_s0 - 1);
                }
                if (var_s1_3 == D_80098B2D) {
                    var_s0 = D_80098B31;
                }
                if (var_s0 != 0) {
                    Sub_screen_rb_parts_calc_sub01(arg0, var_s0 - 1);
                }
                var_s1_3 += 1;
                var_v0_3 = (PL_WORK *) &Player_work.x0[var_s1_3];
            } while (var_s1_3 < var_s3);
        }
    }
}
