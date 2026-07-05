/* func_8004174C — 190 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8004174C.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 func_80031198(void *, void *, s32);             /* extern */
s32 func_8005D8C4(s32);                             /* extern */
s32 func_8005D938();                                /* extern */
? func_8005D98C();                                  /* extern */
s32 ratan2(s16, s16, s32);                          /* extern */
extern u8 Scene_work;

void func_8004174C(PL_WORK *arg0, s32 arg2) {
    s16 temp_v0_3;
    s16 temp_v0_4;
    s16 var_v1;
    s32 temp_s2;
    s32 temp_v1;
    s32 var_a0;
    s32 var_a2;
    s32 var_s1;
    s8 temp_v0_2;
    u8 temp_v0;
    u8 temp_v1_2;
    u8 var_v0;
    void *temp_a1;

    var_a2 = arg2;
    if (Scene_work == 0) {
        temp_s2 = arg0->unk188;
        var_s1 = temp_s2 & 0xFFFFFF;
        temp_a1 = arg0 + 0x190;
        if (var_s1 == 0) {
            if (temp_s2 & 0x01000000) {
                if (var_s1 != 0) {
                    goto block_4;
                }
                goto block_6;
            }
            if ((u32) (arg0->x9 - 0xB) >= 2U) {
                temp_v0 = arg0->x70[3] - 4;
                arg0->x70[3] = temp_v0;
                if (temp_v0 & 0x80) {
                    arg0->x70[3] = 0;
                }
            }
        } else {
block_4:
            var_a2 = var_s1 * arg0->x142[0x2A];
            var_s1 -= var_a2 >> 2;
            if (var_s1 == 0) {
                var_s1 = 1;
            }
block_6:
            temp_v0_2 = arg0->x70[1];
            if (temp_v0_2 <= 0) {
                if (temp_v0_2 == 0) {
                    if (temp_a1->unk6 != 0) {
                        var_a0 = func_80031198(temp_a1, arg0 + 0x14, var_a2);
                    } else {
                        var_a0 = ratan2(arg0->unk190, temp_a1->unk4, var_a2) + 0x800;
                    }
                    arg0->x70[1] = ((u32) (((var_a0 - arg0->x56) + arg0->x116 + 0x200) & 0xFFF) >> 0xA) + 1;
                } else if ((temp_v0_2 & 0x7F) == 1) {
                    arg0->x116 = (u16) arg0->x116 + 0x800;
                }
                temp_v1 = (u8) arg0->x70[1] & 0x7F;
                switch (temp_v1) {                  /* irregular */
                case 2:
                    var_v0 = (arg0->x112 + 1) & 3;
                    var_v1 = (u16) arg0->x116 + 0x400;
block_19:
                    arg0->x112 = var_v0;
                    arg0->x116 = var_v1;
                    break;
                case 4:
                    var_v0 = (arg0->x112 - 1) & 3;
                    var_v1 = (u16) arg0->x116 - 0x400;
                    goto block_19;
                }
            }
            arg0->x171[3] = 0;
            arg0->x171[6] = 0;
            if (temp_s2 & 0x01000000) {
                func_8005D8C4(var_s1);
            }
            if (temp_s2 & 0x40000000) {
                func_8005D98C();
            }
            if ((func_8005D8C4(0) != 0) || (func_8005D938() != 0)) {
                temp_v0_3 = arg0->life;
                if (temp_v0_3 > 0) {
                    temp_v0_4 = temp_v0_3 - var_s1;
                    arg0->life = temp_v0_4;
                    if ((temp_v0_4 << 0x10) <= 0) {
                        arg0->life = 0;
                        func_8005D98C();
                    }
                } else {
                    arg0->life = temp_v0_3 - var_s1;
                }
                temp_v1_2 = arg0->x70[3] + var_s1;
                arg0->x70[3] = temp_v1_2;
                if (((arg0->x142[0x28] + 1) * 0x10) < (s8) temp_v1_2) {
                    arg0->x9 = 0xC;
                    arg0->unkA = 1;
                } else {
                    goto block_36;
                }
            } else {
                if (arg0->life > 0) {
                    func_8005D8C4(var_s1);
                } else {
                    func_8005D98C();
block_36:
                }
                func_80041A44(arg0, temp_s2);
            }
            arg0->x56 = (u16) arg0->x56 - 0x80 + (rand() & 0x100);
            if (((u8) arg0->x70[1] & 0x7F) == 1) {
                arg0->unkB = 1;
                return;
            }
            arg0->unkB = 0;
        }
    }
}
