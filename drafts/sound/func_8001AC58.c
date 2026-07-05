/* func_8001AC58 — 133 asm insns
 * TU: src/rock_neo/sound.c
 * asm: asm/rock_neo/nonmatchings/sound/func_8001AC58.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? SsSepClose(s16);                                  /* extern */
s16 SsSepOpen(s32, s16, u8);                        /* extern */
? SsSepPlay(s16, u8, ?, ?);                         /* extern */
? SsSeqClose(s16);                                  /* extern */
? func_80070114(s16);                               /* extern */
? func_8007013C(s16, u8);                           /* extern */
extern ? D_8008241C;
extern ? D_80098AD1;
extern ? D_80098AD4;
extern ? D_800AD140;
extern s16 D_800AD152;

void func_8001AC58(void *arg0) {
    ? var_a3;
    s16 temp_a0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_v1;
    s8 temp_v0_3;
    u8 temp_a1;
    void *temp_s0;
    void *temp_s1;

    temp_s1 = (arg0->unk2 * 4) + &D_8008241C;
    temp_v0 = func_8001A1FC(temp_s1->unk0 & 0x3F);
    if ((temp_v0 != -1) && (temp_v0_2 = func_8001A238(temp_s1->unk1 & 0x7F), (temp_v0_2 != -1))) {
        temp_s0 = (((u8) temp_s1->unk2 >> 4) * 8) + D_80098AF8;
        if ((arg0->unk4 == 0) && (temp_s0->unk5 != 0) && (temp_s0->unk6 == arg0->unk2)) {
            D_800AD152 = 0x7F;
            return;
        }
        if ((temp_s1->unk2 & 0xF) >= temp_s0->unk3) {
            temp_a0 = temp_s0->unk0;
            if ((temp_a0 != -1) && (temp_v0_3 = temp_s0->unk2, (temp_v0_3 != -1))) {
                if (temp_v0_3 == 0) {
                    func_80070114(temp_a0);
                    SsSeqClose(temp_s0->unk0);
                    var_v1 = temp_v0_2 * 8;
                } else {
                    func_8007013C(temp_a0, temp_s0->unk4);
                    SsSepClose(temp_s0->unk0);
                    goto block_12;
                }
            } else {
block_12:
                var_v1 = temp_v0_2 * 8;
            }
            temp_s0->unk0 = SsSepOpen(*(&D_80098AD4 + var_v1), *(Game_work + 0x1B8 + (temp_v0 * 8)), *(&D_80098AD1 + var_v1));
            temp_s0->unk2 = 1;
            temp_s0->unk3 = (s8) (temp_s1->unk2 & 0xF);
            if (temp_s0->unk0 != -1) {
                temp_a1 = temp_s1->unk3 & 0x1F;
                temp_s0->unk4 = temp_a1;
                if (temp_s1->unk1 & 0x80) {
                    var_a3 = 0;
                } else {
                    var_a3 = 1;
                }
                SsSepPlay(temp_s0->unk0, temp_a1, 1, var_a3);
                D_800AD140.unk12 = 0x7F;
                temp_s0->unk6 = (s16) (u16) arg0->unk2;
            }
        }
    }
}
