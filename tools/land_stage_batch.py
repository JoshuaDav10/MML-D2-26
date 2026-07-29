#!/usr/bin/env python3
"""Land a drafting agent's report into a stage archive (and its twins).

    tools/land_stage_batch.py <report.md> <SEG> <ARCH> [TWIN ...]
    e.g. tools/land_stage_batch.py /tmp/st11_batch2.md Code801000B4 ST11 ST11B

Takes the ```c blocks out of an agent report, keeps only the FUNCTION BODIES, and swaps
each one in for its INCLUDE_ASM stub in every named archive.

Why it discards the agent's typedefs: cc1 with -lang-c keeps the pre-ANSI global member
namespace, so `work->x328` compiles even when the struct has no such field — it silently
resolves against some other struct and the body can be eliminated entirely. Two functions
became 2-word empty bodies that way while the build exited 0. So the archive's file keeps
ONE proven typedef block and agent-supplied layouts are never trusted.

Prints what it changed. Verify afterwards, always:
    make CPP=cpp chunks           # CHECK THE EXIT CODE
    cmp build/<A>.BIN disks/us/CDDATA/DAT/<A>.BIN     # per archive
    tools/checksizes.py build/src/<A>/<ovl>/<SEG>.c.o asm/<A>/<ovl>/nonmatchings/<SEG>
"""
import re, sys, pathlib

if len(sys.argv) < 4:
    sys.exit(__doc__)
report, seg, archives = sys.argv[1], sys.argv[2], sys.argv[3:]

md = pathlib.Path(report).read_text()
bodies = {}
for blk in re.findall(r"```c\n(.*?)```", md, re.S):
    m = re.search(r"^[A-Za-z_][\w \*]*\b(\w+)\s*\([^;{]*\)\s*\{", blk, re.M)
    if not m:
        continue
    i = blk.index("{", m.start())
    depth = 0
    j = i
    while j < len(blk):
        if blk[j] == "{":
            depth += 1
        elif blk[j] == "}":
            depth -= 1
            if depth == 0:
                break
        j += 1
    bodies[m.group(1)] = blk[m.start():j + 1]

if not bodies:
    sys.exit(f"land: no function bodies found in {report}")
print(f"land: {len(bodies)} bodies from {report}")

for arch in archives:
    hits = list(pathlib.Path("src").glob(f"{arch}/*/{seg}.c"))
    if not hits:
        print(f"  {arch}: no src/{arch}/*/{seg}.c — skipped")
        continue
    p = hits[0]
    t = p.read_text()
    n = missing = 0
    for fn, body in bodies.items():
        pat = re.compile(r'^INCLUDE_ASM\("[^"]*",\s*' + re.escape(fn) + r'\);[ \t]*$', re.M)
        if pat.search(t):
            t = pat.sub(lambda _m: body, t, count=1)
            n += 1
        elif f" {fn}(" in t:
            pass                      # already landed
        else:
            missing += 1
            print(f"  {arch}: WARNING {fn} has no stub here")
    p.write_text(t)
    print(f"  {arch}: landed {n}" + (f", {missing} not found" if missing else ""))
