#!/usr/bin/env python3
"""Duplicate finder — which work is already done, and which archive to convert next.

The stage archives hold ~10,107 function instances but only ~7,064 distinct bodies. Doing
them archive-by-archive without checking for twins means redoing the same function dozens
of times. MGS's README warns about exactly this; SOTN solved it with dups/mipsmatch.

Reads build/function_map.json (run tools/gen_map.py first).

    tools/dups.py                    # overview: duplication + best archive to convert next
    tools/dups.py --archive ST05     # what fraction of ST05 is already solved elsewhere?
    tools/dups.py --twins            # bodies matched in one place, still raw elsewhere
                                     #   -> the actual copy-paste queue
    tools/dups.py --near             # mnemonic-skeleton families (template transfers)

Two different notions, never conflate them:
  EXACT twin   identical instruction words. The C already written should reproduce it
               verbatim (modulo relocations). Nearly free.
  FAMILY       identical mnemonic skeleton, different registers/constants. Not free, but
               the source SHAPE transfers — solve one, fill in the blanks for the rest.

And neither is "called by N stages". Callers are a readability signal; they move the
matched count by one. Only COPIES multiply.
"""
import json, sys, collections, pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
MAP = ROOT / "build" / "function_map.json"


def load():
    if not MAP.is_file():
        sys.exit("dups: run tools/gen_map.py first (build/function_map.json missing)")
    return json.load(open(MAP))["functions"]


def overview(F):
    stage = [r for r in F if r["realm"] == "STAGE"]
    bodies = collections.Counter(r["body"] for r in stage if r["body"])
    dup = {b: n for b, n in bodies.items() if n > 1}
    print("=== STAGE DUPLICATION ===")
    print(f"  instances            : {len(stage):,}")
    print(f"  distinct bodies      : {len(bodies):,}")
    print(f"  bodies appearing >1x : {len(dup):,}  covering {sum(dup.values()):,} instances")
    print(f"  redundancy factor    : {len(stage)/max(1,len(bodies)):.2f}x")
    print(f"  -> real remaining work is {len(bodies):,} bodies, not {len(stage):,} instances\n")

    # Which archive is cheapest to convert next: most of its bodies already solved,
    # or shared with other archives (so the work pays off more than once).
    matched_bodies = {r["body"] for r in F if r["state"] == "MATCHED" and r["body"]}
    per = collections.defaultdict(lambda: {"n": 0, "done": 0, "shared": 0, "uniq": set()})
    where = collections.defaultdict(set)
    for r in stage:
        where[r["body"]].add(r["container"])
    for r in stage:
        a = per[r["container"]]
        a["n"] += 1
        a["uniq"].add(r["body"])
        if r["body"] in matched_bodies:
            a["done"] += 1
        if len(where[r["body"]]) > 1:
            a["shared"] += 1

    print("=== NEXT ARCHIVE TO CONVERT (ranked by leverage) ===")
    print(f"  {'archive':<10}{'insts':>7}{'unique':>8}{'already':>9}{'shared':>8}   {'leverage':>8}")
    rows = []
    for a, v in per.items():
        lev = (v["done"] + v["shared"]) / max(1, v["n"])
        rows.append((lev, a, v))
    for lev, a, v in sorted(rows, reverse=True)[:12]:
        print(f"  {a:<10}{v['n']:>7}{len(v['uniq']):>8}{v['done']:>9}{v['shared']:>8}   {lev*100:>7.0f}%")
    print("  already = body already MATCHED somewhere · shared = body also in another archive")


def twins(F):
    """Bodies matched in one place but still raw elsewhere — the copy-paste queue."""
    matched = {}
    for r in F:
        if r["state"] == "MATCHED" and r["body"]:
            matched.setdefault(r["body"], r["name"])
    pending = collections.defaultdict(list)
    for r in F:
        if r["state"] != "MATCHED" and r["body"] in matched:
            pending[r["body"]].append(r)
    print("=== ALREADY-SOLVED BODIES STILL SITTING RAW ELSEWHERE ===")
    if not pending:
        print("  none — no matched body has an unmatched twin.")
        print("  (Expected while stage work is young: only eve19 is converted.)")
        return
    tot = 0
    for b, rs in sorted(pending.items(), key=lambda kv: -len(kv[1])):
        tot += len(rs)
        print(f"  {matched[b]:<26} {rs[0]['insns']:>4} insn  -> {len(rs)} unmatched copies "
              f"in {len(({r['container'] for r in rs}))} places")
    print(f"\n  {tot} instances could be closed by reusing C that already exists.")


def near(F):
    fam = collections.defaultdict(list)
    for r in F:
        if r["state"] != "MATCHED" and r["skel"]:
            fam[r["skel"]].append(r)
    big = sorted((g for g in fam.values() if len(g) > 1), key=lambda g: -len(g))
    solved = {r["skel"] for r in F if r["state"] == "MATCHED" and r["skel"]}
    print("=== FAMILIES (same skeleton, different registers/constants) ===")
    print(f"  {len(big)} families covering {sum(len(g) for g in big):,} unmatched functions\n")
    print(f"  {'size':>5} {'insn':>5}  {'template?':<11} example")
    for g in big[:20]:
        has = "SOLVED ONE" if g[0]["skel"] in solved else "-"
        print(f"  {len(g):>5} {g[0]['insns']:>5}  {has:<11} {g[0]['name']} ({g[0]['container']})")
    print("\n  'SOLVED ONE' = a member is already matched; its C is a direct template.")


def archive(F, name):
    stage = [r for r in F if r["realm"] == "STAGE" and r["container"] == name]
    if not stage:
        sys.exit(f"dups: no archive named {name}")
    matched = {r["body"] for r in F if r["state"] == "MATCHED" and r["body"]}
    elsewhere = collections.Counter()
    for r in F:
        if r["realm"] == "STAGE" and r["container"] != name and r["body"]:
            elsewhere[r["body"]] += 1
    uniq = {r["body"] for r in stage}
    done = sum(1 for r in stage if r["body"] in matched)
    shar = sum(1 for r in stage if elsewhere.get(r["body"], 0) > 0)
    print(f"=== {name} ===")
    print(f"  function instances        : {len(stage)}")
    print(f"  distinct bodies           : {len(uniq)}")
    print(f"  already matched elsewhere : {done}")
    print(f"  also present in other archives : {shar}")
    print(f"  genuinely unique to {name}  : {sum(1 for r in stage if elsewhere.get(r['body'],0)==0)}")
    sz = sorted((r["insns"] or 0) for r in stage)
    if sz:
        print(f"  size: median {sz[len(sz)//2]}, ≤30 insn {sum(1 for x in sz if x<=30)}, "
              f">120 insn {sum(1 for x in sz if x>120)}")


if __name__ == "__main__":
    F = load()
    a = sys.argv[1:]
    if "--archive" in a:
        archive(F, a[a.index("--archive") + 1])
    elif "--twins" in a:
        twins(F)
    elif "--near" in a:
        near(F)
    else:
        overview(F)
