#!/usr/bin/env python3
"""Compare every built function's WORD COUNT against its reference asm.

Why this exists (2026-07-29): landing verified C by hand is a transcription step,
and transcription can silently drop code. I lost an `if` guard while pasting an
agent-verified body into an overlay C file; the function came out 5 words short,
which SHIFTED the whole text section and made 41,209 bytes differ. The build
still succeeded, and `check_overlays` reported 205/205 because a failed/`stale`
BIN comparison is not the same as a fresh one.

A per-function size check finds that class of error in seconds and points at the
exact function, where a whole-binary cmp only says "everything after 0x834 moved".

    tools/checksizes.py <built.o> <nonmatchings-dir>
    tools/checksizes.py --overlay ST0C     # convenience for a wired archive

Exit 0 = every function matches its reference length. Exit 1 = a size mismatch,
with the function named. This does NOT prove a byte match (use tools/bytecmp.sh
or the real build for that) — it proves nothing was dropped or duplicated.
"""
import os
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
os.chdir(ROOT)

OVERLAYS = {
    "ST0C": ("build/src/ST0C/ovl0__progbin_r3_st0c.bin/Code80100BB0.c.o",
             "asm/ST0C/ovl0__progbin_r3_st0c.bin/nonmatchings/Code80100BB0"),
}


def word_counts_from_obj(obj):
    out = subprocess.run(["mipsel-elf-objdump", "-d", obj],
                         capture_output=True, text=True).stdout
    sizes, cur = {}, None
    for line in out.splitlines():
        m = re.match(r"^[0-9a-f]+ <(\w+)>:", line)
        if m:
            cur = m.group(1)
            sizes[cur] = 0
        elif cur and re.match(r"\s+[0-9a-f]+:\s+[0-9a-f]{8}", line):
            sizes[cur] += 1
    return sizes


def word_count_from_asm(path):
    return len(re.findall(r"/\* \w+ \w+ \w{8} \*/", path.read_text()))


def main(argv):
    if len(argv) == 3 and argv[1] == "--overlay":
        if argv[2] not in OVERLAYS:
            sys.stderr.write(f"unknown overlay {argv[2]}; known: {', '.join(OVERLAYS)}\n")
            return 2
        obj, nmdir = OVERLAYS[argv[2]]
    elif len(argv) == 3:
        obj, nmdir = argv[1], argv[2]
    else:
        sys.stderr.write(__doc__)
        return 2

    if not os.path.exists(obj):
        sys.stderr.write(f"no such object: {obj} (build it first)\n")
        return 2

    sizes = word_counts_from_obj(obj)
    nm = pathlib.Path(nmdir)
    checked, bad = 0, []
    for fn, got in sorted(sizes.items()):
        p = nm / f"{fn}.s"
        if not p.exists():
            continue
        ref = word_count_from_asm(p)
        checked += 1
        if ref != got:
            bad.append((fn, ref, got))

    print(f"checked {checked} functions in {os.path.basename(obj)}")
    if not bad:
        print("all function lengths match their reference asm")
        return 0
    print(f"SIZE MISMATCHES: {len(bad)}")
    for fn, ref, got in bad:
        print(f"  {fn}: reference {ref} words, built {got} "
              f"({'dropped' if got < ref else 'added'} {abs(ref - got)})")
    print("\nA short function usually means code was lost in transcription "
          "(a dropped guard/branch);\nit shifts everything after it, so the "
          "whole-binary diff will look catastrophic.")
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
