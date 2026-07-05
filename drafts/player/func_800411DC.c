/* func_800411DC — 197 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_800411DC.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 func_8003A13C();                                /* extern */
u16 func_800414F0(void *, u16 *, s16 *, s32);       /* extern */
s16 rcos(s16);                                      /* extern */
s16 rsin(s16);                                      /* extern */
extern u8 D_8009CF88;
extern u8 D_8009F108;
extern u8 D_800BF118;
extern u8 D_800C3580;

s32 func_800411DC(void *arg0) {
    u16 sp10;
    u16 sp12;
    u16 sp14;
    s16 sp18;
    s16 sp1A;
    s16 sp1C;
    s32 temp_a3;
    s32 temp_a3_2;
    s32 temp_a3_3;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_3;
    s32 var_s1_4;
    u16 temp_s0;
    u16 temp_v0;
    u16 temp_v0_2;
    u16 temp_v0_3;
    u16 temp_v0_4;
    u16 var_s2;
    u8 *var_s0;
    u8 *var_s0_2;
    u8 *var_s0_3;
    u8 *var_s0_4;
    u8 *var_s3;
    u8 *var_s5;
    u8 temp_a0;

    var_s5 = NULL;
    if ((func_8003A13C() == 0) && !(arg0->unk174 & 0xC0) && (arg0->unk177 == 0)) {
        temp_s0 = arg0->unk11C;
        sp1C = 0;
        sp1A = 0;
        sp18 = 0;
        var_s2 = 0x7FFF;
        *(s16 *)0x1F800100 = rsin(arg0->unk56);
        *(s16 *)0x1F800102 = rcos(arg0->unk56);
        if (arg0->unk136 & temp_s0) {
            var_s1 = 0;
            if (arg0->unk74 == 0) {
                var_s0 = &D_8009F108;
                do {
                    if ((var_s0->unk0 & 3) == 3) {
                        temp_a3 = var_s0->unk10;
                        if (temp_a3 != 0) {
                            temp_v0 = func_800414F0(arg0, (u16 *) (var_s0 + 0x14), (s16 *) (var_s0 + 0x54), temp_a3);
                            if ((temp_v0 != 0) && (temp_v0 < var_s2)) {
                                var_s2 = temp_v0;
                                var_s5 = var_s0;
                            }
                        }
                    }
                    var_s1 += 1;
                    var_s0 += 0x3C0;
                } while (var_s1 < 0x10);
                var_s0_2 = &D_800C3580;
                var_s1_2 = 0;
                do {
                    if ((var_s0_2->unk0 & 3) == 3) {
                        temp_a3_2 = var_s0_2->unk10;
                        if (temp_a3_2 != 0) {
                            temp_v0_2 = func_800414F0(arg0, (u16 *) (var_s0_2 + 0x14), (s16 *) (var_s0_2 + 0x54), temp_a3_2);
                            if ((temp_v0_2 != 0) && (temp_v0_2 < var_s2)) {
                                var_s2 = temp_v0_2;
                                var_s5 = var_s0_2;
                            }
                        }
                    }
                    var_s1_2 += 1;
                    var_s0_2 += 0x5A4;
                } while (var_s1_2 < 4);
                var_s0_3 = &D_8009CF88;
                var_s1_3 = 0;
                do {
                    if (((var_s0_3->unk0 & 3) == 3) && ((u8) var_s0_3->unk2 < 0x80U)) {
                        temp_a3_3 = var_s0_3->unk10;
                        if (temp_a3_3 != 0) {
                            temp_v0_3 = func_800414F0(arg0, (u16 *) (var_s0_3 + 0x14), (s16 *) (var_s0_3 + 0x54), temp_a3_3);
                            if ((temp_v0_3 != 0) && (temp_v0_3 < var_s2)) {
                                var_s2 = temp_v0_3;
                                var_s5 = var_s0_3;
                            }
                        }
                    }
                    var_s1_3 += 1;
                    var_s0_3 += 0x10C;
                } while (var_s1_3 < 0x20);
                var_s3 = &D_800BF118;
                var_s1_4 = 0;
                var_s0_4 = &D_800BF118;
                do {
                    temp_a0 = *var_s3;
                    if (((temp_a0 & 3) == 3) && (var_s0_4->unk10 != 0)) {
                        if (temp_a0 & 4) {
                            sp10 = var_s0_4->unk38;
                            sp12 = var_s0_4->unk3A;
                            sp14 = var_s0_4->unk3C;
                        } else {
                            sp10 = var_s0_4->unk1C;
                            sp12 = var_s0_4->unk1E;
                            sp14 = var_s0_4->unk20;
                        }
                        temp_v0_4 = func_800414F0(arg0, &sp10, &sp18, var_s0_4->unk10);
                        if ((temp_v0_4 != 0) && (temp_v0_4 < var_s2)) {
                            var_s2 = temp_v0_4;
                            var_s5 = var_s0_4;
                        }
                    }
                    var_s1_4 += 1;
                    var_s0_4 += 0x50;
                    var_s3 += 0x50;
                } while (var_s1_4 < 0x20);
                if (var_s2 < 0xD0U) {
                    arg0->unk182 = var_s2;
                    arg0->unk184 = var_s5;
                    return 1;
                }
                goto block_39;
            }
        }
block_39:
        arg0->unk184 = NULL;
        /* Duplicate return node #40. Try simplifying control flow for better match */
        return 0;
    }
    return 0;
}
