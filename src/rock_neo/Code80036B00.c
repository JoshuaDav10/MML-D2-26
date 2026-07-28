#include "common.h"

/* The unsized-array spelling on the LOADS is load-bearing here, and not for
 * addressing: it sets MEM_IN_STRUCT_P on the load so GCC 2.7's
 * true_dependence cannot hoist it above the p[1..] stores. See LESSONS. */
extern u8 D_8009A370[];
extern u8 D_8009A371[];
extern u8 D_8009A372[];
extern u16 D_8009A374[];

void func_80036B00(s16 *p) {
    p[0] = D_8009A370[0];
    p[1] = D_8009A371[0];
    p[2] = D_8009A372[0];
    p[3] = D_8009A374[0];
}
