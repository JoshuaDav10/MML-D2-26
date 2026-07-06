#include "common.h"
extern void func_8001D2BC(u8, u8 *, u8 *);
extern void CdReadyCallback(s32);
extern void CdIntToPos(s32, u8 *);
extern void func_8001D078();
extern s32 D_80082CD0[][3];
extern u8 D_80098B41, D_80098B42, D_800988C0, D_8009896C, D_80098964, D_80098814;
extern s32 D_80098828, D_80098998, D_80098A7C, D_8009881C;
extern u8 D_80098A98[];

void func_8001CF98(s32 arg) {
    s32 i = 9;
    u8 *p = &D_80098B41;
    do { *p = 0; p--; } while (--i >= 0);
    D_800988C0 = 1;
    D_80098B42 = 7;
    D_8009896C = 0;
    D_80098828 = 0;
    D_80098964 = 0;
    D_80098998 = 0;
    D_80098A7C = D_80082CD0[arg][0];
    CdIntToPos(D_80082CD0[arg][0], &D_80098814);   /* CdIntToPos(int, CdlLOC*) */
    D_8009881C = D_80082CD0[arg][1];               /* cc1 DUPLICATES this load/store (once before the call); needs -dc dump */
    CdReadyCallback((s32)func_8001D078);
    func_8001D2BC(6, &D_80098814, D_80098A98);
}
