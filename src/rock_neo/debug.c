#include "common.h"
#include "rock_neo/scene.h"
#include "rock_neo/joy.h"

extern s32 Debug_work[];
extern void (*D_8008DBD4[])();

void func_800629E0(void) {
    Debug_work[0] = 0;
}

void func_800629F0(void) {
    u16 a = D_800C0C2A;
    u16 b = D_800C0C26;
    s32 i = *(s8 *)0x800C4C18; /* Debug_work.x8 — raw address keeps the lb
                                  independent of the call arg (no CSE) */

    /* stores through the NEIGHBOR symbol (Scene_work = Debug_work+0x38):
       symbol MEM pins the fn-table load below the stores (raw addresses
       get hoisted past), and a symbol distinct from the Debug_work arg
       defeats cc1's anchor-CSE (arg = store_addr - k). Bytes identical. */
    ((u16 *)&Scene_work)[-26] = a; /* Debug_work.x4 */
    ((u16 *)&Scene_work)[-25] = b; /* Debug_work.x6 */
    D_8008DBD4[i](Debug_work);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/debug", func_80062A50);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/debug", func_80062B04);

extern void (*D_8008DBE0[])();

s32 func_80062C6C(void *arg0) {
    void (**fp)();
    s32 i;
    *(s32 *)((u8 *)arg0 + 0x0) = 0;
    *(s32 *)((u8 *)arg0 + 0x8) = 0;
    *(s32 *)((u8 *)arg0 + 0xC) = 0;
    ((u8 *)arg0)[0x10] = 3;
    ((u8 *)arg0)[0x18] = 3;
    ((u8 *)arg0)[0x20] = 3;
    ((u8 *)arg0)[0x28] = 3;
    ((u8 *)arg0)[0x30] = 3;
    i = 0;
    fp = D_8008DBE0;
    do {
        (*fp++)(arg0);
    } while (++i < 5);
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/debug", func_80062CF0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/debug", func_80063058);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/debug", func_80063334);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/debug", func_80063604);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/debug", func_800638D8);
