#include "common.h"

extern u16 D_8015C006[];

/* The goto-shared-return label is load-bearing: it lets the sltu flag and the
 * result share $v0 (flag dies at the branch, r is born in the delay slot).
 * An accumulator initialised before the compare puts r in $v1 and costs a
 * trailing `move $v0,$v1`; a plain early return emits an extra `j`. */
s32 func_80037C14(u16 i) {
    s32 r;

    if (i > 0x7FFF) {
        r = -1;
        goto end;
    }
    r = D_8015C006[i * 6] & 0xFFF;
end:
    return r;
}
