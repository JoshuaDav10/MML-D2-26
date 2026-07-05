#include "common.h"
#include "rock_neo/game.h"

// clang-format off

// game.c views Code800133D8_work through CODE_800133D8_WORK (s16 x0); here it
// is zeroed as three consecutive words, so a local array view is used instead.
extern s32 Code800133D8_work[3];
void func_80012F78(void (*)());
void func_80013420();

void func_800133D8(void) {
    Code800133D8_work[0] = 0;
    Code800133D8_work[1] = 0;
    Code800133D8_work[2] = 0;
    func_80012F78(func_80013420);
}

void func_80013418(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013420);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013578);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_800135DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_800136B8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_800137D4);

void func_80013890(u8 *arg0) {
    arg0[0] = 0;
    arg0[1] = 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_8001389C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013CA0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013E0C);

void func_80013F60(GAME_WORK* arg0) {
    if (D_80098A72 == 0) {
        arg0->x1++;
    }
}

void func_80013F8C(GAME_WORK* arg0) {
    if (D_80098A72 == 0) {
        arg0->routine_0++;
        arg0->x1 = 0;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80013FB8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_80014394);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code800133D8", func_8001456C);
