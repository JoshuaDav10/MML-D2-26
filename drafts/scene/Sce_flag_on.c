/* Sce_flag_on — 145 asm insns
 * TU: src/rock_neo/scene.c
 * asm: asm/rock_neo/nonmatchings/scene/Sce_flag_on.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

extern ? Player_work;

s32 Sce_flag_on(s32 arg0) {
    s32 temp_s0;
    s32 temp_s1;
    s32 var_v1;
    u32 var_v0;
    u32 var_v1_2;
    u8 *temp_a0;

    temp_a0 = &Sce_flag[(u32) arg0 >> 3];
    *temp_a0 |= 0x80 >> (arg0 & 7);
    if ((u32) (arg0 - 0x500) < 0x20U) {
        var_v1 = 0;
loop_2:
        if (*(&Player_work + 0x454 + var_v1) != 0) {
            var_v1 += 1;
            if (var_v1 < 0x20) {
                goto loop_2;
            }
        } else {
            *(&Player_work + 0x454 + var_v1) = arg0 + 1;
        }
    }
    if (Player_work.unk16C != 0) {
        var_v1_2 = arg0 - 0x500;
        if ((u32) (arg0 - 0x53A) < 3U) {
            temp_s0 = Sce_flag_test(0x53A);
            temp_s1 = Sce_flag_test(0x53B);
            Player_work.unk16C = (s8) (temp_s0 + temp_s1 + Sce_flag_test(0x53C));
            goto block_7;
        }
    } else {
block_7:
        var_v1_2 = arg0 - 0x500;
    }
    if (var_v1_2 < 0x80U) {
        if (var_v1_2 < 0x20U) {
            goto block_24;
        }
        if ((u32) (arg0 - 0x520) < 0x20U) {
            if (arg0 == 0x528) {
                Sce_flag_on(0x582);
                Sce_flag_on(0x583);
                Sce_flag_on(0x584);
                Sce_flag_on(0x585);
                Sce_flag_on(0x586);
                Sce_flag_on(0x587);
                Sce_flag_on(0x5C1);
                Sce_flag_on(0x5C2);
            }
            if (arg0 == 0x53A) {
                Sce_flag_off(0x5BA);
                Sce_flag_on(0x5BB);
            }
            var_v0 = 0x53C;
            if (arg0 == 0x53B) {
                Sce_flag_off(0x5BB);
                Sce_flag_on(0x5BC);
                var_v0 = 0x53C;
            }
            if (arg0 == 0x53C) {
                return Sce_flag_off(0x5BC);
            }
            /* Duplicate return node #30. Try simplifying control flow for better match */
            return (s32) var_v0;
        }
        if ((u32) (arg0 - 0x540) >= 4U) {
            var_v0 = arg0 - 0x558;
            if ((u32) (arg0 - 0x544) >= 0x14U) {
                if (var_v0 < 0x28U) {
                    goto block_24;
                }
                goto block_26;
            }
        } else {
block_24:
            Sce_flag_on(arg0 + 0x80);
            goto block_25;
        }
        /* Duplicate return node #30. Try simplifying control flow for better match */
        return (s32) var_v0;
    }
block_25:
block_26:
    var_v0 = 0x51;
    if (arg0 == 0x2A) {
        Sce_flag_on(0x580);
        Sce_flag_on(0x581);
        Sce_flag_on(0x5C3);
        var_v0 = 0x51;
    }
    if (arg0 == 0x51) {
        Sce_flag_on(0x5BA);
        Sce_flag_on(0x588);
        var_v0 = Sce_flag_on(0x589);
    }
    return (s32) var_v0;
}
