/* func_8003F5D8 — 134 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003F5D8.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 func_8004030C(PL_WORK *, u8);                   /* extern */
s32 func_800403D4(PL_WORK *, u8);                   /* extern */
s32 func_80040468(PL_WORK *, u8);                   /* extern */
s32 func_800404FC(PL_WORK *, u8);                   /* extern */
? func_80040574(PL_WORK *);                         /* extern */
s32 func_80040630(PL_WORK *, u8);                   /* extern */
s32 func_80040FA8(PL_WORK *);                       /* extern */
s32 func_80041044(PL_WORK *);                       /* extern */
s32 func_800410D4(PL_WORK *, u16);                  /* extern */
s32 func_80041158(PL_WORK *);                       /* extern */
s32 func_800411DC(PL_WORK *);                       /* extern */

void func_8003F5D8(PL_WORK *arg0) {
    u16 temp_a1;
    u16 temp_s1;
    u16 temp_s2;

    temp_s2 = arg0->x11C;
    temp_s1 = arg0->x11E;
    if ((func_80040AEC(arg0) != 0) || (func_800411DC(arg0) != 0)) {
        arg0->x44A[0] = 0;
        return;
    }
    if (temp_s1 & (arg0->x124 | arg0->x126 | (arg0->unk12C | arg0->unk12E))) {
        arg0->unkB = 0U;
    }
    temp_a1 = arg0->x12A;
    if ((temp_s2 & (arg0->x138 | (arg0->x124 | arg0->x126 | (arg0->unk12C | arg0->unk12E)) | (arg0->unk13A | arg0->unk13C) | (arg0->x128 | temp_a1))) || (arg0->x171[5] & 3)) {
        arg0->x44A[0] = 0;
        if ((func_800410D4(arg0, temp_a1) != 0) || ((func_80040B68(arg0, 0), (func_800406A8(arg0) == 0)) && ((func_80041044(arg0) != 0) || (func_80040FA8(arg0) == 0) || (arg0->unkB != 0) || ((func_80040380(arg0) == 0) && (func_800403D4(arg0, arg0->unkB) == 0) && (func_80040468(arg0, arg0->unkB) == 0) && (func_800404FC(arg0, arg0->unkB) == 0))) && (((s8) arg0->x171[3] & 0xC0) || ((s8) arg0->x171[6] != 0) || ((func_800402C4(arg0) == 0) && (func_80040630(arg0, arg0->unkB) == 0) && (func_8004030C(arg0, arg0->unkB) == 0))))) {
            if (func_80041158(arg0) == 0) {
                func_800405F4(arg0);
            }
            goto block_23;
        }
    } else {
block_23:
        func_80040574(arg0);
    }
}
