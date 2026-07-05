/* func_8001E068 — 105 asm insns
 * TU: src/rock_neo/scene.c
 * asm: asm/rock_neo/nonmatchings/scene/func_8001E068.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 func_8002FE74(u8);                              /* extern */
? func_80031824(u8 *);                              /* extern */

void func_8001E068(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    s32 var_v0_2;
    s32 var_v1;
    u32 var_v0;
    u8 *temp_s0;
    u8 temp_v1;

    var_s1 = 0;
    if (D_800988E8 != 0) {
        temp_v0 = D_800988E8;
        do {
            if (temp_v0 & (0x80000000U >> var_s1)) {
                temp_s0 = D_800ACD40[var_s1];
                temp_v0_2 = func_8002FE74(temp_s0->unk1);
                if (temp_v0_2 != 0x40) {
                    if (temp_v0_2 < 0x41) {
                        if (temp_v0_2 != 0x20) {
                            var_v0 = 0x80000000U >> var_s1;
                        } else {
                            temp_v1 = temp_s0->unk2;
                            if (temp_v1 != 0x10) {
                                temp_s0->unk8 = 2;
                                var_v1 = *((temp_v1 * 4) + *(s32 *)0x1F800058);
                                var_v0_2 = temp_s0->unk4 * 4;
                                goto block_16;
                            }
                            func_80031824(temp_s0);
                            var_v0 = 0x80000000U >> var_s1;
                        }
                    } else if (temp_v0_2 != 0x60) {
                        var_v0 = 0x80000000U >> var_s1;
                        if (temp_v0_2 != 0xE0) {

                        } else {
                            temp_s0->unk8 = 2;
                            var_v1 = *(s32 *)0x1F80006C;
                            var_v0_2 = temp_s0->unk2 * 4;
                            goto block_16;
                        }
                    } else {
                        temp_s0->unk8 = 2;
                        var_v1 = *(s32 *)0x1F800060;
                        var_v0_2 = temp_s0->unk2 * 4;
                        goto block_16;
                    }
                } else {
                    temp_s0->unk8 = 2;
                    var_v1 = *((temp_s0->unk2 * 4) + *(s32 *)0x1F80005C);
                    var_v0_2 = temp_s0->unk4 * 4;
block_16:
                    *(var_v0_2 + var_v1)(temp_s0);
                    var_v0 = 0x80000000U >> var_s1;
                }
                D_800988E8 &= ~var_v0;
            }
            var_s1 += 1;
        } while (D_800988E8 != 0);
    }
}
