#include "common.h"

/* Not an empty function: the `sb $zero, 7($a0)` sits in the jr delay slot, so it runs.
 * Clears one flag byte at +7 of whatever it is handed. */
void func_80036470(u8 *p) {
    p[7] = 0;
}
