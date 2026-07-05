/* func_80019DE0 — 109 asm insns
 * TU: src/rock_neo/sound.c
 * asm: asm/rock_neo/nonmatchings/sound/func_80019DE0.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

? SpuSetEnv(s32 *, ?);                              /* extern */
? SsSetSerialAttr(?, ?, ?);                         /* extern */
? SsSetSerialVol(?, ?, ?);                          /* extern */
? SsSetTableSize(? *, ?, ?);                        /* extern */
? SsSetTickMode(?);                                 /* extern */
? SsStart();                                        /* extern */
? func_8006B594();                                  /* extern */
extern s8 D_80098788;
extern s8 D_80098840;
extern s16 D_800AD144;
extern s16 D_800AD146;
extern s16 D_800AD148;
extern s16 D_800AD14A;
extern s16 D_800AD14C;
extern s16 D_800AD14E;
extern s16 D_800AD150;
extern s16 D_800AD152;
extern s8 D_800AD158;
extern s8 D_800AD159;
extern s8 D_800AD15A;
extern s32 D_800AD160;
extern SND_CMD D_800BE4F8;
extern ? D_800C1010;
extern ? Player_work;
extern ? *p_Player_work;

void func_80019DE0(void) {
    s32 sp10;
    s32 sp14;
    s32 var_v1;
    s32 var_v1_2;

    func_8006B594();
    SsSetTableSize(&D_800C1010, 4, 4);
    SsSetTickMode(1);
    D_80098938 = &D_800BE4F8;
    D_800AD148 = 0x40;
    D_800AD14A = 0x64;
    D_80098840 = 4;
    D_80098788 = 0;
    D_80098958 = 0;
    D_800AD144 = 0x7F;
    D_800AD146 = 0x7F;
    D_800AD152 = 0x7F;
    D_800AD150 = 0x7F;
    D_800AD14E = 0x7F;
    D_800AD14C = 0x7F;
    D_800AD158 = 0xF;
    D_800AD159 = 0;
    SsSetSerialAttr(0, 0, 1);
    SsSetSerialVol(0, 0x7F, 0x7F);
    var_v1 = 0;
    D_800AD15A = 0;
    D_800AD160 = 0;
    *D_800AD15C = D_800821F8;
    do {
        (Game_work + 0x1B8)[var_v1] = -1;
        (Game_work + 0x1BA)[var_v1] = -1;
        var_v1 += 8;
    } while (var_v1 < 0x28);
    var_v1_2 = 0;
    do {
        D_80098AFA[var_v1_2] = -1;
        D_80098AD0[var_v1_2] = 0xFF;
        var_v1_2 += 8;
    } while (var_v1_2 < 0x20);
    p_Player_work = &Player_work;
    sp10 = 1;
    sp14 = 0;
    SpuSetEnv(&sp10, -1);
    SsStart();
    D_80098958 |= 1;
}
