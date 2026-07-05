/* Map_screen_task — 127 asm insns
 * TU: src/rock_neo/sub_scrn.c
 * asm: asm/rock_neo/nonmatchings/sub_scrn/Map_screen_task.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? map_cursor_set();                                 /* extern */
? map_screen_set();                                 /* extern */
extern ? D_8008DB28;
extern ? D_8008DB2C;
extern u8 D_800987F5;

s32 Map_screen_task(void) {
    s16 var_a0;
    s32 temp_a0;
    u16 temp_s0;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 var_v1;

    temp_s0 = D_800C0C2A;
    if (Map_screen_work.unk0 != 1) {
        if (Map_screen_work.unk0 < 2) {
            if (Map_screen_work.unk0 != 0) {
                return 0;
            }
            map_cursor_set();
            map_screen_set();
            if (temp_s0 & 0x1009) {
                Sound_call(0x83, 1, 0);
                return 1;
            }
            temp_a0 = temp_s0 & 0x480;
            if (!(temp_s0 & 0x4828) != (temp_a0 == 0)) {
                if (temp_a0 != 0) {
                    temp_v0 = D_800987F5 - 1;
                    D_800987F5 = temp_v0;
                    if (temp_v0 & 0x80) {
                        var_v1 = 1;
                        if ((s8) *D_800987F4 > 0) {
                            var_v1 = 2;
                        }
                        D_800987F5 = var_v1;
                    }
                } else {
                    temp_v0_2 = D_800987F5 + 1;
                    D_800987F5 = temp_v0_2;
                    if ((s8) *D_800987F4 > 0) {
                        if ((s8) temp_v0_2 < 3) {

                        } else {
                            goto block_18;
                        }
                    } else if ((s8) temp_v0_2 >= 2) {
block_18:
                        Map_screen_work.unk5 = 0;
                    }
                }
                Sound_call(0x82, 1, 0);
                goto block_25;
            }
            goto block_28;
        }
        if (Map_screen_work.unk0 != 2) {
            return 0;
        }
        if (Cd_read_sync2() == 0) {
            Map_screen_work.unk0 = 0;
        }
        /* Duplicate return node #29. Try simplifying control flow for better match */
        return 0;
    }
    if ((s8) *D_800987F4 == 0) {
        var_a0 = *(&D_8008DB28 + ((s8) D_800987F5 * 2));
    } else {
        var_a0 = *(&D_8008DB2C + ((s8) D_800987F5 * 2));
    }
    Cd_read_comb((enum _CD_COMB) var_a0);
block_25:
    Map_screen_work.unk0 = (s8) ((u8) Map_screen_work.unk0 + 1);
block_28:
    return 0;
}
