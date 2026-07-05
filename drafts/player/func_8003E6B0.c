/* func_8003E6B0 — 108 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003E6B0.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_800395E8();                                  /* extern */

void func_8003E6B0(PL_WORK *arg0) {
    s16 temp_a1;
    s16 var_v0;
    s16 var_v0_2;
    s32 temp_v1;
    u8 temp_a0;

    temp_a0 = arg0->unkA;
    temp_v1 = temp_a0 & 0xFF;
    switch (temp_v1) {                              /* irregular */
    case 0:
        if (arg0->unkB == 0) {
            func_80041DDC(arg0, 0x20, 0, 1);
            var_v0 = -0x200;
        } else {
            func_80041DDC(arg0, 0x21, 0, 1);
            var_v0 = 0x200;
        }
        arg0->unk44 = var_v0;
        arg0->unk46 = 0;
        arg0->unk48 = 0;
        arg0->x142[0x26] = 1;
        arg0->x142[0x27] = 1;
        arg0->x44A[0] = 0;
        Pl00_shot_enable_off(arg0);
        func_80041E90(arg0, arg0->unk44, arg0->unk46, arg0->unk48);
        func_8001B2F0(0x16);
        Sound_call(0x97, 0, 0);
        func_800395E8();
        arg0->x108 = 0;
block_19:
        arg0->unkA = (u8) (arg0->unkA + 1);
        return;
    case 1:
        if ((s8) arg0->x76[0x30] == temp_v1) {
            arg0->unkA = (u8) (temp_a0 + 1);
        }
        /* fallthrough */
    case 2:
        if (arg0->unkB == 0) {
            var_v0_2 = (u16) arg0->unk44 + 0x40;
        } else {
            var_v0_2 = (u16) arg0->unk44 - 0x40;
        }
        arg0->unk44 = var_v0_2;
        temp_a1 = arg0->unk44;
        if (temp_a1 != 0) {
            func_80041E90(arg0, temp_a1, arg0->unk46, arg0->unk48);
            return;
        }
        arg0->unk44 = 0;
        goto block_19;
    case 3:
        if ((s8) arg0->x76[0x31] == -1) {
            arg0->x9 = 8;
            arg0->unkA = 6U;
            arg0->x70[1] = 0;
            arg0->x142[0x26] = 0;
            arg0->x142[0x27] = 0;
        }
        return;
    }
}
