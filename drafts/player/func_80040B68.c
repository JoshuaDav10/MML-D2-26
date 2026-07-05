/* func_80040B68 — 166 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_80040B68.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 func_80037440();                                /* extern */
s32 func_80040E00(PL_WORK *, ?, s16, u8);           /* extern */
extern ? D_80089A88;

void func_80040B68(PL_WORK *arg0, s32 arg1) {
    u16 temp_s1;
    u8 temp_s3;
    u8 temp_s4;

    temp_s1 = arg0->x11C;
    temp_s4 = *(&D_80089A88 + (arg0->weapon_right_no * 0xC));
    temp_s3 = *(&D_80089A88 + ((s8) arg0->x171[0] * 0xC));
    if (!((s8) arg0->x171[3] & 0xC0)) {
        if ((s8) arg0->x171[6] == 1) {
            if ((!(arg0->unk13A & temp_s1) && ((arg0->x171[5] & 3) != 2)) || ((arg0->x171[4] & 5) != 5) || (func_80040E00(arg0, 0, (s16) arg1, temp_s3) == 0)) {
                goto block_7;
            }
            goto block_13;
        }
block_7:
        if (((s8) arg0->x171[6] == 2) && ((arg0->unk13C & temp_s1) || ((arg0->x171[5] & 3) == 1)) && ((arg0->x171[4] & 3) == 3) && (func_80037440() != 0) && (func_80040E00(arg0, 2, (s16) arg1, temp_s4) != 0)) {
block_13:
            arg0->unk178 = 1;
            arg0->x171[6] = 0;
            arg0->x171[5] = 0;
            return;
        }
        if (((arg0->unk13A & temp_s1) || ((arg0->x171[5] & 3) == 2)) && (arg0->weapon_data[(s8) arg0->x171[0]].unk6 > 0) && ((arg0->x171[4] & 5) == 5)) {
            arg0->x171[5] = 0;
            func_80040E00(arg0, 4, (s16) arg1, temp_s3);
            return;
        }
        if (((arg0->unk13C & temp_s1) || ((arg0->x171[5] & 3) == 1)) && (arg0->weapon_data[arg0->weapon_right_no].unk6 > 0) && ((arg0->x171[4] & 3) == 3) && (func_80037440() != 0)) {
            arg0->x171[5] = 0;
            func_80040E00(arg0, 6, (s16) arg1, temp_s4);
            return;
        }
        goto block_25;
    }
block_25:
    arg0->x171[5] = 0;
}
