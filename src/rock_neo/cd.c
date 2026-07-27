#include "common.h"

/* --- decls: parallel grind wave 2, 2026-07-26 --- */
/* --- func_8001B3E4 --- */
extern s32 CdInit(void);

extern void func_8001D2BC(u8, u8 *, u8 *);

extern u8 D_800988C0;

extern u8 D_800988DC;

extern u8 D_800988EC;

extern u8 D_80098AA0;

extern u8 D_80098A98[];

extern s32 D_80098828;

extern s32 D_80098968;

extern s32 D_80098998;

/* MOVED here from just above func_8001B858 (~line 110) — cc1-27 rejects a
   duplicate typedef, so DELETE the original two lines, do not copy them. */
typedef struct { u8 p[8]; s32 x8; } D_80098A84_t;

extern D_80098A84_t *D_80098A84;

extern void CdIntToPos(s32, u8 *);

extern void func_8001CD60();

extern s32 D_80082CD0[][3];

extern u8 D_80098814;

extern s32 D_80098828, D_80098A7C, D_8009881C, D_800987A4;

extern u8 D_80098A98_b __asm__("D_80098A98");

/* --- decls: parallel grind wave 1, 2026-07-26 --- */
/* func_8001B644 */
extern u16 D_800AD142[];
extern u8 D_800988D0;
extern u8 D_80098964;
extern u8 D_80098B42;
void func_8001D394(u8);
void func_8001CB7C(void);
void func_8001CAAC(void);

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

void func_8001B3E4(void) {
    s32 i;

    while (CdInit() == 0) {}
    D_800988DC = 0;
    D_80098998 = 0;
    D_800988C0 = 0;
    D_80098B42 = 7;
    D_800988EC = 0;
    D_80098AA0 = 0;
    D_800988DC = 0;
    func_8001D2BC(0xE, &D_800988EC, D_80098A98);
    D_80098964 = 1;
    for (i = 0xF0; i >= 0; i -= 0x10) {
        *(s32 *)&D_800A3A40[i] = 0;
    }
    D_80098A84 = (D_80098A84_t *)D_800A3A40;
    unknown_Cd_strucptr = (CD_CMD *)D_800A3A40;
    D_80098828 = 0;
    D_80098968 = 0;
    D_800989C8 = 0;
    D_800989C4 = 0;
    D_800988D0 = 0;
}

void func_8001B4C4(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001B4CC);

void func_8001B63C(void) {}

/* CD state machine step (twin of func_8001B6FC, differs only in the
   D_80098B42 value). switch on D_800989C4 with case 0 FALLING THROUGH
   into case 1 — the beqz / beq-1 / j-default ladder is the switch
   expander's sparse-case shape; an if/else chain gets jump-inverted. */
void func_8001B644(void) {
    s32 i;

    switch (D_800989C4) {
    case 0:
        for (i = 0x7F; i > 0; i--) {
            func_8001D394(i);
        }
        D_800AD142[0] |= 0x8000;
        func_8001CB7C();
        D_800988D0 = 1;
        D_800989C4 += 1;
        /* fallthrough */
    case 1:
        if (D_80098964 != 0) {
            D_80098B42 = 1;
            func_8001CAAC();
        }
        break;
    }
}

/* Twin of func_8001B644; identical except D_80098B42 = 2. */
void func_8001B6FC(void) {
    s32 i;

    switch (D_800989C4) {
    case 0:
        for (i = 0x7F; i > 0; i--) {
            func_8001D394(i);
        }
        D_800AD142[0] |= 0x8000;
        func_8001CB7C();
        D_800988D0 = 1;
        D_800989C4 += 1;
        /* fallthrough */
    case 1:
        if (D_80098964 != 0) {
            D_80098B42 = 2;
            func_8001CAAC();
        }
        break;
    }
}

extern u16 D_800AD142[];
extern u16 D_800AD146;
extern u16 D_80098994;
void func_8001CAAC(void);
void func_8001D394(u8);

void func_8001B7B4(void) {
    s32 i;

    for (i = 0x7F; i > 0; i--) {
        func_8001D394(i);
    }
    D_800AD142[0] |= 0x8000;
    if (D_800988DC != 0) {
        D_800988DC = 0;
        D_800AD146 = D_80098994;
    } else {
        u16 t = D_800AD146;
        D_800AD146 = 0;
        D_800988DC = 1;
        D_80098994 = t;
    }
    func_8001CAAC();
}

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

/* Start CD streaming for track D_80082CD0[arg0], remembering arg1 in
   D_800987A4.  Near-twin of func_8001CF98 below: same store order, minus the
   D_80098B41-down clear loop and the D_80098998 = 0, and it issues command 2
   (+ a bare command 6) instead of a single command 6, with func_8001CD60 as
   the ready callback.
   D_80098A98's address is needed TWICE here.  The file's `extern u8
   D_80098A98[]` view is an incomplete type, so -G8 calls its address
   expensive and cc1 CSEs it into $s0 (`la $s0,D_80098A98` + `addu
   $a2,$s0,$zero` per call) -- one word short and the wrong delay slot.  The
   1-byte alias below is "small" to ENCODE_SECTION_INFO, so cc1 emits an
   independent lui %hi / addiu %lo at each use, matching the target. */
void func_8001CC7C(s32 arg0, s32 arg1) {
    s32 v;

    D_800988C0 = 1;
    D_80098B42 = 7;
    D_8009896C = 0;
    D_80098828 = 0;
    D_80098964 = 0;
    v = D_80082CD0[arg0][0];
    D_80098A7C = v;
    CdIntToPos(v, &D_80098814);
    D_800987A4 = arg1;
    D_8009881C = D_80082CD0[arg0][1];
    CdReadyCallback((s32)func_8001CD60);
    func_8001D2BC(2, &D_80098814, &D_80098A98_b);
    func_8001D2BC(6, 0, &D_80098A98_b);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001CD60);

extern void func_8001D078();
extern u8 D_80098B41, D_80098B42, D_80098814;
extern s32 D_80098828, D_80098998, D_80098A7C, D_8009881C;

void func_8001CF98(s32 arg) {
    s32 i = 9;
    u8 *p = &D_80098B41;
    s32 v;
    do { *p = 0; p--; } while (--i >= 0);
    D_800988C0 = 1;
    D_80098B42 = 7;
    D_8009896C = 0;
    D_80098828 = 0;
    D_80098964 = 0;
    D_80098998 = 0;
    v = D_80082CD0[arg][0];
    D_80098A7C = v;
    CdIntToPos(v, &D_80098814);
    D_8009881C = D_80082CD0[arg][1];
    CdReadyCallback((s32)func_8001D078);
    func_8001D2BC(6, &D_80098814, D_80098A98);
}

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

extern u8 D_800AD140[];
extern u8 D_800AD154;
extern u8 D_800AD155;
extern u8 D_800AD156;
extern u8 D_800AD157;
void CdMix(u8*);

void func_8001D394(u8 arg0) {
    u8 *p = D_800AD140;
    if (D_800AD142[0] & 1) {
        D_800AD154 = arg0;
        D_800AD155 = arg0;
        D_800AD156 = arg0;
        D_800AD157 = arg0;
    } else {
        D_800AD154 = arg0;
        D_800AD155 = 0;
        D_800AD156 = arg0;
        D_800AD157 = 0;
    }
    CdMix(p + 0x14);
}

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

/* Enqueue a cmd-2 entry. When arg1 == 1, first drop every already-queued
   {cmd 2, arg1 1} entry (scan starts at queue[1] — the head is never
   touched) by shifting the tail down over it. The inner shift MUST be the
   dst[1] field-by-field shape used by func_8001CAAC: that is what keeps
   the second `dst + 1` induction variable ($t2 outer / $a3 inner) alive.
   A `*dst = dst[1]` struct copy collapses to a 4-load/4-store block move,
   loses the IV, and then clobbers $a1 (arg1) too. `end` is cached before
   the inner loop (the surviving `addu $t3, $v1, $zero`); the outer loop
   deliberately re-reads the tail pointer each iteration. */
void func_8001D494(s32 arg0, s32 arg1, s32 arg2) {
    CD_CMD *q;
    CD_CMD *dst;
    CD_CMD *end;
    CD_CMD *p;

    if (arg1 == 1) {
        for (q = (CD_CMD *)D_800A3A40 + 1; q < unknown_Cd_strucptr; q++) {
            if (q->cmd == 2 && q->arg1 == 1) {
                end = unknown_Cd_strucptr;
                for (dst = q; dst + 1 < end; dst++) {
                    dst->cmd = dst[1].cmd;
                    dst->arg0 = dst[1].arg0;
                    dst->arg1 = dst[1].arg1;
                    dst->xC = dst[1].xC;
                }
                unknown_Cd_strucptr--;
            }
        }
    }
    p = unknown_Cd_strucptr;
    p->cmd = 2;
    p->arg0 = arg0;
    p->arg1 = arg1;
    p->xC = arg2;
    unknown_Cd_strucptr = p + 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D58C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/cd", func_8001D648);

/* Enqueue command 7 (arg1 = arg0) after purging every OTHER pending
   command-7 entry from slot 1 upward. Each purge slides the tail down one
   16-byte entry and drops the write pointer.

   Shape notes:
   - the inner shift is a FIELD-by-field copy driven by the DESTINATION
     cursor (`d->cmd = d[1].cmd; ...`), the same idiom as func_8001CAAC.
     The loads then all hang off the d+1 giv ($a1) and cc1 folds three of
     the four stores into that same base as -0xC/-0x8/-0x4, leaving only the
     first store on the d biv ($a2) — the original's exact two-cursor shape.
     A whole-struct `*d = *s` emits 4 loads + 4 stores off two bases (long);
     a source-cursor copy (`s[-1] = s[0]`) drops the second cursor (short).
   - the inner bound is `unknown_Cd_strucptr` read DIRECTLY, NOT cached in a
     local: loop-invariant motion hoists the load to the preheader where cse
     collapses it to `addu $t0,$a0,$zero` against the outer loop's already
     loaded copy. A cached `end` local emits that copy one insn too early,
     which loses the bne delay slot and moves the pointer out of $a0. */
void func_8001D6D8(s32 arg0) {
    CD_CMD *e;
    CD_CMD *d;
    CD_CMD *p;

    for (e = (CD_CMD *)(D_800A3A40 + 0x10); e < unknown_Cd_strucptr; e++) {
        if (e->cmd == 7) {
            for (d = e; d + 1 < unknown_Cd_strucptr; d++) {
                d->cmd = d[1].cmd;
                d->arg0 = d[1].arg0;
                d->arg1 = d[1].arg1;
                d->xC = d[1].xC;
            }
            unknown_Cd_strucptr--;
        }
    }
    p = unknown_Cd_strucptr;
    p->cmd = 7;
    p->arg1 = arg0;
    unknown_Cd_strucptr = p + 1;
}

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
