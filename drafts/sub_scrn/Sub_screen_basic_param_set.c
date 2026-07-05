/* Sub_screen_basic_param_set — 135 asm insns
 * TU: src/rock_neo/sub_scrn.c
 * asm: asm/rock_neo/nonmatchings/sub_scrn/Sub_screen_basic_param_set.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 *Sub_screen_basic_param_set_sub(s32 *, ?, s32, ?); /* extern */

s32 Sub_screen_basic_param_set(void) {
    s32 *temp_a0;
    s32 *temp_a3;
    s32 *temp_v0_2;
    s32 *var_a3;
    s32 temp_a0_2;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a2;
    s32 var_t0;
    s32 var_v0;
    void *var_a1;

    var_a2 = 0;
    temp_a3 = *(s32 **)0x1F800070;
    temp_a3->unk4 = 0xE100000D;
    temp_a0 = temp_a3 + 0x14;
    temp_a3->unk0 = 0x02000000;
    temp_a3->unk8 = 0;
    temp_a3->unk0 = ((s32) temp_a0 & 0xFFFFFF) | 0x02000000;
    temp_v1 = (s32) (Player_work.unkB0 - 1) >> 4;
    var_a3 = temp_a0;
    if (temp_v1 >= 0) {
        var_t0 = 0x31;
        var_a1 = var_a3 + 0xC;
        do {
            *var_a3 = ((s32) (var_a3 + 0x14) & 0xFFFFFF) | 0x04000000;
            temp_a0_2 = var_a2 * 0x10;
            var_a1->unk-4 = (s32) (var_t0 | 0x240000);
            var_a1->unk-8 = 0x64808080;
            var_a1->unk4 = 0x80008;
            if (temp_a0_2 < (s16) Player_work.life) {
                if ((temp_a0_2 < ((s16) Player_work.life - 0x10)) || !((s16) Player_work.life & 0xF)) {
                    var_v0 = 0x3E75B870;
                } else {
                    var_v0 = 0x3E75B878;
                }
            } else {
                var_v0 = 0x3E75B880;
            }
            var_a1->unk0 = var_v0;
            var_a1 += 0x14;
            var_a3 += 0x14;
            var_a2 += 1;
            var_t0 += 5;
        } while (temp_v1 >= var_a2);
    }
    temp_v0_2 = Sub_screen_basic_param_set_sub(Sub_screen_basic_param_set_sub(Sub_screen_basic_param_set_sub(var_a3, 0x2400A4, Game_work.zennyCount, 0x306), 0x240106, Game_work.time / 108000, 2), 0x24011A, (Game_work.time / 1800) % 60, 2);
    *(void *)0x1F800070 = temp_v0_2;
    temp_v0_2->unk-14 = (s32) ((temp_v0_2->unk-14 & 0xFF000000) | ((s32) D_80098934->x78 & 0xFFFFFF));
    temp_v0 = ((s32) D_80098934->x78 & 0xFF000000) | ((s32) temp_a3 & 0xFFFFFF);
    D_80098934->x78 = (void *) temp_v0;
    return temp_v0;
}
