# Document Index

Master map of the external reference corpus. Use index-driven lookup: pull only the file (or line range) needed for the current task.

All paths below are relative to `notes/reference/` (symlink to `/home/X/Documents/MML/Decompilation documents/`).

## MIPS / assembly

| Path | Index | Description |
|------|-------|-------------|
| `mips_manual_chunks/` | `INDEX.txt` | MIPS R3000 instruction set manual, chunked for search |
| `padua_psx_docs/` | `README.md` | Practical PSX assembly notes |

## PlayStation hardware

| Path | Index | Description |
|------|-------|-------------|
| `PSXSPX/psxspx_sections/` | `INDEX.txt` | nocash PSX hardware spec (GPU, GTE, SPU, DMA, CD-ROM, etc.) |
| `drhell_psx_research/` | `README.md` | Dr. Hell PSX hardware research (memory map, I/O, XEBRA) |

**Oversized chunks:** Some PSXSPX sections are very large (e.g. `01_Memory_Map_Memory_Map.txt` ~1.7 MB). Pull by **line range only**, never whole-file into Fable or long chat sessions.

## Sony SDK

| Path | Index | Description |
|------|-------|-------------|
| `psyq/` | `MAIN_INDEX.txt` | Official PSYQ SDK docs (Core, Hardware, Graphics, Audio, CD-ROM) |
| `PSYQ_Documentation/` | per-folder indexes | Additional PSYQ reference as individual text files |

## MML-specific

| Path | Index | Description |
|------|-------|-------------|
| `mml_specific_docs/` | `README.md` | ChrisNonyminus/mml1 and DashEditor MML notes |
| `MML_Decompilation_Project_Overview.md` | — | Prior workflow/methodology (**historical**; do not treat old progress numbers as current) |

## Project-generated notes

| Path | Description |
|------|-------------|
| `notes/LESSONS.md` | Accumulated compiler idioms & matching tactics, proven against the hash — read before matching |
| `notes/RESEARCH_BRIEF_gcc272_idioms.md` | Deep-research **prompt** for the two open codegen blockers (const-address CSE-hoist; pointer-fold) |
| `notes/RESEARCH_FINDINGS_gcc272_idioms.md` | Deep-research **answer** to that brief — candidate CSE/hoist + pointer-fold idioms, NOT yet hash-gated |

## Usage

1. Start sessions with this index, not the full corpus.
2. For dense docs, condense with a cheaper tool first, then bring summaries to Fable for planning.
3. Keep `progress.md` mapped vs matched counts separate from any historical overview figures.
