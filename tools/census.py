#!/usr/bin/env python3
"""Active INCLUDE_ASM stub census via cpp (not naive source grep)."""

from __future__ import annotations

import glob
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
        ["cpp", *CPP_FLAGS, str(c_path)],
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


def main() -> int:
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


if __name__ == "__main__":
    raise SystemExit(main())
