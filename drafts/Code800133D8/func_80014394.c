/* func_80014394 — 118 asm insns
 * TU: src/rock_neo/Code800133D8.c
 * asm: asm/rock_neo/nonmatchings/Code800133D8/func_80014394.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

void func_80014394(void) {
    u16 temp_v0;
    void *temp_s0;
    void *temp_s0_2;

    temp_s0 = *(void **)0x1F800070;
    temp_s0->unk3 = 9;
    temp_s0->unk7 = 0x2CU;
    temp_s0->unk16 = GetTPage(0, 0, 0x140, 0x100);
    temp_s0->unkE = GetClut(0, 0x1F1);
    temp_s0->unkA = 0xB0;
    temp_s0->unk12 = 0xB0;
    temp_s0->unk1A = 0xC0;
    temp_s0->unk22 = 0xC0;
    temp_s0->unk10 = 0x108;
    temp_s0->unk20 = 0x108;
    temp_s0->unkD = 0xA0;
    temp_s0->unk15 = 0xA0;
    temp_s0->unk8 = 0x38;
    temp_s0->unk18 = 0x38;
    temp_s0->unkC = 0;
    temp_s0->unk14 = 0xD0;
    temp_s0->unk1C = 0;
    temp_s0->unk1D = 0xB0;
    temp_s0->unk24 = 0xD0;
    temp_s0->unk25 = 0xB0;
    temp_s0->unk4 = 0x80;
    temp_s0->unk5 = 0x80;
    temp_s0->unk6 = 0x80;
    temp_s0->unk7 = (u8) (temp_s0->unk7 & 0xFD);
    temp_s0_2 = temp_s0 + 0x28;
    AddPrim(&D_80098934->x78, temp_s0);
    temp_s0_2->unk3 = 9;
    temp_s0_2->unk7 = 0x2CU;
    temp_s0_2->unk16 = GetTPage(0, 0, 0x140, 0x100);
    temp_v0 = GetClut(0, 0x1F1);
    temp_s0_2->unk10 = 0x120;
    temp_s0_2->unk20 = 0x120;
    temp_s0_2->unkE = temp_v0;
    temp_s0_2->unkA = 0xD0;
    temp_s0_2->unk12 = 0xD0;
    temp_s0_2->unk8 = 0x20;
    temp_s0_2->unk18 = 0x20;
    temp_s0_2->unk1A = 0xE8;
    temp_s0_2->unk22 = 0xE8;
    temp_s0_2->unkC = 0;
    temp_s0_2->unkD = 0xB0;
    temp_s0_2->unk14 = 0xFF;
    temp_s0_2->unk15 = 0xB0;
    temp_s0_2->unk1C = 0;
    temp_s0_2->unk1D = 0xC8;
    temp_s0_2->unk24 = 0xFF;
    temp_s0_2->unk25 = 0xC8;
    temp_s0_2->unk4 = 0x80;
    temp_s0_2->unk5 = 0x80;
    temp_s0_2->unk6 = 0x80;
    temp_s0_2->unk7 = (u8) (temp_s0_2->unk7 & 0xFD);
    AddPrim(&D_80098934->x78, temp_s0_2);
    *(void **)0x1F800070 = temp_s0_2 + 0x28;
}
