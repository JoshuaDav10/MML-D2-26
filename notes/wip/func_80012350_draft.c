void func_80012350(s32 p, s32 mask, s32 off) {
    u8 *q;
    u16 v;

    if (!(*(u16 *)(p + 0x2E) & mask) && (*(u16 *)(p + 0x2A) & mask) == mask) {
        q = (u8 *)(p + off);
        v = q[0x38];
        q[0x40] = 0;
        if (v == 0) {
            q[0x38] = 1;
            return;
        }
        q[0x38] = 2;
        v = *(u16 *)(p + 0x2E) | mask;
    } else {
        q = (u8 *)(p + off);
        v = q[0x38];
        if (v == 0) {
            return;
        }
        if (v != 2) {
            u8 c = q[0x40];
            q[0x40] = c + 1;
            if (c < 6) {
                return;
            }
        }
        q[0x38] = 0;
        v = *(u16 *)(p + 0x2E) & ~mask;
    }
    *(u16 *)(p + 0x2E) = v;
}
