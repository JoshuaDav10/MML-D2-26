/* main — 157 asm insns
 * TU: src/rock_neo/main.c
 * asm: asm/rock_neo/nonmatchings/main/main.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

s32 VSync(?);                                       /* extern */
? __main();                                         /* extern */
? func_80012C80(? *);                               /* extern */
? func_80017734();                                  /* extern */
? func_800178FC();                                  /* extern */
? func_80017B24();                                  /* extern */
? func_8001A110();                                  /* extern */
? func_8001C95C();                                  /* extern */
? func_800203A8();                                  /* extern */
? func_80053B40();                                  /* extern */
? func_80064060();                                  /* extern */
? idk_Init_system_maybe();                          /* extern */
extern ? D_800806EC;
extern s32 D_80098810;
extern s32 D_80098864;
extern u8 D_80098A8C;
extern s32 D_80098A90;
extern ? D_8009AE48;
extern ? D_8009BF88;
extern ? D_800A4D40;
extern ? D_800C0C48;
extern s32 D_800D9BCC;
extern s32 D_800D9BD0;

void main(void) {
    UnkStruc_80098934 *temp_v0_2;
    s32 temp_v0;
    u8 temp_a2;

    __main();
    if (D_80097864 == 0) {
        func_80012938();
    }
    idk_Init_system_maybe();
    func_80012E10(0, (s32) func_800131FC);
loop_3:
    do {

    } while ((u32) idk_framecounter_maybe < (u8) *(u8 *)0x1F800001);
    idk_framecounter_maybe = 0;
    temp_v0 = VSync(1);
    D_800D9BD0 = temp_v0;
    D_800D9BCC = temp_v0;
    D_80098A90 = DrawSync(1);
    DrawSync(0);
    ResetGraph(1);
    if (D_80098A8C == 0) {
        PutDispEnv((DISPENV *) D_80098934);
        PutDrawEnv((DRAWENV *) &D_80098934->x4[0x10]);
    }
    func_80017734();
    if (D_80098810 == 0) {
        DrawOTag(D_80098934 + 0x8C);
    }
    func_80012298();
    temp_a2 = *(u8 *)0x1F800000 ^ 1;
    temp_v0_2 = (temp_a2 * 0x90) + &D_800C0C48;
    *(u8 *)0x1F800000 = temp_a2;
    D_80098934 = temp_v0_2;
    *(void **)0x1F80003C = (temp_a2 << 0xE) + &D_800A4D40;
    ClearOTagR((u32 *) &temp_v0_2->x4[0x6C], 8);
    ClearOTagR((*(u8 *)0x1F800000 << 0xE) + &D_800A4D40, 0x1000);
    ClearOTagR((*(u8 *)0x1F800000 << 0xB) + &D_8009BF88, 0x200);
    ClearOTagR((*(u8 *)0x1F800000 << 0xB) + &D_8009AE48, 0x200);
    *(s32 *)0x1F800070 = *((*(u8 *)0x1F800000 * 4) + ((D_80098864 * 8) + &D_800806EC));
    func_80012C80(&D_800806EC);
    func_8001A110();
    func_8001C95C();
    func_800203A8();
    func_80053B40();
    func_80064060();
    DrawSync(0);
    func_80017B24();
    func_800178FC();
    if (D_80097864 != 0) {
        M2C_BREAK(1);
    }
    *(u16 *)0x1F800002 += 1;
    goto loop_3;
}
