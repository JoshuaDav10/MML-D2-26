/* func_8001F3C8 — 110 asm insns
 * TU: src/rock_neo/scene.c
 * asm: asm/rock_neo/nonmatchings/scene/func_8001F3C8.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? Obj_work_flag_change(?, ?);                       /* extern */
? func_8001F5E4();                                  /* extern */
? func_8002CFA4();                                  /* extern */
? func_80033EBC();                                  /* extern */
? func_80051B40();                                  /* extern */
? func_800629E0();                                  /* extern */
extern ? D_800893BC;
extern ? D_80089790;
extern s8 D_800C356C;
extern ? Player_work;

s32 func_8001F3C8(void) {
    s16 temp_s0_2;
    s8 temp_s0;
    s8 temp_s2;

    temp_s2 = Game_work.stage_no;
    temp_s0 = Game_work.area_no;
    func_8001F740();
    if (D_800C356C != -1) {
        temp_s0_2 = ((temp_s0 * 2) + *(&D_800893BC + (temp_s2 * 4)))->unk2;
        if ((temp_s0_2 >= 0) && (Game_work.x6A != temp_s0_2)) {
            Cd_read_comb((s32) temp_s0_2);
            Game_work.x6A = temp_s0_2;
        }
        func_8001F5E4();
        func_8001D888();
        Obj_work_flag_change(0x3FE, 0x80);
        func_80031AA4();
    }
    *(&D_80089790 + (temp_s2 * 4))();
    if (Scene_work.x0 == 0) {
        *(s8 *)0x1F800074 = 0;
        if (D_800C356C != -1) {
            Player_work.unk8 = 0;
            Player_work.unkBD = (u8) (Player_work.unkBD & 0xBF);
        }
    }
    if (Player_work.unk44E >= 0) {
        if (Player_work.unk44E != 0) {
            if (Player_work.unk44F != 0) {
                Player_work.unk170 = (u8) Player_work.unk172;
            } else {
                Player_work.unk170 = 0U;
            }
            Player_work.unk171 = (u8) Player_work.unk173;
        } else {
            Player_work.unk170 = 0U;
            Player_work.unk171 = 0U;
        }
    }
    func_8002CFA4();
    func_80051B40();
    func_80033EBC();
    func_800629E0();
    func_800179FC();
    Game_work.x7E[0] = 0;
    Sce_flag_off(0x1FF);
    return 0;
}
