# wave_b6 — three engine functions

All three byte-compared at exit 0 (0 hard mismatches). Results follow.

---

## func_800153CC — HIGH VALUE (145 duplicates game-wide)

**Bytecmp verdict: PASS** — 0 hard mismatches, 1 reloc flag (jal, expected).

**Ambiguity note (from lane_b_drafts):** both the pass-through form and the K&R-void
form emit identical bytes. The pass-through is preferred (these are state-handler slots
and callers do pass a pointer). I verified both emit the same object — there is no way
to distinguish them from the asm alone.

```c
void func_80031824(u8 *p);

void func_800153CC(u8 *p) {
    func_80031824(p);
}
```

---

## func_80063BA8

**Bytecmp verdict: PASS** — 0 hard mismatches, 6 reloc flags (lui/%lo pairs, expected).

D_800987B0/B4/B8 use `lui $at,%hi` / `sw $zero,%lo` addressing in the asm.
These symbols do NOT appear as `%gp_rel` anywhere in the asm tree (verified),
so they are NOT in the gp census. Scalar `extern s32` declarations compile to
bare refs which gprel.py leaves as lui/$at — confirmed by the `gprel.py: note:
small extern … not gp-accessed in original asm; leaving it non-gp` messages.
Three separate scalars (not an array) because each has its own independent
lui/$at pair (no base-register CSE across the three stores).

```c
extern s32 D_800987B0;
extern s32 D_800987B4;
extern s32 D_800987B8;

void func_80063BA8(void) {
    D_800987B0 = 0;
    D_800987B4 = 0;
    D_800987B8 = 0;
}
```

---

## func_80067E4C

**Bytecmp verdict: PASS** — 0 hard mismatches, 1 reloc flag (jal, expected).

Declared `void func_80067E4C(void)` in scene.c (confirmed). Delay slot loads
`$a0 = 0x4C0` fresh before calling Sce_flag_on — this function takes no arguments
and passes a fixed constant. rock_neo.h has `unknown_t Sce_flag_on(unknown_t)`;
the overlay usage (ST03B/Code80101064.c) pins it as `void Sce_flag_on(s32 id)`.
Using `s32` is correct here.

```c
void Sce_flag_on(s32 id);

void func_80067E4C(void) {
    Sce_flag_on(0x4C0);
}
```

---

## Integration notes

- All three functions are currently in `[asm]` sections in the splat yaml (52C0,
  543A8, 5864C). No existing C source file owns them.
- func_80063BA8 already has a nonmatchings stub at
  `asm/rock_neo/nonmatchings/Code80063BA8/func_80063BA8.s`.
- func_800153CC and func_80067E4C are in bulk .s files; their asm slices were
  extracted to /tmp for bytecmp.
- rock_neo.h declares `unknown_t func_80063BA8()` (game.c:32/209 caller) —
  the void return is confirmed by the asm (jr $ra / nop with no value in $v0).
  A landing session should update that declaration.
- Sce_flag_on: if this TU shares a file with other callers, the `void (s32)`
  prototype must not conflict with rock_neo.h's `unknown_t (unknown_t)` —
  forward-declare locally or reconcile the header.
