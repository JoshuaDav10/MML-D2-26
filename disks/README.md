# Disc files (local only — never commit)

This directory is **gitignored**. Place your US Mega Man Legends disc dump here for the build pipeline.

## Expected layout

Per upstream README:

- US `.cue` file
- Track 1 `.bin` renamed to `mml1.us.track1.bin`
- Track 2 is an empty 32 MB file (if required by your cue)

## Local sources (copy or symlink — do not commit)

- `/home/X/Documents/MML/MML_Decomp_Project/disks/us/`
- `/run/media/X/X9 Pro/Mega Man Legends/Main/Mega Project (D)/MML Tools/MML- Programming/MML Decomp/CDRIP/`

## Warning

**Never commit** disc images, extracted game assets, or copyrighted binaries. This folder must stay local only.
