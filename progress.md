# MML-D2-26 Progress

## Mapped (functions in splat config / INCLUDE_ASM stubs, main exe)
- rock_neo main: 475 functions in asm/rock_neo/nonmatchings (324 still active
  INCLUDE_ASM stubs; count verified by preprocessing src and counting
  `.include nonmatchings` lines)

## Matched (recompiles to identical bytes)
- rock_neo main: 163 (verified: full-binary sha1 OK after a CLEAN rebuild —
  see the func_800605DC note under "Last verified build")
- Volume: ~2550 of ~31,300 mapped instructions (~8.1%)
  - moji: func_800542FC + accessor family func_80054310..func_800543F8, func_80054BAC,
    func_80055304, func_80054694, func_80056180, func_8005A858,
    func_80054410, func_8005457C, func_80054B88, func_8005563C, func_80056128,
    func_80057124, func_80058C08, func_80054424, func_8005459C,
    func_80054AB4 + func_80054A84 (script/script2 stack pops),
    func_8005531C, func_80055660, MojiTaskExec2, MojiTaskExec (task-slot
    initializer, 127 insns — biggest match yet), func_80057BFC,
    func_80054BB4, func_80055CC4, func_80056148, func_80054B4C,
    func_80057708, func_80057A94 (script call-stack push + table jump),
    func_80057DB8, func_80058DB4, func_80057144, func_8005753C,
    func_80055C80, func_800576C4 (script2 call-stack push), func_80057B70,
    func_800553A8, func_800553F0 (Sce_flag on/off from script operand),
    func_800546B0, func_80054874 (Sound_call from operand), func_8005619C +
    func_800561E8 + func_80056234 (sibling cond-advance triple),
    func_800579D8, func_80057B24 + func_80059E24 (stack pushes w/ table jump),
    func_80058D64 (x10/x12 operand pair), func_80057AD0 (stack2 push),
    func_80057D60 + func_80059530 (stack pushes w/ double-table jumps),
    func_800560D0 (x78 compare-select), func_8005BC90 (x7C state seq),
    func_800555F4, func_80057BB4, func_80058740 (x44 jump-offset table)
  - Code800133D8: func_80013418, func_80013890, func_80013F60, func_80013F8C,
    func_800133D8
  - scene: func_8001D928 (flag-array clear + Scene_work reset), func_8001D878, func_8001DEDC, func_8001F820, func_8001D974,
    func_8001DDC0, func_8001DD88, func_8001D854, func_8001DEE4, func_8001E7E4, func_8001E810,
    func_8001F1DC, func_8001F20C (SCENE_WORK typed), func_8001FB24,
    Sce_flag_test (the flag-bit reader; unused 8-byte frame local),
    func_8001D888 (Cd_read_sync2 drain loop), func_8001FB54, func_8001FB8C
    (func_8001FCA4 attempted, NOT matched — see activity 2026-07-05)
  - cd: func_8001B4C4, func_8001B63C, Cd_read_sync2, func_8001D414,
    Cd_read_comb, func_8001D468 (CD_CMD queue writers), func_8001C7F0,
    func_8001D7AC, func_8001B858 (fn-table dispatch via D_80098A84->x8),
    func_8001CB30 (CdReady/CdSync callback setup + func_8001D254 kick)
  - sub_scrn: Sub_screen_sort_sub, func_800605DC, Map_screen_init
  - sound: func_80019F94, func_8001B2F0, func_8001997C, func_8001B314,
    func_80019A70, func_80019AE0, func_80019AA4, func_800199F8,
    func_80019A34, func_8001A1FC, func_8001A238 (stride-8 search loops),
    func_80019FB4 (Game_work[0x50] fn-table dispatch + D_80098958 |= 0x800)
  - game: func_800155A4, func_80015734, func_80015840, func_80016528,
    func_80016BC0, func_80016BF4, func_80016D0C, func_80016D38,
    func_80016D64, func_80016DAC, func_80016E90
  - main: func_800131FC, func_8001326C, func_80012FA4, func_80012FC8,
    vsync_cb, func_80012F78, func_80012E98, func_80012298, func_80012424,
    func_8001215C (OT/flag init), func_80012938 (OpenEvent setup),
    func_80012ECC, func_80012F24 (thread close pair; D_801F81xx quirk),
    func_80012E10 (OpenTh slot setup), func_80012FEC (PCopen/PCread
    loader; neighbor-symbol constant-index defeats address CSE)
  - player: 10 empty funcs func_8003FFA8, func_80040130..func_800402BC,
    func_8003BE40, func_80040140, func_800406A8, func_800406DC,
    func_80040B34 (key-vs-mask tests, PL_WORK typed), func_80042208,
    func_800405F4 (x11C vs x128|x12A), func_8003F498/F4E8/F538/F588
    (func_80041DDC(pl,0x33..0x36,0,1) sibling quad, xA byte guard),
    func_800402C4 (x124/x134 vs x11C key test, PL_WORK fields typed)
  - debug: func_800629E0

## Last verified build
- 2026-07-05 (Fable overnight, batch 4) — clean rebuild hash OK after
  func_80012E10/func_80012FEC/func_80019FB4; per-function mutation tests
  (E10 *p=2→3, FEC 0x7B4→0x7B8, FB4 0x800→0x400) each FAILED the check as
  required; restored, final clean rebuild OK.
- 2026-07-05 (Fable, MojiTaskExec session) — MojiTaskExec (133-line asm)
  matched: clean rebuild (`touch src/rock_neo/*.c && rm -f build/rock_neo.elf`)
  → hash OK → `cmp` byte-identical; mutation test (x3D=3 → 4) FAILED the
  check as required, restored, OK. Struct changes to moji.h (x4..xE u16
  fields, x38/x3A, x3E s8→u8, x48 ptr, x73/x7D/x7E/x7F, x78 s8→u8) validated
  against all previously matched moji functions by the same clean rebuild.
- 2026-07-05 (Fable audit + harvest) — from-scratch rebuild (`rm -rf build`)
  byte-identical via `cmp`; then two 6-function batches, each clean-rebuilt,
  hash OK, mutation-tested (broke func_80058740/func_80012424/func_800133D8/
  func_800199F8 across the session — each failed the check; restored, OK).
- 2026-07-05 (Opus session) — `make CPP=cpp check_rock_neo_only` OK after a
  CLEAN rebuild (`touch src/rock_neo/*.c && rm -f build/rock_neo.elf`), which
  is what makes this count trustworthy. Mutation tests: func_800605DC and
  func_80057B70 (broken arg failed the check; restored, OK).
- **func_800605DC was a STALE-OBJECT FAKE MATCH from the prior session.**
  `.c.o` files are make intermediates, so when it was converted from
  INCLUDE_ASM to C last session the object was NOT rebuilt — the stale stub
  object stayed linked, the hash passed on the STUB, and the C (which had a
  `void` vs header `unknown_t` type conflict) never actually compiled. A
  forced recompile this session exposed the compile error. Fixed by aligning
  the sub_scrn.h prototype to `void func_800605DC(void)`; now genuinely
  compiled + hash-verified. Lesson: only trust a match after a clean rebuild;
  `touch src/rock_neo/*.c && rm -f build/rock_neo.elf` before every hash claim.
