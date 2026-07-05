/* func_8005F3D0 — 142 asm insns
 * TU: src/rock_neo/sub_scrn.c
 * asm: asm/rock_neo/nonmatchings/sub_scrn/func_8005F3D0.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

void func_8005F3D0(void *arg0) {
    ? var_a2;
    s8 temp_v1;
    u8 temp_v1_2;

    temp_v1 = arg0->unk2;
    switch (temp_v1) {                              /* irregular */
    case 0:
        if (Sce_flag_test(0x524) == 0) {
            Sound_call(0x83, 1, 0);
            goto block_13;
        }
        if ((s16) Player_work.life == Player_work.unkB0) {
            Sound_call(0x82, 1, 0);
            var_a2 = 0x40;
            goto block_12;
        }
        if (Game_work.x7C == 0) {
            Sound_call(0x82, 1, 0);
            var_a2 = 0x41;
block_12:
            MojiTaskExec(0, 0x801F2000, var_a2);
block_13:
            arg0->unk2 = (s8) ((u8) arg0->unk2 + 2);
            return;
        }
        MojiTaskExec(0, 0x801F2000, 0x42);
block_20:
        arg0->unk2 = (s8) ((u8) arg0->unk2 + 1);
        return;
    case 1:
        if (*Moji_flag & 0x40000000) {
            if (!(*Moji_flag & 0xFF)) {
                if ((s32) Game_work.x7C < ((s32) (Player_work.unkB0 - (Player_work.life & 0xF0)) >> 4)) {
                    MojiTaskExec(0, 0x801F2000, 0x44);
                    temp_v1_2 = Game_work.x7C;
                    Game_work.x7C = 0;
                    Player_work.life = (Player_work.life & 0xF0) + (temp_v1_2 * 0x10);
                } else {
                    MojiTaskExec(0, 0x801F2000, 0x43);
                    Game_work.x7C -= (s32) (Player_work.unkB0 - (Player_work.life & 0xF0)) >> 4;
                    Player_work.life = (u16) Player_work.unkB0;
                }
            }
            goto block_20;
        }
        return;
    case 2:
        if (!(*Moji_flag & 0x08000000)) {
            arg0->unk1 = 2;
            arg0->unk2 = 0;
        }
        break;
    }
}
