/* func_8003E068 — 111 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003E068.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_80042044(PL_WORK *, ?, ?, u16);              /* extern */

void func_8003E068(PL_WORK *arg0, s32 arg1) {
    s32 temp_v0_2;
    s32 temp_v0_6;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a1;
    s32 var_v0;
    u16 temp_a0;
    u16 temp_a0_2;
    u16 temp_a3;
    u16 temp_v0_3;
    u16 temp_v0_4;
    u16 temp_v0_7;
    u16 temp_v0_8;
    u16 var_v0_2;
    void *temp_v0;
    void *temp_v0_5;

    temp_v0 = ((s32) (arg1 << 0x10) >> 0xD) + arg0;
    temp_a0 = temp_v0->unk144;
    temp_a3 = arg0->x11C;
    temp_v0_2 = temp_a3 & 0xFFFF;
    temp_v1 = temp_v0_2 & temp_a0;
    if ((temp_v1 != 0) && (temp_v1 != temp_a0)) {
        if (temp_v0_2 & temp_v0->unk146) {
            temp_v0_3 = arg0->unk44 + 0x28;
            arg0->unk44 = temp_v0_3;
            var_v0 = arg1 << 0x10;
            if ((s16) temp_v0_3 >= 0x281) {
                var_v0_2 = 0x280;
                goto block_6;
            }
        } else {
            temp_v0_4 = arg0->unk44 - 0x28;
            arg0->unk44 = temp_v0_4;
            var_v0_2 = -0x280U;
            if ((s16) temp_v0_4 < -0x280) {
block_6:
                arg0->unk44 = var_v0_2;
            }
            goto block_7;
        }
    } else {
block_7:
        var_v0 = arg1 << 0x10;
    }
    temp_v0_5 = arg0 + (var_v0 >> 0xD);
    temp_a0_2 = temp_v0_5->unk148;
    temp_v0_6 = temp_a3 & 0xFFFF;
    temp_v1_2 = temp_v0_6 & temp_a0_2;
    if (temp_v1_2 != 0) {
        if (temp_v1_2 != temp_a0_2) {
            if (temp_v0_6 & temp_v0_5->unk14A) {
                temp_v0_7 = arg0->unk48 - 0x20;
                arg0->unk48 = temp_v0_7;
                if ((s16) temp_v0_7 < -0x100) {
                    arg0->unk48 = -0x100U;
                }
            } else {
                temp_v0_8 = arg0->unk48 + 0x20;
                arg0->unk48 = temp_v0_8;
                if ((s16) temp_v0_8 >= 0x101) {
                    arg0->unk48 = 0x100U;
                }
            }
        } else {
            arg0->unk48 = 0U;
        }
    }
    func_80042044(arg0, 4, 0x20, temp_a3);
    func_80041E90(arg0, (s16) arg0->unk44, arg0->unk46, (s16) arg0->unk48);
    if ((s8) arg0->x74 == 0) {
        if (arg0->x9 == 7) {
            var_a1 = 0x13;
        } else {
            var_a1 = 0x16;
        }
        func_80041DDC(arg0, var_a1, 0, 1);
        Sound_call(0x93, 0, 0);
        arg0->x9 = 8;
        arg0->unkA = 6;
    }
}
