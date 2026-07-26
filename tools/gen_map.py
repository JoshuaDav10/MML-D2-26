#!/usr/bin/env python3
"""THE FUNCTION MAP — one row per function in the whole game, generated from the tree.

Why this exists
---------------
For months this project reported completion as `matched / 484`, where 484 was simply
"functions somebody had already carved into a C file". It was the size of the workbench,
not the size of the game, and it overstated real completion by ~16x. The fix is not a
better-worded doc; it is a map whose denominator is derived from the BINARY, in which
every function in the game appears exactly once with an explicit state.

Nothing here is hand-typed. Regenerate any time:

    tools/gen_map.py                 # write build/function_map.json + notes/FUNCTION_MAP.md
    tools/gen_map.py --check         # exit 1 if the committed map is stale (gate mode)
    tools/gen_map.py --query STATE   # list functions in a state, e.g. --query UNSPLIT

Vocabulary (the states a function can be in)
--------------------------------------------
  MATCHED   real C in the tree that recompiles to the identical bytes. THE deliverable.
  STUB      carved into a C file, but the body is still INCLUDE_ASM (original assembly
            pasted in). Bookkeeping done, work not done. Splitting a function moves it
            UNSPLIT -> STUB and is NOT progress.
  UNSPLIT   engine function still sitting in a raw asm chunk, no C file at all.
  STAGE     lives in a stage overlay. No C pipeline has been set up for its archive yet
            (1 of 37 code archives has been converted). Not blocked - just unstarted.

Realms
------
  ENGINE    ROCK_NEO.EXE. Resident in RAM the whole time you play. Every stage calls it.
  STAGE     one of the 205 CDDATA/DAT archives. Only 37 contain code; 168 are pure asset
            packs (textures/sound/fonts) with zero functions and nothing to decompile.

Duplication
-----------
  copies    how many byte-identical instances of this exact body exist game-wide. A
            function with 342 copies is one decompilation that covers 342 instances.
  family    functions sharing an identical MNEMONIC skeleton but differing in registers
            and constants. Not free, but the C shape transfers - solve one, template the
            rest.
"""
import re, os, sys, json, glob, hashlib, collections, subprocess, pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
os.chdir(ROOT)

GLABEL = re.compile(r"^glabel\s+(\S+)")
INSN = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s*\*/\s*(\S+)")
CALLS = ("jal", "jalr")


def parse_asm(path):
    """Yield {name, words, ops} for each function in an asm file."""
    cur = None
    with open(path, errors="ignore") as fh:
        for line in fh:
            m = GLABEL.match(line)
            if m:
                if cur and cur["words"]:
                    yield cur
                cur = {"name": m.group(1), "words": [], "ops": []}
                continue
            mi = INSN.search(line)
            if mi and cur is not None:
                cur["words"].append(mi.group(1))
                cur["ops"].append(mi.group(2))
    if cur and cur["words"]:
        yield cur


def code_files(root_glob, skip_engine):
    """Asm files that hold CODE. Excludes data/ dirs and chunk headers - `glabel` also
    marks DATA labels, and forgetting that inflates the function count ~20x."""
    out = []
    for f in glob.glob(root_glob, recursive=True):
        if skip_engine and f.startswith("asm/rock_neo/"):
            continue
        if not skip_engine and not f.startswith("asm/rock_neo/"):
            continue
        b = os.path.basename(f)
        if "header" in b or f"{os.sep}data{os.sep}" in f:
            continue
        out.append(f)
    return sorted(out)


def cpp_stub_names():
    """Names still pulled in as INCLUDE_ASM AFTER cpp. Must be ifdef-aware: game.c and
    sub_scrn.c #define ACCEPT_REORDERING_BULLSHIT, so some #ifndef-guarded stubs actually
    compile their #else body. A raw grep over-counts and caused a real inflation event."""
    sys.path.insert(0, str(ROOT / "tools"))
    import census

    names = set()
    for c in sorted((ROOT / "src" / "rock_neo").glob("*.c")):
        for _folder, name in census.cpp_stubs(c):
            names.add(name)
    return names


def objdump_engine_c():
    """Functions present in the built engine C objects, with instruction counts."""
    out = {}
    for o in glob.glob("build/src/rock_neo/*.o"):
        try:
            txt = subprocess.run(["mipsel-elf-objdump", "-t", o],
                                 capture_output=True, text=True, check=True).stdout
        except Exception:
            return {}
        for line in txt.splitlines():
            m = re.match(r"^([0-9a-f]+)\s+.*\sF\s+\.text\s+([0-9a-f]+)\s+(\S+)", line)
            if m:
                out[m.group(3)] = int(m.group(2), 16) // 4
    return out


def build():
    rows = []

    # ---- ENGINE: functions already in C translation units -------------------
    engine_c = objdump_engine_c()
    stubs = cpp_stub_names() if engine_c else set()
    for name, size in engine_c.items():
        rows.append(dict(name=name, realm="ENGINE", container="src/rock_neo",
                         state="STUB" if name in stubs else "MATCHED",
                         insns=size, leaf=None, body=None, skel=None))

    # ---- ENGINE: still raw ---------------------------------------------------
    for f in code_files("asm/rock_neo/*.s", skip_engine=False):
        for fn in parse_asm(f):
            rows.append(dict(name=fn["name"], realm="ENGINE", container=os.path.basename(f),
                             state="UNSPLIT", insns=len(fn["words"]),
                             leaf=not any(o in CALLS for o in fn["ops"]),
                             body=hashlib.md5("".join(fn["words"]).encode()).hexdigest(),
                             skel=hashlib.md5(",".join(fn["ops"]).encode()).hexdigest()))

    # ---- STAGES --------------------------------------------------------------
    for f in code_files("asm/**/*.s", skip_engine=True):
        archive = f.split("/")[1]
        for fn in parse_asm(f):
            rows.append(dict(name=fn["name"], realm="STAGE", container=archive,
                             state="STAGE", insns=len(fn["words"]),
                             leaf=not any(o in CALLS for o in fn["ops"]),
                             body=hashlib.md5("".join(fn["words"]).encode()).hexdigest(),
                             skel=hashlib.md5(",".join(fn["ops"]).encode()).hexdigest()))

    # ---- duplication + families ---------------------------------------------
    bodies = collections.Counter(r["body"] for r in rows if r["body"])
    skels = collections.Counter(r["skel"] for r in rows if r["skel"])
    for r in rows:
        r["copies"] = bodies.get(r["body"], 1) if r["body"] else 1
        r["family_size"] = skels.get(r["skel"], 1) if r["skel"] else 1

    uniq_stage = len({r["body"] for r in rows if r["realm"] == "STAGE"})
    engine_n = sum(1 for r in rows if r["realm"] == "ENGINE")
    matched = sum(1 for r in rows if r["state"] == "MATCHED")
    return rows, dict(engine_total=engine_n, stage_instances=sum(1 for r in rows if r["realm"] == "STAGE"),
                      stage_unique=uniq_stage, whole_game=engine_n + uniq_stage, matched=matched)


def render(rows, s):
    by = collections.Counter(r["state"] for r in rows)
    eng = [r for r in rows if r["realm"] == "ENGINE"]
    stg = [r for r in rows if r["realm"] == "STAGE"]
    arch = collections.Counter(r["container"] for r in stg)
    pct = lambda a, b: f"{100*a/b:.1f}%" if b else "n/a"

    L = []
    A = L.append
    A("# The Function Map\n")
    A("<!-- GENERATED BY tools/gen_map.py — DO NOT EDIT. `tools/gen_map.py --check` fails on drift. -->\n")
    A("Every function in the game, exactly once, with an explicit state. The denominator")
    A("is derived from the binary, not from what a script happens to parse — that mistake")
    A("is what produced a completion figure ~16x too high for months.\n")

    A("## Birds-eye\n")
    A("| realm | what it is | functions | done |")
    A("|---|---|---|---|")
    A(f"| **ENGINE** | `ROCK_NEO.EXE`. Resident in RAM always; every stage calls into it. | **{s['engine_total']:,}** | **{s['matched']}** ({pct(s['matched'], s['engine_total'])}) |")
    A(f"| **STAGES** | 37 code archives (168 more are asset-only). {s['stage_instances']:,} copies of {s['stage_unique']:,} unique bodies. | **{s['stage_unique']:,}** | 0 |")
    A(f"| **WHOLE GAME** | | **{s['whole_game']:,}** | **{s['matched']}** ({pct(s['matched'], s['whole_game'])}) |")
    A("")

    A("## States\n")
    A("| state | meaning | count |")
    A("|---|---|---|")
    A(f"| `MATCHED` | real C, recompiles to identical bytes. **The deliverable.** | **{by['MATCHED']:,}** |")
    A(f"| `STUB` | in a C file but body is still `INCLUDE_ASM`. Bookkeeping only. | {by['STUB']:,} |")
    A(f"| `UNSPLIT` | engine function still in a raw asm chunk, no C file. | {by['UNSPLIT']:,} |")
    A(f"| `STAGE` | in a stage archive; C pipeline not yet set up for it (1 of 37 done). | {by['STAGE']:,} |")
    A("")
    A("> Moving `UNSPLIT -> STUB` is a *split*: bookkeeping, **not progress**. Only")
    A("> `-> MATCHED` counts.\n")

    A("## Engine — the reachable target\n")
    A("| container | functions | matched | stub | unsplit |")
    A("|---|---|---|---|---|")
    for c in sorted({r["container"] for r in eng}):
        g = [r for r in eng if r["container"] == c]
        A(f"| `{c}` | {len(g)} | {sum(1 for r in g if r['state']=='MATCHED')} | "
          f"{sum(1 for r in g if r['state']=='STUB')} | {sum(1 for r in g if r['state']=='UNSPLIT')} |")
    A("")

    A("### Engine work pools (not yet MATCHED)\n")
    todo = [r for r in eng if r["state"] != "MATCHED"]
    A("| pool | count |")
    A("|---|---|")
    for lim in (10, 20, 30, 50, 120):
        A(f"| ≤{lim} instructions | {sum(1 for r in todo if r['insns'] and r['insns'] <= lim)} |")
    A(f"| leaf (calls nothing) | {sum(1 for r in todo if r['leaf'])} |")
    A(f"| in a family of ≥2 (template transfers) | {sum(1 for r in todo if r['family_size'] > 1)} |")
    A(f"| >120 instructions (**nothing this big has ever matched**) | {sum(1 for r in todo if r['insns'] and r['insns'] > 120)} |")
    A("")

    A("## Stages — where the volume is\n")
    A(f"{len(arch)} archives contain code; 168 of the 205 are asset packs with zero functions.\n")
    A("| archive | function copies |")
    A("|---|---|")
    for a, n in arch.most_common(10):
        A(f"| `{a}` | {n} |")
    A(f"| … {max(0,len(arch)-10)} more | |")
    A("")

    A("### Duplication — real match-once-cover-many\n")
    seen, dup = set(), []
    for r in sorted(rows, key=lambda r: -r["copies"]):
        if r["copies"] > 1 and r["body"] not in seen:
            seen.add(r["body"]); dup.append(r)
    A(f"{len(dup):,} distinct bodies appear more than once, covering "
      f"{sum(r['copies'] for r in dup):,} instances.\n")
    A("| function | instructions | copies | leaf |")
    A("|---|---|---|---|")
    for r in dup[:12]:
        A(f"| `{r['name']}` | {r['insns']} | **{r['copies']}** | {'yes' if r['leaf'] else 'no'} |")
    A("")
    A("> Do not confuse this with *callers*. \"Called by 37 stages\" moves the matched")
    A("> count by **one** — stages `jal` into the resident engine, they hold no copy.")
    A("> Copies are the only thing that multiplies.\n")

    A("## How to use this\n")
    A("```\ntools/gen_map.py            # regenerate\n"
      "tools/gen_map.py --check    # gate: fails if committed map is stale\n"
      "tools/gen_map.py --query UNSPLIT --max-insns 20 --leaf\n```\n")
    A("Full per-function detail: `build/function_map.json`.")
    return "\n".join(L) + "\n"


def main():
    args = sys.argv[1:]
    rows, s = build()

    if "--query" in args:
        st = args[args.index("--query") + 1]
        mx = int(args[args.index("--max-insns") + 1]) if "--max-insns" in args else 10**9
        leaf = "--leaf" in args
        sel = [r for r in rows if r["state"] == st and (r["insns"] or 0) <= mx
               and (r["leaf"] if leaf else True)]
        for r in sorted(sel, key=lambda r: (r["insns"] or 0)):
            print(f"{r['insns']:5d}  {r['name']:<28} {r['container']:<22} "
                  f"copies={r['copies']:<4} family={r['family_size']}")
        print(f"\n{len(sel)} functions", file=sys.stderr)
        return 0

    md = render(rows, s)
    if "--check" in args:
        cur = pathlib.Path("notes/FUNCTION_MAP.md")
        if not cur.is_file() or cur.read_text() != md:
            print("FUNCTION_MAP.md is stale — run tools/gen_map.py")
            return 1
        print("function map current")
        return 0

    pathlib.Path("notes/FUNCTION_MAP.md").write_text(md)
    pathlib.Path("build").mkdir(exist_ok=True)
    pathlib.Path("build/function_map.json").write_text(json.dumps(dict(summary=s, functions=rows), indent=1))
    print(f"gen_map: {s['whole_game']:,} functions mapped "
          f"({s['engine_total']:,} engine + {s['stage_unique']:,} unique stage), "
          f"{s['matched']} matched -> notes/FUNCTION_MAP.md + build/function_map.json")
    return 0


if __name__ == "__main__":
    sys.exit(main())
