/* Sub_screen_status_calc — 123 asm insns
 * TU: src/rock_neo/sub_scrn.c
 * asm: asm/rock_neo/nonmatchings/sub_scrn/Sub_screen_status_calc.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

extern ? D_8008D7E8;
extern ? D_8008DB08;

void Sub_screen_status_calc(void *arg0, s32 arg1) {
    u16 *temp_a3;

    if (arg1 < 0) {
        arg0->unk16 = 0;
        arg0->unk11 = 0U;
        arg0->unk1E = 0;
        arg0->unk19 = 0U;
        arg0->unk26 = 0;
        arg0->unk21 = 0U;
        arg0->unk2E = 0;
        arg0->unk29 = 0U;
        arg0->unk36 = 0;
        arg0->unk31 = 0U;
        return;
    }
    temp_a3 = (arg1 * 2) + &D_8008DB08;
    arg0->unk11 = (u8) *((*temp_a3 * 0x14) + &D_8008D7E8 + Player_work.weapon_data[arg1].attack_level);
    arg0->unk19 = (u8) *((*temp_a3 * 0x14) + (&D_8008D7E8 + 4) + Player_work.weapon_data[arg1].bullet_level);
    arg0->unk21 = (u8) *((*temp_a3 * 0x14) + (&D_8008D7E8 + 8) + Player_work.weapon_data[arg1].dist_level);
    arg0->unk29 = (u8) *((*temp_a3 * 0x14) + (&D_8008D7E8 + 0xC) + Player_work.weapon_data[arg1].repeat_level);
    arg0->unk31 = (u8) *((*temp_a3 * 0x14) + (&D_8008D7E8 + 0x10) + Player_work.weapon_data[arg1].unk[0]);
    if (arg0->unk11 == 0xFF) {
        arg0->unk11 = 0x58U;
        arg0->unk16 = 1;
    } else {
        arg0->unk16 = 0;
    }
    if (arg0->unk19 == 0xFF) {
        arg0->unk19 = 0x58U;
        arg0->unk1E = 1;
    } else {
        arg0->unk1E = 0;
    }
    if (arg0->unk21 == 0xFF) {
        arg0->unk21 = 0x58U;
        arg0->unk26 = 1;
    } else {
        arg0->unk26 = 0;
    }
    if (arg0->unk29 == 0xFF) {
        arg0->unk29 = 0x58U;
        arg0->unk2E = 1;
    } else {
        arg0->unk2E = 0;
    }
    if (arg0->unk31 == 0xFF) {
        arg0->unk31 = 0x58U;
        arg0->unk36 = 1;
        return;
    }
    arg0->unk36 = 0;
}
