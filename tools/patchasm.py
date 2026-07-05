# patches MIPS assembly output from GCC to fix shenanigans caused by the assembler

import sys
import re

def patch_asm(asm):
    re_li_pattern = r"li\s+(\$\w+),\s*(0x[0-9a-fA-F]+)"
    # Ensure that any li instance where the \2 value is less than 0xFFFF is addiu instead
    # This is because the assembler will automatically convert these lis to ori for whatever reason
    for line in asm.splitlines():
        match = re.match(re_li_pattern, line)
        if match:
            reg, val = match.groups()
            if int(val, 16) < 0xFFFF:
                asm = asm.replace(line, f"addiu {reg}, $0, {val}")

    return asm


# cc1-27 at -O1+ defers all compiled C function bodies to the end of the
# translation unit, after every top-level __asm__ (INCLUDE_ASM) block, which
# scrambles .text layout for files that mix real C with INCLUDE_ASM stubs.
# It does, however, emit a `.globl NAME` at the function's source position.
# This pass moves each deferred body back up to its marker.
ENT_RE = re.compile(r"^\s*\.ent\s+(\S+)")
END_RE = re.compile(r"^\s*\.end\s+(\S+)")
GLOBL_RE = re.compile(r"^\s*\.globl\s+(\S+)")

def reorder_deferred_functions(asm):
    lines = asm.split("\n")

    # Collect .ent/.end blocks. Blocks containing a .include are INCLUDE_ASM
    # chunks (already in the right place); the rest are compiled C bodies.
    blocks = []
    cur = None
    for idx, line in enumerate(lines):
        if cur is None:
            m = ENT_RE.match(line)
            if m:
                cur = [m.group(1), idx, None, False]
                continue
        else:
            if ".include" in line:
                cur[3] = True
            m = END_RE.match(line)
            if m and m.group(1) == cur[0]:
                cur[2] = idx
                blocks.append(tuple(cur))
                cur = None

    deferred = {name: (start, end) for name, start, end, is_inc in blocks if not is_inc}
    if not deferred:
        return asm

    in_block = set()
    for _, start, end, _ in blocks:
        in_block.update(range(start, end + 1))

    # Marker = the lone `.globl NAME` outside any block for a deferred function.
    markers = {}
    for idx, line in enumerate(lines):
        if idx in in_block:
            continue
        m = GLOBL_RE.match(line)
        if m and m.group(1) in deferred and m.group(1) not in markers:
            markers[m.group(1)] = idx

    removed = set()
    inserts = {}  # marker line idx -> body lines
    for name, marker_idx in markers.items():
        start, end = deferred[name]
        if marker_idx > start:
            continue  # body already precedes its marker; don't touch
        inserts[marker_idx] = lines[start:end + 1]
        removed.update(range(start, end + 1))

    out = []
    for idx, line in enumerate(lines):
        if idx in removed:
            continue
        out.append(line)
        if idx in inserts:
            out.extend(inserts[idx])
    return "\n".join(out)


def main():
    asm = reorder_deferred_functions(sys.stdin.read())
    print(patch_asm(asm))
    f = open("build/test.s", "w")
    f.write(patch_asm(asm))

if __name__ == "__main__":
    main()
