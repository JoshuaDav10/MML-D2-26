? LoadImage(? *, void *, void *, u32);              /* extern */
? SsVabClose(s16);                                  /* extern */
s16 SsVabOpenHeadSticky(s32, s16, s32);             /* extern */
s16 SsVabTransBodyPartly(void *, ?, s16);           /* extern */
? SsVabTransCompleted(?);                           /* extern */
? func_80017684(void *, u32, s32);                  /* extern */
? func_800176A4(void *, void *, s32, u32);          /* extern */
? func_800176DC(void *, u32, ?);                    /* extern */
? func_8001B33C(?);                                 /* extern */
? func_8001CAAC();                                  /* extern */
? func_8001CB7C();                                  /* extern */
? func_8001CF98(s32);                               /* extern */
? func_8001D324(?);                                 /* extern */
extern s32 D_800987A8;
extern s32 D_80098828;
extern s32 D_80098868;
extern s8 D_800988D0;
extern s32 D_80098958;
extern u8 D_80098964;
extern u8 D_8009896C;
extern u32 D_800989C4;
extern u32 D_800989C8;
extern u32 D_80098A54;
extern void *D_80098A84;
extern ? D_80098AD0;
extern ? D_80098AD1;
extern ? D_80098AD4;
extern ? D_80098B38;
extern ? D_8009BE48;
extern s16 D_8009BE4A;
extern s16 D_8009BE4C;
extern s16 D_8009BE4E;
extern ? D_800AD168;
extern ? D_800B5DB0;
extern ? D_800C5604;
extern s32 D_800C5608;
extern s32 D_800C5614;
extern s8 D_800C5618;
extern s8 D_800C561C;
extern u8 D_800C5620;
extern u32 D_800C5624;
extern u32 D_800C5628;
extern s32 D_800C5634;
extern s32 D_800C5638;
extern ? Game_work;

void func_8001BB4C(void) {
    s16 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a1_3;
    s32 temp_v0;
    s32 temp_v0_10;
    s32 temp_v0_11;
    s32 temp_v0_12;
    s32 temp_v0_13;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_8;
    s32 temp_v0_9;
    s32 temp_v1_5;
    s32 temp_v1_6;
    s32 temp_v1_7;
    s32 var_a2_2;
    u32 temp_a0;
    u32 temp_a0_2;
    u32 temp_a3;
    u32 temp_a3_2;
    u32 temp_v0_14;
    u32 temp_v0_16;
    u32 temp_v0_6;
    u32 temp_v1;
    u32 temp_v1_2;
    u32 temp_v1_3;
    u32 temp_v1_4;
    u32 var_a2;
    u32 var_a2_3;
    u32 var_v0;
    u8 *var_a2_4;
    u8 *var_v1;
    u8 *var_v1_2;
    u8 *var_v1_3;
    u8 temp_v0_15;
    u8 temp_v0_17;
    u8 temp_v0_7;
    void *temp_a2;
    void *temp_s2;
    void *temp_s4;

    switch (D_800989C4) {                           /* switch 1 */
    case 0:                                         /* switch 1 */
        if (D_80098964 == 0) {
            func_8001CB7C();
        }
        D_800989C4 += 1;
        /* fallthrough */
    case 1:                                         /* switch 1 */
        if (D_80098964 != 0) {
            func_8001D324(0xA0);
            D_800989C4 += 1;
        case 2:                                     /* switch 1 */
            if (D_80098828 != 0) {
                D_80098828 -= 1;
                return;
            }
            func_8001CF98(D_80098A84->unk4);
            D_800988D0 = 0;
            D_800987A8 = 0;
            D_80098828 = 0;
            D_800989C4 += 1;
        case 3:                                     /* switch 1 */
            temp_v0 = D_80098828 + 1;
            D_80098828 = temp_v0;
            if (temp_v0 == 0x96) {
                D_8009896C |= 2;
                return;
            }
            if (*(&D_80098B38 + D_800987A8) == 1) {
                temp_s2 = &D_800C5604 + 8;
                temp_s4 = &D_800C5604 + 0x1C;
loop_14:
                switch (D_800989C8) {               /* switch 2 */
                case 0:                             /* switch 2 */
                    temp_a2 = (D_800987A8 << 0xB) + &D_800B5DB0;
                    temp_v1 = temp_a2->unk0;
                    D_80098A54 = temp_v1;
                    if (temp_v1 != -1U) {
                        D_800C5604.unk0 = (u32) temp_a2->unk4;
                        D_800C5604.unk4 = (s32) temp_a2->unk8;
                        switch (temp_v1) {          /* switch 3 */
                        case 0:                     /* switch 3 */
                            D_800C5604.unk8 = (u32) temp_a2->unkC;
                            temp_s2->unk-4 = (s32) (temp_s2->unk-4 - 1);
                        default:                    /* switch 3 */
block_46:
                            D_800989C8 = D_80098A54 + 1;
                            break;
                        case 1:                     /* switch 3 */
                            temp_v1_2 = temp_a2->unkC;
                            D_800C5604.unk8 = temp_v1_2;
                            temp_s2->unk4 = (s32) temp_a2->unk10;
                            temp_s2->unk8 = (s32) temp_a2->unk14;
                            temp_s2->unkC = (u32) temp_a2->unk18;
                            temp_s2->unk10 = (s32) temp_a2->unk1C;
                            temp_s2->unk14 = (s32) temp_a2->unk20;
                            temp_s2->unk18 = (s32) temp_a2->unk24;
                            temp_s2->unk2C = 0;
                            temp_s2->unk28 = 0;
                            temp_s2->unk1C = (s32) temp_a2->unk28;
                            if (temp_v1_2 != -1U) {
                                temp_a3 = temp_a2->unkC;
                                if (temp_a3 < 0x140U) {
                                    temp_a1 = temp_a2->unk10;
                                    temp_v1_3 = temp_a1 & 0x100;
                                    func_800176A4(temp_a2 + 0x100, ((temp_a3 + ((((temp_a1 + (temp_v1_3 >> 4)) - 0xF0) - temp_v1_3) << 8)) * 2) + &D_800AD168, ((u32) (temp_a2->unk18 * temp_a2->unk14) >> 4) * 2, temp_a3);
                                    temp_v0_2 = temp_a2->unk10;
                                    temp_a0 = temp_v0_2 & 0x100;
                                    D_80098868 |= 1 << (((temp_v0_2 + (temp_a0 >> 4)) - 0xF0) - temp_a0);
                                } else {
                                    D_8009BE48.unk0 = (s16) temp_a3;
                                    D_8009BE4A = (s16) temp_a2->unk10;
                                    D_8009BE4C = (s16) temp_a2->unk14;
                                    D_8009BE4E = (s16) temp_a2->unk18;
                                    LoadImage(&D_8009BE48, temp_a2 + 0x100, temp_a2, temp_a3);
                                }
                            }
                            D_8009BE48.unk4 = 0x20;
                            D_8009BE48.unk6 = 0x20;
block_41:
                            D_800C5608 -= 1;
                            goto block_46;
                        case 3:                     /* switch 3 */
                            D_800C5604.unk8 = (u32) temp_a2->unkC;
                            temp_s2->unk4 = (s32) temp_a2->unk10;
                            temp_s2->unk8 = (s32) temp_a2->unk14;
                            temp_s2->unk2C = 0;
                            temp_s2->unk28 = 0;
                            D_8009BE4C = 0x20;
                            D_8009BE4E = 0x20;
                            temp_s2->unkC = (u32) temp_a2->unk18;
                            goto block_46;
                        case 4:                     /* switch 3 */
                        case 9:                     /* switch 3 */
                        case 10:                    /* switch 3 */
                            temp_a3_2 = temp_a2->unkC;
                            if (temp_a3_2 < 0x100U) {
                                temp_a1_2 = temp_a2->unk10;
                                temp_v1_4 = temp_a1_2 & 0x100;
                                func_800176A4(temp_a2 + 0x100, ((temp_a3_2 + ((((temp_a1_2 + (temp_v1_4 >> 4)) - 0xF0) - temp_v1_4) << 8)) * 2) + &D_800AD168, ((u32) (temp_a2->unk18 * temp_a2->unk14) >> 4) * 2, temp_a3_2);
                                temp_v0_3 = temp_a2->unk10;
                                temp_a0_2 = temp_v0_3 & 0x100;
                                D_80098868 |= 1 << (((temp_v0_3 + (temp_a0_2 >> 4)) - 0xF0) - temp_a0_2);
                            } else {
                                D_8009BE48.unk0 = (s16) temp_a3_2;
                                D_8009BE4A = (s16) temp_a2->unk10;
                                if (D_80098A54 != 0xA) {
                                    D_8009BE4C = (s16) temp_a2->unk14;
                                    var_v0 = temp_a2->unk18;
                                } else {
                                    D_8009BE4C = 0x40;
                                    var_v0 = (u32) (temp_a2->unk14 * temp_a2->unk18) >> 6;
                                }
                                D_8009BE4E = (s16) var_v0;
                                LoadImage(&D_8009BE48, temp_a2 + 0x100, temp_a2, temp_a3_2);
                            }
block_86:
                            D_800989C8 = 0;
                            break;
                        case 5:                     /* switch 3 */
                            D_800C5604.unk8 = (u32) temp_a2->unkC;
                            temp_s2->unk4 = (s32) temp_a2->unk10;
                            temp_s2->unk8 = (s32) temp_a2->unk14;
                            temp_s2->unkC = (u32) temp_a2->unk18;
                            temp_s2->unk14 = 1;
                            temp_s2->unk10 = (s32) ((temp_a2->unkC * 0x1820) + 0x8014B000);
                            if (temp_a2->unkC == 4) {
                                if (Game_work.unk1DA == temp_a2->unk10) {
                                    temp_s2->unk14 = 0;
                                }
                                if (D_80098958 & 0x800) {
                                    temp_s2->unk14 = 1;
                                    func_8001B33C(0x8014B000);
                                }
                            }
                            if (D_800C5620 == 1) {
                                *(&Game_work + 0x1BA + (temp_a2->unkC * 8)) = (s8) temp_a2->unk10;
                                *(&Game_work + 0x1BC + (temp_a2->unkC * 8)) = temp_a2->unk14;
                                temp_a0_3 = *(&Game_work + 0x1B8 + (temp_a2->unkC * 8));
                                if (temp_a0_3 != -1) {
                                    SsVabClose(temp_a0_3);
                                }
                            }
                            goto block_41;
                        case 7:                     /* switch 3 */
                            temp_v0_4 = temp_a2->unk14 + 0x8013B000;
                            *(&D_80098AD4 + (temp_a2->unkC * 8)) = temp_v0_4;
                            D_800C5614 = temp_v0_4;
                            D_800C5618 = 0;
                            temp_v1_5 = temp_a2->unk10;
                            temp_a0_4 = temp_a2->unkC * 8;
                            if (*(&D_80098AD0 + temp_a0_4) != temp_v1_5) {
                                *(&D_80098AD0 + temp_a0_4) = (s8) temp_v1_5;
                                D_800C5618 = 1;
                            }
                            goto block_46;
                        case 8:                     /* switch 3 */
                            temp_v0_5 = temp_a2->unk18 + 0x8013B000;
                            *(&D_80098AD4 + (temp_a2->unkC * 8)) = temp_v0_5;
                            D_800C5618 = temp_v0_5;
                            D_800C561C = 0;
                            temp_v1_6 = temp_a2->unk10;
                            temp_a0_5 = temp_a2->unkC * 8;
                            if (*(&D_80098AD0 + temp_a0_5) != temp_v1_6) {
                                *(&D_80098AD0 + temp_a0_5) = (s8) temp_v1_6;
                                *(&D_80098AD1 + (temp_a2->unkC * 8)) = (s8) temp_a2->unk14;
                                D_800C561C = 1;
                            }
                            goto block_46;
                        }
                    default:                        /* switch 2 */
block_87:
                        *(&D_80098B38 + D_800987A8) = 0;
                        temp_v1_7 = D_800987A8 + 1;
                        D_800987A8 = temp_v1_7;
                        if (temp_v1_7 == 0xA) {
                            D_800987A8 = 0;
                        }
                        if (*(D_800987A8 + &D_80098B38) != 1) {
                            return;
                        }
                        goto loop_14;
                    }
                    D_800989C4 = 4;
                    return;
                case 1:                             /* switch 2 */
                    if ((u32) D_800C5604.unk0 >= 0x800U) {
                        func_800176DC((D_800987A8 << 0xB) + &D_800B5DB0, D_800C5604.unk8, 0x40);
                        D_800C5604.unk0 = (u32) (D_800C5604.unk0 - 0x800);
                        D_800C5604.unk8 = (u32) (D_800C5604.unk8 + 0x800);
                    } else {
                        var_a2 = D_800C5604.unk8;
                        var_v1 = (D_800987A8 << 0xB) + &D_800B5DB0;
                        if (D_800C5604.unk0 != 0) {
                            do {
                                temp_v0_7 = *var_v1;
                                var_v1 += 1;
                                *var_a2 = temp_v0_7;
                                var_a2 += 1;
                                temp_v0_6 = D_800C5604.unk0 - 1;
                                D_800C5604.unk0 = temp_v0_6;
                            } while (temp_v0_6 != 0);
                        }
                    }
                    temp_v0_8 = D_800C5608 - 1;
                    D_800C5608 = temp_v0_8;
                    if (temp_v0_8 == 0) {
                        goto block_86;
                    }
                    goto block_87;
                case 2:                             /* switch 2 */
                    D_8009BE48.unk0 = (s16) ((s32) D_800C561C + (D_800C5634 << 5));
                    D_8009BE4A = (s32) D_800C5620 + (D_800C5638 << 5);
                    LoadImage(&D_8009BE48, (D_800987A8 << 0xB) + &D_800B5DB0);
                    temp_v0_9 = D_800C5634 + 1;
                    D_800C5634 = temp_v0_9;
                    if (temp_v0_9 == ((u32) D_800C5624 >> 5)) {
                        D_800C5634 = 0;
                        temp_v0_10 = D_800C5638 + 1;
                        D_800C5638 = temp_v0_10;
                        if (temp_v0_10 != ((u32) D_800C5628 >> 5)) {

                        } else {
                            goto block_86;
                        }
                    }
                    goto block_87;
                case 4:                             /* switch 2 */
                    D_8009BE48.unk0 = (s16) (D_800C5604.unk8 + (temp_s2->unk28 << 5));
                    D_8009BE4A = temp_s2->unk4 + (temp_s2->unk2C << 5);
                    LoadImage(&D_8009BE48, (D_800987A8 << 0xB) + &D_800B5DB0);
                    temp_v0_11 = temp_s2->unk28 + 1;
                    temp_s2->unk28 = temp_v0_11;
                    if (temp_v0_11 == temp_s2->unk8) {
                        temp_s2->unk28 = 0;
                        temp_v0_12 = temp_s2->unk2C + 1;
                        temp_s2->unk2C = temp_v0_12;
                        if (temp_v0_12 == temp_s2->unkC) {
                            goto block_86;
                        }
                    }
                    goto block_87;
                case 6:                             /* switch 2 */
                    if (D_800C5604.unk1C == 0) {
                        temp_v0_13 = temp_s4->unk-18 - 1;
                        temp_s4->unk-18 = temp_v0_13;
                        if (temp_v0_13 == 0) {
                            goto block_86;
                        }
                    } else {
                        var_a2_2 = temp_s4->unk-8;
                        if (var_a2_2 >= 0x800) {
                            func_800176DC((D_800987A8 << 0xB) + &D_800B5DB0, temp_s4->unk-4, 0x40);
                            temp_s4->unk-4 = (u32) (temp_s4->unk-4 + 0x800);
                            temp_s4->unk-8 = (s32) (temp_s4->unk-8 - 0x800);
                        } else if (var_a2_2 != 0) {
                            if (var_a2_2 < 0) {
                                var_a2_2 += 3;
                            }
                            func_80017684((D_800987A8 << 0xB) + &D_800B5DB0, temp_s4->unk-4, var_a2_2 >> 2);
                            temp_a1_3 = temp_s4->unk-14;
                            temp_s4->unk-8 = 0;
                            *(&Game_work + 0x1B8 + (temp_s4->unk-14 * 8)) = SsVabOpenHeadSticky((temp_a1_3 * 0x1820) + 0x8014B000, (s16) temp_a1_3, temp_s4->unk-C);
                        } else {
                            if (SsVabTransBodyPartly((D_800987A8 << 0xB) + &D_800B5DB0, 0x800, *(&Game_work + 0x1B8 + (temp_s4->unk-14 * 8))) == *(&Game_work + 0x1B8 + (temp_s4->unk-14 * 8))) {
                                D_800989C8 = 0;
                            }
                            SsVabTransCompleted(1);
                        }
                    }
                    goto block_87;
                case 8:                             /* switch 2 */
                    if ((u32) D_800C5604.unk0 >= 0x800U) {
                        if (D_800C5604.unk14 != 0) {
                            func_800176DC((D_800987A8 << 0xB) + &D_800B5DB0, D_800C5604.unk10, 0x40);
                        }
                        D_800C5604.unk0 = (u32) (D_800C5604.unk0 - 0x800);
                        D_800C5604.unk10 = (u32) (D_800C5604.unk10 + 0x800);
                    } else {
                        if (D_800C5604.unk14 != 0) {
                            var_a2_3 = D_800C5604.unk10;
                            var_v1_2 = (D_800987A8 << 0xB) + &D_800B5DB0;
                            if (D_800C5604.unk0 != 0) {
                                do {
                                    temp_v0_15 = *var_v1_2;
                                    var_v1_2 += 1;
                                    *var_a2_3 = temp_v0_15;
                                    var_a2_3 += 1;
                                    temp_v0_14 = D_800C5604.unk0 - 1;
                                    D_800C5604.unk0 = temp_v0_14;
                                } while (temp_v0_14 != 0);
                            }
                        }
                        goto block_86;
                    }
                    goto block_87;
                case 9:                             /* switch 2 */
                    if ((u32) D_800C5604.unk0 >= 0x800U) {
                        if (D_800C5604.unk18 != 0) {
                            func_800176DC((D_800987A8 << 0xB) + &D_800B5DB0, (u32) D_800C5604.unk14, 0x40);
                        }
                        D_800C5604.unk0 = (u32) (D_800C5604.unk0 - 0x800);
                        D_800C5604.unk14 = (s32) ((s32) D_800C5604.unk14 + 0x800);
                    } else {
                        if (D_800C5604.unk18 != 0) {
                            var_a2_4 = (u8 *) D_800C5604.unk14;
                            var_v1_3 = (D_800987A8 << 0xB) + &D_800B5DB0;
                            if (D_800C5604.unk0 != 0) {
                                do {
                                    temp_v0_17 = *var_v1_3;
                                    var_v1_3 += 1;
                                    *var_a2_4 = temp_v0_17;
                                    var_a2_4 += 1;
                                    temp_v0_16 = D_800C5604.unk0 - 1;
                                    D_800C5604.unk0 = temp_v0_16;
                                } while (temp_v0_16 != 0);
                            }
                        }
                        goto block_86;
                    }
                    goto block_87;
                }
            }
        } else {
        default:                                    /* switch 1 */
            return;
        }
        break;
    case 4:                                         /* switch 1 */
        func_8001CAAC();
        break;
    }
}
