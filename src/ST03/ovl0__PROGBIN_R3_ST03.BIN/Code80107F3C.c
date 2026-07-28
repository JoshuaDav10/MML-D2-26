#include "common.h"

/* ST03 ovl0, high-duplicate cluster (64 functions, no jump tables).
 * These bodies recur across the whole game -- func_80107F3C alone has 144
 * copies in 33 level files -- so the C here is written to be copy-pasted.
 *
 * WORK is the per-object work struct; only the fields this file touches are
 * typed. Signed/unsigned width is load-bearing: a 16-bit field tested after
 * arithmetic emits `sll 16` when s16 and `andi 0xffff` when u16.
 *
 * Mirror of this file lives at src/ST03B/... (identical code chunk). Keep in sync.
 */
typedef struct WORK {
    u8  pad0[0xA];
    u8  xA;                      /* 0x0A - routine/step byte */
    u8  padB[0xA7 - 0xB];
    s8  xA7;                     /* 0xA7 - signed state flag */
    u8  padA8[0x350 - 0xA8];
    s16 x350;                    /* 0x350 - countdown timer (signed: sll 16) */
} WORK;

void func_80031824(void);

void func_80107F3C(void) {
    func_80031824();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80107F5C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80107F98);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108008);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010807C);

void func_801080E4(WORK *p) {
    if (p->xA7 < 0) {
        p->xA = 0;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108100);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010813C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108194);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801082F8);

void func_801083DC(WORK *p) {
    if (--p->x350 == 0) {
        p->xA = 1;
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108404);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108480);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801084FC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108560);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108648);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108704);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801087A8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801087EC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108828);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010885C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801089C8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108B9C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108CCC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108CF4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108D9C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108E28);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108E64);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80108EC0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801090E8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109288);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801092B0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109358);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801093BC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010940C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109448);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109534);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109608);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801096AC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109730);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801097F4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109830);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010991C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_801099F0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109AB4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109AF0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109C44);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109C94);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109CF8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109D34);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109E94);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109F1C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_80109FD4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A02C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A100);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A13C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A224);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A2A4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A344);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A3AC);

s32 func_8010A44C(u8 a) {
    return a == 0x11;
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A45C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A49C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code80107F3C", func_8010A4EC);
