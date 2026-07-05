# MML-D2-26 Progress

## Mapped (functions in splat config / INCLUDE_ASM stubs, main exe)
- rock_neo main: 475 functions in asm/rock_neo/nonmatchings (453 still stubs)

## Matched (recompiles to identical bytes)
- rock_neo main: 24 (2 upstream + 22 this session)
  - moji: func_800542FC + accessor family func_80054310..func_800543F8, func_80054BAC
  - Code800133D8: func_80013418, func_80013890
  - scene: func_8001D878, func_8001DEDC, func_8001F820
  - cd: func_8001B4C4, func_8001B63C
  - debug: func_800629E0

## Last verified build
- 2026-07-04 — build/rock_neo.exe sha1 ffc08fd2b6113e84dc0e75ba63a4feaca0eb1d46 (perfect match)
