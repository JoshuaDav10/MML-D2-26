# MML-D2-26 Progress

<!-- BEGIN GENERATED COUNTS -->
| metric | value |
|---|---|
| matched functions | **407** |
| C-mapped slice | 407 / **566** = 71.9% |
| main executable | 407 / **1119** = **36.4%** |
| **stage overlays** | 51 / **~7064** |
| **whole game** | 458 / **~8629** = **~5.3%** |
| active INCLUDE_ASM stubs | **159** |
| still unsplit raw asm | **553** functions in `asm/rock_neo/*.s` |
<!-- END GENERATED COUNTS -->


## Mapped (functions in splat config / INCLUDE_ASM stubs, main exe)
- rock_neo main: **488** functions in linked object code (`tools/census.py
  --matched`; see `notes/COUNTS.md`). **478** have splat asm under
  `asm/rock_neo/nonmatchings/`; **206** active INCLUDE_ASM stubs (cpp census, 2026-07-26);
  **9** extra symbols in `game.c` only (no nonmatching `.s`).

## Matched (recompiles to identical bytes)
- rock_neo main: **282** (AUTHORITATIVE — tools/audit_count.sh: clean rm -rf build,
  hash OK, raw cmp byte-identical, census 282. 488 total, 206 active stubs.
  Denominators (name which — see notes/WORK_MAP.md): 282/488 = 57.8% C-slice ·
  282/1119 = 25.2% main exe · 282/~8,000 ≈ 3.5% WHOLE GAME.)
  - +1 (func_8002F9C4, 2026-07-26 autonomous chain): FIRST match harvested from the
    newly-unlocked raw-asm queue via the Phase 0 split template. 10-insn wrapper
    tail-calling func_8002FA38(a0, 0x8016C000, a1); called from 37 stage overlays
    (high leverage). matched + hash + mutation-tested + census delta 281->282.
  - +1 (func_80040764, player): matched + hash + mutation-tested. Key-mask state
    chooser (two OR'd mask groups; equal-emptiness -> rand()&1, else pick by group 2).
    FIVE knobs, all from LESSONS: K&R decl (caller func_80040140 passes leftover $a0
    with no arg setup — and retyping this callee void->s32 did NOT disturb that
    matched caller); evaluate the x12A|x12E group BEFORE x128|x12C; keep the raw
    masked value AND its ==0 boolean both live (target tests `bnez` on the value but
    `sltiu` on the boolean); `if (a != b)` so the rand block is the branch TARGET;
    and `m & pl->x138` (not `pl->x138 & m`) to fix the final $v0/$v1 load mirror —
    AND operand order follows source order.
  - +1 (func_80040178, player): matched + hash + mutation-tested. Guarded state
    transition (body is func_8003EE68's + x44/x46/x48 stores). THREE knobs needed:
    (1) x16/x44/x46/x48/x78 fall in PL_WORK pad arrays -> byte-offset casts;
    (2) the s16 read must land in an s32 local or cc1 emits `lhu` not `lh`;
    (3) `(ret - 0x20) - w` must be forced with a temp — written inline, cc1
    reassociates to `ret - (w + 0x20)` (addiu +0x20 instead of -0x20).
  - +1 (func_8001FBC4, scene): matched FIRST BUILD + hash + mutation-tested. Sets
    D_8009823A/3E/40 from Game_work.x52==5 + Sce_flag_test(0x1C/0x1B); the two
    early arms `return` directly and the last two share a tail (the goto-shared-tail
    shape falls out of if/else + a `v` local). All three are gp-accessed => PLAIN
    SCALAR externs (LESSONS §2).
  - +1 (func_8001B7B4, cd): matched FIRST BUILD + hash + mutation-tested. CD mute
    toggle: pulse func_8001D394(0x7F..1), `D_800AD142[0] |= 0x8000` (array decl for the
    single materialized read+write address), then swap D_800AD146 <-> D_80098994 gated
    on D_800988DC, then func_8001CAAC(). Needed forward decls (the stub sits above
    cd.c's own definitions).
  - +1 (func_80057924, moji): matched + hash + mutation-tested. 3rd permuter win
    (mml_57924). Script-opcode handler: `script2[1]==0` ? scale (x7D-x7C)*10 : use
    zennyCount, negate into func_80043294(v,0,0), store x7D->x7C, script2+=2, return 1.
    Hand analysis reached a pure $v0/$v1 mirror (10 rows); permuter's lever = use a
    SEPARATE local for the delta instead of reusing the result variable.
  - +1 (func_80062C6C, debug): matched + hash + mutation-tested. 2nd permuter
    win (mml_62C6C). Register-mirror: fn-ptr vs counter in swapped $s1/$s2;
    lever = init `i=0` BEFORE `fp=D_8008DBE0` (reorders pseudo alloc). fn-table
    dispatch loop, `(*fp++)(arg0)` x5, return 1.
  - +1 (func_8001E4C4, scene): matched + hash-verified + mutation-tested. FIRST
    PERMUTER WIN of the near-miss lane. Hand analysis got it to a 1-diff
    register-mirror (i↔arg1 in $s1/$s2 — allocno tie, unforceable by hand); set up
    `tools/decomp-permuter/mml_E4C4` (recipe in HARVEST_NEARMATCHES.md), permuter
    hit score 0 at iter ~19k. Winning lever: `long c;` + a redundant
    `if(i){c=*arg0;}else{c=*arg0;}` that flips the allocno tie. Scratch score-0 was
    then LANDED + verified by full audit (scratch-match != full hash — always gate
    on audit_count.sh).
  - +1 (func_8001A63C, sound): matched + hash-verified + mutation-tested (census
    273→274, hash OK). Magic-div-by-12 volume table lookup: `q=|arg0|/12` shifts
    0x1000 (`<<` pos / `>>` neg), `D_80082C70/CA0[|arg0|%12]` table, return
    `(a1*v0)>>16`. Knobs: positive branch is the fall-through (`bltz` to neg);
    compute the table lookup BEFORE the shift (schedules the sllv/srav last);
    reassign `a1 = a1*v0` so cc1 reuses $a1 for the mflo result (not $a2).
    NOTE: census counts a function matched from its .o even when a register nit
    fails the FULL hash — always gate on `audit_count.sh` (hash), not census alone.
  - ⚠️ COUNT CORRECTION (2026-07-14): an earlier note in this session claimed
    277→280 by "un-gating" func_800155A4 (game) + func_8005EC80 (sub_scrn) and
    3 more game fns. THOSE WERE PHANTOM: game.c AND sub_scrn.c both carry
    `#define ACCEPT_REORDERING_BULLSHIT` at the top (since commit 957191c), so
    every `#ifndef ACCEPT_REORDERING_BULLSHIT` guard ALREADY takes the body
    branch — those bodies were compiled + matching all along. Un-gating them is a
    no-op. The naive `grep INCLUDE_ASM` stub census counts them as stubs (it can't
    see the define) → overcount. ALWAYS trust `census.py --matched` (reads .o),
    never the grep. Genuine NEW matches this session = only the 2 below.
  - +1 (func_8001D394, cd): matched + mutation-tested. Held-pointer idiom:
    `u8 *p = D_800AD140; ... CdMix(p + 0x14);` forces cc1 to KEEP the base in a
    reg (reused `addu a0,v1,0x14`) instead of folding `SYM+0x14` to an absolute
    `&D_800AD154` address. Same genus as the BB4C addressing-crux — see LESSONS.
  - +1 (func_80016434, game): matched + mutation-tested (0x18→0x19 broke hash,
    restore→OK). Straight-line global initializer under `if (arg0 == 2)`.
    Knobs: D_800C356F pre-declared in rock_neo.h as `u8[]` (use `[0]`); the
    other D_800C35xx are fresh per-width scalar externs; Game_work+0x79/0x7A =
    `Game_work.x78[1]`/`[2]`; the trailing `=1` store lets cc1 hoist its `li`.
  - +1 (func_8001F6C4, scene): matched + mutation-tested; reused-variable call
    arg + delay-slot load hoist (see LESSONS 2026-07-12 Opus scene entry).
  - +4 this session (f2e025e): Sub_screen_sort_{attack,energy,range,rapid} —
    parked drafts activated by stripping ACCEPT_REORDERING_BULLSHIT guards.
- Volume: ~15% of ~31,300 mapped instructions
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
    func_80053AA4 (kill-one-slot: xC2=0xFF + flag mask clear; the mask AND
    needs a temporary to force (f & lit) & ~shift, not f & (lit & ~shift)),
    func_80053788 (moji reset-all: Cd_read_comb + loop clearing Moji_work[0..4]
    flags/script2/x6/x48; script2 store MUST use a `m=&Moji_work[no]` pointer
    while the other 3 stay array-absolute — mixed addressing matched exactly),
    func_80057C2C (bit-scan of D_800BE3DB builds D_800BE2F8 list, then CALL-op
    dispatch; explicit `*p++` walking ptr since count++ is conditional, u32 i
    for sltiu, held `*q`=&D_800BE3DB via aliasing, if/else+single-return; the
    `0x80` mask had to be a named local placed BETWEEN i and p in the for-init
    to fix preheader instruction order),
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
    func_80054ADC + func_80055BB0 (D_8008AAC0 fn-table dispatchers),
    func_8005497C + func_800564C8 + func_80056558 + func_80056610 +
    func_800566CC (script CALL-opcode family; op index remapped via
    D_800BE2F8 or Player_work+0x450/0x454 tables, -1 bias folded into
    D_8008CAC8), func_80055438 + func_80055A78 (jump-byte select +
    ((u16*)x44)[b] table; Sce_flag / Game_work.stage_no guards),
    func_80057184 (D_8008D0D4 guard before MojiTaskExec), func_80055B14
    (Game_logo_kill / func_80063FC0 two-arm), func_8005721C (D_80098B6C
    predicate via Sce_flag_test(0x2EE)+func_8004327C),
    func_800545C8 + func_80054700 + func_800557B8 (wait-counter trio:
    0x1000000-init + x4 countdown; 545C8 full-reset arm, 54700 slim
    script2-only expiry, 557B8 0x40000-gated — s16 local + `||` shape +
    direct returns, see LESSONS 2026-07-11)
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
    func_8001FC50 + func_8001FDE4 (switch family completed: case tree via
    explicit case 0; fallthrough-into-default for the shared 0x81 leg —
    the whole scene jump-canonicalization family is now closed),
    func_8001FCE4 (FIRST JUMP-TABLE FUNCTION — cc1-emitted rodata table),
    func_8001F8DC + func_8001F9AC (MULTI-TABLE proof: all 3 scene jump
    tables now emitted by scene.c.o(.rodata), contiguous at 0x800108C0;
    F9AC's case-0 || ternary keeps the value pseudo in $v0 — see LESSONS)
  - cd: func_8001B4C4, func_8001B63C, Cd_read_sync2, func_8001D414,
    Cd_read_comb, func_8001D468 (CD_CMD queue writers), func_8001C7F0,
    func_8001D7AC, func_8001B858 (fn-table dispatch via D_80098A84->x8),
    func_8001CB30 (CdReady/CdSync callback setup + func_8001D254 kick),
    func_8001D7E4 (queue-drain wait loop via func_80012E98(1)),
    func_8001D254 + func_8001D2BC (CdSync-then-CdControl/CdControlB retry
    twins; u8 com param masked at use),
    func_8001CAAC (CD queue dequeue/shift-down; field-by-field copy keeps
    the two-IV cursor), func_8001CB7C (CD retry re-arm; D_800AD142[] array
    decl for single-materialized-address RMW),
    func_8001CF98 (CD arm: byte-clear loop + D_80082CD0[arg] table; local `v`
    ASSIGNED IN BODY to force single-load-reused-for-store-and-arg while
    keeping arg*12 index CSE'd across the CdIntToPos call) — three BB4C callees
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
    func_80019918 + func_800199A4 (ex-PARKED jump-canon family, cracked via
    goto-chain w/ $v0 reuse + switch range-collapse; harvest-agent drafts,
    foreground hash-gated 2026-07-11),
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
    func_8004030C + func_800404FC + func_80040574 + func_80040630 (key-test
    siblings: Scene_work-gated k-select, x44A hold counter, x130/x132/x16B
    fields; harvest-agent drafts, foreground hash-gated 2026-07-11),
    func_800403D4 + func_80040468 (dual-mask k-select pair x12C|x12E),
    func_80040FA8 + func_80041044 + func_800410D4 + func_80041158 (weapon
    stat-table family: D_80089A84 stride-12, goto-shared-return-1;
    harvest-agent batch 2, hash-gated 2026-07-11),
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
- 2026-07-06 (Fable, cd CB7C) — clean rebuild 0 errors, hash OK, cmp
  byte-identical after cd func_8001CB7C; mutation test failed while
  mutated; restored, OK.
- 2026-07-06 (Fable, cd sibling) — clean rebuild 0 errors, hash OK, cmp
  byte-identical after cd func_8001CAAC; mutation test failed while
  mutated; restored, OK.
- 2026-07-06 (Fable, multi-table proof) — clean rebuild 0 errors, hash OK,
  cmp byte-identical after scene func_8001F8DC + func_8001F9AC; all three
  scene jump tables now from scene.c.o(.rodata) at 0x800108C0 (size 0x80,
  function order). Mutation tests (F8DC case value; F9AC case→body table
  remap) both failed while mutated; restored, OK.
- 2026-07-06 (Fable, jump-table infra) — clean rebuild 0 errors, hash OK,
  cmp byte-identical after scene func_8001FCE4 with its cc1-emitted jump
  table (rodata carve 0x1120/0x1140 + ld placement); 2/2 mutation tests
  (case value; table-only remap) failed while mutated; restored, OK.
- 2026-07-06 (Fable, batch 17) — clean rebuild 0 errors, hash OK, cmp
  byte-identical after scene func_8001FC50 + func_8001FDE4 (switch trick
  closes the scene jump-canonicalization family); 2/2 mutation tests
  failed while mutated; restored, 0 errors, OK.
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
