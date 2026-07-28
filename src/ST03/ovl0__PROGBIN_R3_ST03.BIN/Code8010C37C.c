#include "common.h"

/* ST03 ovl0, second high-duplicate cluster (119 functions).
 * 7 functions in this range use jump tables and MUST stay INCLUDE_ASM: a C
 * file's .rodata can only be a TAIL of the rodata section, so C-ifying a switch
 * here would shift every jump table after it.
 *
 * The tail-call wrappers below are the single most duplicated shape in the game:
 * the func_800318D8 one appears 6x in ST03 and 60x across all level files.
 * `void f(void)` and `void f(WORK *p) { callee(p); }` are byte-indistinguishable
 * here ($a0 is already in place), so the parameter is not recoverable from the
 * bytes; the void form is used for consistency.
 *
 * Mirror lives at src/ST03B/... (identical code chunk). Keep in sync.
 */
void func_800318D8(void);
void func_80031988(void);

/* Merged from the three handlers in this cluster. NOTE: the 0x38 identifier
 * block is declared 8 bytes, not 0x10 -- only its OFFSET matters for codegen
 * (it is just passed as a pointer), and 0x10 would overlap unk46 at 0x46. */
typedef struct ENEMY_WORK {
    u8      pad0[0x8];
    u8      routine;             /* 0x008 */
    u8      pad9[0xB - 0x9];
    u8      step;                /* 0x00B */
    s8      timer;               /* 0x00C frame counter */
    u8      padD[0x38 - 0xD];
    u8      name[0x8];           /* 0x038 identifier block (passed by address) */
    u8      pad40[0x46 - 0x40];
    s16     unk46;               /* 0x046 */
    u8      pad48[0x54 - 0x48];
    s16     speed;               /* 0x054 clamped to 0x400 */
    s16     angle;               /* 0x056 facing (12-bit) */
    u8      pad58[0x66 - 0x58];
    s16     targetAngle;         /* 0x066 */
} ENEMY_WORK;

extern s32  rand(void);
extern void func_80032538(ENEMY_WORK *);
extern void func_800334C4(ENEMY_WORK *);
extern s32  func_80048C60(void *);
extern s32  Sce_flag_test(s32);
extern u8   D_800B51C4[];

void func_8010C37C(ENEMY_WORK *work, s32 enable) {
    if (enable) {
        work->timer++;
        if (work->timer >= 0x3D) {
            work->timer = 0;
            /* The three summands must stay FLAT and left-to-right with the
             * constant in the middle -- parenthesising the rand term attaches
             * the addiu to the angle register instead. See LESSONS. */
            work->targetAngle = (work->angle + 0x7D0 + (rand() & 0x3F)) & 0xFFF;
            work->routine = 8;
        }
    }
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C3F0);

void func_8010C460(ENEMY_WORK *work) {
    work->unk46 += 4;
    work->speed += 0x20;
    if (work->speed > 0x400) {
        work->speed = 0x400;
    }
    func_80032538(work);
    func_800334C4(work);
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C4C4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C560);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C5F4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C66C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C6A8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C7E8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C934);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C954);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010C974);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010CAB8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010CB4C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010CC94);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010CD68);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D020);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D0B4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D0F4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D130);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D268);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D3C4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D3E4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D400);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D4C0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D5EC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D730);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D7C4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D968);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010D9A4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010DAE8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010DC34);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010DC54);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010DC74);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010DE18);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E014);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E0A8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E198);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E300);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E3A8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E4D8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E5C8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E6C8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E720);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E800);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E820);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E8FC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010E950);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010EAB4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010EC34);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010EC54);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010EFB0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F060);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F238);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F248);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F474);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F6B8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F734);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F7EC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F878);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010F8E8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FA58);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FAE4);

void func_8010FB18(void) {
    func_80031988();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FB38);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FB74);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FCBC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FD1C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FE24);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8010FF4C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8011006C);

void func_80110128(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80110148);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8011027C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80110284);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801103F0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801104C0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801105B8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801106B8);

void func_80110768(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80110788);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801107FC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80110BEC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80110C28);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80110D70);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80110E88);

void func_80111050(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111070);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801111A4);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8011121C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_8011137C);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111524);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111930);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111A50);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111ABC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111B64);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111CAC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111D64);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111EBC);

void func_80111F78(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80111F98);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112098);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112190);

void func_80112220(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112240);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112300);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112324);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112348);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112448);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801125F8);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801128FC);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_801129E8);

void func_80112AF8(void) {
    func_800318D8();
}

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112B18);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112C94);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112CD0);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112F88);

INCLUDE_ASM("config/overlay/splat.us.ST03/../../../asm/ST03/ovl0__PROGBIN_R3_ST03.BIN/nonmatchings/Code8010C37C", func_80112FF4);

void func_8011311C(ENEMY_WORK *work) {
    if (func_80048C60(work->name) == func_80048C60(D_800B51C4)) {
        if (!Sce_flag_test(0x7B1)) {
            work->routine++;
            work->step = 0;
        }
    }
}
