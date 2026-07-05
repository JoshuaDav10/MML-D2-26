#!/usr/bin/env python3
"""Generate m2c drafts for active stubs in the 100-199 insn tier (Brief 3)."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
M2C = Path(__file__).resolve().parent / "m2c.py"
PINNED = (Path(__file__).resolve().parent / "PINNED_COMMIT").read_text().strip().split()[0]
DRAFTS = ROOT / "drafts"
BANNER = "UNTRUSTED DRAFT — never copy into src/ without byte-match iteration"

CENSUS_CPP_FLAGS = [
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

CTX_CPP_FLAGS = [
    "-Iinclude",
    "-undef",
    "-DPERMUTER",
    "-D__asm__(x)=",
    "-D_LANGUAGE_C",
    "-DLANGUAGE_C",
    "-Dmips",
    "-D__GNUC__=2",
    "-D__OPTIMIZE__",
    "-D__mips__",
    "-Dpsx",
    "-D_PSYQ",
    "-D_MIPSEL",
    "-DHACKS",
]

STUB_RE = re.compile(r"nonmatchings/(\w+)/(\w+)\.s")
INSN_RE = re.compile(
    r"/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]{8}\s+\*/"
)
M2C_TARGET = "mipsel-gcc-c"
MIN_INSNS = 100
MAX_INSNS = 199


def cpp_stubs(c_path: Path) -> set[tuple[str, str]]:
    proc = subprocess.run(
        ["cpp", *CENSUS_CPP_FLAGS, str(c_path.relative_to(ROOT))],
        cwd=ROOT,
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        return set()
    collapsed = proc.stdout.replace('"', "").replace(" ", "")
    return {(m.group(1), m.group(2)) for m in STUB_RE.finditer(collapsed)}


def insn_count(s_path: Path) -> int:
    return len(INSN_RE.findall(s_path.read_text(encoding="utf-8")))


def collect_targets() -> list[dict[str, str | int]]:
    rows: list[dict[str, str | int]] = []
    for c_path in sorted((ROOT / "src" / "rock_neo").glob("*.c")):
        for folder, name in cpp_stubs(c_path):
            asm = ROOT / f"asm/rock_neo/nonmatchings/{folder}/{name}.s"
            if not asm.is_file():
                continue
            count = insn_count(asm)
            if MIN_INSNS <= count <= MAX_INSNS:
                rows.append(
                    {
                        "func": name,
                        "folder": folder,
                        "tu": c_path.name,
                        "asm": str(asm.relative_to(ROOT)),
                        "insns": count,
                    }
                )
    rows.sort(key=lambda r: (-int(r["insns"]), str(r["func"])))
    return rows


def make_context(tu: str) -> tuple[str | None, str | None]:
    c_path = ROOT / "src" / "rock_neo" / tu
    proc = subprocess.run(
        ["cpp", *CTX_CPP_FLAGS, str(c_path.relative_to(ROOT))],
        cwd=ROOT,
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        return None, proc.stderr.strip() or "cpp failed"
    lines = [
        ln
        for ln in proc.stdout.splitlines()
        if not ln.startswith("#") and ln.strip()
    ]
    if not lines:
        return None, "empty cpp output"
    return "\n".join(lines) + "\n", None


def run_m2c(func: str, asm: Path, ctx_path: Path) -> tuple[str | None, str | None]:
    proc = subprocess.run(
        [
            sys.executable,
            str(M2C),
            "-t",
            M2C_TARGET,
            "--context",
            str(ctx_path),
            "-f",
            func,
            str(asm),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        err = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        return None, err
    out = proc.stdout.strip()
    if not out:
        return None, "empty m2c output"
    return out + "\n", None


def write_draft(row: dict[str, str | int], body: str) -> Path:
    out_dir = DRAFTS / str(row["folder"])
    out_dir.mkdir(parents=True, exist_ok=True)
    out_path = out_dir / f"{row['func']}.c"
    header = (
        f"/* {row['func']} — {row['insns']} asm insns\n"
        f" * TU: src/rock_neo/{row['tu']}\n"
        f" * asm: {row['asm']}\n"
        f" * m2c: {PINNED}\n"
        f" * {BANNER}\n"
        f" */\n\n"
    )
    out_path.write_text(header + body, encoding="utf-8")
    return out_path


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--func",
        help="Regenerate one function only (name without path)",
    )
    args = parser.parse_args()

    if not M2C.is_file():
        print(f"generate_drafts: missing {M2C}", file=sys.stderr)
        return 1

    targets = collect_targets()
    if args.func:
        targets = [r for r in targets if r["func"] == args.func]
        if not targets:
            print(f"generate_drafts: {args.func} not in {MIN_INSNS}-{MAX_INSNS} tier", file=sys.stderr)
            return 1

    ctx_cache: dict[str, tuple[str | None, str | None]] = {}
    ctx_dir = DRAFTS / ".ctx"
    ctx_dir.mkdir(parents=True, exist_ok=True)

    results: list[dict[str, str | int]] = []
    for row in targets:
        tu = str(row["tu"])
        if tu not in ctx_cache:
            ctx_cache[tu] = make_context(tu)
        ctx_text, ctx_err = ctx_cache[tu]
        if ctx_text is None:
            results.append({**row, "status": "context-missing", "detail": ctx_err or ""})
            continue

        ctx_path = ctx_dir / f"{Path(tu).stem}.c"
        ctx_path.write_text(ctx_text, encoding="utf-8")

        body, m2c_err = run_m2c(str(row["func"]), ROOT / str(row["asm"]), ctx_path)
        if body is None:
            results.append({**row, "status": "m2c-errored", "detail": m2c_err or ""})
            continue

        write_draft(row, body)
        results.append({**row, "status": "ok", "detail": ""})

    ok = sum(1 for r in results if r["status"] == "ok")
    print(f"generated {ok}/{len(results)} drafts ({MIN_INSNS}-{MAX_INSNS} insns)")
    for r in results:
        if r["status"] != "ok":
            print(f"  {r['status']:16s} {r['func']} ({r['detail'][:80]})")
    write_readme(results)
    return 0


def write_readme(results: list[dict[str, str | int]]) -> None:
    lines = [
        "# m2c drafts (100–199 insn tier)",
        "",
        "UNTRUSTED rough decompiles for the medium-function tier. Never copy into",
        "`src/` without byte-match iteration (`tools/bytecmp.sh`).",
        "",
        "## How produced",
        "",
        f"- m2c pinned at `{PINNED}` (`tools/m2c/PINNED_COMMIT`)",
        f"- Target list: active stubs with {MIN_INSNS}–{MAX_INSNS} asm comment lines",
        "  (`python3 tools/census.py`, cpp-based — not naive INCLUDE_ASM grep)",
        "- Context: `cpp` on `src/rock_neo/<tu>.c` with `-DPERMUTER` (stubs stripped)",
        f"- Decompile: `tools/m2c/m2c.py -t {M2C_TARGET} --context … -f <func> <asm>`",
        "",
        "## Regenerate one",
        "",
        "```bash",
        "source .venv/bin/activate",
        "python3 tools/m2c/generate_drafts.py --func func_800411DC",
        "```",
        "",
        "## Regenerate tier",
        "",
        "```bash",
        "python3 tools/m2c/generate_drafts.py",
        "```",
        "",
        f"## Status ({len(results)} functions)",
        "",
        "| insns | function | TU | status |",
        "|------:|----------|-----|--------|",
    ]
    for r in results:
        lines.append(
            f"| {r['insns']} | `{r['func']}` | `{r['tu']}` | {r['status']} |"
        )
    ok = sum(1 for r in results if r["status"] == "ok")
    lines.extend(
        [
            "",
            f"**Summary:** {ok} ok, {len(results) - ok} m2c-errored/context-missing "
            f"(total {len(results)} = tier census count).",
        ]
    )
    (DRAFTS / "README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    raise SystemExit(main())
