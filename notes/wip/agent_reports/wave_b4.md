# wave_b4 — func_80045980, func_8004674C, func_8004676C

## Family verdict

All three are members of a large known family of 8-instruction functions.

- **func_80045980** and **func_8004674C** are byte-identical copies of the A31–A37 trivial
  tail-call wrapper (`family=324`, `copies=43`): `addiu $sp,-0x18 / sw $ra / jal CALLEE / nop / lw $ra / addiu $sp,0x18 / jr $ra / nop`.
  Both call `func_80031988`. Template source already proven in the tree (`scene.c`, `game.c`
  family). Copied directly, no re-derivation needed.

- **func_8004676C** is a byte-identical copy of the S30/S31 init-function shape
  (lane_b_drafts.md §S30/S31, `copies=2`): stores a negative s16 constant at x2E, a u16 at
  x34, increments x8 as u8. Same body as func_800459A0.

## Bytecmp results

| function | hard mismatches | reloc flags | verdict |
|---|---|---|---|
| func_80045980 | 0 | 1 (jal addr) | **PASS** |
| func_8004674C | 0 | 1 (jal addr) | **PASS** |
| func_8004676C | 0 | 0 | **PASS** |

Scratch object: `/tmp/bc_b4w_258545/bc_func_8004676C.o` (and siblings).

---

## func_80045980

```c
void func_80031988(void *);

void func_80045980(void *o) {
    func_80031988(o);
}
```

## func_8004674C

```c
void func_80031988(void *);

void func_8004674C(void *o) {
    func_80031988(o);
}
```

## func_8004676C

Fields: `x2E` must be `s16` (compiler uses `addiu $v0,$zero,-0x28` for a signed
negative constant; if declared `u16` the compiler chooses `ori` = `D8FF0234` ≠ `D8FF0224`).
`x34` is `u16` (constant 5, no sign issue either way, but nothing blocks u16).
`x8` is `u8` (`lbu`/`sb` confirm unsigned byte).

```c
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

typedef struct {
    char pad0[8];
    u8 x8;
    char pad9[0x25];
    s16 x2E;
    char pad30[4];
    u16 x34;
} OBJ;

void func_8004676C(OBJ *o) {
    o->x2E = -0x28;
    o->x34 = 5;
    o->x8 = o->x8 + 1;
}
```

### Notes

- The `typedef struct OBJ` above is local to this scratch TU. The real landing will use the
  project's existing `OBJ` type from `include/rock_neo/`. Adjust offset names to the real
  field names if they differ.
- `func_80031988` forward-declaration type: the overlay sources all use `void func_80031988(void)` (K&R no-arg). Either form (`void *o` pass-through or `void` K&R) produces identical bytes for these 8-insn wrappers because `$a0` is already set by the caller and `func_80031988` is only referenced via a jal relocation. The pass-through form is preferred per lane_b_drafts §A31–A37.
- No new symbol introductions. `func_80031988` is already declared in every overlay source; no conflict possible.
