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


def linked_chunks():
    """Engine chunk basenames the generated linker script actually pulls in.

    Stale chunk files linger in asm/rock_neo/ after a re-split changes segment
    boundaries (they are gitignored, so nothing cleans them). Counting them
    double-counts functions that also live in a live chunk — it inflated the engine
    total from 1,119 to 1,345. Same principle as census's phantom guard: count what
    the BINARY contains, not what happens to be on disk."""
    ld = ROOT / "rock_neo.ld"
    if not ld.is_file():
        return None
    return set(re.findall(r"build/asm/rock_neo/([0-9A-Fa-f]+)\.s\.o", ld.read_text()))


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
        if not skip_engine:
            if "nonmatchings" in f:
                continue        # per-function copies; the chunk already covers them
            lk = linked_chunks()
            ld = (ROOT / "rock_neo.ld").read_text() if (ROOT / "rock_neo.ld").is_file() else ""
            if lk is not None and b[:-2] not in lk and f"psxsdk/{b}.o" not in ld:
                continue        # orphan chunk: on disk, not in the binary
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
    ld = (ROOT / "rock_neo.ld").read_text() if (ROOT / "rock_neo.ld").is_file() else ""
    for o in glob.glob("build/src/rock_neo/*.o"):
        # Skip stale objects whose .c no longer exists, and objects the linker script
        # does not reference — neither is in the binary. (A deleted TU leaves its .o
        # behind; counting it re-creates the phantom-match bug in a second tool.)
        src = pathlib.Path("src/rock_neo") / os.path.basename(o)[:-2]
        if ld and (not src.is_file() or f"src/rock_neo/{src.name}.o" not in ld):
            continue
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


def objdump_stage_c():
    """Functions present in built STAGE overlay C objects: {(archive, name): insns}.

    Mirrors objdump_engine_c() for src/<ARCHIVE>/<overlay>/*.c. Linkage is proven from
    the overlay's OWN map (build/<ARCHIVE>.<overlay>.map): an object the linker never
    pulled in is not in the binary, and counting it would recreate the phantom-match
    bug in a third place. Added 2026-07-26, when the first stage matches landed and
    every counting tool was structurally unable to see them.
    """
    out = {}
    for o in sorted(glob.glob("build/src/*/*/*.o")):
        parts = o.split("/")                     # build / src / ARCH / OVL / file.c.o
        if len(parts) != 5 or parts[2] == "rock_neo":
            continue
        arch, ovl, obj = parts[2], parts[3], parts[4]
        if not (pathlib.Path("src") / arch / ovl / obj[:-2]).is_file():
            continue                             # stale .o, source deleted
        mp = pathlib.Path("build") / f"{arch}.{ovl}.map"
        if mp.is_file() and o not in mp.read_text():
            continue                             # compiled but never linked
        try:
            txt = subprocess.run(["mipsel-elf-objdump", "-t", o],
                                 capture_output=True, text=True, check=True).stdout
        except Exception:
            continue
        for line in txt.splitlines():
            m = re.match(r"^([0-9a-f]+)\s+.*\sF\s+\.text\s+([0-9a-f]+)\s+(\S+)", line)
            if m:
                out[(arch, m.group(3))] = int(m.group(2), 16) // 4
    return out


def cpp_stub_names_stage():
    """(archive, name) still pulled in as INCLUDE_ASM after cpp, for stage C files."""
    sys.path.insert(0, str(ROOT / "tools"))
    import census

    names = set()
    for c in sorted(glob.glob("src/*/*/*.c")):
        arch = c.split("/")[1]
        if arch == "rock_neo":
            continue
        try:
            for _folder, name in census.cpp_stubs(ROOT / c):
                names.add((arch, name))
        except Exception:
            continue
    return names



_NONMATCH_IDX = None


def nonmatching_index():
    """name -> path of the per-function asm kept under any nonmatchings/ directory.

    Matched functions are C now, so there is no assembly in their .o to fingerprint —
    which made family/duplicate detection blind to 350 of 362 matches and silently
    UNDERCOUNT how many templates we already own. But splat leaves the original
    per-function .s on disk even after a match lands, so the fingerprint is recoverable.
    Added 2026-07-28 after dups.py found a 669-member family whose template we already
    had, and could only see it because the two solved members happened to be stage
    functions (the only ones then carrying a hash).
    """
    global _NONMATCH_IDX
    if _NONMATCH_IDX is None:
        _NONMATCH_IDX = {}
        for f in glob.glob("asm/**/nonmatchings/**/*.s", recursive=True):
            _NONMATCH_IDX.setdefault(os.path.basename(f)[:-2], f)
    return _NONMATCH_IDX


def fingerprint(name):
    """(body_hash, skel_hash, leaf, insns) from the archived asm, or Nones."""
    path = nonmatching_index().get(name)
    if not path:
        return None, None, None, None
    for fn in parse_asm(path):
        if fn["name"] != name:
            continue
        return (hashlib.md5("".join(fn["words"]).encode()).hexdigest(),
                hashlib.md5(",".join(fn["ops"]).encode()).hexdigest(),
                not any(o in CALLS for o in fn["ops"]),
                len(fn["words"]))
    return None, None, None, None


def build():
    rows = []

    # ---- ENGINE: functions already in C translation units -------------------
    engine_c = objdump_engine_c()
    stubs = cpp_stub_names() if engine_c else set()
    for name, size in engine_c.items():
        b, sk, lf, _n = fingerprint(name)
        rows.append(dict(name=name, realm="ENGINE", container="src/rock_neo",
                         state="STUB" if name in stubs else "MATCHED",
                         insns=size, leaf=lf, body=b, skel=sk))

    # ---- ENGINE: still raw ---------------------------------------------------
    # Recursive. It used to be "asm/rock_neo/*.s", which does not descend into
    # subdirectories — so asm/rock_neo/psxsdk/code.s (446 functions, genuinely linked via
    # rock_neo.ld) was invisible to every count this map produced. Same defect as the
    # 484-vs-1119 scandal: a denominator set by what a glob matches, not by the binary.
    for f in code_files("asm/rock_neo/**/*.s", skip_engine=False):
        for fn in parse_asm(f):
            sdk = "psxsdk" in f
            rows.append(dict(name=fn["name"],
                             realm="SDK" if sdk else "ENGINE",
                             container=("psxsdk/" if sdk else "") + os.path.basename(f),
                             state="SDK" if sdk else "UNSPLIT", insns=len(fn["words"]),
                             leaf=not any(o in CALLS for o in fn["ops"]),
                             body=hashlib.md5("".join(fn["words"]).encode()).hexdigest(),
                             skel=hashlib.md5(",".join(fn["ops"]).encode()).hexdigest()))

    # ---- STAGES --------------------------------------------------------------
    # A stage function keeps its asm-derived row (so instance/unique counts stay
    # exact); only its STATE changes once a C translation unit owns it. Creating a
    # second row for the C version would double-count it.
    stage_c = objdump_stage_c()
    stage_stubs = cpp_stub_names_stage() if stage_c else set()
    for f in code_files("asm/**/*.s", skip_engine=True):
        archive = f.split("/")[1]
        for fn in parse_asm(f):
            key = (archive, fn["name"])
            if key in stage_c:
                st = "STUB" if key in stage_stubs else "MATCHED"
            else:
                st = "STAGE"
            rows.append(dict(name=fn["name"], realm="STAGE", container=archive,
                             state=st, insns=len(fn["words"]),
                             leaf=not any(o in CALLS for o in fn["ops"]),
                             body=hashlib.md5("".join(fn["words"]).encode()).hexdigest(),
                             skel=hashlib.md5(",".join(fn["ops"]).encode()).hexdigest()))

    # ---- GUARD: stale bulk .s shadowing carved functions ---------------------
    # When a yaml subsegment changes from `[OFF, asm]` to `[OFF, c, Name]`, splat
    # writes the new nonmatchings/*.s but does NOT delete the old bulk OFF.s. The
    # recursive glob above then sees every function TWICE and every instance count
    # doubles. Caught 2026-07-29 on the ST0C triplet: 645 phantom rows, 34 real C
    # functions reported as 68. The headline unique-BODY count was UNAFFECTED (a
    # duplicate row carries the same body hash), which is exactly why this could
    # have sat unnoticed — so fail loudly rather than dedupe quietly.
    _keys = collections.Counter((r.get("container"), r["name"]) for r in rows)
    _dupes = {k: v for k, v in _keys.items() if v > 1}
    if _dupes:
        _arch = sorted({k[0] for k in _dupes})
        sys.stderr.write(
            "gen_map: FATAL — %d duplicate rows across %d (archive, function) keys.\n"
            "  affected archives: %s\n"
            "  Cause: a stale bulk .s still holds functions now carved into a C segment,\n"
            "  so they are counted twice. asm/ is generated and gitignored — delete the\n"
            "  stale file(s) and re-run. To find them: in each chunk dir, a *.s whose\n"
            "  glabels also appear under that chunk's nonmatchings/ tree.\n"
            % (sum(v - 1 for v in _dupes.values()), len(_dupes), ", ".join(_arch))
        )
        raise SystemExit(1)

    # ---- duplication + families ---------------------------------------------
    bodies = collections.Counter(r["body"] for r in rows if r["body"])
    skels = collections.Counter(r["skel"] for r in rows if r["skel"])
    for r in rows:
        r["copies"] = bodies.get(r["body"], 1) if r["body"] else 1
        r["family_size"] = skels.get(r["skel"], 1) if r["skel"] else 1

    uniq_stage = len({r["body"] for r in rows if r["realm"] == "STAGE"})
    sdk_n = sum(1 for r in rows if r["realm"] == "SDK")
    engine_n = sum(1 for r in rows if r["realm"] == "ENGINE")
    unsplit_n = sum(1 for r in rows if r['state'] == 'UNSPLIT')
    # Name every numerator, like every denominator. A bare "matched" is how the two
    # realms silently merged before anyone noticed which one they were quoting.
    eng_matched = sum(1 for r in rows if r["realm"] == "ENGINE" and r["state"] == "MATCHED")
    stg_matched_inst = sum(1 for r in rows if r["realm"] == "STAGE" and r["state"] == "MATCHED")
    stg_matched = len({r["body"] for r in rows
                       if r["realm"] == "STAGE" and r["state"] == "MATCHED"})
    return rows, dict(engine_total=engine_n, unsplit_engine=unsplit_n,
                      stage_instances=sum(1 for r in rows if r["realm"] == "STAGE"),
                      stage_unique=uniq_stage, sdk_total=sdk_n, whole_game=engine_n + uniq_stage + sdk_n,
                      engine_matched=eng_matched, stage_matched=stg_matched,
                      stage_matched_instances=stg_matched_inst,
                      matched_total=eng_matched + stg_matched)


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
    A(f"| **ENGINE** | `ROCK_NEO.EXE`. Resident in RAM always; every stage calls into it. | **{s['engine_total']:,}** | **{s['engine_matched']}** ({pct(s['engine_matched'], s['engine_total'])}) |")
    A(f"| **STAGES** | 37 code archives (168 more are asset-only). {s['stage_instances']:,} copies of {s['stage_unique']:,} unique bodies. | **{s['stage_unique']:,}** | **{s['stage_matched']}** ({pct(s['stage_matched'], s['stage_unique'])}) |")
    A(f"| **SDK** | Sony PSY-Q library linked into the exe (`asm/rock_neo/psxsdk/code.s`). NOT Capcom code — matchable from published source, a cheaper class of work. | **{s['sdk_total']:,}** | 0 |")
    A(f"| **WHOLE GAME** | | **{s['whole_game']:,}** | **{s['matched_total']}** ({pct(s['matched_total'], s['whole_game'])}) |")
    A("")

    A("## States\n")
    A("*\"functions\" = distinct functions to decompile. The stage row also shows raw")
    A("instances, because the same body is duplicated across archives — decompiling it")
    A("once covers every copy.*\n")
    A("| state | realm | meaning | functions | (raw instances) |")
    A("|---|---|---|---|---|")
    A(f"| `MATCHED` | engine | real C, recompiles to identical bytes. **The deliverable.** | **{by['MATCHED']:,}** | — |")
    A(f"| `STUB` | engine | in a C file but body is still `INCLUDE_ASM`. Bookkeeping only. | {by['STUB']:,} | — |")
    A(f"| `UNSPLIT` | engine | still in a raw asm chunk, no C file. | {by['UNSPLIT']:,} | — |")
    A(f"| `STAGE` | stages | in a stage archive; C pipeline not yet set up (1 of 37 done). | {s['stage_unique']:,} | {by['STAGE']:,} |")
    A(f"| | | **total distinct functions in the game** | **{s['whole_game']:,}** | |")
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
          f"{s['engine_matched']} engine + {s['stage_matched']} stage matched "
          f"-> notes/FUNCTION_MAP.md + build/function_map.json")
    return 0


if __name__ == "__main__":
    sys.exit(main())
