#!/usr/bin/env python3
"""Extract callgraph, data xrefs, and family hints from rock_neo asm."""

from __future__ import annotations

import re
import sys
from collections import defaultdict
from itertools import combinations
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM_ROOT = ROOT / "asm" / "rock_neo" / "nonmatchings"
OUT = ROOT / "notes" / "XREF.md"

# Reuse census stub discovery (import would run main; duplicate minimal hook)
sys.path.insert(0, str(ROOT / "tools"))
from census import cpp_stubs  # noqa: E402

INSN_RE = re.compile(r"/\*[^*]+\*/\s+(\w+)\s+(.*)$")
GP_RE = re.compile(r"%gp_rel\(([^)]+)\)\(\$gp\)")
HI_RE = re.compile(r"%hi\(([^)]+)\)")
LO_RE = re.compile(r"%lo\(([^)]+)\)")

LOAD_STORE = frozenset({"lb", "lbu", "lh", "lhu", "lw", "sb", "sh", "sw"})


def active_stubs() -> set[str]:
    stubs: set[str] = set()
    for c_path in sorted((ROOT / "src" / "rock_neo").glob("*.c")):
        for _folder, name in cpp_stubs(c_path):
            stubs.add(name)
    return stubs


def func_name_from_path(path: Path) -> str:
    m = re.match(r"func_[0-9A-Fa-f]+\.s", path.name)
    if m:
        return path.stem
    return path.stem


def parse_asm_file(path: Path) -> tuple[str, list[str], set[str], dict[str, set[tuple[str, str]]]]:
    """Return (func, callees, tail_js, data_refs[symbol] -> set of (width, mode))."""
    func = func_name_from_path(path)
    callees: list[str] = []
    tail_js: set[str] = set()
    data_refs: dict[str, set[tuple[str, str]]] = defaultdict(set)

    for raw in path.read_text(encoding="utf-8").splitlines():
        m = INSN_RE.search(raw)
        if not m:
            if raw.strip().startswith("glabel "):
                func = raw.split()[1]
            continue
        mnemonic, operands = m.group(1), m.group(2).strip()

        if mnemonic == "jal":
            target = operands.split()[0] if operands else ""
            if target:
                callees.append(target)
        elif mnemonic == "j" and operands and not operands.startswith("."):
            tail_js.add(operands.split()[0])

        if mnemonic in LOAD_STORE:
            gp = GP_RE.search(operands)
            if gp:
                data_refs[gp.group(1)].add((mnemonic, "gp"))
            lo = LO_RE.search(operands)
            if lo:
                mode = "lui/at" if HI_RE.search(raw) or "($at)" in operands else "lui/at"
                data_refs[lo.group(1)].add((mnemonic, mode))
        elif mnemonic == "lui" and HI_RE.search(operands):
            # lui-only hi ref (address materialization)
            sym = HI_RE.search(operands).group(1)
            data_refs[sym].add(("lui", "lui/at"))

    return func, callees, tail_js, data_refs


def scan_all() -> tuple[
    dict[str, tuple[list[str], set[str]]],
    dict[str, dict[str, set[tuple[str, str]]]],
]:
    callgraph: dict[str, tuple[list[str], set[str]]] = {}
    data_by_func: dict[str, dict[str, set[tuple[str, str]]]] = {}

    for path in sorted(ASM_ROOT.rglob("*.s")):
        func, callees, tail_js, data_refs = parse_asm_file(path)
        callgraph[func] = (callees, tail_js)
        data_by_func[func] = data_refs
    return callgraph, data_by_func


def reverse_callgraph(
    callgraph: dict[str, tuple[list[str], set[str]]],
) -> dict[str, set[str]]:
    rev: dict[str, set[str]] = defaultdict(set)
    for caller, (jals, tails) in callgraph.items():
        for callee in jals:
            rev[callee].add(caller)
        for callee in tails:
            rev[callee].add(caller)
    return rev


def find_families(
    unmatched: set[str],
    callgraph: dict[str, tuple[list[str], set[str]]],
    data_by_func: dict[str, dict[str, set[tuple[str, str]]]],
) -> list[tuple[int, str, list[str]]]:
    callee_sets = {
        f: set(callgraph.get(f, ([], set()))[0])
        | set(callgraph.get(f, ([], set()))[1])
        for f in unmatched
    }
    data_sets = {f: set(data_by_func.get(f, {})) for f in unmatched}

    seen_groups: set[frozenset[str]] = set()
    families: list[tuple[int, str, list[str]]] = []

    def add(group: list[str], reason: str) -> None:
        key = frozenset(group)
        if key in seen_groups or len(group) < 3:
            return
        seen_groups.add(key)
        families.append((len(group), reason, sorted(group)))

    all_callees = sorted({c for s in callee_sets.values() for c in s})
    for c1, c2 in combinations(all_callees, 2):
        group = [f for f in unmatched if c1 in callee_sets[f] and c2 in callee_sets[f]]
        add(group, f"shared callees: {c1}, {c2}")

    all_data = sorted({d for s in data_sets.values() for d in s})
    for d1, d2 in combinations(all_data, 2):
        group = [f for f in unmatched if d1 in data_sets[f] and d2 in data_sets[f]]
        add(group, f"shared data: {d1}, {d2}")

    families.sort(key=lambda x: (x[0], x[1], x[2]))
    return families


def format_data_sites(sites: set[tuple[str, str]]) -> str:
    parts = sorted(f"{w}/{m}" for w, m in sites)
    return ", ".join(parts)


def render(
    callgraph: dict[str, tuple[list[str], set[str]]],
    rev: dict[str, set[str]],
    data_by_func: dict[str, dict[str, set[tuple[str, str]]]],
    data_rev: dict[str, set[str]],
    families: list[tuple[int, str, list[str]]],
    unmatched: set[str],
) -> str:
    lines = [
        "# rock_neo cross-reference index",
        "",
        "Generated by `python3 tools/xref.py`. Regenerate after asm or stub changes.",
        "",
        f"Functions scanned: {len(callgraph)} asm files under `asm/rock_neo/nonmatchings/`.",
        f"Active unmatched stubs (cpp census): {len(unmatched)}.",
        "",
        "## 1. Callgraph",
        "",
        "Per-function `jal` targets (in order) and `j` tail-calls to non-local glabels.",
        "",
    ]

    for func in sorted(callgraph):
        jals, tails = callgraph[func]
        tail_s = ", ".join(sorted(tails)) if tails else "—"
        jal_s = ", ".join(jals) if jals else "—"
        lines.append(f"### `{func}`")
        lines.append(f"- jal: {jal_s}")
        lines.append(f"- j (glabel): {tail_s}")
        lines.append("")

    lines.extend(["## 2. Reverse callgraph (callee → callers)", ""])
    for callee in sorted(rev):
        callers = ", ".join(sorted(rev[callee]))
        lines.append(f"- `{callee}` ← {callers}")
    lines.append("")

    lines.extend(["## 3. Data xrefs (by symbol)", ""])
    all_symbols = sorted({sym for d in data_by_func.values() for sym in d})
    for sym in all_symbols:
        lines.append(f"### `{sym}`")
        for func in sorted(data_by_func):
            if sym not in data_by_func[func]:
                continue
            sites = format_data_sites(data_by_func[func][sym])
            lines.append(f"- `{func}`: {sites}")
        lines.append("")

    lines.extend(["## 4. Family suggestions (unmatched only)", ""])
    if not families:
        lines.append("(none)")
    else:
        for size, reason, group in families:
            funcs = ", ".join(f"`{f}`" for f in group)
            lines.append(f"- **{size}** — {reason}: {funcs}")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    unmatched = active_stubs()
    callgraph, data_by_func = scan_all()
    rev = reverse_callgraph(callgraph)

    data_rev: dict[str, set[str]] = defaultdict(set)
    for func, refs in data_by_func.items():
        for sym in refs:
            data_rev[sym].add(func)

    families = find_families(unmatched, callgraph, data_by_func)
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(
        render(callgraph, rev, data_by_func, data_rev, families, unmatched),
        encoding="utf-8",
    )
    print(f"xref: wrote {OUT} ({len(callgraph)} functions, {len(families)} families)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
