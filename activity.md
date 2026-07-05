# Activity Log

## 2026-07-04
- Fresh clone from ChrisNonyminus/mml1
- Remotes: upstream + origin (MML-D2-26)
- Added notes/, progress.md, activity.md, plan.md
- Build not yet attempted

## 2026-07-04 (later — diff loop + first match)
- Validated ./diff.py inner loop (deps + Python 3.14 ast fix + root symlink)
- Added function-reorder pass to tools/patchasm.py (fixes cc1-27 deferring C
  bodies past INCLUDE_ASM blocks — real C can now sit mid-file)
- First matched function: func_800542FC (moji.c), sha1 holds
- Branch: match/func_800542FC
- Matched moji accessor family (12 funcs, func_80054310..func_800543F8), sha1 holds
- Matched 9 more small funcs across Code800133D8/scene/cd/debug/moji, sha1 holds
- Matched 8 more (Cd_read_sync2, Sub_screen_sort_sub, func_8001D974,
  func_80019F94, 4 moji script ops); introduced MOJI_TASK struct and
  refactored the 13 earlier moji matches onto it; sha1 holds (32 total)
- Enabled upstream's 4 disabled game.c functions (reorder fix made them
  viable) + 10 empty player.c funcs: 46 total matched, sha1 holds

## 2026-07-04 (evening — sdata/gp pipeline + WIP batch salvage)
- Found ~10 uncommitted, unverified function attempts left in the tree; build
  did NOT match. Salvaged per-function from a stash.
- main.c func_80012FA4/FC8: pointer-into-local form fixed a recomputed store
  address (lui/ori const materialization + single address reg).
- ROOT CAUSE DISCOVERY: the C pipeline could never emit $gp-relative (sdata)
  access — GAS runs -G0, so cc1's bare small-extern refs always expanded to
  lui/$at, and cc1's `.extern sym,size` created bogus COMMON symbols that
  shifted the data segment. Original binary DOES use $gp (e.g. Moji_flag,
  D_80098198).
- Added tools/gprel.py (pipeline pass between maspsx and patchasm): census of
  %gp_rel symbols in the extracted asm decides which small externs get
  rewritten to explicit %gp_rel(sym)($gp); also re-materializes load-delay
  nops maspsx deleted assuming $at expansion. Declaration style stays the
  per-TU control (scalar extern = gp candidate, unsized array = lui).
- D_80098910: original mixes addressing per TU (gp in unsplit asm, lui in
  game.c) — flipped game.c's shared decl to unsized array to keep lui there.
- func_80057124: zennyCount load must precede the script2 store (may-alias
  blocks hoisting) — read the global into a temp first.
- func_8001DDC0 (scene) matched: first real gp-relative C function.
- Batch verified: 11 new matches (2 main, 7 moji, 1 scene, 1 sound).
- Recount: game.c had 7 more upstream-written functions compiled+matching all
  along but uncounted (func_80016BC0..func_80016E90 range); true total is
  64 matched, 420 active stubs. sha1 OK; mutation test done (broke
  func_8005457C → check failed → restored → OK).

## 2026-07-04 (night — 18-function batch, 82 total)
- Matched 18 more small functions (9-12 instructions each, ~189 instructions):
  main vsync_cb + func_80012F78 (first NEW gp-relative C, two sdata stores);
  sound func_8001997C + func_8001B314 (SpuSetKey); cd func_8001D414 +
  Cd_read_comb + func_8001D468 (CD_CMD command-queue writers, new struct);
  Code800133D8 func_80013F60/F8C (GAME_WORK duplicates of game.c pair);
  scene func_8001D854, func_8001DEE4, func_8001E7E4, func_8001E810;
  moji func_80054424 (u32 reader), func_8005459C, func_80054AB4
  (script call-stack pop — MOJI_TASK.stack[8] at 0x18 + u16 xBE sp),
  func_8005531C + func_80055660 (flag-clear opcodes).
- MOJI_TASK grew: u16 x8, stack[8] @0x18, u8 x70/x71, u16 xBE @0xBE.
- unknown_Cd_strucptr: array-decl convention only matches SINGLE-use
  functions; multi-use needs plain scalar pointer decl (cc1 CSEs the array
  base otherwise). Cd_read_sync2 re-verified with the scalar decl.
- vsync_cb needed `volatile` on the frame counter (original reloads after
  store; plain extern lets cc1 CSE the reload away).
- gprel.py now drops ALL small `.extern` directives (COMMON-leak prevention
  no longer depends on census membership).
- Register-allocation lesson: statement order != emission order; cc1
  sched1(pseudo) -> RA -> sched2 means the right source order can look
  "shuffled" (func_8005459C matched with x10,x12,x3E,script order emitting
  x8,x70,script loads first). Brute-forcing 4-8 orderings in a scratch TU
  through the real pipeline is fast and decisive.
- sha1 OK; mutation test (func_8001D414) done. 82 matched, 402 stubs left,
  ~3.3% instruction volume.

## 2026-07-04 (late night — 17-function batch, 99 total)
- Matched 17 more (~209 instructions): moji MojiTaskExec2 (wrapper passing
  script base 0x80153000), func_80054A84 (script2 stack pop — MOJI_TASK
  gains stack2[8] @0x4C + u16 xC0), func_80057BFC (gp global D_80098AF4 +
  `m->script += 1` passed as call arg); player func_8003BE40, func_80040140,
  func_800406A8/DC + func_80040B34 (key-mask tests; PL_WORK gains x9, xA,
  x11C, x11E, x138, x13E, x449), func_80042208; scene func_8001F1DC/F20C
  (new SCENE_WORK struct + include/rock_neo/scene.h), func_8001FB24,
  Sce_flag_test; cd func_8001C7F0 (CdSyncCallback); main func_80012E98;
  sound func_80019A70/AE0 (Sce_flag_test callers).
- Sce_flag_test was the puzzle of the batch: needed (a) `mask` computed as a
  separate local so the load schedules into the middle of the mask
  computation, (b) param reassignment (`flagno = (u32)flagno >> 3`) for the
  in-place srl, and (c) an UNUSED `u8 buf[8]` local to reproduce the
  original's empty 8-byte stack frame.
- Condition sense matters: func_80042208 needed `if (x >= 0) {...; return 1;}
  return 0;` (bgez around) not `if (x < 0) return 0;` (bltz to).
- if/else-with-direct-stores again beat a temp for func_8001FB24 (4 stores
  cross-jumped, value in $v0 not $a0).
- sha1 OK; mutation test (Sce_flag_test mask). 99 matched, 385 stubs,
  ~4.0% instruction volume.

## 2026-07-05 (overnight session)
- Matched 6 more (105 total): moji func_80054BB4/func_80055CC4 (flag-opcode
  pair: compute-into-locals-at-load-site form needed to keep three loads
  live before the stores), moji func_80056148 (statement order x10=x8;
  x12+=script[1]; x3E=x70; script+=2 found by brute-forcing orderings in a
  scratch TU), scene func_8001DD88 (gp-byte-pair reader; the return lh had
  to be written `*(s16*)(&D_80098199 - 1)` so it aliases the sb and stays
  AFTER it — same bytes, forced order), cd func_8001D7AC (stride-12 table
  `extern s32 D_80082CD0[][3]`), main func_80012298.
- Debugged a 4-byte whole-data-segment shift: NOT a COMMON leak — my first
  func_80056148 compiled one instruction short, shrinking .text by 4 and
  shifting everything after. Bisected by stashing per-file.
- MOJI_TASK.xBC (u16) typed. sha1 OK; mutation test on func_80056148.
- Second batch, 6 more (111 total): moji func_80054B4C (D_8008AB08 fn-table
  dispatch via new MOJI_TASK.x7C), func_80057708/func_80057A94 (identical
  script-stack-push + D_8008CBA4[D_80098830] table jump), func_80057DB8
  (Moji_flag u32 masking via *(u32*)Moji_flag), func_80058DB4 (0x80-byte
  0xFF fill; init order c,i,p found by brute force), sub_scrn func_800605DC.
- Pipeline fix: gprel.py now also gp-rewrites refs to small `.comm` symbols
  (tentative definitions like `u8 Moji_flag[8];` in moji.c) when the census
  approves them — the COMMON itself is intentional (splat carves 0x80098A58
  out of the extracted data for C to provide), only the refs needed the
  rewrite. Without this, func_80057DB8's three Moji_flag accesses each grew
  by one lui (+12 bytes of .text shifting all data).
- sha1 OK; mutation test on func_80057DB8.
- Third batch, 6 more (117 total): moji func_80057144 (zennyCount ->
  m->x40 + func_8005BF10 call with `m->script += 1` as arg),
  moji func_8005753C (0xD5-opcode conditional script skip — needed
  `p + (p[1] + 2)` parenthesization to add before the pointer),
  scene func_8001D888 (Cd_read_sync2 drain loop), func_8001FB54/FB8C
  (stage-band -> id via ternary CHAIN, value lands in $v0),
  sound func_80019AA4 (reusing the call-result variable for the if/else
  keeps the value in $v0). sha1 OK; mutation test on func_8001FB54.

## 2026-07-05 (Opus session)
- Matched 6 more (123 total, ~5.1%): Code800133D8 func_800133D8 (zeroes
  Code800133D8_work as three words via a local `s32[3]` view — game.c keeps
  the s16-x0 struct view of the same symbol), cd func_8001B858 (fn-table
  dispatch `D_80087670[D_80098A84->x8]()`), moji func_80055C80
  (`*D_8008D0D4[script2[1]] = script2[2]`), func_800576C4 (script2 call-stack
  push, mirror of func_80057708 using stack2/xC0), func_80057B70, sound
  func_800199F8 (6-halfword backfill).
- **Found + fixed a stale-object fake match**: func_800605DC (claimed matched
  last session) never actually compiled — its C had a `void` vs sub_scrn.h
  `unknown_t` return-type conflict, but the stale INCLUDE_ASM stub object
  stayed linked so the hash passed on the stub. A forced clean recompile this
  session surfaced the error. Fixed the header prototype; now genuinely
  verified. All counts here are post-clean-rebuild.
- New idiom (func_80057B70): a called function's DECLARED RETURN TYPE affects
  the CALLER's register allocation even when the result is unused. Declaring
  `s32 func_80043294(...)` (its real type) keeps $v0 reserved across the call
  so the trailing `m->script2 += 1` lands in $v1 and `return 1` fills the
  load-delay slot — matches. Declared `void`, it collapsed to $v0 + an extra
  nop (one instruction long).
- func_800199F8: reconfirmed the init-order lesson (i before p → counter=$v1,
  pointer=$v0).
- sha1 OK (clean rebuild); mutation tests on func_800605DC + func_80057B70.

## 2026-07-05 (Fable session: audit + 12 more)
- Audited the Opus session first: from-scratch rebuild (rm -rf build) →
  byte-identical cmp; independent mutation tests on func_800133D8 and
  func_800199F8 (not the ones Opus self-tested); recount reconciled at 123.
  Opus's stale-object diagnosis of func_800605DC confirmed from git history.
- Batch 1 (6): player func_800405F4 (x11C vs x128|x12A key test — new PL_WORK
  x128/x12A fields), sound func_80019A34 (5-halfword backfill @D_800822B0),
  sound func_8001A1FC/func_8001A238 (stride-8 five-entry search loops),
  moji func_800553A8/func_800553F0 (Sce_flag_on/off of a 16-bit script
  operand via func_80054410).
- Batch 2 (6): moji func_800555F4 (x10 += operand), func_80057BB4
  (negated-zenny func_80043294 call), func_80058740 (script call-stack push
  + jump via x44 u16 offset table — new MOJI_TASK x44/xC2 fields),
  sub_scrn Map_screen_init, cd func_8001CB30 (callback setup),
  main func_80012424 (joy-chord toggle of D_80097864).
- **NOT matched: scene func_8001FCA4** (stage-band like FB54/FB8C but with an
  ==5 leg). ~12 source forms tried: ternary chain (both == and != — cc1
  canonicalizes them identically to the bne/87-delay mirror), flat and nested
  if/else chains (leg 2 comes out beq+j instead of bnez+delay-85), &&-guard,
  goto form, preset-r. Target needs chain-style legs 1-2 (value in $v0,
  bnez→end with 85 in delay) but an UN-inverted beq→end (86 in delay) +
  j→end (87 in delay) for the equality leg. Everything that fixes one leg
  breaks the other. Re-stubbed; candidate for a stronger-model pass or for
  revisiting after more == ternaries are seen elsewhere in the binary.
- 135 matched / 349 stubs (~5.8% volume); every batch clean-rebuilt,
  hash-verified, byte-compared, mutation-tested.

## 2026-07-05 — Fable (MojiTaskExec session)
- **MojiTaskExec matched** (moji.c, 133-line asm / 127 insns — biggest single
  match so far). Clean rebuild → hash OK → cmp byte-identical → mutation test
  (x3D 3→4 broke the check; restored, OK). 136 matched (~6.3% volume).
- The hard part was a callee-saved s2/s3 mirror between `no` and the CSE'd
  0x40000 mask: an EXACT allocno-priority tie (12/88 == 3/22 in gcc 2.7's
  floor_log2(refs)*refs/live_length formula), diagnosed via cc1 -dl/-dg
  dumps rather than blind permutation. Declaring the `op` param u8 adds the
  QImode entry copy that breaks the tie the original way. Full mechanism in
  LESSONS.md "2026-07-05 (Fable, MojiTaskExec)".
- moji.h struct growth: x4/x6/x8/xA/xC/xE u16 row, u16 x38, u8 x3A, x3E
  s8→u8, u8* x48, u8 x73, u8 x7D/x7E/x7F, x78 s8→u8; prototype now
  `s32 MojiTaskExec(s32, u8*, u8)`; new extern `MOJI_TASK Moji_work[]`
  (0x800BB6B8, stride 0xC4) + `s32 D_80098824`. All prior moji matches
  revalidated by the same clean-rebuild hash.
- Whole function drafted and iterated in the scratch-TU pipeline
  (cpp|cc1|maspsx|gprel|patchasm + a normalizing stream-diff script) before
  touching the tree — tree got exactly one edit and matched first try.

## 2026-07-05 — Fable (overnight autonomous, batch 1)
- 9 moji matches (145 total, ~6.8% volume): func_800546B0, func_80054874,
  func_8005619C/61E8/6234 (sibling triple gated on func_8005DA78/DAEC/DA1C),
  func_800579D8 (Game_work.x7C/x7D u8 fields typed), func_80057B24,
  func_80058D64, func_80059E24 (new extern u8 *D_8008CE5C[]).
- Clean rebuild + hash OK + mutation test (func_80059E24 broken -> FAIL,
  restored -> OK). Two iterations: u8 local re-masked (use s32 for lbu
  operands), and a script-vs-script2 misread (0x14 vs 0x6C).

## 2026-07-05 — Fable (overnight autonomous, batch 2)
- 9 more matches (154 total, ~7.4% volume): moji func_80057AD0, func_80057D60,
  func_800560D0 (if/else not ternary — $v0 rule), func_80059530 (x73 is s8:
  lb readback), func_8005BC90; player func_8003F498/F4E8/F538/F588 quad.
- New externs: u8 *D_8008CE10[]/D_8008D0D4[]/D_8008CCA4[], s8 D_800BE2F7[],
  u8 D_8009899C (gp), u16 D_80098912[].
- Clean rebuild + hash OK + mutation test (player 0x35->0x37 FAILED, restored OK).

## 2026-07-05 — Fable (overnight autonomous, batch 3)
- 6 more matches (160 total, ~7.8% volume): main func_8001215C/12938/12ECC/
  12F24, scene func_8001D928, player func_800402C4 (x124/x134 typed).
- **maspsx patched (tools/maspx)**: the D_801F81xx family in main.c expands
  bare-symbol+reg accesses base-reg-first (addu $at,reg,$at) in the original
  binary, unlike everywhere else ($at-first) — added a per-symbol quirk
  table (ASPSX_REGFIRST_SYM_PREFIXES) + store expansion for quirk symbols.
  Clean rebuild validated the change against all 160 matched functions.
- Mutation test: func_80012F24 store 0->1 FAILED the check; restored, OK.

## 2026-07-05 — Fable (overnight autonomous, batch 4)
- 3 more matches (163 total, ~8.1% volume): main func_80012E10 (OpenTh
  thread-slot setup) + func_80012FEC (PCopen/PCread overlay loader),
  sound func_80019FB4 (Game_work[0x50] dispatch).
- New idiom proven: when the SAME symbol-indexed slot is read twice
  (D_801F811C[n<<5]), cc1 CSEs the address into a register (wrong bytes).
  Writing the second access as a constant-index off a NEIGHBOR symbol
  (((u8 **)D_801F8114)[(n<<5)+2]) gives distinct RTL — both loads stay in
  the $at symbol-indexed form and the reloc resolves to identical bytes.
- Mutation tests: all three perturbations failed the hash check; restored, OK.

## 2026-07-05 — Fable (overnight autonomous, batch 5)
- 5 more matches (168 total, ~8.5% volume): player func_80040380/
  func_80040710/func_80040AEC (PL_WORK xB4/x112/x113/x140 typed),
  sound Sound_call (SND_CMD 0x14-stride queue writer, sentinel D_800BE6D8),
  cd func_8001D7E4 (drain-wait loop; reuses CD_CMD/D_800A3A40 sentinel).
- All five matched in the scratch-TU pipeline before touching the tree.
- func_80042044 parked: 20-insn body matches except register allocation
  (original: k in $v0, ret in $t0, x12A in $a3, m never copied). Proven so
  far: callers pass a1 => signature is (pl, arg1, arg2) forwarded to
  func_80042154 (this alone moves ret to $t0). Remaining delta: cc1 births
  k's pseudo before a/b so it lands in $v0 — no C shape found yet.
- Mutation tests: all five perturbations failed the hash check; restored, OK.

## 2026-07-05 — Fable (overnight autonomous, batch 6)
- 4 more matches (172 total, ~8.7% volume): scene func_8001FD3C/func_8001FD90
  (x52==1 if/else + x52-range ternary chain into gp s16 pairs), sound
  Sound_call2, sub_scrn func_8005EC34 (pre-existing draft un-gated from
  ACCEPT_REORDERING_BULLSHIT — byte-matches now that patchasm reorders).
- New idiom proven (Sound_call2): write the queue-pointer advance as
  `q++; D_80098938 = q;` — the in-place increment clobbers q's register, so
  cc1's scheduler cannot hoist the global store above `q->x8 = ...`.
  `D_80098938 = q + 1` (fine in Sound_call, which has no trailing loads)
  reorders the tail here.
- Parked, same family as func_8001FCA4: func_8001FC50 (cc1 cross-jumps all
  0x81 ternary legs into one block, 2 insns short, value lands in $v1 not
  $v0) and func_8001FDE4 (one branch inverted: original beq->store with
  value in delay + j return; every C/goto shape tried gives bne->return).
  cc1 jump-canonicalization; not worth more brute force tonight.
- Mutation tests: all four perturbations failed the hash check; restored, OK.

## 2026-07-05 — Fable (overnight autonomous, batch 7)
- 9 more matches (181 total, ~10.2% volume — crossed 10%): sub_scrn
  func_8005EC80, func_80060DB8, Sub_screen_cancel_check,
  Sub_screen_shift_check, Sub_screen_sort_attack/energy/range/rapid,
  Sub_screen_rb_parts_set. These were pre-existing draft bodies gated
  behind ACCEPT_REORDERING_BULLSHIT from before the patchasm reorder pass
  existed; with the pass they now byte-match.
- One real fix needed: the drafts call MojiTaskExec(0, D_8008CB94, -1) and
  the ANSI prototype (u8 op) truncated -1 to 0xFF at the call site.
  moji.h now declares `s32 MojiTaskExec();` K&R-style so callers pass raw
  int (original binary has addiu a2,-1) while moji.c keeps the ANSI u8
  definition whose entry copy is register-allocation-load-bearing.
- func_800600CC stays gated (renamed guard): compiles 0x174 vs 0x17C.
- game.c already #defines the macro, so its drafts were live all along.
- Mutation tests: 4 sampled functions all failed the hash check; restored, OK.

## 2026-07-05 — Fable (overnight autonomous, batch 8)
- func_800600CC matched (182 total, ~10.5% volume). Two fixes:
  1. The function needs lui-form accesses to Moji_flag/Moji_flag3 while the
     rest of the TU (and the gp census) uses %gp_rel — same TU, same symbol,
     both forms, so the original source must use different expressions.
     Raw-address derefs (*(u32 *)0x80098A58 / 0x80098B30) assemble to the
     original's exact lui/lw bytes.
  2. tools/maspx: bare-constant-address loads (lw $2,-2146858192) fell
     through every load-delay branch and got no hazard nop; the r_source-is-
     None branch now covers constants as well as symbols (ASPSX treated
     both alike). Clean rebuild re-validated all prior matches.
- Mutation test: Cd_read_comb arg perturbation failed the check; restored, OK.

## 2026-07-05 — Fable (overnight autonomous, batch 8b: consistency fix)
- Caught a stale-object hazard the batch-7/8 builds masked: the K&R
  `s32 MojiTaskExec();` declaration CONFLICTS with the ANSI definition
  (u8 promotes), so moji.c had been FAILING to compile and the link kept
  using the last good moji.c.o — the hash still passed because the stale
  object's bytes were correct. Fixed by converting MojiTaskExec's
  definition to K&R style (params s32/u8*/u8 declared old-style):
  .text is byte-identical to the ANSI object (checked via objcopy cmp;
  only COFF debug metadata differed).
- Tried enabling the USE_OG_COMPILER-gated Sub_screen_rb_parts_calc draft:
  compiles 0x1AC vs 0x1C4 (6 insns short) — re-gated with a note.
- Verified with a FULL from-scratch rebuild (`rm -rf build`): zero compile
  errors and hash OK.

## 2026-07-05 — Fable (overnight autonomous, batch 9)
- 5 more matches (187 total, ~10.8% volume): moji func_80057D00 (stack2
  sibling of func_80057D60) + func_80057DF4, player func_8003EE68 (PL_WORK
  x74/x75 typed), sound func_800198C0, sub_scrn func_80060248.
- GAME_WORK extended: GW84_ENTRY x84_tbl[] at 0x84 (0x10-stride, u16
  vals[6] at +4), indexed by gp-u8 D_800989D4.
- func_80057DF4 needed two subtleties: u32 loop counter (sltiu) and the
  `((GW84_ENTRY *)&Game_work.x84_tbl)[idx].vals[i]` cast form — direct
  member indexing folds vals' +4 into the symbol addend (wrong bytes),
  the cast keeps GW+0x84 as the base with +4 in the lhu.
- func_800199A4 parked: FC50/FCA4 jump-canonicalization family (one
  value-leg cross-jumped + final branch inverted, 2 insns short).
- Mutation tests: all five perturbations failed the hash check; restored,
  OK (final rebuild error-grep clean).
