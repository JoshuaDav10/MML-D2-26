#!/usr/bin/env python3
"""Structural diff: reference splat .s vs scratch .o disassembly.
Aligns on normalized (opcode, regs) so insert/delete blocks pop out."""
#
# Usage: python3 tools/structdiff.py [ref.s] [obj.o] [funcname]
# Defaults target func_80053B40; run tools/bytecmp.sh first to produce build/scratch/bytecmp.o.
# Aligns normalized (opcode,regs) streams so insert/delete blocks pop out of reg-cascade noise.
import re, subprocess, sys
from difflib import SequenceMatcher

REF = sys.argv[1] if len(sys.argv) > 1 else "asm/rock_neo/nonmatchings/moji/func_80053B40.s"
OBJ = sys.argv[2] if len(sys.argv) > 2 else "build/scratch/bytecmp.o"
FUNC = sys.argv[3] if len(sys.argv) > 3 else "func_80053B40"

def norm(text):
    t = text.strip()
    t = re.sub(r"\s+", " ", t)
    t = t.replace("$", "")
    t = re.sub(r"%(hi|lo|gp_rel)\([^)]*\)", "RELOC", t)
    op = t.split(" ")[0]
    # fold objdump aliases to canonical forms
    extra = ""
    if op == "move":
        op = "addu"
        extra = ",zero"
    elif op == "li":
        op = "addiu"  # close enough for alignment
        extra = ",zero"
    elif op == "negu":
        op = "subu"
    elif op in ("b", "j"):
        op = "j"
    elif op in ("beqz", "beq"):
        op = "beq"
    elif op in ("bnez", "bne"):
        op = "bne"
    regs = re.findall(r"\b(zero|at|v[01]|a[0-3]|t[0-9]|s[0-7]|s8|fp|gp|sp|ra|hi|lo)\b", t)
    # canonical alias forms: li rd,imm == addiu rd,zero,imm; move rd,rs == addu rd,rs,zero
    return op + " " + ",".join(regs) + extra

# --- reference stream ---
ref = []
for line in open(REF):
    m = re.match(r"\s*/\* \w+ \w+ (\w{8}) \*/\s+(\S+)\s*(.*)", line)
    if m:
        word, op, rest = m.groups()
        ref.append((word, op + " " + rest.strip()))

# --- actual stream ---
out = subprocess.run(["mipsel-elf-objdump", "-dr", OBJ], capture_output=True, text=True).stdout
act = []
started = False
for line in out.splitlines():
    if f"<{FUNC}>:" in line:
        started = True
        continue
    if not started:
        continue
    m = re.match(r"\s+[0-9a-f]+:\s+([0-9a-f]{8})\s+(\S+)\s*(.*)", line)
    if m:
        word, op, rest = m.groups()
        rest = re.sub(r"<[^>]*>", "", rest).strip()
        act.append((word, op + " " + rest))
    elif started and re.match(r"^[0-9a-f]+ <", line):
        break

a = [norm(t) for _, t in ref]
b = [norm(t) for _, t in act]
sm = SequenceMatcher(None, a, b, autojunk=False)
neq = 0
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag == "equal":
        continue
    neq += max(i2 - i1, j2 - j1)
    print(f"--- {tag} ref[{i1}:{i2}] act[{j1}:{j2}]")
    for k in range(max(i2 - i1, j2 - j1)):
        l = f"  {ref[i1+k][1]:<50}" if i1 + k < i2 else f"  {'':<50}"
        r = f" | {act[j1+k][1]}" if j1 + k < j2 else " |"
        print(l + r)
print(f"\ntotal structurally-unaligned: {neq} (ref {len(ref)} words, act {len(act)} words)")
