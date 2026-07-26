void func_800137D4(void) {
    u8 *ot;
    u8 *dc;
    s32 n;

    func_800175D8();
    func_80015428(Code800133D8_work);
    func_80016FD4();
    func_80038370(D_800979DC, D_800979EC);
    func_800175FC();
    func_80028DE0();

    n = (*(u8 *)0x1F800000) << 14;
    dc = (u8 *)D_80098934;
    ot = D_800A4D40 + n;
    *(s32 *)ot = (*(s32 *)ot & 0xFF000000) | (*(s32 *)(dc + 0x80) & 0xFFFFFF);
    *(s32 *)(dc + 0x80) =
        (*(s32 *)(dc + 0x80) & 0xFF000000) | ((u32)(D_800A4D40 + n + 0x3FFC) & 0xFFFFFF);
}
