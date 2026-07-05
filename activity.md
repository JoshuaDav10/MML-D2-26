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
