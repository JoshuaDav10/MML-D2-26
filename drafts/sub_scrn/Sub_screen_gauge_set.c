/* Sub_screen_gauge_set — 137 asm insns
 * TU: src/rock_neo/sub_scrn.c
 * asm: asm/rock_neo/nonmatchings/sub_scrn/Sub_screen_gauge_set.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

void Sub_screen_gauge_set(s16 arg0, s16 arg1, s32 arg2, s32 arg3) {
    s16 temp_a1;
    s16 temp_t0_2;
    s16 temp_t1;
    s16 temp_t2;
    s32 temp_a0;
    s32 temp_t0;
    s32 temp_t3;
    s32 temp_t3_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_t8;
    void *temp_t8;

    temp_t0 = arg0 + arg2;
    temp_t1 = temp_t0 + 6;
    temp_a1 = arg1 + (arg3 * 2);
    temp_t2 = temp_a1 + 5;
    temp_t0_2 = temp_t0 + 0xA;
    temp_v1 = arg3 * 8;
    temp_a0 = (temp_v1 + 0x90) << 8;
    temp_t8 = *(void **)0x1F800070;
    temp_v1_2 = (temp_v1 + 0x95) << 8;
    temp_t3 = temp_t8 + 0x28;
    temp_t8->unk0 = (s32) ((temp_t3 & 0xFFFFFF) | 0x09000000);
    temp_t8->unk18 = arg0;
    temp_t8->unk8 = arg0;
    temp_t8->unkC = (s16) (temp_a0 | 0x60);
    temp_t8->unk14 = (s16) (temp_a0 | 0x61);
    temp_t8->unk1C = (s16) (temp_v1_2 | 0x60);
    temp_t8->unk4 = 0x2C808080;
    temp_t8->unk20 = temp_t1;
    temp_t8->unk10 = temp_t1;
    temp_t8->unk12 = temp_a1;
    temp_t8->unkA = temp_a1;
    temp_t8->unk22 = temp_t2;
    temp_t8->unk1A = temp_t2;
    temp_t8->unk16 = 0xC;
    temp_t8->unkE = 0x3E83;
    temp_t8->unk24 = (s16) (temp_v1_2 | 0x61);
    temp_t3_2 = temp_t3 + 0x28;
    temp_t3->unkC = (s16) (temp_a0 | 0x6F);
    temp_t8->unk28 = (s32) ((temp_t3_2 & 0xFFFFFF) | 0x09000000);
    temp_t3->unk4 = 0x2C808080;
    temp_t3->unk18 = temp_t1;
    temp_t3->unk8 = temp_t1;
    temp_t3->unk20 = temp_t0_2;
    temp_t3->unk10 = temp_t0_2;
    temp_t3->unk12 = temp_a1;
    temp_t3->unkA = temp_a1;
    temp_t3->unk22 = temp_t2;
    temp_t3->unk1A = temp_t2;
    temp_t3->unk16 = 0xC;
    temp_t3->unkE = 0x3E83;
    temp_t3->unk14 = (s16) (temp_a0 | 0x73);
    temp_t3->unk1C = (s16) (temp_v1_2 | 0x6F);
    temp_t3->unk24 = (s16) (temp_v1_2 | 0x73);
    var_t8 = temp_t3_2;
    if (arg3 == 0) {
        if (arg1 != 0x46) {
            if (arg2 != 0x46) {

            } else {
                goto block_6;
            }
        } else if (arg2 == 0x28) {
block_6:
            temp_t3->unk28 = (s32) (((var_t8 + 0x28) & 0xFFFFFF) | 0x04000000);
            var_t8->unk4 = 0x64808080;
            if (arg2 == 0x46) {
                var_t8->unk8 = (s16) (arg0 + 0x18);
            } else {
                var_t8->unk8 = (s16) (arg0 + 0xA);
            }
            var_t8->unkA = arg1;
            var_t8->unkC = 0x3E82F028;
            var_t8->unk10 = 0x60018;
            var_t8 += 0x28;
        }
    }
    *(void *)0x1F800070 = (void *) var_t8;
    var_t8->unk-28 = (s32) ((var_t8->unk-28 & 0xFF000000) | ((s32) D_80098934->x78 & 0xFFFFFF));
    D_80098934->x78 = (void *) (((s32) D_80098934->x78 & 0xFF000000) | ((s32) temp_t8 & 0xFFFFFF));
}
