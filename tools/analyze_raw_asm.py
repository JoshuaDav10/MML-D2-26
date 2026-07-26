#!/usr/bin/env python3
import re, glob, os, collections

FILES = [f for f in glob.glob("asm/rock_neo/*.s") if os.path.basename(f) != "header.s"]
insn_re = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s*\*/\s*(\S+)")
glabel_re = re.compile(r"^glabel\s+(func_[0-9A-Fa-f]+|D_[0-9A-Fa-f]+|\w+)")

funcs = []  # (name, ninsn, ncalls, has_backbranch, file)
for fn in FILES:
    cur = None
    with open(fn) as fh:
        for line in fh:
            m = glabel_re.match(line)
            if m:
                if cur: funcs.append(cur)
                cur = {"name": m.group(1), "ninsn":0, "ncalls":0, "back":False, "file":os.path.basename(fn)}
                continue
            mi = insn_re.search(line)
            if mi and cur is not None:
                op = mi.group(1)
                cur["ninsn"] += 1
                if op in ("jal","jalr"): cur["ncalls"] += 1
                if op in ("b","beq","bne","blez","bgtz","bltz","bgez","beqz","bnez","bqez"):
                    cur["back"] = True  # approx; refined below not needed for buckets
    if cur: funcs.append(cur)

# only real functions (func_ prefix)
funcs = [f for f in funcs if f["name"].startswith("func_")]
tot = len(funcs); tins = sum(f["ninsn"] for f in funcs)
leaves = [f for f in funcs if f["ncalls"]==0]

print(f"TOTAL raw-asm functions: {tot}")
print(f"TOTAL instructions:      {tins}")
print(f"LEAF (no jal/jalr):      {len(leaves)}  ({100*len(leaves)//tot}%)")
print()

buckets = [("tiny  <=10 insn",0,10),("small 11-25",11,25),("med   26-60",26,60),
           ("large 61-150",61,150),("huge  151-400",151,400),("giant >400",401,10**9)]
print(f"{'bucket':18} {'#funcs':>7} {'cumul%':>7} {'#leaf':>6} {'insns':>9}")
cum=0
for name,lo,hi in buckets:
    b=[f for f in funcs if lo<=f["ninsn"]<=hi]
    cum+=len(b)
    bl=sum(1 for f in b if f["ncalls"]==0)
    bi=sum(f["ninsn"] for f in b)
    print(f"{name:18} {len(b):>7} {100*cum//tot:>6}% {bl:>6} {bi:>9}")

print()
# "easy win" heuristic: small AND leaf (leaf funcs are self-contained, no call-arg matching)
easy = [f for f in funcs if f["ninsn"]<=25 and f["ncalls"]==0]
easyish = [f for f in funcs if f["ninsn"]<=25]
print(f"EASY-WIN candidates (<=25 insn AND leaf): {len(easy)}  ({100*len(easy)//tot}% of 635)")
print(f"Small (<=25 insn, any):                   {len(easyish)}  ({100*len(easyish)//tot}% of 635)")
print()
print("Per-file function counts:")
byfile = collections.Counter(f["file"] for f in funcs)
for k,v in sorted(byfile.items(), key=lambda x:-x[1]):
    fi = sum(x["ninsn"] for x in funcs if x["file"]==k)
    print(f"  {k:12} {v:>4} funcs  {fi:>7} insn")
