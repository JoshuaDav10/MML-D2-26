/* func_800414F0 — 151 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_800414F0.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? RotMatrixYXZ(?, ? *, s16, s16);                   /* extern */
? func_800301A8(? *, s32 *, s32 *);                 /* extern */
s32 func_80030374(void *, s16 *, s16, s32);         /* extern */

s32 func_800414F0(void *arg0, void *arg1, ? arg2, void *arg3) {
    s16 sp18;
    s16 sp1A;
    s16 sp1C;
    s32 sp20;
    s32 sp24;
    s32 sp28;
    s32 sp30;
    s32 sp34;
    s32 sp38;
    ? sp40;
    s16 temp_a0_3;
    s16 temp_a2;
    s16 temp_a2_2;
    s16 temp_a3;
    s16 temp_t0;
    s16 temp_v1;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_s5;
    s32 temp_s6;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 var_a0;
    s32 var_a1;
    s32 var_a3;
    s32 var_t0;
    s32 var_t3;
    u16 temp_t2;

    temp_a0 = arg1->unk0 << 0x10;
    sp30 = temp_a0;
    sp20 = temp_a0 + ((arg3->unk0 + arg3->unk2) << 0xF);
    temp_a0_2 = arg1->unk4 << 0x10;
    sp38 = temp_a0_2;
    sp28 = temp_a0_2 + ((arg3->unk4 + arg3->unk6) << 0xF);
    temp_v0 = arg1->unk2 << 0x10;
    sp34 = temp_v0;
    sp24 = temp_v0;
    temp_a2 = arg3->unk0;
    temp_t0 = arg3->unk2;
    temp_a3 = arg3->unk6;
    temp_v0_2 = (s32) (temp_a2 + temp_t0) / 2;
    temp_s6 = temp_a2 - temp_v0_2;
    temp_a2_2 = arg3->unk4;
    temp_s5 = temp_t0 - temp_v0_2;
    temp_v0_3 = (s32) (temp_a2_2 + temp_a3) / 2;
    RotMatrixYXZ(arg2, &sp40, temp_a2_2, temp_a3);
    func_800301A8(&sp40, &sp20, &sp30);
    var_t3 = 0;
    var_a3 = 0xC;
    var_t0 = 0x50;
loop_1:
    var_a0 = *(s16 *)0x1F800100 * var_t0;
    if (var_a0 < 0) {
        var_a0 += 0xFFF;
    }
    temp_a0_3 = arg0->unk14 - (var_a0 >> 0xC);
    var_a1 = *(s16 *)0x1F800102 * var_t0;
    if (var_a1 < 0) {
        var_a1 += 0xFFF;
    }
    temp_v1 = arg0->unk18 - (var_a1 >> 0xC);
    temp_t2 = arg0->unk16;
    sp1A = unksp26;
    sp1C = unksp2A;
    sp18 = unksp22;
    if ((unksp22 >= ((temp_a0_3 - var_a3) - temp_s5)) && (((temp_a0_3 + var_a3) - temp_s6) >= unksp22) && (unksp2A >= ((temp_v1 - var_a3) - (temp_a3 - temp_v0_3))) && (((temp_v1 + var_a3) - (temp_a2_2 - temp_v0_3)) >= unksp2A) && (unksp26 >= ((s16) temp_t2 - arg3->unkA))) {
        var_a3 += 0xC;
        if (((s16) temp_t2 - arg3->unk8) >= unksp26) {
            return func_80030374(arg0 + 0x14, &sp18, unksp2A, var_a3);
        }
        goto block_13;
    }
    var_a3 += 0xC;
block_13:
    var_t3 += 1;
    var_t0 += 0x30;
    if (var_t3 >= 3) {
        return 0;
    }
    goto loop_1;
}
