# Parts List — the MML decomp toolchain inventory

Every tool in play, what it does, and its status. Status legend:
**[core]** = used every build/session · **[active]** = used when relevant ·
**[underused]** = present but not part of the regular flow · **[external]** =
off-machine resource, optional.

## Compiler & build pipeline (the six-stage pipe)

| Tool | Role | Status |
|---|---|---|
| `bin/cc1-27` | The compiler — GCC 2.7.2 `cc1`, the thing we match against. `-O2 -G8 -mcpu=3000`. | [core] |
| system `cpp` | Preprocessor (via the `CPP=cpp` override, required every `make`). | [core] |
| `tools/maspx/maspsx.py` | ASPSX assembler emulation (delay-slot nops, div expansion, `$at` order). | [core] |
| `tools/gprel.py` | gp-relative / small-data rewrite; drops small `.extern` to prevent COMMON leaks. | [core] |
| `tools/patchasm.py` | Function-reorder pass (moves deferred C bodies to their `.globl` markers) + `li`→`addiu`. | [core] |
| `mipsel-elf-{as,ld,objcopy}` | GNU binutils — assemble (`-G0`), link, flatten to raw exe. | [core] |
| `.venv` | Python env; needs **rabbitizer** (disasm) and **toml** (permuter). Activate first every session. | [core] |

## Extraction & mapping

| Tool | Role | Status |
|---|---|---|
| `tools/splat` (+ `splat_ext`) | ROM splitter/disassembler; cuts the binary into asm + symbol scaffolding per the splat yaml. | [core] |
| `tools/generate_rock_neo_syms.py` | Generate symbol definitions from the ELF. | [active] |
| `tools/rename_symbol.py` | Rename a symbol across the tree (`rename_symbol.py <old> <new>`). | [active] |
| `tools/xref.py` | Extract callgraph, data xrefs, and family hints from the asm (find sibling function families). | [active] |

## Decompilation aids (first-draft C)

| Tool | Role | Status |
|---|---|---|
| `tools/m2c/` | **m2c / mips_to_c** — decompiles MIPS asm into a first-draft C you then massage toward a match. Attacks the "novel logic" cost directly. Installed in the venv (`m2c`). | [active] |
| `tools/m2ctx.py` | Builds the type/context header m2c needs (so its output uses real struct names). Outputs to `drafts/.ctx/*.m2c`. | [active] |

## Matching & diffing

| Tool | Role | Status |
|---|---|---|
| `./diff.py` (`tools/asm-differ`) | Instruction-level diff of built vs original (submodule; carries a local Py3.14 patch). **Normalizes — gate on the hash, not this.** | [core] |
| `tools/tryfn.sh` | Pipe ONE scratch `.c` through the real pipeline + objdump — brute-force orderings without touching the tree. | [active] |
| `tools/bytecmp.sh` | Assemble a scratch TU, compare instruction words vs the splat `.s` comment column. | [active] |
| `tools/decomp-permuter/` | Last-mile random search for register/scheduling near-matches. Needs `toml`; see `notes/PERMUTER_GUIDE.md`. **Fixed & working as of 2026-07-12.** | [active] |

## Verification & counting

| Tool | Role | Status |
|---|---|---|
| `tools/census.py` | Count matched vs active stubs (cpp-aware). Reads built `.c.o` — run AFTER a fresh build. | [active] |
| `make CPP=cpp check_rock_neo_only` / `check_overlays` / `check` | The verification ladder (main-exe sha1 / 205 overlays / everything). | [core] |
| `tools/bytecmp.sh`, `cmp` | Ground-truth byte compare. | [active] |

## Overlays (CDDATA/DAT)

| Tool | Role | Status |
|---|---|---|
| `tools/buildoverlay.py` | Build a CDDATA/DAT overlay BIN. (Checks return codes now — the ST1A silent-fallback fix.) | [active] |
| `tools/chunk2splatyaml.py` | Generate per-overlay splat config. **Never run on the whole folder — it overwrites all 205 configs.** | [active] |
| `tools/chunkunpacker.py` | Unpack overlay archive chunks. | [active] |

## Misc / infra

| Tool | Role | Status |
|---|---|---|
| `tools/regexr.py` | Command-line regex replace. | [active] |
| `tools/git-hooks/` | Pre-push hook gating on the overlays. | [core] |
| `tools/SCRATCH_TOOLS.md` | Docs for the scratch-TU tools (`tryfn.sh`, `bytecmp.sh`). | — |

## External / untapped

| Resource | Role | Status |
|---|---|---|
| **decomp.me** | Collaborative matching website — upload a stubborn function, get community eyes on it. Not currently used; an option for the hard walls. | [external] |
| **Other PSY-Q GCC 2.7.2 decomp projects** | Their compiler-idiom notes transfer to us (same compiler). | [external] |
| **GCC 2.7.2 source** | The open-source compiler source — the key to the hard "walls" (see the planned codegen-reference effort). Not yet obtained. | [external] |

## Known gaps (as of 2026-07-12)
- **GCC 2.7.2 source not yet pulled** — the deliberate lever for the parked
  compiler walls (53B40, F798). See the planning discussion.
- decomp.me unused — optional collaborative resource for stuck functions.
