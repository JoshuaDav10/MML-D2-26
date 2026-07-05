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
