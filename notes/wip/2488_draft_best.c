#include "common.h"

/* Animation/sequence actor. Layout derived from this function plus func_80032488,
 * func_80033A28 and func_8003346C, which share it. No header type exists yet; both
 * single-function TUs declare it locally rather than couple through include/. */
typedef struct {
    u8 b0;
    u8 cnt;
    u8 b2;
    s8 code;
    u32 w4;
} SEQ_CMD; /* 8 bytes */

/* The 0xA5 tick counter overlaps byte 1 of the 0xA4 command word — the union is
 * mandatory, not stylistic: func_80032488 decrements 0xA5 while this function stores
 * the whole word at 0xA4. */
typedef union {
    u32 w;
    struct {
        u8 b0;
        s8 tick;
        u8 b2;
        s8 code;
    } b;
} SEQ_CUR;

typedef struct {
    char pad00[0x90];
    SEQ_CMD **tbl;  /* 0x90 — per-index sequence table */
    SEQ_CMD *base;  /* 0x94 — current sequence base    */
    SEQ_CMD *cur;   /* 0x98 — current command          */
    char pad9C[8];
    SEQ_CUR cc;     /* 0xA4 (tick at 0xA5)             */
    char padA8[4];
    u8 idx;         /* 0xAC — current sequence index   */
} SEQ_WORK;

void func_80033A28(SEQ_WORK *);
void func_8003346C(SEQ_WORK *);

/* Tick the current command; on expiry advance, jump, or reload the repeat count.
 *
 * `p` is hoisted above the `if` deliberately — that is what puts `lw 0x98` in the lbu's
 * load-delay slot. `c` must be s8 (not u8): the lb + bgez pattern is the signed-char test;
 * a u8 field emits andi 0xff instead. -funsigned-char is on, so plain `char` will not do.
 * s8 also gives the QImode local a stack slot, which is where the frame's 8 var bytes
 * come from (the leaf twin func_800323FC is addiu $sp,-0x8). */
void func_80032488(SEQ_WORK *m) {
    SEQ_CMD *p = m->cur;
    s8 c;

    if (--m->cc.b.tick == 0) {
        c = p->code;
        if (c < 0) {
            if (c == -1) {
                m->cc.b.tick = p->cnt;
            } else {
                m->cur = m->base + (c & 0x7F);
                m->cc.w = *(u32 *)m->cur;
            }
        } else {
            /* re-reads 0x98: this arm is a branch target outside cse's extended block */
            m->cur = m->cur + 1;
            m->cc.w = *(u32 *)m->cur;
        }
    }
    func_80033A28(m);
    func_8003346C(m);
}
