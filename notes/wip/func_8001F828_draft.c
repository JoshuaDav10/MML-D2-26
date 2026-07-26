void func_8001F828(void) {
    s32 x = Game_work.x52;
    s16 v;

    if (x == 1) {
        goto one;
    }
    if (x < 2) {
        if (x == 0) {
            goto zerocase;
        }
        v = 0x32;
        goto other;
    }
    if (x < 6) {
        v = 0x32;
        goto mid;
    }
    v = 0x32;
    goto other;
zerocase:
    if (Sce_flag_test(0xC)) {
        D_800981BC = 0x32;
    } else {
        D_800981BC = 0x31;
    }
    return;
one:
    D_800981BC = 0x32;
    v = 0x36;
    goto tail;
mid:
    D_800981BC = v;
    v = 0x34;
    goto tail;
other:
    D_800981BC = v;
    v = 0x35;
tail:
    D_800981C2 = v;
    D_800981C0 = v;
    D_800981BE = v;
}
