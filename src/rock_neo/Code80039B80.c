#include "common.h"

extern u8 D_800A3960;
extern s32 D_800A396C;

/* NOTE: the early-return shape is load-bearing. The single-accumulator form
 * (`s32 var = 0; if (D_800A3960) var = D_800A396C; return var;`) allocates the
 * flag to $v0 and the accumulator to $v1, costing a trailing `move $v0,$v1`. */
s32 func_80039B80(void) {
    if (D_800A3960 == 0) {
        return 0;
    }
    return D_800A396C;
}
