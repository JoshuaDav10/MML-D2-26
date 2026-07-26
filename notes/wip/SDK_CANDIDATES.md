# SDK / Library Function Candidates in Main-Exe Raw Asm
Generated: 2026-07-26  
Source: `tools/dep_priority.py` heuristics + manual inspection of `asm/rock_neo/*.s`

**STATUS: CANDIDATE LIST ONLY.** None of these are confirmed matches. Each entry
lists the heuristic that flagged it and a confidence level. Cross-check against
PSY-Q source / libgcc source before asserting any is confirmed.

---

## Methodology

Scanned all 635 functions in `asm/rock_neo/{10638,32A38,4C710,52C0,543A8,77D4}.s`
with the following heuristics:

1. **memset**: branch + zero-write (sb/sw $zero) + no load from another pointer
2. **memcpy**: load + store + branch + no jal + similar load/store counts
3. **strcpy/strlen**: lbu/lb loop + beqz/bnez null-check (+ sb for strcpy)
4. **GTE-heavy**: ≥3 cop2/ctc2/mtc2/mfc2/cfc2 instructions
5. **libgcc-style**: shift ops + loop + no stack frame + no jal
6. **Small math leaf**: ≤15 insn + no stack + no call + uses mult/mflo/mfhi

False-positive rate is moderate — many game functions trigger structural heuristics
by coincidence. Manual confirmation required before use.

---

## High-Confidence Candidates (manual inspection confirms plausibility)

### func_800177CC — `bzero` / `ClearMemFast` variant  
**File:** `asm/rock_neo/77D4.s`  **Size:** 5 insn  
**Heuristic:** memset  
**Confidence:** HIGH

```
sw $zero, 0x0($a0)
bne $a1, $a0, func_800177CC   ; loop while ptr != end
addiu $a0, $a0, 0x4            ; delay slot: ptr += 4
jr $ra
nop
```
Word-aligned zero-fill loop. Prototype: `void bzero(void *start, void *end)` or
`void ClearMemFast(u32 *dst, u32 *end)`. The end-pointer convention (not length)
matches PSY-Q's `bzero` and GCC runtime `__bzero` style.

---

### func_800177E0 — fill-memory-with-0xFFFFFFFF  
**File:** `asm/rock_neo/77D4.s`  **Size:** 6 insn  
**Heuristic:** memset variant  
**Confidence:** HIGH

Identical structure to `func_800177CC` but fills with -1 (addiu $v0, $zero, -1
then sw $v0). This may be a PSY-Q `FillMemFast` or game-internal "mark all slots
used" helper. No direct PSY-Q SDK equivalent known; likely custom.

---

### func_800176DC — 32-byte bulk `memcpy`  
**File:** `asm/rock_neo/77D4.s`  **Size:** 22 insn  
**Heuristic:** memcpy  
**Confidence:** HIGH

Unrolled 8×lw / 8×sw loop, processes 32 bytes per iteration using $v0–$t5, with
addiu decrement on $a2 (count). Called from `func_80017618` (the scratchpad-swap
function) and from `func_800371A0` (rank 6 in DEP_PRIORITY — called from 36
stages). This is the bulk-copy inner loop.

Prototype: `void memcpy32(u32 *dst, const u32 *src, s32 count_32bytes)`  
or equivalent PSY-Q `MoveMemory`.

---

### func_800175D8 — `SetScratchpadAsStack` / scratchpad swap  
**File:** `asm/rock_neo/77D4.s`  **Size:** 9 insn  
**Heuristic:** manual (scratchpad address 0x1F800400)  
**Confidence:** HIGH

Saves `$sp` to a global variable at 0x80098B44, then sets `$sp = 0x1F800400`
(the PSX scratchpad base). This is a well-known PSY-Q/game pattern for using
the 1 KB scratchpad as a temporary stack during interrupt handlers or critical
sections.

Prototype: `void SaveStack(void)` or `void UseScratchpad(void)`

---

### func_800175FC — `RestoreStack` / restore saved SP  
**File:** `asm/rock_neo/77D4.s`  **Size:** 7 insn  
**Heuristic:** manual (pair with func_800175D8)  
**Confidence:** HIGH

Reads `$sp` back from 0x80098B44 and restores it. Direct pair to `func_800175D8`.

Prototype: `void RestoreStack(void)`

---

### func_80020A4C — GTE initialization / `InitGeom`-class  
**File:** `asm/rock_neo/10638.s`  **Size:** 90 insn  
**Heuristic:** GTE-heavy (ctc2 to GTE control regs $5/$6/$7)  
**Confidence:** MEDIUM

Reads from 0x1F800030–0x1F800038 (scratchpad data), then issues `ctc2 $t4,$5`,
`ctc2 $t5,$6`, `ctc2 $t6,$7` (GTE rotation matrix registers) and subsequently
reads 0x1F800040–0x1F800043 (GPU display mode bytes). This matches the pattern
of PSY-Q `InitGeom()` or a post-geometry matrix setup routine. The large size (90
insn) suggests it also handles display mode initialization.

---

### func_80020BB4 — GTE / GPU bulk setup  
**File:** `asm/rock_neo/10638.s`  **Size:** 425 insn  
**Heuristic:** GTE-heavy  
**Confidence:** LOW-MEDIUM

Same file region as func_80020A4C; 3 ctc2 instructions in a 425-insn body. The
GTE use is not the dominant pattern here — more likely a large init/render function
that happens to set a few GTE registers among other GPU operations.

---

### func_8002D404 — integer division helper  
**File:** `asm/rock_neo/10638.s`  **Size:** 24 insn  
**Heuristic:** libgcc div  
**Confidence:** MEDIUM

Uses `divu $zero, $a0, $a3` (hardware divide, check-for-zero trap), then reads
`mflo`, multiplies quotient back by divisor, subtracts to get remainder, then
combines quotient and remainder bytes using shifts/or. This is consistent with
a combined div+mod helper returning both results, possibly `__divmod` or an
internal `FixedDiv` helper.

---

## Medium-Confidence / Flagged For Review

These triggered heuristics but manual spot-checks reveal game-specific code mixed
with stock patterns. Listed for future verification.

### memset-shaped functions (branch + zero-write, no load)

| Function | Insn | Notes |
|----------|------|-------|
| `func_800177CC` | 5 | Confirmed above (HIGH) |
| `func_800177E0` | 6 | Confirmed above (HIGH) |
| `func_80017618` | 20 | Scratchpad/stack swap (HIGH, but not a simple memset) |
| `func_80050BF4` | 14 | Zero-fill; game data struct |
| `func_80063EF0` | 14 | Zero-fill; likely struct clear |
| `func_800398E0` | 6 | 6 insn; actually a single conditional sb, not a loop |
| `func_8005D7C0` | 33 | Larger; needs manual check |
| `func_8005DA78` | 29 | Larger; needs manual check |

### memcpy-shaped functions (load + store + branch, no jal, balanced load/store)

Most flagged functions (45 total) are game-internal struct-copy or array-process
routines — not generic memcpy. The most structurally clean ones:

| Function | Insn | Notes |
|----------|------|-------|
| `func_800176DC` | 22 | Confirmed bulk memcpy (HIGH) |
| `func_800371A0` | 168 | lwl/lwr pairs (unaligned copy); large, called 36 stages |
| `func_8002D464` | 21 | Short, balanced load/store |
| `func_8002D4B8` | 21 | Similar to func_8002D464 |
| `func_800399EC` | 19 | Short, balanced |
| `func_8003BDC0` | 30 | Has lbu + sb — could be byte-copy or game logic |

### libgcc shift-loop candidates (true positives more selective)

Most "libgcc div" hits are game bit-manipulation routines. The cleaner ones:

| Function | Insn | Notes |
|----------|------|-------|
| `func_8002D404` | 24 | Confirmed div+mod pattern (MEDIUM above) |
| `func_80037C50` | 34 | Shift loop, stackless; inspect manually |
| `func_8003B2D4` | 35 | Shift loop, stackless |
| `func_8003B360` | 48 | More complex; may be bit-table operation |
| `func_8003B420` | 39 | Stackless shift loop |
| `func_8003B4BC` | 33 | Stackless shift loop |
| `func_8003B540` | 29 | Stackless shift loop (NOTE: cluster of similar at 0x8003B2xx) |

The cluster at `0x8003B2D4`–`0x8003B540` (4 consecutive stackless shift-loop
functions in 32A38.s) may be a battery of libgcc int math helpers
(__udivsi3, __umodsi3, __divsi3, __modsi3) inlined together by the linker.

### Small math leaf candidates (≤15 insn, no stack/call, uses mult)

| Function | Insn | Likely purpose |
|----------|------|----------------|
| `func_8002DE04` | 7 | sll + addu + load + store; struct slot write |
| `func_8002DE20` | 5 | sll + addu + load + store; smaller variant |
| `func_8002DE34` | 10 | Similar; array indexing |
| `func_8002FE74` | 12 | Mult + mflo; fixed-point multiply |
| `func_800375D4` | 12 | Shift + table lookup |
| `func_80037604` | 12 | Similar to func_800375D4 |
| `func_800177F8` | 10 | From 77D4.s; small math in the engine-init cluster |

---

## Notable FALSE POSITIVES (heuristic triggered, manual inspection shows game code)

These were flagged but confirmed NOT to be library functions:

| Function | Heuristic | Actual purpose |
|----------|-----------|----------------|
| `func_80039B80` | strlen (9 insn) | Conditional table-pointer load (1 branch, no loop) |
| `func_8004324C` | strlen (12 insn) | Flag-check returning bool (reads offset 0x174, 0x177) |
| `func_8002F928` | strlen (28 insn) | lhu pair add; accumulates short fields |
| `func_8002F3A8` | strcpy (30 insn) | Bit-flag xori/ori manipulation on byte fields |
| `func_80037534` | strcpy (24 insn) | Struct array fill loop with table lookup |
| `func_80045918` | strcpy (13 insn) | Byte counter decrement with conditional branch |
| `func_80031618` | libgcc div | Bit-scan allocation loop (sllv, not div) |
| `func_80022FD4` | libgcc div | Scene dispatch function; uses Scene_work + scratchpad |
| `func_80051B68` | strlen | Task queue push; checks flags, stores args to queue struct |

---

## Summary Counts

| Category | Flagged | Likely True Positives |
|----------|---------|-----------------------|
| memset / bzero | 9 | 2–3 (177CC, 177E0, maybe 50BF4/63EF0) |
| memcpy | 45 | ~3 (176DC, 371A0 partial, a few short ones) |
| strcpy / strlen | 24 | ~0 confirmed; mostly game logic |
| GTE-heavy | 2 | 1–2 (20A4C likely, 20BB4 partial) |
| libgcc int math | 23 | 1 confirmed (2D404); B2D4–B540 cluster plausible |
| Small math leaf | 21 | ~5–8 worth individual inspection |

**Recommended next steps:**
1. Verify the libgcc cluster at `0x8003B2D4`–`0x8003B540` by comparing instruction
   sequences against GCC 2.7.2 mips `libgcc2.c` compiled output.
2. Confirm `func_800177CC` and `func_800177E0` against PSY-Q `bzero` / `ClearMem`.
3. Confirm `func_800176DC` against PSY-Q `MoveMemory` or GCC `memcpy`.
4. Check `func_80020A4C` against PSY-Q `InitGeom` source.
5. Run `tools/dep_priority.py` to check which of these are called from overlays
   (if any appear in the priority table, naming them has double leverage).
