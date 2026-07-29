# Wave A2 — Three Empty Engine Functions

All three functions confirmed `jr $ra; nop` (2-instruction empty body).
Precedent: `src/rock_neo/Code8005D96C.c` pattern.

Note: all three live in **bulk chunk** `.s` files (not under `nonmatchings/`),
so bytecmp required explicit `REFERENCE.s` paths extracted from the chunks.
No hard mismatches in any case.

---

## func_80047BD0

Asm source: `asm/rock_neo/383D0.s`
Bytecmp verdict: **PASS** — 2/2 words match, 0 hard mismatches, 0 reloc flags.

```c
void func_80047BD0(void) {
}
```

---

## func_8005C860

Asm source: `asm/rock_neo/4D060.s`
Bytecmp verdict: **PASS** — 2/2 words match, 0 hard mismatches, 0 reloc flags.

```c
void func_8005C860(void) {
}
```

---

## func_8005D974

Asm source: `asm/rock_neo/4E174.s`
Bytecmp verdict: **PASS** — 2/2 words match, 0 hard mismatches, 0 reloc flags.

```c
void func_8005D974(void) {
}
```

---

No new typedefs, structs, or externs needed for any of the three.
