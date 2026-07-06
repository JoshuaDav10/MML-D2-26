# MML-D2-26 Progress

## Mapped (functions in splat config / INCLUDE_ASM stubs, main exe)
- rock_neo main: **484** functions in linked object code (`tools/census.py
  --matched`; see `notes/COUNTS.md`). **475** have splat asm under
  `asm/rock_neo/nonmatchings/`; **260** active INCLUDE_ASM stubs (cpp census);
  **9** extra symbols in `game.c` only (no nonmatching `.s`).

## Matched (recompiles to identical bytes)
- rock_neo main: 224 (verified: full-binary sha1 OK after a CLEAN rebuild —
  see the func_800605DC note under "Last verified build")
- Volume: ~4388 of ~31,300 mapped instructions (~14.0%)
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
    func_800555F4, func_80057BB4, func_80058740 (x44 jump-offset table),
    func_80058EA0 (58788 twin + D_80098851=0xFF), MojiTaskKill (flags
    sweep+accumulate over Moji_work[0..4], cond func_8001D494 kick),
    func_80054804 (Moji_work[s[1]].flags gate), func_800570B0 (55344
    sibling gated on Moji_flag[0]==s[1]; c1/c2-before-key/flag init order),
    func_80057D00 (stack2 push + double-table jump), func_80057DF4
    (script2 patch from Game_work.x84_tbl[D_800989D4]),
    func_800594CC (stack2 push + D_8008CCA4[D_800BE2F7[...]] jump),
    func_80054798 (x8/xA operand pair + x7E/x7F bytes), func_80055344
    (MojiTaskExec re-dispatch, x44-or-D_8008CACC base), func_80055C1C
    (flag 0x10000 clear + func_80039E18 call; callee typed s32),
    func_80057A24 (stack2 push + Sce_flag_on(D_80098830|0x500)),
    func_80058788 (slot-index magic-div, Moji_flag3 bit clear),
    func_80054ADC + func_80055BB0 (D_8008AAC0 fn-table dispatchers)
  - Code800133D8: func_80013418, func_80013890, func_80013F60, func_80013F8C,
    func_800133D8
  - scene: func_8001D928 (flag-array clear + Scene_work reset), func_8001D878, func_8001DEDC, func_8001F820, func_8001D974,
    func_8001DDC0, func_8001DD88, func_8001D854, func_8001DEE4, func_8001E7E4, func_8001E810,
    func_8001F1DC, func_8001F20C (SCENE_WORK typed), func_8001FB24,
    Sce_flag_test (the flag-bit reader; unused 8-byte frame local),
    func_8001D888 (Cd_read_sync2 drain loop), func_8001FB54, func_8001FB8C
    func_8001FD3C, func_8001FD90 (Game_work.x52 -> gp-half pairs;
    if/else + ternary-chain split), func_8001DE84 (goto-shared return-0
    label defeats the setcc/sltu tail), func_8001F740 (stage_no dispatch),
    func_8001E390 (Sce_flag on/off sweep 0x2E0..0x2EB, u32 counter),
    func_8001E460 (flag pair set/clear at n+0x7E0/n+0x7C0),
    func_8001D8C0 (Sce_flag clear-or-restore via func_800176DC),
    func_8001F580 (Cd_read_comb kick; distinct u16 keep copy of u16 no),
    func_8001E3F0 (Game_work.x60 += D_800891B4[n], clamp 0..0xFF),
    func_8001EB98 (save/restore Scene_work.x24[*p] around func_8001EAE8;
    dead 8-byte frame local + t-reuse/in-place-shift for the v1 index),
    func_8001DFEC (slot register: D_800ACD40[k]=Scene_work.x24[k] +
    D_800988E8 bit; integer-typed e addend keeps the scaled index in rs)
    func_8001FCA4 (switch on Game_work.x52 keeps the ==5 beq un-inverted;
    closed the jump-canonicalization open problem — Cursor + Fable)
    (func_8001FC50, func_8001FDE4 attempted, NOT matched — same
    jump-canonicalization family, retry with switch; see LESSONS 2026-07-06)
  - cd: func_8001B4C4, func_8001B63C, Cd_read_sync2, func_8001D414,
    Cd_read_comb, func_8001D468 (CD_CMD queue writers), func_8001C7F0,
    func_8001D7AC, func_8001B858 (fn-table dispatch via D_80098A84->x8),
    func_8001CB30 (CdReady/CdSync callback setup + func_8001D254 kick),
    func_8001D7E4 (queue-drain wait loop via func_80012E98(1)),
    func_8001D254 + func_8001D2BC (CdSync-then-CdControl/CdControlB retry
    twins; u8 com param masked at use)
  - sub_scrn: Sub_screen_sort_sub, func_800605DC, Map_screen_init,
    func_8005EC34 (back-ground set + routine_0 table dispatch),
    func_8005EC80, func_80060DB8, Sub_screen_cancel_check,
    Sub_screen_shift_check, Sub_screen_sort_attack/energy/range/rapid,
    Sub_screen_rb_parts_set (pre-existing ACCEPT_REORDERING_BULLSHIT
    drafts un-gated; MojiTaskExec K&R decl fixes the -1 arg),
    func_800600CC (routine_1 switch; raw-address Moji_flag derefs +
    maspsx bare-constant-load nop fix), func_80060248 (byte-shuffle
    x1x/x2x/x3x slots via compute-into-locals)
  - sound: func_80019F94, func_8001B2F0, func_8001997C, func_8001B314,
    func_80019A70, func_80019AE0, func_80019AA4, func_800199F8,
    func_80019A34, func_8001A1FC, func_8001A238 (stride-8 search loops),
    func_80019FB4 (Game_work[0x50] fn-table dispatch + D_80098958 |= 0x800),
    Sound_call + Sound_call2 (SND_CMD queue writers, 0x14 stride; call2
    needs in-place q++ to pin the store order), func_800198C0
    (Sce-flag ternary into $a0 + descending D_8008222A fill loop),
    func_8001A0A8 (SsIsEos poll over the 8-stride D_80098AF8 seq table)
    (func_800199A4 + func_80019918 attempted, NOT matched — FC50
    canonicalization family; func_8001A6DC attempted, NOT matched —
    register-birth: a shares n's $s0 instead of taking $s2)
  - game: func_800155A4, func_80015734, func_80015840, func_80016528,
    func_80016BC0, func_80016BF4, func_80016D0C, func_80016D38,
    func_80016D64, func_80016DAC, func_80016E90
  - main: func_800122D0 (func_80012350 x6 sequence), func_800131FC, func_8001326C, func_80012FA4, func_80012FC8,
    vsync_cb, func_80012F78, func_80012E98, func_80012298, func_80012424,
    func_8001215C (OT/flag init), func_80012938 (OpenEvent setup),
    func_80012ECC, func_80012F24 (thread close pair; D_801F81xx quirk),
    func_80012E10 (OpenTh slot setup), func_80012FEC (PCopen/PCread
    loader; neighbor-symbol constant-index defeats address CSE),
    func_8001319C (0x801F8300 table clear loops; left-assoc pointer
    arithmetic pins the addu operand order),
    func_8001D324 (u8 state change latch -> func_8001D2BC(0xE,...)),
    func_8001CC08 (CdSync callback: r==2 latches D_80098964, else
    D_8009896C|=8; K&R def to match the unprototyped decl)
  - player: 10 empty funcs func_8003FFA8, func_80040130..func_800402BC,
    func_8003BE40, func_80040140, func_800406A8, func_800406DC,
    func_80040B34 (key-vs-mask tests, PL_WORK typed), func_80042208,
    func_800405F4 (x11C vs x128|x12A), func_8003F498/F4E8/F538/F588
    (func_80041DDC(pl,0x33..0x36,0,1) sibling quad, xA byte guard),
    func_800402C4 (x124/x134 vs x11C key test, PL_WORK fields typed),
    func_8003EE68 (state 7 setup, x74/x75 typed, shot enable + 41DDC),
    func_80040380 (x112/x113 swap on key match), func_80040710 (x128 vs
    x12A select, xA=0/0x100), func_80040AEC (Game_work x83==1 or x140 key),
    func_80041E90 (x56 -=/+= x116 around func_8002FEA4, s16 params),
    func_8003F224 (x83-gated key test; goto-shared return-1 defeats the
    sltiu tail; field-first & order), func_80040224 (state reset unless
    x83==1 or x140 key; x108 typed), func_800400B8 (40818-gated retry of
    40710 else force state 2 via func_80040B68),
    func_8003FDA8 (xA<3 + x11C&x134 gate; preloaded x11C local hoists the
    lhu), func_80041A44 (state 0xB/0xC ladder; v assigned after the
    compares so it stays dead across them and lands in $v0)
    (func_80041EF4 attempted, NOT matched — cc1 elides the original's two
    andi 0xFFFF truncations; every source shape proves nonzero_bits ≤0xFFFF.
    See activity 2026-07-05 day session)
  - game: func_800164B4 (x1 switch: case 0 advances by func_800665FC(1),
    case 1 sets routine 3/x1=2; SWITCH keeps the ==0 leg un-inverted where
    if/else emits bnez) — plus the 7 upstream game.c functions counted earlier
  - debug: func_800629E0, func_800629F0 (Debug_work joy latch + fn-table
    dispatch; stores through the NEIGHBOR symbol &Scene_work[-k] pin the
    table load AND defeat arg anchor-CSE)

## Last verified build
- 2026-07-06 (Fable, FCA4 audit) — clean rebuild 0 errors, hash OK,
  cmp byte-identical after auditing Cursor's scene func_8001FCA4 switch
  match (branch match/fca4-switch, ff-merged to dev); mutation test
  (0x86→0x88) failed while mutated; restored, 0 errors, OK.
- 2026-07-05 (Fable day session 2, batch 16) — clean rebuild 0 errors,
  hash OK, cmp byte-identical after player func_800400B8 + game
  func_800164B4; mutation test failed while mutated; restored OK.
- 2026-07-05 (Fable day session 2, batch 15) — clean rebuild 0 errors,
  hash OK, cmp byte-identical after moji MojiTaskKill/func_80054804/
  func_800570B0, cd func_8001CC08, scene func_8001DFEC, main
  func_800122D0; 2/2 mutation tests failed while mutated; restored OK.
  NOTE: first rebuild of this batch was a FALSE PASS (moji.c 'conflicting
  types for MojiTaskKill' — moji.h says s32; error-grep caught it).
- 2026-07-05 (Fable day session 2, batch 14) — clean rebuild 0 errors,
  hash OK, cmp byte-identical after moji func_80058EA0, scene
  func_8001E3F0/func_8001EB98, cd func_8001D324, player
  func_8003FDA8/func_80041A44; 2/2 mutation tests failed while mutated;
  restored OK + byte-identical.
- 2026-07-05 (Fable day session, batch 13) — clean rebuild 0 errors, hash
  OK, cmp byte-identical after player func_8003F224/func_80040224, scene
  func_8001D8C0, moji func_80054ADC/func_80055BB0; 3/3 mutation tests
  failed while mutated; restored OK.
- 2026-07-05 (Fable day session, batch 12) — clean rebuild 0 compile
  errors, hash OK, `cmp` byte-identical after cd func_8001D254/D2BC, moji
  func_80057A24/58788, sound func_8001A0A8, scene func_8001E460; 4/4
  mutation tests failed while mutated. NOTE: this batch caught a FALSE
  PASS (cd.c decl conflict + partial .o + stale exe) — see LESSONS.
- 2026-07-05 (Fable day session, batch 11) — clean rebuild hash OK after
  4 moji + player func_80041E90 + scene func_8001E390/F580; 3/3 mutation
  tests failed while mutated; restored OK.
- 2026-07-05 (Fable day session, batch 10) — clean rebuild (error-grepped,
  0 rock_neo compile errors) hash OK after scene func_8001DE84/func_8001F740,
  main func_8001319C, debug func_800629F0; three mutation tests (one per
  touched TU) all FAILED the check as required; restored, final clean
  rebuild OK.
- 2026-07-05 (Fable overnight, batch 9) — clean rebuild (error-grepped) hash
  OK after moji func_80057D00/func_80057DF4, player func_8003EE68, sound
  func_800198C0, sub_scrn func_80060248; all five mutation tests FAILED the
  check as required; restored, final clean rebuild OK (0 compile errors).
- 2026-07-05 (Fable overnight, batch 8) — clean rebuild hash OK after
  func_800600CC + a tools/maspx patch (bare-CONSTANT-address loads now get
  the same load-delay-nop rules as bare-symbol loads; the clean rebuild
  re-validates all 181 prior matches against the tool change). Mutation
  test (Cd_read_comb 205→206) FAILED the check as required; restored, OK.
- 2026-07-05 (Fable overnight, batch 7) — clean rebuild hash OK after
  un-gating 9 sub_scrn ACCEPT_REORDERING_BULLSHIT drafts (only byte delta
  was MojiTaskExec's -1 arg truncated by the ANSI u8 prototype; fixed with
  a K&R unprototyped declaration in moji.h, body signature untouched).
  Mutation tests on 4 of the 9 (EC80 -1→-2, sort_attack 0x1f→0x1e,
  shift_check Sound_call arg, rb_parts_set 3→4) all FAILED the check as
  required; restored, final clean rebuild OK.
- 2026-07-05 (Fable overnight, batch 6) — clean rebuild hash OK after scene
  func_8001FD3C/func_8001FD90, sound Sound_call2, sub_scrn func_8005EC34
  (old ACCEPT_REORDERING_BULLSHIT draft un-gated — matches now that
  patchasm's reorder pass exists); all four mutation tests FAILED the check
  as required; restored, final clean rebuild OK.
- 2026-07-05 (Fable overnight, batch 5) — clean rebuild hash OK after
  player func_80040380/func_80040710/func_80040AEC, sound Sound_call,
  cd func_8001D7E4; all five per-function mutation tests FAILED the check
  as required; restored, final clean rebuild OK. func_80042044 attempted,
  NOT matched (register-allocation mismatch: original puts k in $v0/ret in
  $t0 with b in $a3 and no m copy; every C shape tried yields an m copy or
  k stealing $a0 — pass-through args (pl, arg1, arg2)->func_80042154 got
  ret->$t0 but not the rest; see activity).
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
