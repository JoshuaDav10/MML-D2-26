# MML-D2-26 Progress

## Mapped (functions in splat config / INCLUDE_ASM stubs, main exe)
- rock_neo main: 475 functions in asm/rock_neo/nonmatchings (445 still stubs)

## Matched (recompiles to identical bytes)
- rock_neo main: 32 (2 upstream + 30 this session)
  - moji: func_800542FC + accessor family func_80054310..func_800543F8, func_80054BAC,
    func_80055304, func_80054694, func_80056180, func_8005A858 (now typed via MOJI_TASK)
  - Code800133D8: func_80013418, func_80013890
  - scene: func_8001D878, func_8001DEDC, func_8001F820, func_8001D974
  - cd: func_8001B4C4, func_8001B63C, Cd_read_sync2
  - sub_scrn: Sub_screen_sort_sub
  - sound: func_80019F94
  - debug: func_800629E0

## Last verified build
- 2026-07-04 — build/rock_neo.exe sha1 ffc08fd2b6113e84dc0e75ba63a4feaca0eb1d46 (perfect match)
