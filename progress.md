# MML-D2-26 Progress

## Mapped (functions in splat config / INCLUDE_ASM stubs, main exe)
- rock_neo main: 475 functions in asm/rock_neo/nonmatchings (420 still active
  INCLUDE_ASM stubs; count verified by preprocessing src and counting
  `.include nonmatchings` lines)

## Matched (recompiles to identical bytes)
- rock_neo main: 64 (verified: 64 = .text function symbols across
  build/src/rock_neo/*.c.o minus active INCLUDE_ASM stubs, with the
  full-binary sha1 OK)
  - moji: func_800542FC + accessor family func_80054310..func_800543F8, func_80054BAC,
    func_80055304, func_80054694, func_80056180, func_8005A858 (typed via MOJI_TASK),
    func_80054410, func_8005457C, func_80054B88, func_8005563C, func_80056128,
    func_80057124, func_80058C08 (script2 opcode handlers)
  - Code800133D8: func_80013418, func_80013890
  - scene: func_8001D878, func_8001DEDC, func_8001F820, func_8001D974,
    func_8001DDC0 (first $gp-relative/sdata match, via tools/gprel.py)
  - cd: func_8001B4C4, func_8001B63C, Cd_read_sync2
  - sub_scrn: Sub_screen_sort_sub
  - sound: func_80019F94, func_8001B2F0
  - game: func_800155A4, func_80015734, func_80015840, func_80016528
    (upstream's pre-written matches, unlocked by the patchasm reorder fix)
    + func_80016BC0, func_80016BF4, func_80016D0C, func_80016D38,
    func_80016D64, func_80016DAC, func_80016E90 (upstream pre-written,
    compiled+matching all along but previously uncounted)
  - main: func_800131FC (upstream), func_8001326C (upstream),
    func_80012FA4, func_80012FC8
  - player: 10 empty funcs func_8003FFA8, func_80040130..func_800402BC
  - debug: func_800629E0

## Last verified build
- 2026-07-04 (late) — `make CPP=cpp check_rock_neo_only` OK after the gprel.py
  pipeline addition + 11-function batch; mutation test performed
  (broke func_8005457C, check failed; restored, OK)
