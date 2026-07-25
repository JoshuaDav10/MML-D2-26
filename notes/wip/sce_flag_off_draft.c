s32 Sce_flag_off(s32 flagno) {
    u8 *base = Sce_flag;
    u8 *fp = base + ((u32)flagno >> 3);
    s32 mask = 0x80 >> (flagno & 7);
    u8 *p;
    u8 *e;
    s32 t;

    *fp &= ~mask;
    if ((u32)(flagno - 0x500) < 0x20) {
        t = flagno - 0x4FF;
        p = &Player_work[0x454];
        e = p + 0x20;
    l1:
        if (*p == t) {
            *p = 0;
            goto l2s;
        }
        p++;
        if (p < e) goto l1;
    l2s:
        t = flagno - 0x4FF;
        p = &Player_work[0x450];
        e = p + 3;
    l2:
        if (*p == t) {
            *p = 0;
            goto done;
        }
        p++;
        if (p < e) goto l2;
    }
done:
    ;
}
