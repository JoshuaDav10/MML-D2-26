/* func_80041B8C — 148 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_80041B8C.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_80051BAC(PL_WORK *);                         /* extern */

void func_80041B8C(PL_WORK *arg0) {
    u8 temp_a2;
    u8 temp_v1;
    u8 var_a1;

    temp_v1 = arg0->x9;
    switch (temp_v1) {                              /* irregular */
    case 0:
        arg0->x171[0x2C7] = (unaligned s32) arg0->xC[8];
        arg0->x171[0x2CB] = (unaligned s32) arg0->xC[0xC];
        arg0->unk428 = (s32) arg0->unk1C;
        arg0->unk42C = (s32) arg0->unk20;
        arg0->unk430 = (s32) arg0->unk24;
        arg0->unk434 = (s32) arg0->unk28;
        arg0->x171[0x2CF] = (unaligned s32) arg0->xC[0x48];
        arg0->x171[0x2D3] = (unaligned s32) arg0->x58[0];
        arg0->x171[0x2D7] = 0xFF;
        /* fallthrough */
    case 1:
        Pl00_shot_enable_off(arg0);
        arg0->xBD = 0x40;
        arg0->x9 = 2;
        /* fallthrough */
    case 2:
        arg0->xC[0x20] = (unaligned s32) arg0->xC[8];
        arg0->xC[0x24] = (unaligned s32) arg0->xC[0xC];
        arg0->unk34 = (s32) arg0->unk1C;
        arg0->unk38 = (s32) arg0->unk20;
        arg0->unk3C = (s32) arg0->unk24;
        arg0->unk40 = (s32) arg0->unk28;
        arg0->x58[4] = (unaligned s32) arg0->xC[0x48];
        arg0->x58[8] = (unaligned s32) arg0->x58[0];
        arg0->x76[0x37] = arg0->x76[0x36];
        arg0->xC[8] = (unaligned s32) arg0->x171[0x2C7];
        arg0->xC[0xC] = (unaligned s32) arg0->x171[0x2CB];
        arg0->unk1C = (s32) arg0->unk428;
        arg0->unk20 = (s32) arg0->unk42C;
        arg0->unk24 = (s32) arg0->unk430;
        arg0->unk28 = (s32) arg0->unk434;
        arg0->xC[0x48] = (unaligned s32) arg0->x171[0x2CF];
        arg0->x58[0] = (unaligned s32) arg0->x171[0x2D3];
        var_a1 = arg0->x171[0x2D7];
        arg0->xBD &= 0xF7;
        if ((var_a1 != 0xFF) && ((temp_a2 = arg0->x76[0x36], (temp_a2 != var_a1)) || (var_a1 = temp_a2, ((s8) arg0->x76[0x31] == -1)))) {
            func_80041DDC(arg0, (s32) var_a1, 0, 1);
            arg0->x171[0x2D7] = 0xFF;
        }
        if ((s8) arg0->x76[0x30] & 0x80) {
            Sound_call(0x90, 0, 0);
        }
        break;
    case 3:
        Pl00_shot_enable_on(arg0);
        arg0->unk8 = 1;
        arg0->xBD = 8;
        break;
    }
    func_80051BAC(arg0);
}
