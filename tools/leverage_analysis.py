#!/usr/bin/env python3
"""Where is the LEVERAGE? Quantify match-once-cover-many across the whole game.

For a matching decomp, the cheapest path is: match a function ONCE and have that
single C body satisfy every byte-identical copy of it across all binaries. This
script measures how much of the game that covers, and finds the library/engine
core (heavily shared functions) that pays back the most per match.
"""
import re, os, glob, hashlib, collections

insn_re = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s*\*/\s*(\S+)")
glabel_re = re.compile(r"^glabel\s+(func_[0-9A-Fa-f]+)")

def parse(files, tag):
    out = []
    for fn in files:
        cur = None
        with open(fn, errors="ignore") as fh:
            for line in fh:
                m = glabel_re.match(line)
                if m:
                    if cur: out.append(cur)
                    cur = {"name":m.group(1),"file":fn,"tag":tag,"words":[],"ops":set()}
                    continue
                mi = insn_re.search(line)
                if mi and cur is not None:
                    cur["words"].append(mi.group(1)); cur["ops"].add(mi.group(2))
        if cur: out.append(cur)
    return [f for f in out if f["words"]]

overlay_files = [f for f in glob.glob("asm/**/*.s", recursive=True)
    if not f.startswith("asm/rock_neo/") and "header" not in os.path.basename(f)
    and f"{os.sep}data{os.sep}" not in f]
main_files = glob.glob("asm/rock_neo/*.s")  # the 7 giant un-split files

ov = parse(overlay_files, "ovl")
mn = parse(main_files, "main")

def h(f): return hashlib.md5("".join(f["words"]).encode()).hexdigest()

# --- overlay leverage: copies per unique, cumulative coverage ---
groups = collections.defaultdict(list)
for f in ov: groups[h(f)].append(f)
uniq = sorted(groups.values(), key=lambda g: -len(g))
inst = sum(len(g) for g in uniq)
print(f"=== OVERLAY MATCH-ONCE LEVERAGE ===")
print(f"instances (copies) : {inst}")
print(f"unique functions   : {len(uniq)}")
print(f"if you match unique funcs MOST-COPIED first, cumulative copies covered:")
cum=0
for k in (25,50,100,200,500,1000,2000,len(uniq)):
    cum = sum(len(g) for g in uniq[:k])
    print(f"  first {k:5} unique matches -> {cum:6} / {inst} copies covered ({100*cum//inst}%)")

# --- library/engine core: heavily shared, likely stock code ---
shared = [g for g in uniq if len(g) >= 10]
sh_inst = sum(len(g) for g in shared)
sh_leaf = sum(1 for g in shared if "jal" not in g[0]["ops"] and "jalr" not in g[0]["ops"])
print(f"\n=== LIBRARY / ENGINE-CORE CANDIDATES (appear >=10x) ===")
print(f"{len(shared)} unique funcs, covering {sh_inst} copies; {sh_leaf} are leaves")
print("top 15 by copies (size in insn, leaf?):")
for g in uniq[:15]:
    f=g[0]; leaf = not ({"jal","jalr"} & f["ops"])
    print(f"  {len(g):4}x  {len(f['words']):4} insn  {'leaf' if leaf else 'calls'}  {f['name']}")

# --- main-exe <-> overlay overlap (byte-identical shared code) ---
ovh = set(groups.keys())
mnh = collections.defaultdict(list)
for f in mn: mnh[h(f)].append(f)
overlap = [hh for hh in mnh if hh in ovh]
ov_inst_in_overlap = sum(len(groups[hh]) for hh in overlap)
print(f"\n=== MAIN-EXE  <->  OVERLAY  byte-identical overlap ===")
print(f"main-exe raw funcs         : {len(mn)}  ({len(mnh)} unique)")
print(f"main funcs also in overlays: {len(overlap)} unique  "
      f"-> cover {ov_inst_in_overlap} overlay copies + {sum(len(mnh[hh]) for hh in overlap)} main copies")
print("(these are shared engine/library; one C body could satisfy BOTH sides)")

# --- small-leaf fast-win pools ---
def pool(fs):
    u = {}
    for f in fs: u.setdefault(h(f), f)
    small_leaf = [f for f in u.values() if len(f["words"])<=25 and not ({"jal","jalr"}&f["ops"])]
    return len(u), len(small_leaf)
ou, osl = pool(ov); muc = len({h(f):f for f in mn});
print(f"\n=== FAST-WIN POOLS (unique, <=25 insn, leaf) ===")
print(f"overlay unique small-leaf : {osl}")
