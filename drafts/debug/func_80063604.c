/* func_80063604 — 181 asm insns
 * TU: src/rock_neo/debug.c
 * asm: asm/rock_neo/nonmatchings/debug/func_80063604.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_8001FE38(s32, ? *);                          /* extern */
? func_800425FC(? *, u16);                          /* extern */
extern s32 D_8008DD14;
extern ? D_8008DD18;
extern s8 D_800987D4;
extern ? Game_work;
extern ? Player_work;

s32 func_80063604(void *arg0) {
    ? *var_a1;
    ? *var_s1;
    s32 *var_v1;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a0;
    s32 var_s0;
    s32 var_v0;
    s8 temp_v0_2;
    s8 temp_v0_3;
    s8 temp_v0_4;
    s8 temp_v1;
    u16 temp_a1_2;
    u16 temp_v1_2;
    void *temp_a1;

    temp_v1 = arg0->unk28;
    temp_a1 = arg0 + 0x28;
    if (temp_v1 != 1) {
        if (temp_v1 < 2) {
            var_a0 = 0;
            if (temp_v1 != 0) {
                return 0;
            }
            var_v1 = &D_8008DD14;
            do {
                var_a0 += 1;
                *var_v1 &= 0xFFFBFFFF;
                var_v1 += 0x10;
            } while (var_a0 < 2);
            temp_a1->unk6 = (u16) (s8) Game_work.unk54;
            temp_v0 = ((s8) Game_work.unk54 & 0xFFFF) * 0x10;
            *(&D_8008DD14 + temp_v0) = *(&D_8008DD14 + temp_v0) | 0x40000;
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
            arg0->unk28 = 1;
            goto block_14;
        case 3:                                     /* switch 1 */
            temp_a1_2 = arg0->unk2E;
            if (temp_a1_2 != (s8) Game_work.unk54) {
                func_800425FC(&Player_work, temp_a1_2);
                Game_work.unk54 = (u8) arg0->unk2E;
            }
            return 0;
        default:                                    /* switch 1 */
            return 0;
        }
    } else {
block_14:
        var_v0 = 0x10;
        do {
            *(&D_8008DD18 + var_v0) = 0x20;
            var_v0 -= 0x10;
        } while (var_v0 >= 0);
        if (arg0->unkC == 3) {
            temp_v1_2 = arg0->unk4;
            switch (temp_v1_2) {                    /* switch 2; irregular */
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
                temp_v1_3 = temp_a1->unk6 * 0x10;
                *(&D_8008DD14 + temp_v1_3) = *(&D_8008DD14 + temp_v1_3) & 0xFFFBFFFF;
                temp_v1_4 = temp_a1->unk4 * 0x10;
                *(&D_8008DD14 + temp_v1_4) = *(&D_8008DD14 + temp_v1_4) | 0x40000;
                temp_a1->unk6 = (u16) (s8) (u8) temp_a1->unk4;
                break;
            }
            arg0->unkD = (s8) (u8) temp_a1->unk4;
            *(&D_8008DD18 + (temp_a1->unk4 * 0x10)) = 0x3E;
        }
        var_s1 = &D_8008DD18;
        var_s0 = 0;
        var_a1 = &D_8008DD18;
        do {
            var_s1 += 0x10;
            temp_a0 = *(&D_8008DD14 + var_s0);
            D_800987D4 = 0;
            var_s0 += 0x10;
            func_8001FE38(temp_a0, var_a1);
            var_a1 = var_s1;
        } while (var_s0 < 0x20);
        return 0;
    }
}
