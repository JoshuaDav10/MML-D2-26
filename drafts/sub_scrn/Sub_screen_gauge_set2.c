/* Sub_screen_gauge_set2 — 111 asm insns
 * TU: src/rock_neo/sub_scrn.c
 * asm: asm/rock_neo/nonmatchings/sub_scrn/Sub_screen_gauge_set2.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

void Sub_screen_gauge_set2(s16 arg0, s16 arg1, s32 arg2, s32 arg3) {
    s16 temp_a0;
    s16 temp_t3;
    s16 temp_v1;
    s32 temp_a0_2;
    s32 temp_a1;
    s32 temp_t1;
    s32 var_t5;
    void *temp_t5;

    temp_a0 = arg1 + 8;
    temp_v1 = arg0 + arg2;
    temp_t3 = temp_v1 + 4;
    temp_t5 = *(void **)0x1F800070;
    temp_t1 = temp_t5 + 0x28;
    temp_t5->unk0 = (s32) ((temp_t1 & 0xFFFFFF) | 0x09000000);
    temp_t5->unk18 = arg0;
    temp_t5->unk8 = arg0;
    temp_t5->unkC = 0x9048;
    temp_t5->unk14 = 0x9049;
    temp_t5->unk1C = 0x9848;
    temp_t5->unk4 = 0x2C808080;
    temp_t5->unk20 = temp_v1;
    temp_t5->unk10 = temp_v1;
    temp_t5->unk12 = arg1;
    temp_t5->unkA = arg1;
    temp_t5->unk22 = temp_a0;
    temp_t5->unk1A = temp_a0;
    temp_t5->unk16 = 0xC;
    temp_t5->unkE = 0x3E83;
    temp_t5->unk24 = 0x9849;
    temp_a1 = temp_t1 + 0x28;
    temp_t5->unk28 = (s32) ((temp_a1 & 0xFFFFFF) | 0x09000000);
    temp_t1->unkC = 0x904C;
    temp_t1->unk14 = 0x9050;
    temp_t1->unk1C = 0x984C;
    temp_t1->unk4 = 0x2C808080;
    temp_t1->unk18 = temp_v1;
    temp_t1->unk8 = temp_v1;
    temp_t1->unk20 = temp_t3;
    temp_t1->unk10 = temp_t3;
    temp_t1->unk12 = arg1;
    temp_t1->unkA = arg1;
    temp_t1->unk22 = temp_a0;
    temp_t1->unk1A = temp_a0;
    temp_t1->unk16 = 0xC;
    temp_t1->unkE = 0x3E83;
    temp_t1->unk24 = 0x9850;
    var_t5 = temp_a1;
    if (arg3 != 0) {
        if (arg2 == 0x58) {
            temp_a0_2 = var_t5 + 0x28;
            temp_t1->unk28 = (s32) ((temp_a0_2 & 0xFFFFFF) | 0x04000000);
            var_t5->unk4 = 0x64808080;
            var_t5->unk8 = (s16) (arg0 + 0x18);
            var_t5->unkA = arg1;
            var_t5->unkC = 0x3E82F828;
            var_t5->unk10 = 0x80028;
            var_t5 = temp_a0_2;
        }
    }
    *(void *)0x1F800070 = (void *) var_t5;
    var_t5->unk-28 = (s32) ((var_t5->unk-28 & 0xFF000000) | ((s32) D_80098934->x78 & 0xFFFFFF));
    D_80098934->x78 = (void *) (((s32) D_80098934->x78 & 0xFF000000) | ((s32) temp_t5 & 0xFFFFFF));
}
