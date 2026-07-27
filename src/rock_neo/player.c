#include "common.h"
#include "rock_neo/player.h"
#include "rock_neo/game.h"
#include "rock_neo/scene.h"
#include "rock_neo/sound.h"

/* --- decls: parallel grind wave 2, 2026-07-26 --- */
/* func_8003D124 — forward proto; the ANSI definition is later in this same
   file (src/rock_neo/player.c, `void func_80041E90(PL_WORK *pl, s16 x, s16 y,
   s16 z)`), and no header declares it. */
void func_80041E90(PL_WORK*, s16, s16, s16);

/* func_80042094 calls func_80042154, which is defined later in this file
   (player.c:743) with exactly this signature. Place this prototype anywhere
   above func_80042094. */
s32 func_80042154(PL_WORK *pl, s32 a1, s32 a2, s32 a3);

/* func_8003C418 */
extern void (*D_80089F94[])();

s32 func_8004324C(PL_WORK*);

void func_800321C8(PL_WORK*, s32, s32, s32);

void func_800323FC(PL_WORK*, s32);

void func_800320B8(PL_WORK*, s32, s32);

void func_8003234C(PL_WORK*);

/* forward decl for the definition further down player.c (line ~723);

func_8003D960 sits above it and needs the prototype. */
void func_80041E90(PL_WORK *pl, s16 x, s16 y, s16 z);

/* func_80041AB0: block-copy helper types. PL_BLK8 has alignment 1 (all-u8),
   which is what makes cc1 emit the lwl/lwr + swl/swr pairs; PL_BLK16 has
   alignment 4 and gives the plain lw/sw quartet. Do not change the member
   types -- the alignment IS the codegen. */
typedef struct { u8 b[8]; } PL_BLK8;

typedef struct { s32 w[4]; } PL_BLK16;

void func_8004B414(PL_WORK*);

/* func_8003D6DC */
void func_80042044(PL_WORK*, s32, s32);

/* --- decls: parallel grind wave 1, 2026-07-26 --- */
/* func_8003F188 */
s32 abs(s32);   /* gcc-2.7 builtin: expands inline to the mips abssi2 triple
                   `bgez $a1,1f / move $v0,$a1 / neg $v0,$v0` — no libc call,
                   no undefined symbol. No header in include/ declares it. */
/* func_80040E00 */
/* add above func_80040E00 in src/rock_neo/player.c (player.h is already
   included there; nothing else calls func_80040E9C from this TU) */
s32 func_80040E9C(PL_WORK* pl, s16 no);

s32 func_800406A8(PL_WORK*);
void func_8003BE6C(PL_WORK*, s32);
s32 func_80041DDC(PL_WORK*, s32, s32, s32);
s32 func_80040764();

void func_8003BE40(s32 arg0) {
    func_8003BE6C(&Player_work, arg0);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003BE6C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C07C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C2D8);

void func_8003C418(PL_WORK* pl) {
    D_80089F94[((u8*)pl)[0x8]]();
    if (func_8004324C(pl) == 0) {
        ((u8*)pl)[0x104] = ((u8*)pl)[0xAC];
        ((u8*)pl)[0x100] = ((u8*)pl)[0xA4];
    }
    if (((u8*)pl)[0x104] != ((u8*)pl)[0x105]) {
        func_800321C8(pl, ((u8*)pl)[0x174] & 3, ((u8*)pl)[0x104],
                      ((u8*)pl)[0x100]);
        ((u8*)pl)[0x105] = ((u8*)pl)[0x104];
    } else {
        func_800323FC(pl, ((u8*)pl)[0x174] & 3);
    }
    if (((u8*)pl)[0xAC] != ((u8*)pl)[0xAD]) {
        func_800320B8(pl, ((u8*)pl)[0xAC], ((u8*)pl)[0xA4]);
    } else {
        func_8003234C(pl);
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C4EC);

/* player-state reset; every offset falls inside PL_WORK pad arrays -> byte-offset form */
void func_8003C7B0(PL_WORK* pl) {
    *(s32*)((u8*)pl + 0x8) = 1;
    *(s16*)((u8*)pl + 0x6C) = 60;
    *(s16*)((u8*)pl + 0x44) = 0;
    *(s16*)((u8*)pl + 0x46) = 0;
    *(s16*)((u8*)pl + 0x48) = 0;
    *(s16*)((u8*)pl + 0x114) = 0;
    *(s16*)((u8*)pl + 0x116) = 0;
    *(s16*)((u8*)pl + 0x118) = 0;
    *((u8*)pl + 0x112) = 0;
    *((u8*)pl + 0x113) = 0;
    *((u8*)pl + 0x110) = 0;
    *((u8*)pl + 0x71) = 0;
    *((u8*)pl + 0x73) = 0;
    *(s32*)((u8*)pl + 0x188) = 0;
    *(s32*)((u8*)pl + 0x18C) = 0;
    *(s32*)((u8*)pl + 0x190) = 0;
    *(s32*)((u8*)pl + 0x194) = 0;
    *((u8*)pl + 0x168) = 0;
    *((u8*)pl + 0x169) = 0;
    *((u8*)pl + 0x104) = 0;
    *((u8*)pl + 0xAC) = 0;
    *((u8*)pl + 0x105) = 0;
    *((u8*)pl + 0xAD) = 0;
    *((u8*)pl + 0x174) = 0;
    *((u8*)pl + 0x177) = 0;
    *((u8*)pl + 0x175) = 7;
    *(s16*)((u8*)pl + 0x108) = 0;
    *(s16*)((u8*)pl + 0x10A) = 0;
    *((u8*)pl + 0x44B) = 0;
    *((u8*)pl + 0x44A) = 0;
    *(s32*)((u8*)pl + 0x184) = 0;
    *((u8*)pl + 0x6) |= 3;
    func_80041DDC(pl, 0, 0, 1);
    func_800321C8(pl, 0, 5, 0);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003C88C);

/* xA is u16 in PL_WORK but this routine touches only its low byte -> the
   file's usual *(u8*)&pl->xA view. 0x44/0x48/0x177/0x44D fall inside pad
   arrays, so byte-offset casts.
   The `jal Pl00_shot_enable_on` has no `move $a0,$s0` in front of it in the
   original: $a0 still holds pl on that fall-through path, so cse deletes the
   redundant hard-reg copy. Writing the call normally reproduces that. */
void func_8003CB20(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        *(u8*)&pl->xA = 1;
        *(s16*)((u8*)pl + 0x44) = 0;
        *(s16*)((u8*)pl + 0x48) = 0;
        pl->x174 &= 0xFC;
        if (!(pl->x174 & 0xC0) && *(s8*)((u8*)pl + 0x177) == 0) {
            Pl00_shot_enable_on(pl);
            func_80041DDC(pl, 0, 0, 0);
        } else if (pl->x174 & 0x40) {
            func_80041DDC(pl, 0x40, pl->x100, 0);
        } else {
            func_80041DDC(pl, 0x60, pl->x100, 0);
        }
    } else {
        *((u8*)pl + 0x44D) = 0;
        pl->xB4 = 0;
        if (!(pl->x174 & 0xC0) && *(s8*)((u8*)pl + 0x177) == 0) {
            func_80041DDC(pl, 0, 0, 0);
        }
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003CBF8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003CD4C);

/* Key-mask steer helper (sibling of func_8003E068, which has this exact
   shape twice).  Three idioms are load-bearing:
   - `s32` (NOT u16) mask locals: with u16 masks cc1 ANDs the HI subregs and
     zero-extends the RESULT (and + andi); the original zero-extends KEY first
     (andi $v0,$a3,0xFFFF then and $v1,$v0,$a0), which only happens when the
     other operand is already SImode.  The andi then survives combine because
     CSE gives it two uses (the m0 and m1 ANDs) so it cannot be deleted.
   - the scaled base hoisted into `u16 *p`: every inline form
     ((u8*)pl + no*8 + 0x148, ((u16*)pl)[no*4+0xA4], const-first, parenthesised)
     emits `addu $a1,$a1,$a2`; the pointer local emits `addu $a1,$a2,$a1`.
   - `*(s16 *)((u8 *)pl + 0xB4)` instead of pl->xB4 (u16): the signed lvalue
     keeps -0x40 an `addiu $v0,$zero,-0x40` rather than `ori $v0,0xffc0`. */
void func_8003D124(PL_WORK *pl, s16 no) {
    u16 key = pl->x11C;
    u16 *p = (u16 *)((u8 *)pl + no * 8);
    s32 m0 = p[0xA4];
    s32 m1 = p[0xA5];

    if ((key & m0) != 0 && (key & m0) != m0) {
        if (key & m1) {
            *(s16 *)((u8 *)pl + 0x48) = -0x140;
            if (!(key & (pl->x128 | pl->x12A))) {
                *(s16 *)((u8 *)pl + 0xB4) = 0x40;
            }
        } else {
            *(s16 *)((u8 *)pl + 0x48) = 0x140;
            if (!(key & (pl->x128 | pl->x12A))) {
                *(s16 *)((u8 *)pl + 0xB4) = -0x40;
            }
        }
    } else {
        *(s16 *)((u8 *)pl + 0x48) = 0;
    }
    func_80041E90(pl, *(s16 *)((u8 *)pl + 0x44), 0, *(s16 *)((u8 *)pl + 0x48));
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003D1E0);

/* 0xB6 and 0x44D fall inside PL_WORK pad arrays -> byte-offset form; xA's low
   byte is the `*(u8 *)&pl->xA` view already used by func_80041A44.
   Two knobs are load-bearing:
   - Both `k & <field>` tests must be written with the CACHED key FIRST:
     `k & pl->x134` / `k & pl->x128` give `and $v0,$v0,$a1` / `and $v0,$a0,$a1`;
     the natural `pl->x134 & k` spelling emits the operands swapped (rs<->rt)
     and costs exactly those two words.  (`(k & t) != t` keeps its written
     order, so the rule is not a blanket "flip everything".)
   - The five func_80041DDC calls are written out separately, one per leg (not
     hoisted into a single call on a `mode` local).  cc1 cross-jumps the common
     `move $a2,0 / move $a3,0 / jal` tail into one block but leaves each leg its
     own `move $a0,pl` + `li $a1,K` -- including the two duplicate 0xD legs,
     which stay as two `li $a1,13` blocks exactly as in the original.
   - The two outer conditions are written inverted relative to the obvious
     reading (`(pl->x174 & 0xC0) == 0` with the 0x60/0x40 pair in the else,
     `(k & t) != t` with the 0-leg in the else) so the fall-through block is
     the one the original places first. */
void func_8003D6DC(PL_WORK *pl) {
    u16 k = pl->x11C;
    s32 a;
    s32 b;
    s32 t;

    if (*(u8 *)&pl->xA == 0) {
        (*(u8 *)&pl->xA)++;
        *(s16 *)((u8 *)pl + 0xB6) = 0;
        *(u8 *)((u8 *)pl + 0x44D) = 0;
    }

    if (k & pl->x134) {
        a = 2;
        b = 0x20;
    } else {
        a = 4;
        b = 0x48;
    }

    if ((pl->x174 & 0xC0) == 0) {
        t = pl->x128 | pl->x12A;
        if ((k & t) != t) {
            if (k & pl->x128) {
                func_80041DDC(pl, 0xD, 0, 0);
            } else {
                func_80041DDC(pl, 0xD, 0, 0);
            }
        } else {
            func_80041DDC(pl, 0, 0, 0);
        }
    } else {
        if (pl->x174 & 0x80) {
            func_80041DDC(pl, 0x60, 0, 0);
        } else {
            func_80041DDC(pl, 0x40, 0, 0);
        }
    }

    func_80042044(pl, a, b);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003D7D4);

/* x48/xA4 fall inside PL_WORK pad arrays -> byte-offset form.
   `u16 k = pl->x11C;` up top is what schedules the lhu above the first branch
   (same idiom as func_8003FDA8 / func_80040224).
   The `k & pl->x130` operand ORDER is load-bearing: spelled `pl->x130 & k`,
   cc1 emits `and $v0,$a1,$a0` where the original has `and $v0,$a0,$a1`.
   The outer test is written `!= m` (not `== m` with the arms swapped) so the
   zero-store lands in the else block, out of line after the call -- matching
   `beq $v1,$v0,.L8003D9EC`.
   Two SEPARATE func_80041DDC calls, not one call with a merged argument:
   cc1's cross-jumping merges their tails, which is what leaves
   `addu $a0,$s0,$zero` duplicated in both arms plus the `j .L8003D9D8`. */
void func_8003D960(PL_WORK *pl) {
    u16 k = pl->x11C;
    u16 m;

    if (*(u8 *)&pl->xA == 0) {
        *(u8 *)&pl->xA = 1;
        pl->xB4 = 0;
        pl->x174 |= 4;
    }
    m = pl->x130 | pl->x132;
    if ((k & m) != m) {
        if (k & pl->x130) {
            *(s16 *)((u8 *)pl + 0x48) = 0x20;
            func_80041DDC(pl, 0xF, 0, 0);
        } else {
            *(s16 *)((u8 *)pl + 0x48) = -0x20;
            func_80041DDC(pl, 0xE, 0, 0);
        }
    } else {
        *(s16 *)((u8 *)pl + 0x48) = 0;
    }
    func_80041E90(pl, 0, 0, *(s16 *)((u8 *)pl + 0x48));
    if (*(u8 *)((u8 *)pl + 0xA4) == 8) {
        Sound_call(0x95, 0, 0);
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003DA34);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003DC5C);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E068);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E224);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E4A8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E6B0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003E860);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003EB08);

void func_8003EE68(PL_WORK* pl) {
    pl->x9 = 7;
    pl->xA = 4;
    pl->x75 = 1;
    pl->x74 = 1;
    Pl00_shot_enable_on(pl);
    func_80041DDC(pl, 0x12, 0, 1);
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003EEC0);

void func_8003F188(PL_WORK* pl, s32 diff, s32 flag) {
    if (abs(diff) < 0x80) {
        pl->xB4 = 0;
        pl->x56 = *(u16*)pl->x10A;
        if (flag != 0) {
            func_80041DDC(pl, 0, 0, 0);
        }
    } else {
        /* the signed rate MUST go through an s32 local: assigning the ternary
           straight to the u16 field folds -0x80 to 0xFF80 and emits ori
           (0x3402FF80) instead of the original's addiu (0x2402FF80).
           Written (diff <= 0) ? 0x80 : -0x80 so cc1's inversion yields
           bgtz + (-0x80 in the delay slot); the (diff > 0) ? -0x80 : 0x80
           spelling produces blez with the arms swapped. */
        s32 v = (diff <= 0) ? 0x80 : -0x80;
        pl->xB4 = v;
        if (flag != 0) {
            func_80041DDC(pl, 0xD, 0, 0);
        }
        pl->x56 += pl->xB4;
    }
}

s32 func_8003F224(PL_WORK *pl) {
    u16 k = pl->x11C;
    s32 g;

    if (Game_work.x54[2] != 0) goto one;
    /* the shared return-1 label keeps the tail a branch (separate returns
       collapse to a setcc/sltiu) — same trick as scene func_8001DE84 */
    g = ((u8 *)&Game_work)[0x83];
    if (g == 0) {
        if (pl->x13E & k) {
            return 1;
        }
    }
    if (g != 1) {
        return 0;
    }
    if (pl->x140 & k) {
        return 0;
    }
one:
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F288);

void func_8003F3E8(PL_WORK* pl) {
    switch (*(u8*)&pl->xA) {
    case 0:
        func_80041DDC(pl, 0x30, 0, 1);
        *(u8*)&pl->xA += 1;
        break;
    case 1:
        break;
    case 2:
        func_80041DDC(pl, 0x31, 0, 1);
        *(u8*)&pl->xA += 1;
        break;
    case 3:
        if (*(s8*)((u8*)pl + 0xA7) == -1) {
            pl->x9 = 0;
            *(u8*)&pl->xA = 0;
        }
        break;
    }
}

void func_8003F498(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x33, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

void func_8003F4E8(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x34, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

void func_8003F538(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x35, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

void func_8003F588(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 0) {
        func_80041DDC(pl, 0x36, 0, 1);
        *(u8*)&pl->xA += 1;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F5D8);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F7F0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003F97C);

void func_8003FDA8(PL_WORK* pl) {
    u16 k = pl->x11C; /* preloaded so the lhu schedules above the first branch */
    if (*(u8*)&pl->xA < 3) {
        if ((k & pl->x134) == 0) {
            pl->x44A = 0;
            Sound_call(0x8F, 0, 0);
            if (func_800406A8(pl) == 0) {
                pl->x9 = 0;
                pl->xA = 0;
            }
        }
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003FE1C);

void func_8003FFA8(void) {}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8003FFB0);

s32 func_80040818(PL_WORK*);
s32 func_80040710(PL_WORK*);
void func_80040B68(PL_WORK*, s32);

void func_800400B8(PL_WORK* pl) {
    if (func_80040818(pl) == 0) {
        if (*(s8*)&pl->x74 == 0 && *(u8*)&pl->xA == 1) {
            if (func_80040710(pl) != 0) {
                return;
            }
        }
        if (*(s8*)&pl->x112 != 2) {
            func_80040B68(pl, 2);
        }
    }
}

void func_80040130(void) {}

void func_80040138(void) {}

void func_80040140(PL_WORK* pl) {
    if (*(u8*)&pl->xA == 2) {
        func_80040764();
    }
}

void func_80040170(void) {}

s32 func_80048E6C(void);

/* x16/x44/x46/x48/x78 fall inside PL_WORK pad arrays -> byte-offset form */
void func_80040178(PL_WORK *pl) {
    s32 v;

    if (*(u8 *)&pl->xA == 1) {
        s32 r;
        v = *(s16 *)((u8 *)pl + 0x16);
        r = func_80048E6C() - 0x20;
        if (r - *(s16 *)(*(u8 **)((u8 *)pl + 0x78) + 0xA) < v) {
            pl->x9 = 7;
            pl->xA = 4;
            pl->x75 = 1;
            pl->x74 = 1;
            *(s16 *)((u8 *)pl + 0x44) = -0x2A;
            *(s16 *)((u8 *)pl + 0x46) = 0;
            *(s16 *)((u8 *)pl + 0x48) = 0;
            Pl00_shot_enable_on(pl);
            func_80041DDC(pl, 0x12, 0, 1);
        }
    }
}

void func_800394AC(void);

void func_80040224(PL_WORK *pl) {
    u16 k = pl->x11C;

    func_80040B68(pl, 0);
    if (((u8 *)&Game_work)[0x83] != 1) {
        if (!(pl->x140 & k)) {
            pl->x9 = 0;
            pl->xA = 0;
            func_800394AC();
            pl->x108 = 0;
        }
    }
}

void func_80040294(void) {}

void func_8004029C(void) {}

void func_800402A4(void) {}

void func_800402AC(void) {}

void func_800402B4(void) {}

void func_800402BC(void) {}

s32 func_800402C4(PL_WORK* pl) {
    if (pl->x11C & pl->x124) {
        if (pl->x11C & pl->x134) {
            pl->x9 = 1;
            pl->xA = 0;
            return 1;
        }
    }
    return 0;
}

s32 func_8004030C(PL_WORK* pl, s32 arg1) {
    u16 k;

    if (Scene_work.x0 != 0 || arg1 == 0) {
        k = pl->x11C;
    } else {
        k = pl->x11E;
    }
    if (k & pl->x126) {
        if (k & pl->x134) {
            pl->x9 = 1;
            pl->xA = 0x100;
            return 1;
        }
    }
    return 0;
}

s32 func_80040380(PL_WORK* pl) {
    u8 t;

    if ((pl->x11C & pl->x124) && !(pl->x11C & pl->x134)) {
        t = pl->x112;
        pl->x9 = 2;
        pl->xA = 0;
        pl->x112 = 0;
        pl->x113 = t;
        return 1;
    }
    return 0;
}

s32 func_800403D4(PL_WORK* pl, s32 arg1) {
    u16 k, m0, m1;
    u8 t;

    if (Scene_work.x0 != 0 || arg1 == 0) {
        k = pl->x11C;
    } else {
        k = pl->x11E;
    }
    m0 = pl->x12C;
    m1 = pl->x12E;
    if ((k & (m0 | m1)) != (m0 | m1)) {
        if (k & m0) {
            if (!(k & pl->x134)) {
                t = pl->x112;
                pl->x9 = 2;
                pl->xA = 0;
                pl->x112 = 1;
                pl->x113 = t;
                return 1;
            }
        }
    }
    return 0;
}

s32 func_80040468(PL_WORK* pl, s32 arg1) {
    u16 k, m0, m1;
    u8 t;

    if (Scene_work.x0 != 0 || arg1 == 0) {
        k = pl->x11C;
    } else {
        k = pl->x11E;
    }
    m0 = pl->x12C;
    m1 = pl->x12E;
    if ((k & (m0 | m1)) != (m0 | m1)) {
        if (k & m1) {
            if (!(k & pl->x134)) {
                t = pl->x112;
                pl->x9 = 2;
                pl->xA = 0;
                pl->x112 = 3;
                pl->x113 = t;
                return 1;
            }
        }
    }
    return 0;
}

s32 func_800404FC(PL_WORK* pl, s32 arg1) {
    u16 k;
    u8 t;

    if (Scene_work.x0 != 0 || arg1 == 0) {
        k = pl->x11C;
    } else {
        k = pl->x11E;
    }
    if ((k & pl->x126) && !(k & pl->x134)) {
        t = pl->x112;
        pl->x9 = 2;
        pl->xA = 0;
        pl->x112 = 2;
        pl->x113 = t;
        return 1;
    }
    return 0;
}

s32 func_80040574(PL_WORK* pl) {
    u8 c;

    if ((pl->x11C & pl->x134) && (pl->x16B & 2)) {
        c = pl->x44A + 1;
        pl->x44A = c;
        if (c < 9) {
            return 0;
        }
        if (pl->x44B != 0) {
            return 0;
        }
        pl->x9 = 3;
        pl->xA = 0;
        return 1;
    }
    pl->x44A = 0;
    return 0;
}

s32 func_800405F4(PL_WORK* pl) {
    if (pl->x11C & (pl->x128 | pl->x12A)) {
        pl->x9 = 4;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

s32 func_80040630(PL_WORK* pl, s32 arg1) {
    if (Scene_work.x0 != 0 || arg1 == 0) {
        arg1 = pl->x11C;
    } else {
        arg1 = pl->x11E;
    }
    if (pl->x134 & arg1) {
        if (arg1 & (pl->x130 | pl->x132)) {
            pl->x9 = 6;
            pl->xA = 0;
            return 1;
        }
    }
    return 0;
}

s32 func_800406A8(PL_WORK* pl) {
    if (pl->x11E & pl->x138) {
        pl->x9 = 7;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

s32 func_800406DC(PL_WORK* pl) {
    if (pl->x11E & pl->x138) {
        pl->x9 = 8;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

s32 func_80040710(PL_WORK* pl) {
    u16 m0 = pl->x128;
    u16 m1 = pl->x12A;
    u16 k = pl->x11C & (m0 | m1);

    if (k == m0) {
        pl->x9 = 9;
        pl->xA = 0;
        return 1;
    }
    if (k == m1) {
        pl->x9 = 9;
        pl->xA = 0x100;
        return 1;
    }
    return 0;
}

s32 func_80040764(pl) /* K&R: caller func_80040140 passes the leftover $a0 */
PL_WORK *pl;
{
    u16 k = pl->x11C;
    s32 a;
    s32 b;
    s32 g2;
    u16 m;

    m = pl->x11E;
    if (!(m & pl->x138)) {
        return 0;
    }
    a = (k & (pl->x12A | pl->x12E)) == 0;
    g2 = k & (pl->x128 | pl->x12C);
    b = g2 == 0;
    if (a != b) {
        if (g2 != 0) {
            pl->x9 = 9;
            pl->xA = 0;
        } else {
            pl->x9 = 9;
            pl->xA = 0x100;
        }
    } else if (rand() & 1) {
        pl->x9 = 9;
        pl->xA = 0;
    } else {
        pl->x9 = 9;
        pl->xA = 0x100;
    }
    *(u8 *)((u8 *)pl + 0x168) = 0;
    *(u8 *)((u8 *)pl + 0x169) = 0;
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040818);

s32 func_80040AEC(PL_WORK* pl) {
    u16 k = pl->x11C;

    if ((((u8*)&Game_work)[0x83] == 1) || (k & pl->x140)) {
        pl->x9 = 0xE;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

s32 func_80040B34(PL_WORK* pl) {
    if (pl->x11C & pl->x13E) {
        pl->x9 = 5;
        pl->xA = 0;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040B68);

s32 func_80040E00(PL_WORK* pl, s16 a1, s16 a2, s16 a3) {
    /* three SEPARATE call sites are load-bearing: post-reload cross-jumping
       merges the identical `sll/sra/jal/li 1` tails, which keeps the s16 param
       in $a1 (case 0 does `addiu $a1,$a1,1` in place).  A single trailing
       `func_80040E9C(pl, a1); return 1;` with `a1++; break;` in case 0 makes a1
       an assigned pseudo, costs an entry `move $v1,$a1`, and pushes
       `sw $ra,0x10($sp)` out of the first branch delay slot. */
    switch (a2) {
    case 0:
        func_80040E9C(pl, a1 + 1);
        return 1;
    case 1:
        if (a3 == 0) {
            pl->x9 = 0;
            pl->xA = 0;
            return 0;
        }
        func_80040E9C(pl, a1);
        return 1;
    case 2:
        if (a3 == 0) {
            return 0;
        }
        func_80040E9C(pl, a1);
        return 1;
    }
    return 1;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80040E9C);

extern u8 D_80089A84[]; // weapon stride-12 stat table, level column

s32 func_80040FA8(PL_WORK* pl) {
    s32 c = pl->x174 & 0xC0;

    if (c == 0) {
        goto ret1;
    }
    if (c == 0x40) {
        if ((u32)pl->x100 > D_80089A84[pl->weapon_right_no * 12]) {
            return 1;
        }
    }
    if (c != 0x80) {
        return 0;
    }
    if ((u32)pl->x100 > D_80089A84[pl->x171 * 12]) {
    ret1:
        return 1;
    }
    return 0;
}

extern u8 D_80089A88[]; // weapon stride-12 stat table, +4 column

s32 func_80041044(PL_WORK* pl) {
    u16 k = pl->x11C;

    if (k & pl->x13C) {
        if (D_80089A88[pl->weapon_right_no * 12] == 0) {
            goto ret1;
        }
    }
    if (!(k & pl->x13A)) {
        return 0;
    }
    if (D_80089A88[pl->x171 * 12] == 0) {
    ret1:
        return 1;
    }
    return 0;
}

extern u8 D_80089A89[]; // weapon stride-12 stat table, +5 column

s32 func_800410D4(PL_WORK* pl) {
    s32 c = pl->x174 & 0xC0;

    if (c == 0x40) {
        if (D_80089A89[pl->weapon_right_no * 12] == 0) {
            goto ret1;
        }
    }
    if (c != 0x80) {
        return 0;
    }
    if (D_80089A89[pl->x171 * 12] == 0) {
    ret1:
        return 1;
    }
    return 0;
}

extern u8 D_80089A8A[]; // weapon stride-12 stat table, +6 column

s32 func_80041158(PL_WORK* pl) {
    s32 c = pl->x174 & 0xC0;

    if (c == 0x40) {
        if (D_80089A8A[pl->weapon_right_no * 12] == 0) {
            goto ret1;
        }
    }
    if (c != 0x80) {
        return 0;
    }
    if (D_80089A8A[pl->x171 * 12] == 0) {
    ret1:
        return 1;
    }
    return 0;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800411DC);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_800414F0);

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_8004174C);

s32 func_80041A44(PL_WORK* pl, s32 k) {
    s32 v;
    if (*(s8*)&pl->x74 != 0) v = 0xC;
    else if (k & 0x41000000) v = 0xC;
    else if (*(s16*)&pl->life <= 0) v = 0xC;
    else if (pl->x9 == 0xB && *(u8*)&pl->xA < 2) return 0xB; /* v assigned only after the compares — keeps v dead there so it lands in $v0 */
    else v = 0xB;
    pl->x9 = v;
    *(u8*)&pl->xA = 0;
    return v;
}

void func_80041AB0(PL_WORK* pl) {
    /* align-1 struct copies -> lwl/lwr + swl/swr. Statement ORDER in the
       4-pair block below is load-bearing: cc1 rotates the block (source
       stmt 3 is emitted first), so the target's emitted 0x74/0xAC/0x6E/0x76
       order requires the source order 0xAC, 0x6E, 0x74, 0x76. All 24
       permutations were tried; only this one reproduces the register
       sequence $a0,$v0,$v1,$a1 against the right offsets. */
    *(PL_BLK8*)((u8*)pl + 0x5C) = *(PL_BLK8*)((u8*)pl + 0x54);
    *((u8*)pl + 0xAD) = *((u8*)pl + 0xAC);
    *(u16*)((u8*)pl + 0xB2) = pl->life;
    pl->x75 = pl->x74;
    *((u8*)pl + 0x77) = *((u8*)pl + 0x76);
    func_8004B414(pl);
    /* 0x14 block is align 1 (lwl/lwr), the 0x1C block is align 4 (lw/sw);
       two separate copies, both +0x18 -- func_8004B414 takes pl+0x14 and
       pl+0x2C as a current/backup pair. */
    *(PL_BLK8*)((u8*)pl + 0x2C) = *(PL_BLK8*)((u8*)pl + 0x14);
    *(PL_BLK16*)((u8*)pl + 0x34) = *(PL_BLK16*)((u8*)pl + 0x1C);
    if (pl->x9 == 0) {
        Game_work.routine_0 = 9;
        Game_work.x1 = 0;
        pl->x9++;
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041B8C);

/* xA4/xAC/xAD fall inside PL_WORK pad arrays -> byte-offset form.
   K&R definition: the narrow declared types are load-bearing -- `u8 a3` gives the
   in-place entry `andi $a3,$a3,0xff`, `s32 a1` + explicit (u8) casts give the two
   separate use-site `andi ..,$a1,0xff`, and `s16 a2` (HImode) is what turns the
   final `a2 >>= 1` into `sll 16 / sra 17`.  Promoted K&R types are int/int/int, so
   the ANSI prototype at player.c:9 stays byte-for-byte untouched (callers unaffected).
   No value is returned: every caller ignores it and the original never writes $v0. */
s32 func_80041DDC(pl, a1, a2, a3)
PL_WORK *pl;
s32 a1;
s16 a2;
u8 a3;
{
    if (a3 != 0) {
        *(u8 *)((u8 *)pl + 0xAD) = 0xFF;
    } else if ((u8)a1 == *(u8 *)((u8 *)pl + 0xAD)) {
        return;
    }
    /* case 0xC first: its body must fall through from the 2-leg beq dispatch */
    switch ((u8)a1) {
    case 0xC:
        if (*(u8 *)((u8 *)pl + 0xAD) == 1) {
            a2 = *(u8 *)((u8 *)pl + 0xA4) * 2 + 4;
            if (a2 >= 0x20) {
                a2 -= 0x20;
            }
        }
        break;
    case 1:
        if (*(u8 *)((u8 *)pl + 0xAD) == 0xC) {
            a2 = *(u8 *)((u8 *)pl + 0xA4) - 4;
            if (a2 < 0) {
                a2 += 0x20;
            }
            a2 >>= 1;
        }
        break;
    }
    *(u8 *)((u8 *)pl + 0xAC) = a1;
    *(u8 *)((u8 *)pl + 0xA4) = a2;
}

void func_8002FEA4(PL_WORK *, s16, s16, s16);

void func_80041E90(PL_WORK *pl, s16 x, s16 y, s16 z) {
    pl->x56 -= pl->x116;
    func_8002FEA4(pl, x, y, z);
    pl->x56 += pl->x116;
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80041EF4);

/* Sibling of func_80042154 (same file, already matched) - identical skeleton on
   x10C/x108 instead of xB4/x56, no return value, and a +-0x200 clamp instead of
   the & 0xFFF wrap.
   The two HImode locals are load-bearing (LESSONS 2026-07-11): `v` buys the
   otherwise unexplained `addiu $sp,-0x10` frame plus the `lh` + `move $v1,$v0`
   copy in each arm, and `w` is what turns the post-store re-read of x108 into
   `sll 16 / sra 16` on the value still in $v0 rather than a reload.
   x108/x10C are u16, so the accumulate loads are `lhu`; the compares are signed
   only because they run through `w`/`v`.
   `w = -0x200; pl->x108 = w;` (NOT `pl->x108 = -0x200;`) is required: assigning
   the constant straight to the u16 member folds it to 0xFE00 and emits
   `ori $v0,$zero,0xFE00`; routing it through the s16 local keeps it signed and
   gives the original's `addiu $v0,$zero,-0x200`.
   `-a2` written inline at both use sites keeps the `negu` AFTER the sll/sra. */
void func_80041F54(PL_WORK *pl, s32 a1, s32 a2, s32 a3) {
    s16 v;
    s16 w;

    if (pl->x11C & a3) {
        v = (s16)pl->x10C;
        if (v < 0) {
            pl->x10C = 0;
        } else {
            v += a1;
            pl->x10C = v;
            if (v > a2) {
                pl->x10C = a2;
            }
        }
        w = pl->x108 + pl->x10C;
        pl->x108 = w;
        if (w > 0x200) {
            pl->x108 = 0x200;
        }
    } else {
        v = (s16)pl->x10C;
        if (v > 0) {
            pl->x10C = 0;
        } else {
            v -= a1;
            pl->x10C = v;
            if (v < -a2) {
                pl->x10C = -a2;
            }
        }
        w = pl->x108 + pl->x10C;
        pl->x108 = w;
        if (w < -0x200) {
            w = -0x200;
            pl->x108 = w;
        }
    }
}

INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/player", func_80042044);

/* `pl->xB4 = k >> 16` IS A WRITE OF ZERO -- k is u16, so the promoted shift is
   identically 0 and cc1's combine pass folds it to `sh $zero,0xB4($s0)`.  The
   spelling is load-bearing: at life_analysis time the read of k sits AFTER the
   func_80042154 call, so the x11C pseudo gets calls_crossed=1 and is forced
   into a callee-saved reg ($s1).  That is what leaves $a1/$a2 free to carry
   a1/a2 straight through to func_80042154.  Written `pl->xB4 = 0;` the pseudo
   is block-local, grabs $a1, and cc1 spills the a1 param into $t0: +1 insn,
   one fewer saved register, frame 0x20 instead of 0x28. */
s32 func_80042094(PL_WORK *pl, s32 a1, s32 a2, s32 flag) {
    u16 k = pl->x11C;
    s32 ret = 0;

    if (!(k & (pl->x12A | pl->x12E)) != !(k & (pl->x128 | pl->x12C))) {
        ret = func_80042154(pl, a1, a2, pl->x12A | pl->x12E);
        if (flag != 0) {
            func_80041DDC(pl, 0xD, 0, 0);
        }
    } else {
        if (flag != 0) {
            func_80041DDC(pl, 0, 0, 0);
        }
        pl->xB4 = k >> 16;
    }
    return ret;
}

/* xB6 falls inside a PL_WORK pad array -> byte-offset form.
   The two per-arm `s16 v` HImode locals are load-bearing: they buy the otherwise
   unexplained `addiu $sp,-0x10` frame and the `lh` + `move $v1,$v0` copy in each
   arm (LESSONS 2026-07-11).  `ret` accumulated in a local (not direct returns)
   is what produces the trailing `move $v0,$a3`.  `-a2` written inline at both
   use sites (not hoisted to `a2 = -a2;`) keeps the `negu` AFTER the sll/sra. */
s32 func_80042154(PL_WORK *pl, s32 a1, s32 a2, s32 a3) {
    s32 ret;

    if (pl->x11C & a3) {
        s16 v = (s16)pl->xB4;
        ret = 1;
        if (v < 0) {
            pl->xB4 = 0;
        } else {
            v += a1;
            pl->xB4 = v;
            if (v > a2) {
                pl->xB4 = a2;
            }
            *(s16 *)((u8 *)pl + 0xB6) = a1;
        }
    } else {
        s16 v = (s16)pl->xB4;
        ret = 2;
        if (v > 0) {
            pl->xB4 = 0;
        } else {
            v -= a1;
            pl->xB4 = v;
            if (v < -a2) {
                pl->xB4 = -a2;
            }
            *(s16 *)((u8 *)pl + 0xB6) = -a1;
        }
    }
    pl->x56 = (pl->x56 + pl->xB4) & 0xFFF;
    return ret;
}

s32 func_80042208(void) {
    if (*(s16*)&Player_work.life >= 0) {
        Player_work.x449 = 1;
        return 1;
    }
    return 0;
}
