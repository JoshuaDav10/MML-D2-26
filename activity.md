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
