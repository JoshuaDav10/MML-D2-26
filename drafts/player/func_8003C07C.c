/* func_8003C07C — 151 asm insns
 * TU: src/rock_neo/player.c
 * asm: asm/rock_neo/nonmatchings/player/func_8003C07C.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? func_8003C418(PL_WORK *, u8 *, u8);               /* extern */
extern u16 D_800C0C26;
extern u16 D_800C0C2A;
extern u16 D_800C0C2C;
extern u16 D_800C0C2E;
extern u16 D_800C0F96;
extern u16 D_800C0F9A;
extern u16 D_800C0F9C;
extern u16 D_800C0F9E;
extern u16 D_800C0FE6;
extern u16 D_800C0FEA;
extern u16 D_800C0FEC;
extern u16 D_800C0FEE;
extern PL_WORK Player_work2;

void func_8003C07C(u16 arg1) {
    s32 var_v0;
    u16 var_a1;

    var_a1 = arg1;
    if ((s8) Player_work.x0[0] & Player_work.x0[6] & 1) {
        var_v0 = Player_work.xBD & 0x10;
        if (!(Player_work.xBD & 0x40)) {
            if (!(Player_work.xBD & 0x20)) {
                var_a1 = D_800C0C2E;
                Player_work.x11C = D_800C0C26;
                Player_work.x11E = D_800C0C2A;
                Player_work.unk120 = (u16) D_800C0C2C;
                Player_work.unk122 = var_a1;
            } else {
                var_a1 = D_800C0F9E;
                Player_work.x11C = D_800C0F96;
                Player_work.x11E = D_800C0F9A;
                Player_work.unk120 = (u16) D_800C0F9C;
                Player_work.unk122 = var_a1;
                var_v0 = Player_work.xBD & 0x10;
                goto block_6;
            }
        } else {
            Player_work.unk11C = 0;
            Player_work.unk120 = 0;
block_6:
            if (var_v0 != 0) {
                Player_work.xC[8] = (unaligned s32) Player_work.x171[0x2C7];
                Player_work.xC[0xC] = (unaligned s32) Player_work.x171[0x2CB];
                Player_work.unk1C = (s32) Player_work.unk428;
                Player_work.unk20 = (s32) Player_work.unk42C;
                Player_work.unk24 = (s32) Player_work.unk430;
                Player_work.unk28 = (s32) Player_work.unk434;
                var_a1 = (u16) &Player_work.x171[0x2CF];
                Player_work.xC[0x48] = (unaligned s32) var_a1->unk0;
                Player_work.x58[0] = (unaligned s32) var_a1->unk4;
                Player_work.xBD &= 0xEF;
            }
        }
        func_8003C418(&Player_work, (u8 *) var_a1, Player_work.xBD);
    }
    if ((s8) Player_work2.x0[0] & Player_work2.x0[6] & 1) {
        if (!(Player_work2.xBD & 0x40)) {
            Player_work2.x11C = D_800C0FE6;
            Player_work2.x11E = D_800C0FEA;
            Player_work2.unk120 = (u16) D_800C0FEC;
            Player_work2.unk122 = (u16) D_800C0FEE;
        } else {
            Player_work2.unk11C = 0;
            Player_work2.unk120 = 0;
        }
        func_8003C418(&Player_work2);
    }
}
