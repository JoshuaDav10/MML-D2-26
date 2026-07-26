#include "common.h"

/* NOT YET MATCHED — 3 rows off. See notes/wip/2488_draft_best.c and the note below.
 *
 * 2026-07-26: two source shapes each get PART of the target and neither gets both:
 *   (a) `SEQ_CMD *p = m->cur;` hoisted above the `if`  -> correct INSTRUCTIONS (lw lands
 *       in the lbu's load-delay slot at 0x22c9c) but regalloc comes out p=$a1, c=$a0,
 *       copy=$v1 where the target wants p=$v1, c=$a1, copy=$a0.  7 rows.
 *   (b) no source hoist, `c = m->cur->code;`           -> regalloc EXACTLY right (the
 *       anonymous cse temp gets $v1), but the load stays inside the `if`, so the
 *       scheduler cannot lift it across the conditional branch: 2 extra insns.  3 rows.
 * Tried and closed: declaration order (LESSONS is right — that governs stack slots, not
 * regalloc), a named mask local (12 rows, worse), `register` hint (no change), inverting
 * the arm order (8 rows). Next lever is decomp-permuter seeded from shape (b), or the
 * reload/local-alloc qty-ordering question. Do NOT re-try the five shapes above.
 */
INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/Code80032488", func_80032488);
