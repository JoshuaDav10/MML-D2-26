void func_8001DDE4(s32 arg0) {
    s32 i;

    if (*(s16 *)&Player_work[0x6E] < 0) {
        return;
    }
    Scene_work.x0 = 1;
    Scene_work.x9 = 0;
    Scene_work.x8 = 0;
    Scene_work.x1 = arg0;
    Scene_work.x10 = 0;
    *(s32 *)((u8 *)&Scene_work + 0xC) = 0;
    Scene_work.x1C = 0;
    Scene_work.x18 = 0;
    *(s32 *)((u8 *)&Scene_work + 0x14) = 0;
    *(s32 *)((u8 *)&Scene_work + 0x4) = 0;
    *((u8 *)&Scene_work + 0xB) = 0;
    *((u8 *)&Scene_work + 0xA) = 0;
    Scene_work.xA4 = 0;
    for (i = 0xF; i >= 0; i--) {
        Scene_work.x24[i] = 0;
    }
}
