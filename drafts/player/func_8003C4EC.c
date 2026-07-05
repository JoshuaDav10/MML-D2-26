/* func_8003C4EC — 177 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003C4EC.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? Model_Shadow_Trans(PL_WORK *, ?, ?, ?);           /* extern */
s32 Sce_flag_test(?);                               /* extern */
? func_8002D004(PL_WORK *, s8, s8);                 /* extern */
? func_800325D4(PL_WORK *);                         /* extern */
? func_800329D4(PL_WORK *);                         /* extern */
s32 func_80048E6C(PL_WORK *);                       /* extern */
s32 func_8004B414(PL_WORK *);                       /* extern */
? func_80050C2C(PL_WORK *);                         /* extern */
? func_80053158(PL_WORK *, s32);                    /* extern */
? func_8005331C(PL_WORK *, s32);                    /* extern */

void func_8003C4EC(PL_WORK *arg0) {
    s16 temp_s0;
    s16 temp_v1;

    if ((arg0->x74 == 1) && (arg0->unk7C & 0x2000) && ((u32) (arg0->x9 - 7) < 2U) && ((u8) arg0->unkA < 4U)) {
        func_80041DDC(arg0, 0x15, 0, 1);
        arg0->unkA = 4U;
        arg0->unk46 = 0;
        arg0->unk4E = 0;
    }
    if (((u32) (arg0->x0[8] - 1) < 2U) && ((s8) arg0->x76[0] != 0)) {
        func_80053158(arg0, arg0->unkC0);
    }
    if ((arg0->x0[8] == 3) && ((s8) arg0->x76[0] != 0)) {
        func_8005331C(arg0, arg0->unkC0);
    }
    if (Sce_flag_test(0x200) == 0) {
        func_80050C2C(arg0);
    }
    if (((u32) (arg0->x0[8] - 1) < 2U) && !((s32) arg0->unk74 & 0xFF00FF)) {
        temp_s0 = arg0->unk16;
        temp_v1 = func_80048E6C(arg0) - arg0->unk78->unkA;
        if (temp_s0 < (temp_v1 - 0x20)) {
            if ((s8) arg0->x75 == 0) {
                if (arg0->x9 == 0xE) {
                    func_800394AC();
                    arg0->x108 = 0;
                }
                if (arg0->x9 == 3) {
                    if ((u8) arg0->unkA < 5U) {
                        Sound_call(0x8E, 0, 0);
                        Sound_call(0x8F, 0, 0);
                    }
                }
                arg0->x9 = 7;
                arg0->xA = 4;
                arg0->unk46 = 0;
                arg0->x142[0x26] = 0;
                arg0->x142[0x27] = 0;
                arg0->x110 = 1;
                arg0->x111 = 0;
            }
            arg0->x74 = 1;
        } else if (arg0->unk16 < temp_v1) {
            arg0->unk16 = temp_v1;
            arg0->unk20 = (s32) (temp_v1 << 0x10);
        }
    }
    if (func_8004B414(arg0) < 0) {
        arg0->unk48 = 0;
        arg0->unk44 = 0;
        arg0->unk14 = (u16) arg0->unk2C;
        arg0->unk18 = (u16) arg0->unk30;
        arg0->unk1C = (s32) arg0->unk34;
        arg0->unk24 = (s32) arg0->unk3C;
        if ((s8) arg0->x74 != 0) {
            arg0->x74 = 0;
            arg0->unk46 = 0;
            arg0->unk16 = (s16) arg0->unk2E;
            arg0->unk20 = (s32) arg0->unk38;
        }
    }
    func_8002D004(arg0, (s8) arg0->x142[0x26], (s8) arg0->x142[0x27]);
    func_800329D4(arg0);
    func_800325D4(arg0);
    if (arg0->x0[0] & 2) {
        Model_Shadow_Trans(arg0, 0x32, 0x2AB, 0);
    }
}
