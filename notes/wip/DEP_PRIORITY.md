# Main-Exe Dependency Priority Queue
Generated: 2026-07-26  
Script: `tools/dep_priority.py`

## Methodology

Scanned all 677 overlay `.s` files (205 overlay directories under `asm/`, excluding
`asm/rock_neo/`) for `jal`/`jalr` instructions targeting main-exe functions.
Main-exe function set = union of `glabel func_*` in `asm/rock_neo/*.s` (628 unique)
and `func_*` names in `src/rock_neo/*.c` (476), total 1079 distinct addresses.

**Status field:**
- `C_FILE` = function address appears in `src/rock_neo/*.c` (may be matched or INCLUDE_ASM stub)
- `RAW_ASM` = still only in `asm/rock_neo/*.s`, not yet in any `.c` file

**Ranking:** primary = distinct stage-program overlay directories calling this function;
secondary = total call-site count across all overlay asm files.

A function called from 37 distinct stages is a game-wide engine call; naming/typing it
(even just giving it a descriptive name and a proper prototype) unlocks readability in
every overlay that references it.

---

## Summary Statistics

| Metric | Count |
|--------|-------|
| Total main-exe functions | 1079 |
| Represented in src/rock_neo/*.c | 476 |
| Still raw asm only | 603 |
| Main-exe functions called from overlays | 185 |
| - already in C file | 43 |
| - still raw asm | 142 |

Of the 185 overlay-called main-exe functions:
- **142 are still raw asm** — highest priority for naming/matching
- **43 are already in a C file** — may already have good names; lower urgency unless INCLUDE_ASM stubs still

---

## Top 50 Priority Functions (sorted by distinct stages, then call sites)

```
RANK  FUNCTION         INSN  SITES  STAGES  STATUS   NOTES
----  ---------------  ----  -----  ------  -------  -----
  1   func_800322A8      23   1370      37  RAW_ASM  tiny! called from every stage
  2   func_8002F9C4      10    629      37  RAW_ASM  tiny! wrapper
  3   func_8002FEA4      --    914      36  C_FILE   already in src (check if matched)
  4   func_80032488      44    545      36  RAW_ASM  medium
  5   func_8002EA50     272     36      36  RAW_ASM  large; complex string/char handler
  6   func_800371A0     168     36      36  RAW_ASM  large; lwl/lwr = memcpy-like
  7   func_8005DC10      41     36      36  RAW_ASM  medium; reads Game_work
  8   func_80031824      22    398      35  RAW_ASM  tiny!
  9   func_80030150      22    312      34  RAW_ASM  tiny!
 10   func_80042238      55    190      34  RAW_ASM  medium
 11   func_80042314      35    185      34  RAW_ASM  small
 12   func_80031618      41    514      31  RAW_ASM  medium
 13   func_8002C8CC      81    324      31  RAW_ASM  large
 14   func_8001EC0C      --    131      31  C_FILE
 15   func_8001DDE4      --    116      31  C_FILE
 16   func_8001F070      --    114      31  C_FILE
 17   func_8001E6C0      --    103      31  C_FILE
 18   func_80048E6C      --    483      30  C_FILE
 19   func_8001EAE8      --    240      30  C_FILE
 20   func_80051B68      17    223      30  RAW_ASM  tiny!
 21   func_80022FD4      92     30      30  RAW_ASM  large
 22   func_80031BEC      46   1270      29  RAW_ASM  medium; 1270 call-sites!
 23   func_80031988      27    244      29  RAW_ASM  small
 24   func_80063BC8      26    244      29  RAW_ASM  small
 25   func_8001DE84      --     97      29  C_FILE
 26   func_8001D494      --     92      29  C_FILE
 27   func_800318D8      22    235      28  RAW_ASM  tiny!
 28   func_80031198      52    195      28  RAW_ASM  medium
 29   func_80032538      39     84      28  RAW_ASM  small
 30   func_80017C30      --    180      27  C_FILE
 31   func_8002FFC4      37    170      27  RAW_ASM  small
 32   func_8004B414     110    413      26  RAW_ASM  large
 33   func_800341BC      84    308      26  RAW_ASM  large
 34   func_8001D58C      --     60      26  C_FILE
 35   func_8003A13C       6    205      25  RAW_ASM  TINY (6 insn)!
 36   func_80031518     140     25      25  RAW_ASM  large
 37   func_80039E18      --     78      25  C_FILE
 38   func_8001D878      --     57      25  C_FILE
 39   func_80030058      48     48      25  RAW_ASM  medium
 40   func_80031D5C      --    277      24  RAW_ASM
 41   func_80048C60     132    132      24  RAW_ASM  large
 42   func_80036478      96     96      24  RAW_ASM  large
 43   func_80039FAC      --     75      24  RAW_ASM
 44   func_8003346C      81     81      23  RAW_ASM  large
 45   func_80049C24      75     75      23  RAW_ASM  large
 46   func_80031A4C     120    120      22  RAW_ASM  large
 47   func_800334C4      49     49      22  RAW_ASM  medium
 48   func_80030418      27     27      22  RAW_ASM  small
 49   func_8003648C      77     77      21  RAW_ASM
 50   func_80039820      68     68      21  RAW_ASM
```

---

## Top-30 Recommended Naming/Typing Priority

These 30 functions are the highest-leverage targets. For each: look up in asm,
understand the signature, give a descriptive name, and export a proper prototype
in a header — even without a full match, a good name + prototype makes every
overlay caller readable.

### Tier 1: Tiny RAW_ASM called from 35-37 stages — immediate wins

| Rank | Function | Insn | Stages | Sites | Action |
|------|----------|------|--------|-------|--------|
| 1 | `func_800322A8` | 23 | 37 | 1370 | Name + match (tiny, 23 insn) |
| 2 | `func_8002F9C4` | 10 | 37 | 629 | Name + match (10 insn wrapper) |
| 8 | `func_80031824` | 22 | 35 | 398 | Name + match (tiny) |
| 9 | `func_80030150` | 22 | 34 | 312 | Name + match (tiny) |

These four are tiny (10-23 instructions), called from virtually every stage program.
Each one matched = one less mystery at 310-1370 call-sites.

### Tier 2: Small-medium RAW_ASM, very wide coverage (31-36 stages)

| Rank | Function | Insn | Stages | Sites | Action |
|------|----------|------|--------|-------|--------|
| 4 | `func_80032488` | 44 | 36 | 545 | Name + attempt match |
| 7 | `func_8005DC10` | 41 | 36 | 36 | Name + attempt match (Game_work reader) |
| 11 | `func_80042314` | 35 | 34 | 185 | Name + attempt match |
| 20 | `func_80051B68` | 17 | 30 | 223 | Name + match (tiny 17 insn) |
| 23 | `func_80031988` | 27 | 29 | 244 | Name + attempt match |
| 24 | `func_80063BC8` | 26 | 29 | 244 | Name + attempt match |
| 27 | `func_800318D8` | 22 | 28 | 235 | Name + match (tiny) |
| 35 | `func_8003A13C` | 6 | 25 | 205 | Name + match (6 insn!) |

### Tier 3: Larger RAW_ASM worth understanding (naming priority even without a full match)

| Rank | Function | Insn | Stages | Sites | Action |
|------|----------|------|--------|-------|--------|
| 5 | `func_8002EA50` | 272 | 36 | 36 | Name + prototype (complex) |
| 6 | `func_800371A0` | 168 | 36 | 36 | Name (lwl/lwr pattern = memcpy?) |
| 10 | `func_80042238` | 55 | 34 | 190 | Name + attempt match |
| 12 | `func_80031618` | 41 | 31 | 514 | Name + attempt match |
| 13 | `func_8002C8CC` | 81 | 31 | 324 | Name |
| 21 | `func_80022FD4` | 92 | 30 | 30 | Name |
| 22 | `func_80031BEC` | 46 | 29 | 1270 | Name + match (1270 call-sites across 29 stages!) |
| 28 | `func_80031198` | 52 | 28 | 195 | Name |
| 32 | `func_8004B414` | 110 | 26 | 413 | Name |
| 33 | `func_800341BC` | 84 | 26 | 308 | Name |

### Notable: func_80031BEC (rank 22)
Called 1270 times from 29 distinct stages — second-highest call-site count overall.
Only 46 instructions. This is almost certainly a core engine dispatch or update
function. Very high ROI for naming and matching.

---

## Context: C_FILE entries in top 30

These are already in `src/rock_neo/*.c` but included for completeness. Their
overlay call-counts confirm they are high-value engine functions. If any of these
are still INCLUDE_ASM stubs (not yet matched), they should be prioritized alongside
the RAW_ASM tier-1 functions.

| Rank | Function | Stages | Sites | Next step |
|------|----------|--------|-------|-----------|
| 3 | `func_8002FEA4` | 36 | 914 | Verify matched or still INCLUDE_ASM stub |
| 14 | `func_8001EC0C` | 31 | 131 | Same |
| 15 | `func_8001DDE4` | 31 | 116 | Same |
| 16 | `func_8001F070` | 31 | 114 | Same |
| 17 | `func_8001E6C0` | 31 | 103 | Same |
| 18 | `func_80048E6C` | 30 | 483 | Same |
| 19 | `func_8001EAE8` | 30 | 240 | Same |
| 25 | `func_8001DE84` | 29 | 97 | Same |
| 26 | `func_8001D494` | 29 | 92 | Same |
| 30 | `func_80017C30` | 27 | 180 | Same |

---

## How to Use This Table

1. **Pick a Tier-1 tiny function** (rank 1, 2, 8, 9 — under 25 insn, called from
   35-37 stages). These are the cheapest-to-match, highest-leverage targets in the
   entire project.

2. **Run `./diff.py func_800XXXXXX`** — if the diff is short (tiny function), attempt
   a match using the existing LESSONS.md patterns.

3. **For larger functions** (Tier 2/3), at minimum: read the asm, identify the
   parameters from the call-sites, give it a meaningful name in the splat YAML or
   a comment header, and note its purpose in a local WIP file. Even a name without
   a match is a productivity multiplier across all 29-37 overlay callers.

4. **Re-run `tools/dep_priority.py`** after naming to confirm the function shows
   up as `C_FILE` once added to a source file.
