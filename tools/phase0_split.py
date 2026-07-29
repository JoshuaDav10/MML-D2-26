#!/usr/bin/env python3
"""Phase-0 split: carve a function out of a giant raw-asm chunk into its own C file.

Mechanises notes/wip/PHASE0_SPLIT_TEMPLATE.md, which was proven by hand on
func_8002F9C4. Splitting does NOT match anything — it moves a function from state
UNSPLIT to STUB so real C can be written for it. Only reaching MATCHED is progress.

    tools/phase0_split.py func_800272C8 [func_8002A158 ...]
    tools/phase0_split.py --empty func_800272C8 ...   # also write `void f(void){}`

--empty is for the `jr $ra; nop` functions: a two-instruction body that is exactly
what an empty C function compiles to, so the split and the match happen together.

Always verify afterwards with `make CPP=cpp check_rock_neo_only` — this tool only
rearranges the build inputs, it never proves a match.
"""
import re, sys, os, pathlib, subprocess

ROOT = pathlib.Path(__file__).resolve().parent.parent
os.chdir(ROOT)
# rom_offset = vram - 0x8000F800, NOT vram - 0x80010000.
# The yaml declares `- [0x800, header]` then `start: 0x800, vram: 0x80010000`, so the
# 0x800 header sits between them. Every phase-0 split before 2026-07-26 used the wrong
# formula and wrote yaml offsets 0x800 too low. It went unnoticed because splat was never
# re-run — the note in the yaml literally says "splat is not re-run here". The first time
# it WAS re-run (by this tool) it regenerated every chunk from those bad offsets and
# truncated 201EC.s mid-function, breaking the link on an undefined local label.
VRAM_BASE = 0x8000F800
YAML = pathlib.Path("config/splat.us.rock_neo.yaml")


def find_in_chunks(fn):
    """Locate fn in the giant asm chunks. Returns (path, size_bytes, body_lines)."""
    for p in sorted(pathlib.Path("asm/rock_neo").glob("*.s")):
        lines = p.read_text(errors="ignore").splitlines()
        for i, l in enumerate(lines):
            if l.strip() == f"glabel {fn}":
                size = None
                for back in range(i - 1, max(-1, i - 4), -1):
                    m = re.match(rf"\s*nonmatching\s+{fn},\s*(0x[0-9A-Fa-f]+)", lines[back])
                    if m:
                        size = int(m.group(1), 16)
                        break
                body = [lines[i]]
                for j in range(i + 1, len(lines)):
                    if lines[j].startswith("glabel") or re.match(r"\s*nonmatching\s", lines[j]):
                        break
                    body.append(lines[j])
                while body and not body[-1].strip():
                    body.pop()
                if size is None:
                    size = sum(1 for b in body if re.search(r"/\*.*\*/", b)) * 4
                return p, size, body
    return None, None, None


def insert_yaml(off, name, end):
    """Split the enclosing `- [0xNNN, asm]` entry into asm / c / asm."""
    txt = YAML.read_text()
    entries = [(int(m.group(1), 16), m.group(0), m.start())
               for m in re.finditer(r"^      - \[0x([0-9A-Fa-f]+), asm\].*$", txt, re.M)]
    encl = [e for e in entries if e[0] <= off]
    if not encl:
        return False, "no enclosing asm segment"
    start, line, pos = max(encl, key=lambda e: e[0])
    if start == off:
        # The function begins exactly AT a segment start. Two sub-cases, and refusing both
        # (as this tool used to) needlessly skipped 13 of 16 targets on 2026-07-29:
        #   - the segment holds ONLY this function -> convert the whole thing to `c`
        #   - it holds more -> emit `c` then reopen `asm` right after the function
        nxt = min((int(m.group(1), 16)
                   for m in re.finditer(r"^      - \[0x([0-9A-Fa-f]+),", txt, re.M)
                   if int(m.group(1), 16) > off), default=None)
        if nxt == end:
            new = f"      - [0x{off:X}, c, {name}]"
        else:
            new = (f"      - [0x{off:X}, c, {name}]\n"
                   f"      - [0x{end:X}, asm]")
        YAML.write_text(txt[:pos] + new + txt[pos + len(line):])
        return True, None
    # A trailing asm segment that starts exactly where the NEXT segment starts is
    # zero-length: splat emits no .s for it, but the generated rock_neo.ld still
    # references its .o and the link dies with "cannot find .../2C640.s.o".
    nxt = min((e[0] for e in entries if e[0] > off),
              default=None)
    after = [int(m.group(1), 16) for m in re.finditer(r"^      - \[0x([0-9A-Fa-f]+),", txt, re.M)
             if int(m.group(1), 16) > off]
    nxt = min(after) if after else None
    tail = "" if nxt == end else f"\n      - [0x{end:X}, asm]"
    new = (f"      - [0x{start:X}, asm]\n"
           f"      - [0x{off:X}, c, {name}]{tail}")
    YAML.write_text(txt[:pos] + new + txt[pos + len(line):])
    return True, None


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    empty = "--empty" in sys.argv
    ok, fail = [], []

    for fn in args:
        vma = int(fn.replace("func_", ""), 16)
        off = vma - VRAM_BASE
        chunk, size, body = find_in_chunks(fn)
        if not chunk:
            fail.append((fn, "not found in asm/rock_neo/*.s")); continue
        seg = f"Code{vma:08X}"

        d = pathlib.Path(f"asm/rock_neo/nonmatchings/{seg}")
        d.mkdir(parents=True, exist_ok=True)
        (d / f"{fn}.s").write_text(
            ".set noat      /* allow manual use of $at */\n"
            ".set noreorder /* don't insert nops after branches */\n\n"
            f"nonmatching {fn}, 0x{size:X}\n\n" + "\n".join(body) + "\n")

        c = pathlib.Path(f"src/rock_neo/{seg}.c")
        if empty:
            c.write_text(
                '#include "common.h"\n\n'
                f"/* {fn} — the whole body is `jr $ra; nop`, i.e. an empty function.\n"
                " * Most likely a stubbed-out hook the developers left in place, or a\n"
                " * table entry that has to exist but does nothing. */\n"
                f"void {fn}(void) {{\n}}\n")
        else:
            c.write_text('#include "common.h"\n\n'
                         f'INCLUDE_ASM("config/../asm/rock_neo/nonmatchings/{seg}", {fn});\n')

        good, err = insert_yaml(off, seg, off + size)
        if not good:
            fail.append((fn, err)); continue
        ok.append((fn, seg, size, chunk.name))

    for fn, seg, size, ch in ok:
        print(f"  split {fn:<20} 0x{size:<5X} from {ch:<12} -> src/rock_neo/{seg}.c")
    for fn, why in fail:
        print(f"  SKIP  {fn:<20} {why}")
    print(f"\n{len(ok)} split, {len(fail)} skipped. Now: make CPP=cpp split_rock_neo && "
          f"make CPP=cpp check_rock_neo_only")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
