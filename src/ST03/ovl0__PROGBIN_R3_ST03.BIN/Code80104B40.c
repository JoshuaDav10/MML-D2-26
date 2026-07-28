#include "common.h"

/* The per-entity "act" trampoline: unless the routine byte is the 0xFF dead
 * sentinel, call slot 1 of the vtable at 0xA4. This body appears 34 times
 * across the level files.
 *
 * Caveat: the target sets up no $a0 (the parameter is already in place), so
 * `act(work)` and `act()` are byte-identical -- the byte match does NOT prove
 * the callee takes an argument. The (work) form is kept as the sensible one.
 *
 * Mirror lives at src/ST03B/... Keep in sync.
 */
typedef struct ENTITY_FUNCS {
    void (*init)(void *);
    void (*act)(void *);
} ENTITY_FUNCS;

typedef struct ENTITY_WORK {
    u8             pad0[0x8];
    u8             routine;      /* 0x008; 0xFF = dead */
    u8             pad9[0xA4 - 0x9];
    ENTITY_FUNCS  *funcs;        /* 0x0A4 */
} ENTITY_WORK;

void func_80104B40(ENTITY_WORK *work) {
    if (work->routine != 0xFF) {
        work->funcs->act(work);
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80104B40", func_80104B80);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80104B40", func_80104CFC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80104B40", func_80104D04);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80104B40", func_80104EBC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80104B40", func_80104F1C);

void func_80104FB4(ENTITY_WORK *work) {
    if (work->routine != 0xFF) {
        work->funcs->act(work);
    }
}
