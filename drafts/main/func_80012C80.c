/* func_80012C80 — 100 asm insns
 * TU: src/rock_neo/main.c
 * asm: asm/rock_neo/nonmatchings/main/func_80012C80.s
 * m2c: 94098d4de68c2fcc13fb8cf1096a1520eb171abe
 * UNTRUSTED DRAFT — never copy into src/ without byte-match iteration
 */

u32 OpenTh(s32, s32, s32);                          /* extern */
? ResetCallback();                                  /* extern */
? StopCallback(u16 *);                              /* extern */
? VSyncCallback(s32 (*)());                         /* extern */

void func_80012C80(void) {
    u16 temp_v0_2;
    u16 temp_v1;
    void *temp_v0;
    void *temp_v0_3;
    void *temp_v0_4;

    D_801F8300 = (u16 *)0x801F8100;
    do {
        temp_v1 = D_801F8300->unk0;
        switch (temp_v1) {                          /* irregular */
        case 0x1:
            temp_v0_2 = D_801F8300->unk2 - 1;
            D_801F8300->unk2 = temp_v0_2;
            if ((temp_v0_2 << 0x10) == 0) {
            case 0x2:
            case 0x4:
            case 0x7F:
                if ((*(void **)0x801F8300)->unk70 != 0) {
                    StopCallback(D_801F8300);
                }
                temp_v0_3 = *(void **)0x801F8300;
                temp_v0_3->unk0 = 0x7F;
                ChangeTh(temp_v0_3->unk8);
                if ((*(void **)0x801F8300)->unk70 != 0) {
                    ResetCallback();
                    VSyncCallback(vsync_cb);
                    (*(void **)0x801F8300)->unk70 = 0U;
                }
                if (D_800979D8 != 0) {
                    D_800979D8 = 0;
                    func_8007FF70();
                    CloseTh((s32) (*(void **)0x801F8300)->unk8);
                    temp_v0_4 = *(void **)0x801F8300;
                    (*(void **)0x801F8300)->unk8 = OpenTh(D_80098158, temp_v0_4->unk10, temp_v0_4->unk44);
                    func_8007FF80();
                }
            }
            break;
        }
        temp_v0 = *(void **)0x801F8300 + 0x80;
        *(void **)0x801F8300 = temp_v0;
    } while ((u32) temp_v0 <= 0x801F82FFU);
}
