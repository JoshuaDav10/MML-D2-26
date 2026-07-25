s32 func_80042154(PL_WORK *pl, s32 step, s32 limit, s32 keymask) {
    u8 pad[16]; /* dead frame space, present in the original */
    s32 ret;
    s32 v;

    (void)&pad;

    if (pl->x11C & keymask) {
        ret = 1;
        v = *(s16 *)&pl->xB4;
        if (v < 0) {
            goto zero;
        }
        v = v + step;
        *(s16 *)&pl->xB4 = v;
        if (limit < (s16)v) {
            *(s16 *)&pl->xB4 = limit;
        }
        *(s16 *)((u8 *)pl + 0xB6) = step;
    } else {
        ret = 2;
        v = *(s16 *)&pl->xB4;
        if (v > 0) {
            goto zero;
        }
        v = v - step;
        *(s16 *)&pl->xB4 = v;
        if ((s16)v < -limit) {
            *(s16 *)&pl->xB4 = -limit;
        }
        *(s16 *)((u8 *)pl + 0xB6) = -step;
    }
    goto tail;
zero:
    *(s16 *)&pl->xB4 = 0;
tail:
    pl->x56 = (pl->x56 + pl->xB4) & 0xFFF;
    return ret;
}
