#include "common.h"

/* ST03 ovl0, first C segment: a 32-function run of scenario-event handlers.
 * rom = vram - 0x800FF800 (overlay constant; the main exe uses 0x8000F800).
 *
 * Field 0x4 is an "already fired" latch: each handler runs its body once, then
 * writes 0xFF. Named EVE_WORK after ST1A/eve19, whose main_dummy body hash is
 * identical to ST03's func_801140F8, so this is very likely Capcom's struct.
 * Only the fields this segment touches are typed.
 *
 * The code chunk of ST03.BIN and ST03B.BIN is byte-identical, so
 * src/ST03B/.../Code80101064.c is a mechanical copy of this file with the
 * ST03 -> ST03B path tokens substituted. Keep the two in sync.
 */
typedef struct EVE_WORK {
    u8  pad0[0x4];
    u8  x4;       /* 0x04 - "already fired" latch, set to 0xFF */
} EVE_WORK;

/* --- ST03-internal --- */
void func_80103AB0(s32 idx);
void func_80101B70(void);

/* --- engine --- */
s32  Sce_flag_test(s32 id);
void Sce_flag_on(s32 id);
u8  *func_80031418(void);
void func_8001EB98(u8 *p);

extern u8 D_801151DC[];

void func_80101064(EVE_WORK *evp) {
    u8 *p;

    if (evp->x4 == 0) {
        p = func_80031418();
        if (p != 0) {
            p[0] = 1;
            p[2] = 9;
            p[4] = 2;
        }
        evp->x4 = 0xFF;
    }
}

void func_801010C8(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x13);
        func_80103AB0(0x25);
        evp->x4 = 0xFF;
    }
}

void func_80101114(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x15);
        evp->x4 = 0xFF;
    }
}

void func_80101158(EVE_WORK *evp) {
}

void func_80101160(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x13);
        func_80103AB0(0x25);
        evp->x4 = 0xFF;
    }
}

void func_801011AC(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x15);
        evp->x4 = 0xFF;
    }
}

void func_801011F0(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x17);
        evp->x4 = 0xFF;
    }
}

void func_80101234(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x18);
        evp->x4 = 0xFF;
    }
}

void func_80101278(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x19);
        evp->x4 = 0xFF;
    }
}

void func_801012BC(EVE_WORK *evp) {
}

void func_801012C4(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x13);
        evp->x4 = 0xFF;
    }
}

void func_80101308(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x15);
        evp->x4 = 0xFF;
    }
}

void func_8010134C(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x1A);
        evp->x4 = 0xFF;
    }
}

void func_80101390(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x1B);
        evp->x4 = 0xFF;
    }
}

void func_801013D4(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x1C);
        evp->x4 = 0xFF;
    }
}

void func_80101418(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x1D);
        evp->x4 = 0xFF;
    }
}

void func_8010145C(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x1E);
        evp->x4 = 0xFF;
    }
}

void func_801014A0(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x1F);
        evp->x4 = 0xFF;
    }
}

void func_801014E4(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x20);
        evp->x4 = 0xFF;
    }
}

void func_80101528(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x21);
        evp->x4 = 0xFF;
    }
}

void func_8010156C(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x22);
        evp->x4 = 0xFF;
    }
}

void func_801015B0(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x23);
        evp->x4 = 0xFF;
    }
}

void func_801015F4(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x24);
        evp->x4 = 0xFF;
    }
}

void func_80101638(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x22);
        evp->x4 = 0xFF;
    }
}

void func_8010167C(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x23);
        evp->x4 = 0xFF;
    }
}

void func_801016C0(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        func_80103AB0(0x24);
        evp->x4 = 0xFF;
    }
}

/* PARKED: the x4==0xFF arm returns with $v0 still holding the 0xFF
 * comparison constant, and the li $v0,1 in the beq delay slot is dead.
 * No clean source shape reproduces that; revisit with ./diff.py. */
INCLUDE_ASM("config/overlay/splat.us.ST03B/../../../asm/ST03B/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80101064", func_80101704);

s32 func_80101778(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        if (Sce_flag_test(0x235)) {
            Sce_flag_on(0x7E2);
            Sce_flag_on(0x7E3);
            Sce_flag_on(0x7E4);
            Sce_flag_on(0x7E8);
        }
    }
    Sce_flag_on(0x1FF);
    return 0;
}

s32 func_801017D8(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        if (Sce_flag_test(0x235)) {
            Sce_flag_on(0x7E2);
            Sce_flag_on(0x7E3);
            Sce_flag_on(0x7E4);
            Sce_flag_on(0x7E8);
        }
    }
    Sce_flag_on(0x1FF);
    return 0;
}

s32 func_80101838(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        if (Sce_flag_test(0x238)) {
            if (Sce_flag_test(0x239)) {
                func_80101B70();
            } else {
                Sce_flag_on(0x239);
                func_8001EB98(D_801151DC);
            }
        }
    }
    Sce_flag_on(0x1FF);
    return 0;
}

s32 func_801018B0(EVE_WORK *evp) {
    if (evp->x4 == 0) {
        if (Sce_flag_test(0x239)) {
            func_80101B70();
        }
    }
    Sce_flag_on(0x1FF);
    return 0;
}

s32 func_801018F8(void) {
    Sce_flag_on(0x1FF);
    return 0;
}
