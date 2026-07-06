#include "common.h"

typedef struct {
    s32 cmd;   // 0x0 — command id (1, 4, 6 seen so far)
    s32 arg0;  // 0x4
    s32 arg1;  // 0x8
    s32 xC;    // 0xC
} CD_CMD;      // 0x10 — Cd command queue entry

extern CD_CMD *unknown_Cd_strucptr;
extern s32 D_800989C4;
extern s32 D_800989C8;
void CdSyncCallback(s32);
s32 func_80012E98(s32);
extern u8 D_800A3A40[];

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B3E4);

void func_8001B4C4(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B4CC);

void func_8001B63C(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B644);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B6FC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B7B4);

typedef struct { u8 p[8]; s32 x8; } D_80098A84_t;
extern D_80098A84_t *D_80098A84;
extern void (*D_80087670[])();

void func_8001B858(void) {
    D_80087670[D_80098A84->x8]();
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B89C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001BB4C);

void func_8001C7F0(void) {
    D_800989C4 += 1;
    CdSyncCallback(0);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001C824);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001C95C);

/* Dequeue head of the CD command queue: shift every entry down by one
   (field-by-field copy — a struct copy strength-reduces to one cursor and
   comes up 2 insns short; the field copy keeps the a1=&dst->xC second IV),
   zero the new tail's cmd, clear the two status words, drop the write
   pointer by one entry. */
void func_8001CAAC(void) {
    CD_CMD *dst = (CD_CMD *)D_800A3A40;
    if (dst->cmd != 0) {
        do {
            dst->cmd = dst[1].cmd;
            dst->arg0 = dst[1].arg0;
            dst->arg1 = dst[1].arg1;
            dst->xC = dst[1].xC;
            dst++;
        } while (dst->cmd != 0);
    }
    dst->cmd = 0;
    D_800989C8 = 0;
    D_800989C4 = 0;
    unknown_Cd_strucptr--;
}

extern u8 D_8009896C;
extern u8 D_80098A98[];
extern u16 D_800AD142[];
extern u8 D_800988D0;
extern u8 D_80098AB8;
void func_8001D394(u8);
void CdReadyCallback(s32);
void func_8001CC08();
void func_8001D254(u8, u8 *, u8 *);

void func_8001CB30(void) {
    D_8009896C = 0;
    CdReadyCallback(0);
    CdSyncCallback((s32)func_8001CC08);
    func_8001D254(9, 0, D_80098A98);
}

/* CD retry/re-arm: pulse func_8001D394 0x7F..1, flag D_800AD142 |= 0x8000
   (array decl -> single materialized address for the read+write), re-arm
   the ready/sync callbacks and re-issue command 9. */
void func_8001CB7C(void) {
    s32 i;
    for (i = 0x7F; i > 0; i--) {
        func_8001D394(i);
    }
    D_8009896C = 0;
    D_800AD142[0] |= 0x8000;
    CdReadyCallback(0);
    CdSyncCallback((s32)func_8001CC08);
    func_8001D254(9, 0, D_80098A98);
    D_800988D0 = 0;
    D_80098AB8 = 0;
}

extern u8 D_80098964;
extern u8 D_800988C0;

void func_8001CC08(r) /* K&R: the forward decl above is unprototyped */
u8 r;
{
    CdSyncCallback(0);
    if (r == 2) {
        D_80098964 = 1;
        D_800988C0 = 0;
    } else {
        D_8009896C |= 8;
        D_800988C0 = 0x80;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CC7C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CD60);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CF98);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D078);

s32 CdSync(s32, u8 *);
s32 CdControl(u8, u8 *, u8 *);
s32 CdControlB(u8, u8 *, u8 *);

void func_8001D254(u8 com, u8 *param, u8 *result) {
    while (CdSync(1, result) == 0) {}
    while (CdControl(com, param, result) == 0) {}
}

void func_8001D2BC(u8 com, u8 *param, u8 *result) {
    while (CdSync(1, result) == 0) {}
    while (CdControlB(com, param, result) == 0) {}
}

extern u8 D_800988EC;
extern s32 D_80098828;

void func_8001D324(u8 com) {
    u8 old = D_800988EC;
    D_80098828 = 0;
    if (old != com) {
        if ((old & 0x80) != (com & 0x80)) {
            D_80098828 = 2;
        }
        D_800988EC = com;
        func_8001D2BC(0xE, &D_800988EC, D_80098A98);
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D394);

void func_8001D414(s32 arg0, s32 arg1) {
    CD_CMD *p = unknown_Cd_strucptr;
    p->cmd = 4;
    p->arg0 = arg0;
    p->arg1 = arg1;
    unknown_Cd_strucptr = p + 1;
}

void Cd_read_comb(s32 arg0) {
    CD_CMD *p = unknown_Cd_strucptr;
    p->cmd = 6;
    p->arg0 = arg0;
    unknown_Cd_strucptr = p + 1;
}

void func_8001D468(s32 arg0, s32 arg1) {
    CD_CMD *p = unknown_Cd_strucptr;
    p->cmd = 1;
    p->arg0 = arg0;
    p->arg1 = arg1;
    unknown_Cd_strucptr = p + 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D494);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D58C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D648);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D6D8);

void func_8001D6D8(s32);
extern s32 D_80082CD0[][3];

void func_8001D7AC(s32 idx) {
    func_8001D6D8(D_80082CD0[idx][0]);
}

void func_8001D7E4(void) {
    while (unknown_Cd_strucptr != (CD_CMD *)D_800A3A40) {
        func_80012E98(1);
    }
}

s32 Cd_read_sync2(void) {
    return unknown_Cd_strucptr != (CD_CMD *)D_800A3A40;
}
