/* func_8003C88C — 165 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003C88C.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_8004174C(PL_WORK *);                         /* extern */
? func_800426F8(PL_WORK *);                         /* extern */
? func_80042734(PL_WORK *);                         /* extern */
s8 func_8004AF28(void *, u16, u8, u8);              /* extern */
s32 func_8004B414(PL_WORK *);                       /* extern */
? func_80051BAC(PL_WORK *);                         /* extern */
extern ? D_80089FA4;
extern ? D_80089FF8;

void func_8003C88C(PL_WORK *arg0) {
    s32 temp_v0_3;
    s32 temp_v1_2;
    s8 temp_v0;
    s8 temp_v0_2;
    u16 temp_a1;
    u8 temp_a2;
    u8 temp_a3;
    u8 temp_v1;
    u8 var_v0;

    arg0->xC[0x20] = (unaligned s32) arg0->xC[8];
    arg0->xC[0x24] = (unaligned s32) arg0->xC[0xC];
    arg0->unk34 = (s32) arg0->unk1C;
    arg0->unk38 = (s32) arg0->unk20;
    arg0->unk3C = (s32) arg0->unk24;
    arg0->unk40 = (s32) arg0->unk28;
    arg0->x58[4] = (unaligned s32) arg0->xC[0x48];
    arg0->x58[8] = (unaligned s32) arg0->x58[0];
    temp_a1 = arg0->life;
    temp_a2 = arg0->x74;
    temp_a3 = arg0->x76[0];
    arg0->x76[0x37] = arg0->x76[0x36];
    arg0->unkB2 = temp_a1;
    arg0->x75 = temp_a2;
    arg0->x76[1] = temp_a3;
    arg0->xBD &= 0xF7;
    temp_v0 = func_8004AF28(arg0 + 0x14, temp_a1, temp_a2, temp_a3);
    if (temp_v0 != 0) {
        arg0->x9 = 0xA;
        arg0->unkA = 0U;
        arg0->unkB = temp_v0;
    } else if ((arg0->unk8 & 0xFFFF00) == 0xA00) {
        arg0->unkA = 1U;
    }
    if (arg0->x449 == 0) {
        temp_v0_2 = arg0->x110;
        if (temp_v0_2 == 0) {
            arg0->x0[6] |= 3;
            func_8004174C(arg0);
        } else {
            if (temp_v0_2 > 0) {
                temp_v1 = arg0->x111 - 1;
                arg0->x111 = temp_v1;
                if (temp_v1 & 0x80) {
                    arg0->x70[1] = 0;
                    arg0->x110 = 0;
                    goto block_13;
                }
                if (temp_v1 & 2) {
                    var_v0 = arg0->x0[6] & 0xFD;
                } else {
                    goto block_12;
                }
            } else {
                goto block_12;
            }
            goto block_14;
        }
    } else {
block_12:
block_13:
        var_v0 = arg0->x0[6] | 3;
block_14:
        arg0->x0[6] = var_v0;
    }
    temp_v1_2 = (s8) arg0->x171[3] & 0xC0;
    arg0->unk188 = 0;
    if (temp_v1_2 != 0) {
        if (temp_v1_2 == 0x80) {
            func_80042734(arg0);
        } else {
            func_800426F8(arg0);
        }
    }
    if (!((s8) arg0->xBD & 0x80)) {
        *(&D_80089FF8 + (arg0->x9 * 4))(arg0);
        *(&D_80089FA4 + (arg0->x9 * 4))(arg0);
    }
    temp_v0_3 = func_8004B414(arg0) & 0x2000;
    if (((s8) arg0->x74 == 1) && (temp_v0_3 != 0) && ((u32) (arg0->x9 - 7) < 2U) && ((u8) arg0->unkA < 4U)) {
        func_80041DDC(arg0, 0x15, 0, 1);
        arg0->unkA = 4U;
        arg0->unk46 = 0;
        arg0->unk4E = 0;
    }
    func_80051BAC(arg0);
}
