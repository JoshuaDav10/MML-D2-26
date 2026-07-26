#!/usr/bin/env python3
"""Rigorous whole-game scope: count overlay functions, dedup by content.

Answers: how many UNIQUE functions exist across the 205 overlays, and how much
of the ~10k label count is duplication (same actor/library code recompiled into
many overlays)?

Dedup is done two ways:
  exact   = hash of the raw instruction words (finds bit-identical copies; works
            when overlays share a load address, which sibling actor overlays do)
  norm    = hash after masking the low 26 bits of jal targets and the low 16 bits
            of lui/addiu/ori/branch immediates -> finds functions identical modulo
            relocation/address fixups (the true "same source function" count)
"""
import re, os, glob, hashlib, collections

ROOT = "asm"
insn_re = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s*\*/\s*(\S+)")
glabel_re = re.compile(r"^glabel\s+(func_[0-9A-Fa-f]+)")

def overlay_ext(path):
    # asm/ST00/ovl0__PROGBIN_R3_ST00.BIN/... -> BIN
    parts = path.split(os.sep)
    for p in parts:
        if p.startswith("ovl") and "." in p:
            return p.rsplit(".", 1)[1]
    return "?"

def norm_word(hexw, op):
    w = int(hexw, 16)
    # mask address-dependent fields so relocated copies hash equal
    if op in ("jal", "j"):
        w &= 0xFC000000
    elif op in ("lui", "addiu", "ori", "andi", "lw", "sw", "lb", "lbu", "lh",
                "lhu", "sb", "sh", "beq", "bne", "blez", "bgtz", "bltz", "bgez",
                "b", "beqz", "bnez", "slti", "sltiu", "li", "la"):
        w &= 0xFFFF0000  # drop the 16-bit immediate/offset
    return w

funcs = []  # (name, ext, file, exact_words[list], norm_words[list])
files = [f for f in glob.glob(f"{ROOT}/**/*.s", recursive=True)
         if not f.startswith(f"{ROOT}/rock_neo/")
         and "header" not in os.path.basename(f)
         and f"{os.sep}data{os.sep}" not in f]

for fn in files:
    ext = overlay_ext(fn)
    cur = None
    with open(fn, errors="ignore") as fh:
        for line in fh:
            m = glabel_re.match(line)
            if m:
                if cur: funcs.append(cur)
                cur = [m.group(1), ext, fn, [], []]
                continue
            mi = insn_re.search(line)
            if mi and cur is not None:
                hexw, op = mi.group(1), mi.group(2)
                cur[3].append(hexw)
                cur[4].append(format(norm_word(hexw, op), "08x"))
    if cur: funcs.append(cur)

total = len(funcs)
exact = {}
norm = {}
for name, ext, fn, ew, nw in funcs:
    if not ew: continue
    eh = hashlib.md5(("".join(ew)).encode()).hexdigest()
    nh = hashlib.md5(("".join(nw)).encode()).hexdigest()
    exact.setdefault(eh, []).append((name, ext))
    norm.setdefault(nh, []).append((name, ext))

nonempty = sum(1 for f in funcs if f[3])
print(f"=== OVERLAY SCOPE (205 overlays, excl rock_neo + data/ + headers) ===")
print(f"total function labels        : {total}")
print(f"  with >=1 instruction       : {nonempty}")
print(f"UNIQUE by exact bytes        : {len(exact)}   (dup factor {nonempty/max(len(exact),1):.1f}x)")
print(f"UNIQUE modulo relocation     : {len(norm)}   (dup factor {nonempty/max(len(norm),1):.1f}x)")
print()

# functions by asset type
byext = collections.Counter(f[1] for f in funcs if f[3])
print("function labels by overlay asset type:")
for ext, n in byext.most_common():
    print(f"  .{ext:6} {n:6}")
print()

# size profile of the UNIQUE (normalized) set -> how much real new work
uniq_sizes = []
seen = set()
for name, ext, fn, ew, nw in funcs:
    if not nw: continue
    nh = hashlib.md5(("".join(nw)).encode()).hexdigest()
    if nh in seen: continue
    seen.add(nh); uniq_sizes.append(len(nw))
uniq_sizes.sort()
tot_insn = sum(uniq_sizes)
print(f"UNIQUE (norm) function set: {len(uniq_sizes)} funcs, {tot_insn} instructions")
for lbl, lo, hi in [("<=10",0,10),("11-25",11,25),("26-60",26,60),
                    ("61-150",61,150),("151-400",151,400),(">400",401,10**9)]:
    c = sum(1 for s in uniq_sizes if lo<=s<=hi)
    print(f"  {lbl:8} {c:5}")

# --- copies-per-unique histogram: is there an uncollapsed shared core? ---
print()
print("copies-per-unique-function (exact-byte groups):")
copies = collections.Counter(len(v) for v in exact.values())
for n in sorted(copies):
    if n<=5 or n%5==0 or n>=20:
        print(f"  appears {n:2}x : {copies[n]:5} functions")
# how many DISTINCT top-level overlay dirs does each unique func span?
def topdir(fn): return fn.split(os.sep)[1]
spans = []
funcs_by_eh = collections.defaultdict(set)
for name, ext, fn, ew, nw in funcs:
    if not ew: continue
    eh = hashlib.md5(("".join(ew)).encode()).hexdigest()
    funcs_by_eh[eh].add(topdir(fn))
multi = sum(1 for s in funcs_by_eh.values() if len(s)>1)
print(f"unique funcs shared across >1 stage dir: {multi} of {len(funcs_by_eh)}")
print(f"unique funcs in exactly ONE stage dir : {len(funcs_by_eh)-multi}")
