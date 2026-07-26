#include "common.h"

/* `addu $v0, $zero, $zero` in the jr delay slot — a constant-zero return, not an
 * empty body. Almost certainly a stubbed-out predicate that always answers "no". */
s32 func_8004E488(void) {
    return 0;
}
