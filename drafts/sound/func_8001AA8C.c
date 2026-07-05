/* func_8001AA8C — 115 asm insns
 * TU: src/rock_neo/sound.c
 * asm: asm/rock_neo/nonmatchings/sound/func_8001AA8C.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? SsSepClose(s16);                                  /* extern */
? SsSeqClose(s16);                                  /* extern */
s16 SsSeqOpen(s32, s16);                            /* extern */
? SsSeqPlay(s16, ?, ?);                             /* extern */
? func_80070114(s16);                               /* extern */
? func_8007013C(s16, u8);                           /* extern */
extern ? D_8008241C;
extern ? D_80098AD4;
extern ? D_800AD140;

void func_8001AA8C(void *arg0) {
    ? var_a2;
    s16 temp_a0;
    s16 temp_a0_2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_v0;
    s8 temp_v0_3;
    void *temp_s0;
    void *temp_s1;

    temp_s1 = (arg0->unk2 * 4) + &D_8008241C;
    temp_v0 = func_8001A1FC(temp_s1->unk0 & 0x3F);
    if (temp_v0 != -1) {
        temp_v0_2 = func_8001A238(temp_s1->unk1 & 0x7F);
        if ((temp_v0_2 != -1) && ((temp_s0 = (((u8) temp_s1->unk2 >> 4) * 8) + D_80098AF8, (arg0->unk4 != 0)) || (temp_s0->unk5 == 0) || (temp_s0->unk6 != arg0->unk2)) && ((temp_s1->unk2 & 0xF) >= temp_s0->unk3)) {
            temp_a0 = temp_s0->unk0;
            var_v0 = temp_v0_2 * 8;
            if (temp_a0 != -1) {
                temp_v0_3 = temp_s0->unk2;
                if (temp_v0_3 != -1) {
                    if (temp_v0_3 == 0) {
                        func_80070114(temp_a0);
                        SsSeqClose(temp_s0->unk0);
                        var_v0 = temp_v0_2 * 8;
                    } else {
                        func_8007013C(temp_a0, temp_s0->unk4);
                        SsSepClose(temp_s0->unk0);
                        goto block_11;
                    }
                } else {
block_11:
                    var_v0 = temp_v0_2 * 8;
                }
            }
            temp_s0->unk0 = SsSeqOpen(*(&D_80098AD4 + var_v0), *(Game_work + 0x1B8 + (temp_v0 * 8)));
            temp_a0_2 = temp_s0->unk0;
            temp_s0->unk2 = 0;
            temp_s0->unk3 = (s8) (temp_s1->unk2 & 0xF);
            if (temp_a0_2 != -1) {
                if (temp_s1->unk1 != 0) {
                    var_a2 = 0;
                } else {
                    var_a2 = 1;
                }
                SsSeqPlay(temp_a0_2, 1, var_a2);
                D_800AD140.unk12 = 0x7F;
                temp_s0->unk6 = (s16) (u16) arg0->unk2;
            }
        }
    }
}
