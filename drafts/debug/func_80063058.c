/* func_80063058 — 183 asm insns
 * TU: src/rock_neo/debug.c
 * asm: asm/rock_neo/nonmatchings/debug/func_80063058.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_8001D7E4();                                  /* extern */
? func_8001F5E4(s16, void *, void *);               /* extern */
? func_8001FE38(s32, ? *);                          /* extern */
extern s32 D_8008DCD4;
extern ? D_8008DCD8;
extern s8 D_800987D4;
extern ? Player_work;

s32 func_80063058(void *arg0) {
    ? *var_a1;
    ? *var_s1;
    s16 temp_v1_2;
    s32 *var_v1;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1_4;
    s32 temp_v1_5;
    s32 var_a0;
    s32 var_s0;
    s32 var_v0;
    s8 temp_v0_2;
    s8 temp_v0_3;
    s8 temp_v0_4;
    s8 temp_v1;
    u16 temp_v1_3;
    void *temp_a1;

    temp_v1 = arg0->unk18;
    temp_a1 = arg0 + 0x18;
    if (temp_v1 != 1) {
        if (temp_v1 < 2) {
            var_a0 = 0;
            if (temp_v1 != 0) {
                return 0;
            }
            var_v1 = &D_8008DCD4;
            do {
                var_a0 += 1;
                *var_v1 &= 0xFFFBFFFF;
                var_v1 += 0x10;
            } while (var_a0 < 2);
            temp_a1->unk6 = (s16) (s8) Player_work.unk171;
            temp_v0 = (s32) ((s8) Player_work.unk171 << 0x10) >> 0xC;
            *(&D_8008DCD4 + temp_v0) = *(&D_8008DCD4 + temp_v0) | 0x40000;
            goto block_10;
        }
        switch (temp_v1) {                          /* switch 1; irregular */
        case 2:                                     /* switch 1 */
block_10:
            temp_v0_2 = arg0->unkD;
            if (temp_v0_2 >= 2) {
                temp_a1->unk4 = 1;
            } else {
                temp_a1->unk4 = temp_v0_2;
            }
            arg0->unk18 = 1;
            goto block_14;
        case 3:                                     /* switch 1 */
            temp_v1_2 = arg0->unk1E;
            if (temp_v1_2 != (s8) Player_work.unk171) {
                Player_work.unk173 = (s8) temp_v1_2;
                Player_work.unk171 = (u8) temp_v1_2;
                func_8001F5E4(temp_v1_2, temp_a1, arg0);
                func_8001D7E4();
            }
            return 0;
        default:                                    /* switch 1 */
            return 0;
        }
    } else {
block_14:
        var_v0 = 0x10;
        do {
            *(&D_8008DCD8 + var_v0) = 0x20;
            var_v0 -= 0x10;
        } while (var_v0 >= 0);
        if (arg0->unkC == 0) {
            temp_v1_3 = arg0->unk4;
            switch (temp_v1_3) {                    /* switch 2; irregular */
            case 0x10:                              /* switch 2 */
                temp_v0_3 = (u8) temp_a1->unk4 - 1;
                temp_a1->unk4 = temp_v0_3;
                if (temp_v0_3 & 0x80) {
                    temp_a1->unk4 = 1;
                }
                break;
            case 0x40:                              /* switch 2 */
                temp_v0_4 = (u8) temp_a1->unk4 + 1;
                temp_a1->unk4 = temp_v0_4;
                if (temp_v0_4 >= 2) {
                    temp_a1->unk4 = 0;
                }
                break;
            case 0x2000:                            /* switch 2 */
                temp_v1_4 = temp_a1->unk6 * 0x10;
                *(&D_8008DCD4 + temp_v1_4) = *(&D_8008DCD4 + temp_v1_4) & 0xFFFBFFFF;
                temp_v1_5 = temp_a1->unk4 * 0x10;
                *(&D_8008DCD4 + temp_v1_5) = *(&D_8008DCD4 + temp_v1_5) | 0x40000;
                temp_a1->unk6 = (s16) (s8) (u8) temp_a1->unk4;
                break;
            }
            arg0->unkD = (s8) (u8) temp_a1->unk4;
            *(&D_8008DCD8 + (temp_a1->unk4 * 0x10)) = 0x3E;
        }
        var_s1 = &D_8008DCD8;
        var_s0 = 0;
        var_a1 = &D_8008DCD8;
        do {
            var_s1 += 0x10;
            temp_a0 = *(&D_8008DCD4 + var_s0);
            D_800987D4 = 0;
            var_s0 += 0x10;
            func_8001FE38(temp_a0, var_a1);
            var_a1 = var_s1;
        } while (var_s0 < 0x20);
        return 0;
    }
}
