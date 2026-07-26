#!/usr/bin/env python3
"""dep_priority.py — Main-exe dependency priority queue.

For every main-exe function called (jal/jalr) from any overlay .s file:
  - Count distinct call-sites and distinct stage programs (overlay dirs)
  - Determine if it is already in src/rock_neo/*.c or still raw asm only
  - Rank descending by overlay call-site count

Usage (run from repo root):
    python3 tools/dep_priority.py [--top N]

Output: tab-separated table + summary.
"""

import re
import os
import glob
import collections
import argparse

# ---------------------------------------------------------------------------
# Regexes
# ---------------------------------------------------------------------------
GLABEL_RE = re.compile(r"^glabel\s+(func_[0-9A-Fa-f]+)")
JAL_RE = re.compile(r"\bjal[r]?\s+(func_[0-9A-Fa-f]+)")

# Main-exe address range: 0x80010000 – 0x800FFFFF
# (overlay local funcs typically share the same 0x801xxxxx range but that
#  is the OVERLAY load address; the main exe lives in 0x8001xxxx–0x8009xxxx)
# Strategy: collect all func_ addresses seen in asm/rock_neo/*.s + src/rock_neo/*.c
# as the definitive main-exe function set, then filter JAL targets to that set.
# ---------------------------------------------------------------------------

def collect_main_exe_funcs():
    """Return a set of all func_XXXXXXXX names defined in the main exe."""
    funcs = set()

    # From raw asm (the 6 giant files)
    for path in glob.glob("asm/rock_neo/*.s"):
        if os.path.basename(path) == "header.s":
            continue
        with open(path, errors="ignore") as fh:
            for line in fh:
                m = GLABEL_RE.match(line)
                if m:
                    funcs.add(m.group(1))

    # From C stubs / matched bodies
    for path in glob.glob("src/rock_neo/*.c"):
        with open(path, errors="ignore") as fh:
            for line in fh:
                for m in re.finditer(r"\bfunc_[0-9A-Fa-f]{8}\b", line):
                    funcs.add(m.group(0))

    return funcs


def collect_c_represented():
    """Return set of func_ names that appear in src/rock_neo/*.c (any context)."""
    represented = set()
    for path in glob.glob("src/rock_neo/*.c"):
        with open(path, errors="ignore") as fh:
            content = fh.read()
        for m in re.finditer(r"\bfunc_[0-9A-Fa-f]{8}\b", content):
            represented.add(m.group(0))
    return represented


def collect_overlay_calls(main_exe_funcs):
    """Scan all overlay asm files and record call-sites.

    Returns:
        call_sites: dict[target_func -> list of (overlay_dir, file_path, addr)]
        stage_programs: dict[target_func -> set of overlay_dir names]
    """
    # call_sites[func] = list of (stage, file)
    call_sites = collections.defaultdict(list)

    overlay_files = [
        f for f in glob.glob("asm/**/*.s", recursive=True)
        if not f.startswith("asm/rock_neo/")
        and os.path.basename(f) not in ("header.s",)
        and "/data/" not in f
    ]

    for path in overlay_files:
        # Derive overlay/stage name: first path component after asm/
        # e.g. asm/ST00/ovl0__PROGBIN.../B24.s  -> "ST00"
        parts = path.split(os.sep)
        # parts[0] = "asm", parts[1] = stage_dir
        stage = parts[1] if len(parts) > 1 else "UNKNOWN"

        with open(path, errors="ignore") as fh:
            for line in fh:
                m = JAL_RE.search(line)
                if m:
                    target = m.group(1)
                    if target in main_exe_funcs:
                        call_sites[target].append((stage, path))

    return call_sites


def main():
    parser = argparse.ArgumentParser(description="Main-exe dependency priority queue")
    parser.add_argument("--top", type=int, default=50,
                        help="Number of top functions to display (default: 50)")
    args = parser.parse_args()

    print("Collecting main-exe function set...", flush=True)
    main_exe_funcs = collect_main_exe_funcs()
    print(f"  {len(main_exe_funcs)} main-exe functions identified", flush=True)

    print("Collecting C-represented functions...", flush=True)
    c_represented = collect_c_represented()
    c_repr_in_main = c_represented & main_exe_funcs
    print(f"  {len(c_repr_in_main)} of those appear in src/rock_neo/*.c", flush=True)

    # Which are still ONLY in raw asm (not in any C file)
    raw_asm_only = main_exe_funcs - c_represented

    print("Scanning overlay asm for jal/jalr to main-exe functions...", flush=True)
    call_sites = collect_overlay_calls(main_exe_funcs)
    print(f"  {len(call_sites)} distinct main-exe targets called from overlays", flush=True)

    # Build priority table
    # For each target: total call-site count, distinct stage count, status
    rows = []
    for func, sites in call_sites.items():
        n_sites = len(sites)
        stages = set(s for s, _ in sites)
        n_stages = len(stages)
        in_c = func in c_represented
        status = "C_FILE" if in_c else "RAW_ASM"
        rows.append((func, n_sites, n_stages, status, sorted(stages)))

    # Sort: primary = number of distinct stages, secondary = total call sites
    rows.sort(key=lambda r: (-r[2], -r[1]))

    # Print summary table
    print()
    print(f"{'RANK':<6}{'FUNCTION':<22}{'SITES':>6}{'STAGES':>7}{'STATUS':<10}  TOP STAGES")
    print("-" * 90)
    for rank, (func, n_sites, n_stages, status, stages) in enumerate(rows[:args.top], 1):
        top_stages = ", ".join(stages[:5])
        if len(stages) > 5:
            top_stages += f" (+{len(stages)-5} more)"
        print(f"{rank:<6}{func:<22}{n_sites:>6}{n_stages:>7}  {status:<10}{top_stages}")

    print()
    print("=== SUMMARY ===")
    print(f"Total main-exe functions:              {len(main_exe_funcs)}")
    print(f"  - represented in src/rock_neo/*.c:   {len(c_repr_in_main)}")
    print(f"  - raw asm only:                      {len(raw_asm_only)}")
    print(f"Main-exe functions called from overlays: {len(call_sites)}")
    print(f"  - already in C file:                 {sum(1 for r in rows if r[3]=='C_FILE')}")
    print(f"  - still raw asm:                     {sum(1 for r in rows if r[3]=='RAW_ASM')}")
    print()
    print("TOP 30 PRIORITY (by distinct stages, then call-site count):")
    for rank, (func, n_sites, n_stages, status, stages) in enumerate(rows[:30], 1):
        print(f"  {rank:2}. {func}  {n_stages:3} stages  {n_sites:4} sites  {status}")

    return rows, main_exe_funcs, c_represented, raw_asm_only


if __name__ == "__main__":
    main()
