#!/usr/bin/env python3
"""Active INCLUDE_ASM stub census via cpp (not naive source grep)."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Makefile lines 20-21
CPP_FLAGS = [
    "-Iinclude",
    "-undef",
    "-Wall",
    "-lang-c",
    "-fno-builtin",
    "-Dmips",
    "-D__GNUC__=2",
    "-D__OPTIMIZE__",
    "-D__mips__",
    "-D__mips",
    "-Dpsx",
    "-D__psx__",
    "-D__psx",
    "-D_PSYQ",
    "-D__EXTENSIONS__",
    "-D_MIPSEL",
    "-D_LANGUAGE_C",
    "-DLANGUAGE_C",
    "-DHACKS",
]

STUB_RE = re.compile(r"nonmatchings/(\w+)/(\w+)\.s")
INSN_RE = re.compile(
    r"/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]{8}\s+\*/"
)


def cpp_stubs(c_path: Path) -> set[tuple[str, str]]:
    proc = subprocess.run(
        ["cpp", *CPP_FLAGS, str(c_path.relative_to(ROOT))],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=True,
    )
    collapsed = proc.stdout.replace('"', "").replace(" ", "")
    return {(m.group(1), m.group(2)) for m in STUB_RE.finditer(collapsed)}


def insn_count(s_path: Path) -> int:
    text = s_path.read_text(encoding="utf-8")
    return len(INSN_RE.findall(text))


def objdump_text_symbols(obj_path: Path) -> list[str]:
    proc = subprocess.run(
        ["mipsel-elf-objdump", "-t", str(obj_path)],
        capture_output=True,
        text=True,
        check=True,
    )
    names: list[str] = []
    for line in proc.stdout.splitlines():
        if " F .text" in line:
            names.append(line.split()[-1])
    return names


def stub_census() -> int:
    stubs: dict[str, tuple[str, int]] = {}
    for c_path in sorted((ROOT / "src" / "rock_neo").glob("*.c")):
        for folder, name in cpp_stubs(c_path):
            rel = f"asm/rock_neo/nonmatchings/{folder}/{name}.s"
            s_path = ROOT / rel
            if not s_path.is_file():
                print(f"census: missing {rel} (from {c_path.name})", file=sys.stderr)
                return 1
            stubs[name] = (rel, insn_count(s_path))

    rows = sorted(stubs.items(), key=lambda kv: (-kv[1][1], kv[0]))
    total_insns = 0
    for name, (rel, count) in rows:
        total_insns += count
        print(f"{count:4d}  {name}  {rel}")

    print()
    print(f"total: {len(rows)} active stubs, {total_insns} instruction lines")
    return 0


def matched_census() -> int:
    build = ROOT / "build" / "src" / "rock_neo"
    if not build.is_dir():
        print(
            "census: build/src/rock_neo/ missing — run `make CPP=cpp` first",
            file=sys.stderr,
        )
        return 1

    total_obj = 0
    total_stub = 0
    total_matched = 0
    print(f"{'TU':<22} {'in .o':>6} {'stubs':>6} {'matched':>8}")
    for c_path in sorted((ROOT / "src" / "rock_neo").glob("*.c")):
        obj_path = build / f"{c_path.name}.o"
        if not obj_path.is_file():
            print(f"census: missing {obj_path}", file=sys.stderr)
            return 1
        obj_syms = objdump_text_symbols(obj_path)
        stub_names = {name for _folder, name in cpp_stubs(c_path)}
        matched = len(set(obj_syms) - stub_names)
        print(
            f"{c_path.name:<22} {len(obj_syms):6d} {len(stub_names):6d} {matched:8d}"
        )
        total_obj += len(obj_syms)
        total_stub += len(stub_names)
        total_matched += matched

    print()
    print(
        f"total: {total_obj} in object files, {total_stub} active stubs, "
        f"{total_matched} matched (matched + stubs = {total_matched + total_stub})"
    )
    if total_obj != total_matched + total_stub:
        print("census: arithmetic mismatch", file=sys.stderr)
        return 1
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--matched",
        action="store_true",
        help="Per-TU matched count via objdump F .text minus cpp-active stubs",
    )
    args = parser.parse_args()
    if args.matched:
        return matched_census()
    return stub_census()


if __name__ == "__main__":
    raise SystemExit(main())
