/* func_8003BE6C — 132 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003BE6C.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_800176DC(void *, u8 *, ?);                   /* extern */
? func_800177CC(u8 *, u8 *);                        /* extern */
? func_80037534(PL_WORK *);                         /* extern */
? func_80037594();                                  /* extern */
? func_800425FC(PL_WORK *, s8);                     /* extern */
extern ? D_80097D90;
extern s32 D_8009888C;
extern ? D_800B5DB0;

void func_8003BE6C(PL_WORK *arg0, s32 arg1) {
    D_8009888C = 0x80000000;
    if ((s8) Game_work.x82 & 0x80) {
        arg0->x0[0] = 0;
        arg0->x0[6] = 0;
    } else {
        arg0->x0[0] = 3;
        arg0->x0[6] = 3;
        arg0->unk2 = 0;
        arg0->unk4 = 0;
        arg0->unkC = 0;
        arg0->unk8 = 0;
    }
    arg0->unk78 = &D_80097D90;
    arg0->xBD = 0;
    arg0->x75 = 0;
    arg0->x74 = 0;
    arg0->x76[1] = 0;
    arg0->x76[0] = 0;
    arg0->unk184 = 0;
    arg0->x110 = 0;
    arg0->x111 = 0;
    arg0->x449 = 0;
    if (arg0 == &Player_work) {
        Player_work.x0[2] = 0;
    } else {
        arg0->x0[2] = 1;
    }
    func_800425FC(arg0, Game_work.x54[0]);
    if (arg1 == 0) {
        arg0->unkB0 = 0x50U;
        arg0->life = 0x50;
        arg0->x142[0x28] = 0;
        arg0->x142[0x29] = 0;
        arg0->x142[0x2A] = 0;
        arg0->weapon_right_no = 0;
        arg0->x171[1] = 0;
        arg0->x171[0] = 0;
        arg0->x171[2] = 1;
        arg0->x44A[2] = 0;
        arg0->x44A[4] = 0;
        arg0->unk450 = 0;
        func_800177CC(arg0->rb_parts_sort_data, &arg0->rb_parts_sort_data[4]);
        func_800177CC(arg0->x474, &arg0->x474[0x3C]);
        func_80037534(arg0);
        return;
    }
    arg0->unkB0 = (u16) D_800B5DB0.unk1CE;
    arg0->life = D_800B5DB0.unk1CE;
    arg0->x142[0x28] = D_800B5DB0.unk1D4;
    arg0->x142[0x29] = D_800B5DB0.unk1D5;
    arg0->x142[0x2A] = D_800B5DB0.unk1D6;
    arg0->weapon_right_no = (s8) D_800B5DB0.unk1D8;
    arg0->x171[0] = D_800B5DB0.unk1D9;
    arg0->x171[1] = D_800B5DB0.unk1DA;
    arg0->x171[2] = D_800B5DB0.unk1DB;
    arg0->x44A[2] = D_800B5DB0.unk1DC;
    arg0->x44A[4] = D_800B5DB0.unk1DE;
    arg0->x44A[5] = D_800B5DB0.unk1DF;
    arg0->unk450 = (s32) D_800B5DB0.unk208;
    func_800176DC(&D_800B5DB0 + 0x20C, arg0->rb_parts_sort_data, 1);
    func_800176DC(&D_800B5DB0 + 0x1C80, arg0->x474, 4);
    func_800176DC(&D_800B5DB0 + 0x22C, (u8 *) arg0->weapon_data, 8);
    func_80037594();
}
