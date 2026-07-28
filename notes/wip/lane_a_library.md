# LANE A — Library-code identification in ROCK_NEO.EXE

Date: 2026-07-28. Read-only analysis; nothing in the repo was modified.
All scripts and machine-readable output live in `/tmp/lane_a/`.

---

## 0. Headline answer

**Of the 614 unsplit ENGINE functions, ZERO are Sony PSY-Q library code or gcc runtime
helpers. All 614 are Capcom code and must be reverse-engineered.**

That is a negative result, but a strongly-evidenced one, and it comes with a compensating
positive result:

**The PSY-Q library *is* in the binary — it is the `psxsdk/code` splat segment at
0x80067FF8–0x800806C4 (100,556 bytes, 446 `glabel`s), which the function map does not
count at all** (`engine_total` 1119 covers only 0x80011C90–0x80067FF8, contiguously, with
zero gaps). Inside that segment I byte-identified 187 addresses against published PSY-Q
objects, including **37 functions that are still named `func_XXXXXXXX` in
`asm/rock_neo/psxsdk/code.s` and can be renamed today**, plus 60 library functions that
have no `glabel` at all (missing function boundaries in that .s).

So the cheap-win the lane was looking for exists — it is just in a different segment than
the brief assumed.

---

## 1. What is actually in the .LIB files, and how I read them

`vendor/psyq-obj-parser-src` is only the single `.cc` from PCSX-Redux; it needs the whole
pcsx-redux support tree (`support/file.h`, `elfio`, `fmt`, `magic_enum`), none of which is
vendored. Building it was not worth it. I re-implemented the format instead:

- `/tmp/lane_a/psyqlib.py` — from-scratch parser for the PSY-Q `LIB` archive container and
  the `LNK` (version 2) object format. Opcode/field layouts were read off
  `vendor/psyq-obj-parser-src/tools/psyq-obj-parser/psyq-obj-parser.cc`.
- One thing that source does **not** tell you, and that I had to derive from the bytes:
  in the `LIB` container each member header is
  `name[8] | date u32 | offs u32 | size u32 | {len-prefixed symbol names} 0x00`,
  where **`size` is the length of the entire member entry (header + symtab + LNK payload)**
  and `offs` is the offset from the entry start to the `LNK` payload. Next entry is at
  `p + size`, *not* `p + offs + size`.

Corpus actually extracted:

| | count |
|---|---|
| `.lib` archives parsed | 74 (psyq_4.3 / 4.4 / 4.5, incl. `4.4/lib/patches/`) |
| library members (objects) | 5,847 |
| `.text` function slices with bytes + per-word relocation masks | **8,108** |
| distinct exported symbol names | **2,598** |
| members that failed to parse | 4 (`CMXboot.lib` one member, `libsn.lib` `CACHE` in 4.4/4.5, one member of 4.3 `libsn.lib`) — all report `bad expr op 54` / bogus header; negligible |

Each slice carries a **byte-level relocation mask** derived from the LNK `RELOCATION`
records (REL26 → low 26 bits unknown, HI16/LO16/GPREL16 → low 16 bits unknown, REL32 →
whole word unknown). That is what makes byte-exact comparison against *linked* game code
possible.

---

## 2. Comparison method

Three tiers, from strictest to loosest:

- **CONFIRMED / exact** — `(game_word ^ lib_word) & ~reloc_mask == 0` for every word of the
  function. Because the mask hides exactly the fields the linker filled in, this is
  byte-identity in the only sense available.
- **LIKELY / loose** — every instruction identical after zeroing all 16-bit immediates and
  26-bit jump targets (registers, opcodes and shift amounts still have to match exactly).
- **GUESS / mnemonic** — same opcode (+funct) sequence only.

Two searches were run:

1. `compare.py` — every one of the 1,119 mapped ENGINE functions against all 8,108 slices.
2. `scan_exe.py` — a **sliding scan of the entire 0xC9000-byte text image** (205,824 words),
   anchored on an 8-instruction loose window per library function, then full verification.
   This does not depend on the function map being right, and it is what found the psxsdk
   segment.

### 2b. The method validates itself

`asm/rock_neo/psxsdk/code.s` already carries 247 human/heuristic names. Where my scan and an
existing name both exist at the same address:

**87 agree, 3 "disagree" — and all 3 disagreements are non-contradictions:**

| addr | project name | my exact hit | explanation |
|---|---|---|---|
| 0x8006B488 | `SsSepClose` | `SsSeqClose` | the two SDK functions compile to the identical 9 words modulo relocation; not distinguishable by bytes. Project name stands. |
| 0x8006F718 | `_SsSndPlay` | `SsSepPause`/`SsSepReplay` | same wrapper shape, same reason. |
| 0x8007FFE0 | `firstfile` | `firstfile2` | same BIOS trampoline, SDK renamed the symbol between versions. |

Zero real conflicts across 90 checks. Names it reproduced unaided include `RotMatrix`,
`RotMatrixYXZ`, `RotMatrixY`, `MulMatrix0/1/2`, `InitGeom`, `rsin`, `rcos`, `CdSync`,
`CdIntToPos`, `CdPosToInt`, `PCopen`, `PCread`, `SetRCnt`, `SsSetMVol`, `_spu_setReverbAttr`
(308 words, exact), `setjmp`, `memcpy`, `printf`.

---

## 3. Result for the 614 unsplit ENGINE functions

Twelve of the 614 produced a hit. **Every one is a false positive**, and I resolved each by
following the call target rather than trusting the shape:

| function | insn | matched shape | verdict | evidence |
|---|---|---|---|---|
| `func_800153CC` | 8 | one-line wrapper | **Capcom** | `jal 0x80031824` — an engine function, not SDK |
| `func_8004493C` | 8 | one-line wrapper | **Capcom** | `jal 0x80031988` (engine) |
| `func_8004557C` | 8 | one-line wrapper | **Capcom** | `jal 0x80031988` (engine) |
| `func_80045980` | 8 | one-line wrapper | **Capcom** | `jal 0x80031988` (engine) |
| `func_8004674C` | 8 | one-line wrapper | **Capcom** | `jal 0x80031988` (engine) |
| `func_80047BD8` | 8 | one-line wrapper | **Capcom** | `jal 0x80031988` (engine) |
| `func_80047C64` | 8 | one-line wrapper | **Capcom** | `jal 0x80047CD8` (engine) |
| `func_800485C4` | 8 | one-line wrapper | **Capcom** | `jal 0x80031988` (engine) |
| `func_80067E4C` | 8 | one-line wrapper (loose only) | **Capcom** | `jal 0x8001DA8C` = `Sce_flag_on` |
| `func_8005D95C` | 4 | global getter | **Capcom** | `lui $v0,0x800C; lbu $v0,-0x4279($v0); jr $ra; nop` |
| `func_8005EBC4` | 4 | global getter | **Capcom** | `lui $v0,0x800C; lh $v0,-0x426C($v0); jr $ra; nop` |
| `func_8005EBD4` | 4 | global getter | **Capcom** | `lui $v0,0x800C; lh $v0,-0x426A($v0); jr $ra; nop` |

The "matched" library names for the 8-word group are 21 different SDK symbols at once
(`CdSync`, `CdFlush`, `atol`, `__builtin_vec_new`, `PadStop`, …) — because they *all*
compile to the same eight words:

```
addiu $sp,$sp,-0x18 ; sw $ra,0x10($sp) ; jal <reloc> ; nop
lw $ra,0x10($sp)    ; addiu $sp,$sp,0x18 ; jr $ra ; nop
```

That is just what gcc 2.7.2 emits for `void f(void){ g(); }`. It is a shape, not a
fingerprint. Same for the 4-word getters.

**Nothing else in the engine region matched at any tier.** No `memcpy`, no `memset`, no
`strcpy`, no `strlen`, no `qsort`, no `sprintf`, no GTE/GPU library body.

### Why the SDK is absent from the engine region

`config/splat.us.rock_neo.yaml` line `- [0x587F8, asm, psxsdk/code]` puts the whole SDK in
its own segment starting at vram 0x80067FF8 — exactly where the mapped engine functions
stop. The engine region reaches it by `jal`: **559 `jal` sites from engine code hit 135
distinct addresses in the psxsdk segment.** The link layout is clean; there is no
interleaving to find.

---

## 4. libgcc / gcc runtime helpers

Flags in scope: `-mcpu=3000 -O2 -G8 -msoft-float -fpcc-struct-return -mgas -funsigned-char`.

**Finding: no libgcc helper is present anywhere in ROCK_NEO.EXE.**

Two independent lines of evidence.

**(a) The MIPS backend never calls SImode helpers.** In `vendor/mips-gcc-2.7.2/config/mips/mips.md`,
`divsi3` (line 1385), `modsi3` (1411), `udivsi3` (1437) and `umodsi3` (1463) are all
`define_insn`, i.e. expanded inline to hardware `div`/`divu` + zero-check `break 7`. gcc
2.7.2/mips **cannot** emit a call to `__divsi3`/`__udivsi3`/`__modsi3`/`__umodsi3`. The
SDK_CANDIDATES hypothesis that 0x8003B2D4–0x8003B540 is "a battery of __udivsi3/__umodsi3/
__divsi3/__modsi3 inlined by the linker" is therefore impossible a priori. (Consistently,
those symbols do not exist in any of the 74 .lib files either.)

**(b) The DImode helpers that *do* exist (libsn.lib, 4.4/4.5) are nowhere in the image.**
Best alignment found anywhere in 205,824 words, scored on *mnemonics only* (the loosest
possible test):

| helper | size | best mnemonic match in the whole exe |
|---|---|---|
| `__udivmoddi4` | 388 w | 35/388 (9%) |
| `__udiv_w_sdiv` | 116 w | 23/116 (20%) |
| `__moddi3` | 44 w | 17/44 |
| `__divdi3` | 38 w | 15/38 |
| `__main` | 28 w | 13/28 |
| `__ffsdi2` | 22 w | 10/22 |
| `__ashldi3` / `__lshrdi3` | 19 w | 5/19 |
| `__ashrdi3` | 19 w | 4/19 |
| `__cmpdi2` | 16 w | 4/16 |
| `__gcc_bcmp` | 16 w | 6/16 |
| `__muldi3` | 13 w | 5/13 |
| `__negdi2`, `__ucmpdi2` | 7/16 w | no alignment at all |

Random noise level for mnemonic matching is roughly 20–30%. Nothing here is a signal.
**Conclusion: the game uses no `long long` and no soft-float; the only gcc runtime symbol
linked is whatever `__main` stub sits at 0x80067FF8 (`jr $ra; nop`, already labelled
`__main` in `code.s`).**

Also checked and absent: the compiled `libc2.lib` bodies of `memcpy` (16 w), `memset` (12 w),
`strcpy` (20 w), `strlen` (16 w), `strcmp` (28 w), `strncpy` (28 w), `memmove` (28 w),
`bcopy`, `bzero`, `qsort` (100 w), `sprintf` (544 w). Best loose scores 4–12 words out of
12–544. **MML links the BIOS versions instead** — `libc.lib`'s `memcpy`/`strcpy`/`printf`/
`rand`/`setjmp` are 4-word BIOS trampolines (`addiu $t2,$zero,0xA0/0xB0; jr $t2;
addiu $t1,$zero,N`), and I found the whole table of them **exactly** at
0x8007FC00–0x80080530 (see §6).

---

## 5. Verdicts on `notes/wip/SDK_CANDIDATES.md`

| candidate | doc's claim | verdict | evidence |
|---|---|---|---|
| `func_800177CC` (5 insn) | HIGH: `bzero`/`ClearMemFast` | **Capcom, not library** | `sw $zero,0($a0); bne $a1,$a0,loop; addiu $a0,4; jr $ra`. PSY-Q's `bzero` is a BIOS trampoline; `libc2.lib`'s is 12 words with a *length* argument. No library takes (start,end). Trivial C, but game-internal. |
| `func_800177E0` (6 insn) | HIGH: fill-with-0xFFFFFFFF | **Capcom** | doc already says "likely custom" — confirmed, no SDK equivalent exists. |
| `func_800176DC` (22 insn) | HIGH: bulk `memcpy` | **Capcom** | 8×lw/8×sw unrolled, count is in *32-byte blocks*. PSY-Q `memcpy` = BIOS trampoline; `libc2` `memcpy` = 16 words, byte-count, no unroll. Zero match at any tier. |
| `func_800175D8` / `func_800175FC` | HIGH: scratchpad stack swap | **Capcom** (idiom, not code) | the pattern is a known PSY-Q *idiom*; no SDK function with these bytes exists in 74 libs. |
| `func_80020A4C` (90 insn) | MEDIUM: `InitGeom`-class | **Capcom** | real `InitGeom` is 34 words and sits at 0x8007A4F0 (exact match, and already labelled `InitGeom`). 0x80020A4C matched nothing. |
| `func_80020BB4` (425 insn) | LOW-MED | **Capcom** | no hit. |
| `func_8002D404` (24 insn) | MEDIUM: `__divmod` helper | **Capcom** | `divu $zero,$a0,$a3; bnez $a3; break 7; mflo; mult; …; sll 24/16/8; or,or,or`. Four arguments, packs four bytes into one word — a colour/fixed-point pack routine. The `divu/bnez/break 7` triple is gcc's **inline** division idiom, i.e. *evidence of C source using `/`*, not evidence of a helper. |
| `0x8003B2D4`–`0x8003B540` cluster | "likely `__udivsi3` family" | **Capcom** | impossible: see §4(a). No hit at any tier. |
| all memcpy/strcpy/strlen "medium" tables | — | **all Capcom** | none matched. |

**Net: 0 of the SDK_CANDIDATES entries are library code.** The doc's own warning ("moderate
false positive rate", "none confirmed") was correct; the rate is 100%.

---

## 6. The real prize: the `psxsdk/code` segment (0x80067FF8–0x800806C4)

| | count |
|---|---|
| `glabel`s in `asm/rock_neo/psxsdk/code.s` | 446 |
| already carrying a real (non-`func_`/`D_`) name | 247 |
| still generic `func_XXXXXXXX` / `D_XXXXXXXX` | 199 |
| distinct addresses with a library hit | 193 (187 exact + 6 loose-only) |
| hits landing exactly on a `glabel` | 131 |
| hits landing where there is **no** `glabel` | 62 (60 of them exact) |
| generic labels I can name **exactly** | **37** |
| generic labels I can name loosely | 1 |
| generic labels with no hit at all | 162 |

### 6a. CONFIRMED renames (exact, unambiguous) — 21

| address | current label | identification | source lib |
|---|---|---|---|
| 0x8006A5A4 | `func_8006A5A4` | `_spu_FgetRXXa` | libspu |
| 0x800717D0 | `func_800717D0` | `_SpuIsInAllocateArea` | libspu |
| 0x800730E8 | `func_800730E8` | `_spu_getInTransfer` | libspu |
| 0x800738BC | `func_800738BC` | `_SsVmSeKeyOff` | libsnd |
| 0x8007699C | `func_8007699C` | `PCclose` | libsn/libetc |
| 0x80076D2C | `func_80076D2C` | `VSyncCallbacks` | libetc |
| 0x80076D8C | `func_80076D8C` | `RestartCallback` | libetc |
| 0x80076DCC | `func_80076DCC` | `GetIntrMask` | libapi |
| 0x80077964 | `func_80077964` | `CdComstr` | libcd |
| 0x80077998 | `func_80077998` | `CdIntstr` | libcd |
| 0x8007B118 | `func_8007B118` | `SetGeomOffset` | libgte |
| 0x8007B138 | `func_8007B138` | `SetGeomScreen` | libgte |
| 0x8007FCE0 | `func_8007FCE0` | `StartCARD` | libcard |
| 0x8007FD60 | `func_8007FD60` | `StopCARD2` | libcard |
| 0x8007FDF8 | `func_8007FDF8` | `puts` | libc |
| 0x8007FEA8 | `func_8007FEA8` | `_96_remove` | libapi |
| 0x8007FF70 | `func_8007FF70` | `EnterCriticalSection` | libapi |
| 0x8007FF80 | `func_8007FF80` | `ExitCriticalSection` | libapi |
| 0x800804D8 | `func_800804D8` | `StopPAD2` | libpad |
| 0x800804E8 | `func_800804E8` | `PAD_init2` | libpad |
| 0x8006B594 | `func_8006B594` | `SsInitHot` (narrowed from {`MemCardInit`,`SsInitHot`,`_card_open`} by neighbourhood = libsnd) | libsnd |

`EnterCriticalSection`/`ExitCriticalSection` are corroborated independently: engine code
`jal`s 0x8007FF70 six times and 0x8007FF80 seven times, which is exactly the
enter/exit-pairs pattern.

### 6b. LIKELY renames (exact bytes, several SDK symbols share them) — 16

The bytes are identical to more than one library symbol because those symbols are
themselves identical wrappers. Candidate set given; pick by call-site semantics.

| address | current label | candidate set |
|---|---|---|
| 0x8006EC8C | `func_8006EC8C` | `SpuInit` \| `SsUtReverbOff` |
| 0x800721A4 | `func_800721A4` | `SpuInit` \| `SsUtReverbOff` |
| 0x800721C4 | `func_800721C4` | `SpuInitHot` \| `SsStart` \| `SsUtReverbOn` |
| 0x80070114 | `func_80070114` | `SsSeqPause` \| `SsSeqReplay` \| `SsSeqStop` |
| 0x8007013C | `func_8007013C` | `SsSepPause` \| `SsSepReplay` |
| 0x80072084 | `func_80072084` | `SsUtGetReverbType` — the alternative `GsGetActiveBuff` is excluded because libgs produced no unambiguous hit anywhere in this binary |
| 0x80077760 | `func_80077760` | libcd getter: `DS_shell_open` \| `DsQueueLen` \| … |
| 0x80077880 / 0x80077890 / 0x800778A0 | `func_*` | libcd 4-word getters: `CdLastCom` \| `CdMode` \| `CdStatus` |
| 0x8007792C / 0x800779EC / 0x80077E68 | `func_*` | libcd one-line wrappers: `CdFlush` \| `CdReady` \| `CdDataSync` \| … |
| 0x80077E24 | `func_80077E24` | `CdGetSector2` (0x80077E04 is already `CdGetSector`) |
| 0x80077E44 | `func_80077E44` | `CdDataCallback` |
| 0x8008052C | `func_8008052C` | `DisablePAD` (0x80080518 already `EnablePAD`) |
| 0x8007FD70 | `func_8007FD70` | `_ExitGun`-shaped, loose only — **GUESS** |

### 6c. Library functions with NO `glabel` in `code.s` — 60

These are genuine SDK functions the disassembly never gave a boundary to. Splitting them
out would add 60 named functions for free. The libgte block is the bulk of it:

`SetVertex0/1/2`, `SetVertexTri`, `SetRGBfifo`, `SetIR123`, `SetIR0`, `SetSZfifo3/4`,
`SetSXSYfifo`, `SetRii`, `SetMAC123`, `SetData32`, `LocalLight`, `DpqColor`, `NormalColor`,
`NormalColor3`, `NormalColorDpq`, `NormalColorDpq3`, `NormalColorCol3`, `ColorDpq`,
`ColorCol`, `AverageSZ3`, `AverageSZ4`, `AverageZ3`, `AverageZ4`, `ReadOTZ`, `LightColor`,
`DpqColorLight`, `DpqColor3`, `Intpl`, `Square12`, `Square0`, `Lzc`, `ReadLZC`,
`MulMatrix2` (68 w), `RotMatrixX` (104 w), `RotMatrixZ` (104 w), `DumpClut`, `SetLineF3`,
`SetLineF4`, `SetDrawMove`, `GetODE`, `GetRCnt` (14 w) — all **exact**.
Full list with addresses: `/tmp/lane_a/scan_report.txt` (§"psxsdk region function starts").

### 6d. BIOS trampoline table, exactly identified — 0x8007FC00–0x80080530

Already largely named in `code.s`; my scan confirms every one byte-for-byte:
`_bu_init`, `_card_info`, `_card_load`, `_card_write`, `_new_card`, `InitCARD2`,
`StartCARD2`, `setjmp`, `strcat`, `strcpy`, `memcpy`, `rand`, `printf`, `InitHeap`,
`FlushCache`, `GPU_cw`, `DeliverEvent`, `OpenEvent`, `WaitEvent`, `TestEvent`,
`EnableEvent`, `OpenTh`, `CloseTh`, `ChangeTh`, `ReturnFromException`, `ResetEntryInt`,
`HookEntryInt`, `open`, `read`, `write`, `close`, `format`, `firstfile2`, `nextfile`,
`ChangeClearPAD`, `ChangeClearRCnt`, `InitPAD2`, `StartPAD2`, `SysEnqIntRP`, `SysDeqIntRP`.

**This is the answer to "where is memcpy/strcpy": they are BIOS calls, 16 bytes each,
already sitting in the psxsdk segment.** Nothing to decompile.

---

## 7. Engine → psxsdk call graph (useful for the 614)

559 `jal` sites in the engine region reach 135 distinct psxsdk addresses. 68 of those 135
now carry a library-confirmed name; 65 more already carried a project name; 2 targets
(0x801D8130, 0x801D816C) point outside the main exe (overlay-resident).

Top callees by site count — this is effectively the SDK-API surface the 614 unsplit
functions are written against:

`rand`×41, `rcos`×39, `rsin`×38, `VSync`×26, `GetTPage`×18, `AddPrim`×18, `TestEvent`×16,
`SetDrawMode`×13, `SquareRoot0`×13, `RotMatrix`×13, `RotMatrixY`×12, `RotMatrixYXZ`×12,
`GetClut`×11, `CdSyncCallback`×10, `OpenEvent`×9, `EnableEvent`×9, `CdReadyCallback`×9,
`PCread`×8, `ApplyMatrix`×8, `ScaleMatrix`×8, `ClearOTagR`×7, `ExitCriticalSection`×7,
`ratan2`×7, `EnterCriticalSection`×6, `LoadImage`×6, `catan`×6, `ApplyMatrixLV`×6,
`VectorNormalS`×6, `SetFogNear`×6, `CdGetSector`×5 …

Full table: `/tmp/lane_a/final_tables.txt`.

---

## 8. What I could NOT identify — plainly

| set | size | identified as library | not identified |
|---|---|---|---|
| **614 unsplit ENGINE functions** | 614 | **0** | **614** (all Capcom; 12 gave shape-only false positives, resolved above) |
| 155 STUB engine functions | 155 | 0 | 155 |
| 350 MATCHED engine functions | 350 | 0 | n/a |
| psxsdk labels still generic | 199 | 37 exact + 1 loose | **161** |
| psxsdk library functions with no label | 60 found | 60 exact | (unknown how many more exist) |

The 161 unidentified psxsdk labels are the honest gap. They are almost certainly PSY-Q
too — they sit between confirmed PSY-Q functions and are reached only by SDK-shaped calls —
but their bodies differ from 4.3/4.4/4.5. The most likely reason is the version caveat
below; a secondary reason is that libgpu/libgte primitives (`AddPrim`, `GetTPage`,
`SetDrawMode`, …) are frequently *macro/inline* in `libgpu.h` and so have no `.text` slice
in any `.lib` to compare against.

---

## 9. Confidence and caveats — read before acting

1. **Version drift is real and unquantified.** cc1 reports `2.7.2.SN32.3.7`, which ships
   with PSY-Q 4.0; the mirror only has 4.3+. I got 187 exact hits anyway, so a large part
   of the SDK is byte-stable across 4.0→4.5. But `vendor/psy-q-decomp/psx_psyq_signatures`
   and `vendor/psy-q-decomp/psy-q` are **empty submodules** — the 4.0 toolchain and the
   upstream signature database were never cloned. `prep_40.sh` shows the upstream project
   does support 4.0. **Cloning those two submodules is the single highest-value next step**
   and would likely convert a large slice of the 161 unknowns.
2. **CONFIRMED means "byte-identical modulo relocations".** It does not prove the game
   linked *that* object; it proves the code is that function. For 4-to-9-word functions
   that distinction matters (see the 12 false positives in §3); for anything ≥ 20 words it
   is decisive.
3. **Shape is not identity.** Every false positive in this lane, and every false positive
   in `notes/wip/SDK_CANDIDATES.md`, comes from treating a compiler-output shape (one-line
   wrapper, global getter, unrolled copy loop, `divu`+`break 7`) as a fingerprint. The fix
   that worked here was always to resolve the call target or the size.
4. **The 614 conclusion is robust to the version caveat.** It does not depend on matching
   4.0 bytes: the engine region is a contiguous 0x56368-byte block that ends exactly where
   the SDK segment begins, and every SDK reference from it is an outbound `jal`. Even a
   perfect 4.0 corpus would not put library code inside it.

---

## 10. Reproduction

```
/tmp/lane_a/psyqlib.py        LIB + LNK parser (read-only)
/tmp/lane_a/dump_libs.py      -> libdb.pkl   (8108 slices w/ bytes + reloc masks)
/tmp/lane_a/dump_game.py      -> gamedb.pkl  (1119 engine funcs from disks/us/ROCK_NEO.EXE)
/tmp/lane_a/compare.py        -> compare_out.txt   (per-mapped-function comparison)
/tmp/lane_a/scan_exe.py       -> scanhits.pkl      (whole-image sliding scan)
/tmp/lane_a/report_scan.py    -> scan_report.txt, scan_rows.json
/tmp/lane_a/final_tables.py   -> final_tables.txt, psxsdk_table.json
/tmp/lane_a/near2.py          -> anchor-vote near-match (libc / libgcc absence proof)
/tmp/lane_a/psxsdk_new_names.json   the 38 proposed renames
```

Addressing used throughout: `file_offset = vram - 0x8000F800`, text image at
`exe[0x800 : 0x800+0xC9000]`, load address 0x80010000 (PS-EXE header verified).
