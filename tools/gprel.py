#!/usr/bin/env python3
"""gp-relative (sdata) pass, run between maspsx and patchasm.

cc1-27 emits `.extern NAME, SIZE` for small extern scalars and bare memory
operands (`sb $2,NAME`), delegating the -G8 sdata decision to the assembler.
Our GNU as runs with -G0, which (a) expands every bare ref to lui/$at form
(never $gp) and (b) turns the small `.extern` into a COMMON symbol that the
linker allocates at a bogus address, shifting all data after it.

This pass:
  1. collects symbols from `.extern NAME, SIZE` with 0 < SIZE <= 8 and drops
     ALL small .extern directives, census-approved or not (prevents the
     COMMON allocation; the real definitions live in the extracted asm),
  2. rewrites bare load/store operands on those symbols to explicit
     `%gp_rel(NAME)($gp)`, which GNU as assembles to a single gp-relative
     instruction (same as the original binary),
  3. when maspsx deleted a load-delay nop before such an instruction on the
     assumption it would expand to a multi-instruction lui/$at sequence
     (left behind as a `#nop # DEBUG: ... uses $at` comment), re-materializes
     the real nop -- the gp form is a single instruction, so the original
     binary has the nop.

It must run AFTER maspsx: feeding maspsx explicit %gp_rel operands makes it
mangle them (it treats the operand as a bare symbol needing $at expansion).

Per-symbol control stays in the C declaration:
  extern u8 foo;    -> .extern foo, 1  -> gp-relative   (sdata symbol)
  extern u8 foo[];  -> no .extern      -> lui/%lo form  (regular symbol)

Which symbols are actually gp-accessed is decided by a census of the
extracted original disassembly: any symbol appearing as `%gp_rel(SYM)` in
asm/rock_neo/**/*.s is an sdata symbol in the original binary; everything
else keeps the GAS -G0 lui/$at expansion (which is what the original uses
for it, e.g. joy.h's D_800C0C26). Being within gp range is NOT sufficient
-- plenty of in-range symbols are lui-accessed by the original.

Not handled (extend when a target function needs it): `la $reg,NAME` on an
sdata symbol (address-of taken as a value) should become
`addiu $reg,$gp,%gp_rel(NAME)`.
"""
import os
import re
import sys

EXTERN_RE = re.compile(r"^\s*\.extern\s+([A-Za-z_$][\w$]*)\s*,\s*(\d+)\s*$")

MEM_OPS = (
    "lb", "lbu", "lh", "lhu", "lw", "lwl", "lwr",
    "sb", "sh", "sw", "swl", "swr",
    "lwc1", "swc1", "lwc2", "swc2",
)
# op $reg, SYM   or   op $reg, SYM+4 / SYM-4  (no parens, no % reloc, no ($reg))
MEM_RE = re.compile(
    r"^(\s*)(" + "|".join(MEM_OPS) + r")\s+(\$\w+)\s*,\s*"
    r"([A-Za-z_$][\w$]*(?:\s*[+-]\s*\d+)?)\s*$"
)
DELETED_NOP_RE = re.compile(r"^\s*#nop\b.*uses \$at")

ASM_ROOT = "asm/rock_neo"
GP_REL_RE = re.compile(r"%gp_rel\(([A-Za-z_$][\w$]*)")


def gp_census():
    """Symbols the original binary accesses gp-relative, per extracted asm."""
    syms = set()
    for dirpath, _, filenames in os.walk(ASM_ROOT):
        for fn in filenames:
            if not fn.endswith(".s"):
                continue
            try:
                with open(os.path.join(dirpath, fn)) as f:
                    text = f.read()
            except OSError:
                continue
            if "%gp_rel" in text:
                syms.update(GP_REL_RE.findall(text))
    return syms


def main():
    lines = sys.stdin.read().splitlines()

    small_externs = set()
    for line in lines:
        m = EXTERN_RE.match(line)
        if m and 0 < int(m.group(2)) <= 8:
            small_externs.add(m.group(1))

    sdata_syms = small_externs & gp_census() if small_externs else set()
    for name in sorted(small_externs - sdata_syms):
        print(
            f"gprel.py: note: small extern {name} not gp-accessed in "
            "original asm; leaving it non-gp",
            file=sys.stderr,
        )

    out = []
    for line in lines:
        m = EXTERN_RE.match(line)
        if m and m.group(1) in small_externs:
            # drop ALL small .extern directives (not just sdata ones): their
            # only effects are the sdata hint (handled here via the census)
            # and letting GAS emit a COMMON that the linker allocates at a
            # bogus address, silently shifting the data segment
            continue
        m = MEM_RE.match(line)
        if m:
            indent, op, reg, target = m.groups()
            base_sym = re.match(r"[A-Za-z_$][\w$]*", target).group(0)
            if base_sym in sdata_syms:
                target = target.replace(" ", "")
                line = f"{indent}{op}\t{reg},%gp_rel({target})($gp)"
                # maspsx dropped the load-delay nop assuming this would
                # expand through $at; single gp-relative instruction needs it
                if out and DELETED_NOP_RE.match(out[-1]):
                    out[-1] = "nop"
        out.append(line)

    sys.stdout.write("\n".join(out) + "\n")


if __name__ == "__main__":
    main()
