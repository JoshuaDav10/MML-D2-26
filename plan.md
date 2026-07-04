# MML-D2-26 Plan

## Phase checklist

1. Install toolchain deps (mips cross-compiler, mkpsxiso, python venv)
2. Place disc files in `disks/` (gitignored)
3. `make extract_disk` → `make split_all` → `make`
4. Record baseline mapped/matched counts in `progress.md`
5. Begin branch-per-task workflow (`match/FUN_8006xxxx`)

## Git hygiene

- **upstream** = ChrisNonyminus/mml1; pull with `git fetch upstream && git merge upstream/master`
- **origin** = JoshuaDav10/MML-D2-26
- Work on feature branches (`match/...`, `map/...`); merge only verified work
- Track **mapped** and **matched** separately in `progress.md`
- Never commit `disks/`, `build/`, `asm/`, or game assets
