/* idk_Init_system_maybe — 105 asm insns
 * TU: src/rock_neo/main.c
 * asm: asm/rock_neo/nonmatchings/main/idk_Init_system_maybe.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? InitGeom();                                       /* extern */
? InitPAD(u8 *, ?, u8 *, ?);                        /* extern */
? ResetCallback();                                  /* extern */
? StartPAD();                                       /* extern */
? VSyncCallback(s32 (*)());                         /* extern */
? func_800120A8();                                  /* extern */
? func_80012BC4();                                  /* extern */
? func_800177CC(u8 *, u8 *);                        /* extern */
? func_80019DE0();                                  /* extern */
? func_8001B3E4();                                  /* extern */
? func_8002037C();                                  /* extern */
? func_8006479C();                                  /* extern */
? func_80077748(?);                                 /* extern */
? func_8007B118(?, ?);                              /* extern */
? func_8007B138(?);                                 /* extern */
s16 rcos(s32);                                      /* extern */
s16 rsin(s32);                                      /* extern */
extern s32 D_80098824;
extern s32 D_80098864;
extern s8 D_80098A8C;
extern ? D_800BAEB0;
extern ? D_800BBCF8;
extern s32 D_800C0C34;
extern UnkStruc_80098934 D_800C0C48;
extern s32 D_800C0D9C;
extern u8 D_800C0FC0;
extern s32 D_800D9B00;

void idk_Init_system_maybe(void) {
    s16 temp_v0;
    s32 temp_s0;
    s32 var_s1;

    ResetCallback();
    func_8001B3E4();
    ResetGraph(3);
    D_80098A8C = 1;
    func_80077748(0);
    InitGeom();
    func_8007B118(0xA0, 0x78);
    func_8007B138(0x180);
    func_800177CC(D_800C0C00, D_800C0C00 + 0x4C8);
    func_800177CC(D_800C0D68, D_800C0D68 + 0x4C8);
    func_800177CC(&D_800C0FC0, &D_800C0FC0 + 0x4C8);
    InitPAD(D_800C0C00, 8, D_800C0D68, 8);
    D_800C0C34 = 0x4001;
    D_800C0D9C = 0x4001;
    var_s1 = 0;
    StartPAD();
    func_8006479C();
    func_80019DE0();
    ChangeClearPAD(0);
    func_800120A8();
    func_80012BC4();
    D_80098934 = &D_800C0C48;
    D_800D9B00 = 0;
    *(s8 *)0x1F800000 = 0;
    *(s8 *)0x1F800001 = 2;
    VSyncCallback(vsync_cb);
    D_80098824 = 0;
    D_80098864 = 0;
    do {
        temp_s0 = (var_s1 >> 6) * 2;
        *(temp_s0 + &D_800BAEB0) = rcos(var_s1);
        temp_v0 = rsin(var_s1);
        var_s1 += 0x40;
        *(temp_s0 + &D_800BBCF8) = temp_v0;
    } while (var_s1 < 0x1000);
    func_8002037C();
}
