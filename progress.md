# MML-D2-26 Progress

## Mapped (functions in splat config / INCLUDE_ASM stubs, main exe)
- rock_neo main: 475 functions in asm/rock_neo/nonmatchings (373 still active
  INCLUDE_ASM stubs; count verified by preprocessing src and counting
  `.include nonmatchings` lines)

## Matched (recompiles to identical bytes)
- rock_neo main: 111 (verified: 111 = .text function symbols across
  build/src/rock_neo/*.c.o minus active INCLUDE_ASM stubs, with the
  full-binary sha1 OK)
- Volume: ~1400 of ~31,300 mapped instructions (~4.5%)
  - moji: func_800542FC + accessor family func_80054310..func_800543F8, func_80054BAC,
    func_80055304, func_80054694, func_80056180, func_8005A858,
    func_80054410, func_8005457C, func_80054B88, func_8005563C, func_80056128,
    func_80057124, func_80058C08, func_80054424, func_8005459C,
    func_80054AB4 + func_80054A84 (script/script2 stack pops),
    func_8005531C, func_80055660, MojiTaskExec2, func_80057BFC,
    func_80054BB4, func_80055CC4, func_80056148, func_80054B4C,
    func_80057708, func_80057A94 (script call-stack push + table jump),
    func_80057DB8, func_80058DB4
  - Code800133D8: func_80013418, func_80013890, func_80013F60, func_80013F8C
  - scene: func_8001D878, func_8001DEDC, func_8001F820, func_8001D974,
    func_8001DDC0, func_8001DD88, func_8001D854, func_8001DEE4, func_8001E7E4, func_8001E810,
    func_8001F1DC, func_8001F20C (SCENE_WORK typed), func_8001FB24,
    Sce_flag_test (the flag-bit reader; unused 8-byte frame local)
  - cd: func_8001B4C4, func_8001B63C, Cd_read_sync2, func_8001D414,
    Cd_read_comb, func_8001D468 (CD_CMD queue writers), func_8001C7F0,
    func_8001D7AC
  - sub_scrn: Sub_screen_sort_sub, func_800605DC
  - sound: func_80019F94, func_8001B2F0, func_8001997C, func_8001B314,
    func_80019A70, func_80019AE0
  - game: func_800155A4, func_80015734, func_80015840, func_80016528,
    func_80016BC0, func_80016BF4, func_80016D0C, func_80016D38,
    func_80016D64, func_80016DAC, func_80016E90
  - main: func_800131FC, func_8001326C, func_80012FA4, func_80012FC8,
    vsync_cb, func_80012F78, func_80012E98, func_80012298
  - player: 10 empty funcs func_8003FFA8, func_80040130..func_800402BC,
    func_8003BE40, func_80040140, func_800406A8, func_800406DC,
    func_80040B34 (key-vs-mask tests, PL_WORK typed), func_80042208
  - debug: func_800629E0

## Last verified build
- 2026-07-04/05 (overnight) — `make CPP=cpp check_rock_neo_only` OK after the
  6-function batch; mutation test performed (broke func_80056148 script
  advance and func_80057DB8 mask, check failed both times; restored, OK)
