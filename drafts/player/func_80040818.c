/* func_80040818 — 181 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_80040818.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_80039A38(? *, u16);                          /* extern */
s32 func_80048E6C(void *);                          /* extern */
s32 func_80048EA4(u16 *, ?, ?, u8);                 /* extern */
s32 func_8004976C(void *);                          /* extern */
extern ? D_80097D9C;

s32 func_80040818(void *arg0) {
    u16 sp10;
    u16 sp12;
    u16 sp14;
    u16 sp18;
    s8 sp1A;
    u16 sp1C;
    u16 sp20;
    u16 sp24;
    s16 temp_s0;
    s16 temp_v0;
    s16 temp_v1_3;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_s0_2;
    s32 temp_s2;
    s32 var_v1;
    s32 var_v1_2;
    u16 temp_v1;
    u16 temp_v1_2;
    u16 var_a1;
    u16 var_v0;
    u16 var_v0_2;
    u16 var_v0_3;

    sp18 = (unaligned s32) arg0->unk14;
    sp1C = (unaligned s32) arg0->unk18;
    sp10 = (unaligned s32) sp18;
    sp14 = (unaligned s32) sp1C;
    if ((arg0->unk74 != 0) && (arg0->unk46 > 0)) {
        temp_s0 = arg0->unk16;
        if (((func_80048E6C(arg0) - 0x30) - arg0->unk78->unkA) >= temp_s0) {
            temp_a1 = func_8004976C(arg0);
            temp_a2 = (s32) ((arg0->unk56 - arg0->unk116) + 0x200) >> 0xA;
            temp_s2 = temp_a2 & 3;
            if (temp_a2 & 1) {
                sp14 += arg0->unk78->unk4;
                var_a1 = temp_a1 - *(s16 *)0x1F8000C0;
                sp20 = var_a1;
                sp24 = 0;
                sp1C += arg0->unk78->unk6;
                if (temp_a2 & 2) {
                    var_v0 = arg0->unk78->unk2;
                    var_v1 = sp18 + 1;
                } else {
                    var_v0 = arg0->unk78->unk0;
                    var_v1 = sp18 - 1;
                }
                temp_v1 = var_v1 + (var_v0 + var_a1);
                sp18 = temp_v1;
                sp10 = temp_v1;
            } else {
                sp10 += arg0->unk78->unk0;
                var_a1 = temp_a1 - *(s16 *)0x1F8000C4;
                sp20 = 0;
                sp24 = var_a1;
                sp18 += arg0->unk78->unk2;
                if (temp_a2 & 2) {
                    var_v0_2 = arg0->unk78->unk6;
                    var_v1_2 = sp1C + 1;
                } else {
                    var_v0_2 = arg0->unk78->unk4;
                    var_v1_2 = sp1C - 1;
                }
                temp_v1_2 = var_v1_2 + (var_v0_2 + var_a1);
                sp1C = temp_v1_2;
                sp14 = temp_v1_2;
            }
            var_v0_3 = var_a1;
            if ((s32) var_a1 < 0) {
                var_v0_3 = (u16) -(s32) var_v0_3;
            }
            if ((s32) var_v0_3 < 0x41) {
                sp12 -= 0x98;
                sp1A = (u16) sp1A - 0x98;
                temp_s0_2 = func_80048EA4(&sp10, 0, 0, arg0->unk1);
                if (temp_s0_2 == func_80048EA4(&sp18, 0, 0, arg0->unk1)) {
                    temp_v1_3 = arg0->unk16;
                    if (temp_s0_2 < (temp_v1_3 - 0x68)) {
                        if ((temp_v1_3 - 0x98) < temp_s0_2) {
                            arg0->unk14 = (u16) (arg0->unk14 + sp20);
                            temp_v0 = temp_s0_2 + 0x98;
                            arg0->unk16 = temp_v0;
                            arg0->unk20 = (s32) (temp_v0 << 0x10);
                            arg0->unk56 = (s16) (temp_s2 << 0xA);
                            arg0->unk116 = 0;
                            arg0->unk112 = 0;
                            arg0->unk18 = (u16) (arg0->unk18 + sp24);
                            func_80039A38(&D_80097D9C, sp24);
                            arg0->unk9 = 0xD;
                            arg0->unkA = (s16) (temp_s2 << 8);
                            return 1;
                        }
                        /* Duplicate return node #20. Try simplifying control flow for better match */
                        return 0;
                    }
                    goto block_19;
                }
                /* Duplicate return node #20. Try simplifying control flow for better match */
                return 0;
            }
block_19:
            /* Duplicate return node #20. Try simplifying control flow for better match */
            return 0;
        }
    }
    return 0;
}
