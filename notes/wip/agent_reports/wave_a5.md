# wave_a5 — three engine functions

## func_8002DE20

**bytecmp: PASS — 0 hard mismatches, 0 reloc flags (5/5 words)**

```c
void func_8002DE20(s32 *a0, s32 a1, s32 *a2) {
    a0[a1 * 4] = *a2;
}
```

Analysis: `sll $a1,$a1,4` is `a1*16` byte-offset; with `a0` typed `s32*` the compiler
folds `a0 + a1*4` (where 4 is the s32 stride) into `sll 4; addu`. The load from `$a2`
and store through the computed pointer follow naturally. No externs needed — pure
register arithmetic.

---

## func_80036478

**bytecmp: PASS — 0 hard mismatches, 0 reloc flags (5/5 words)**

```c
void func_80036478(u8 *p, s8 a, u32 b) {
    p[6] = a;
    p[0xF] = b >> 4;
    p[7] = 0;
}
```

Analysis: `srl` (not `sra`) at the top pins the shift parameter as unsigned (`u32`).
Three byte stores to a raw `u8*` base. Draft S28 in `notes/wip/lane_b_drafts.md`
already had the correct shape; this is a straight verify.

---

## Pl00_shot_enable_on

**bytecmp: PASS — 0 hard mismatches, 0 reloc flags (5/5 words)**

Return type is PINNED to `unknown_t` per `include/rock_neo/player.h:83` — changing
it to `void` or `s32` alters the emitted code.

```c
extern unknown_t Pl00_shot_enable_on(PL_WORK *pl);

unknown_t Pl00_shot_enable_on(PL_WORK *pl) {
    pl->x175[0] |= 1;
}
```

Analysis: `lbu $v0,0x175($a0); ori $v0,$v0,1; sb $v0,0x175($a0)` is the RMW
for a byte OR. The nop after lbu is a load-delay slot. `$v0` holds the OR result on
exit — do not write `return`. Draft S26 in `notes/wip/lane_b_drafts.md` was correct;
this is a verify. The extern declaration is only needed if included without `player.h`.
