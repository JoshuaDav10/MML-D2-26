#include "common.h"

/* D_8009F108 is the base of a 16-entry array of 0x3C0-byte work slots (stride confirmed
 * by func_800411DC, which walks it with `+= 0x3C0` for 0x10 iterations).
 * Its address is taken via lui/addiu, so it must be an unsized ARRAY extern — a scalar
 * extern would compile to a $gp-relative access and flip the addressing mode. (LESSONS §2.) */
extern u8 D_8009F108[];

/* Both are reached by bare lui/%lo, never %gp_rel, so plain scalar externs are correct. */
extern u32 D_8009880C;
extern u32 D_800987E8;

/* Marks slot `p` closed: set its bit in the live mask, re-filter by the enable mask, then
 * clear the slot's own in-use byte.
 *
 * The index is a literal unsigned divide by 0x3C0, NOT a pointer subtraction: the target
 * uses multu/mfhi/srl (magic 0x88888889, shift 9 — the /960 sequence) rather than the
 * mflo exact-division gcc emits for `ptr - ptr`. */
void func_80031824(u8 *p) {
    u32 i = ((u32)p - (u32)D_8009F108) / 0x3C0;

    D_8009880C |= 0x80000000 >> i;
    D_8009880C &= D_800987E8;
    *p = 0;
}
