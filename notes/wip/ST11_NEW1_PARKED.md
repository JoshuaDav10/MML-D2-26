# PARKED — the 5 "nearly free" ST11 bodies (st11_new1.md)

Attempted 2026-07-29, reverted. The tree is green; nothing from this batch is landed.
Draft C: `notes/wip/agent_reports/st11_new1.md`.

An agent verified all 5 with bytecmp and reported them ready. They are **not** ready — the
real link found three independent problems bytecmp structurally cannot see. Recording
them because each is a reusable lesson, not just a blocked batch.

## 1. `System_timer` does not resolve in this overlay
```
undefined reference to `System_timer'   (x2, in func_80109034)
```
The agent explicitly claimed it "resolves inside ST11's own chunk — I checked ST11's
generated syms". **The linker disagrees.** An agent inspecting symbol lists is guessing at
what the link will do; only the link knows. Treat any agent claim about symbol resolution
as unverified until a build proves it.

Fix direction: find where `System_timer` actually lives and whether ST11's
`undefined_syms_auto` / `generated.rock_neo.syms.txt` should carry it, or address it the
way the target asm does (a raw address, like `func_80036A78` does for scratchpad).

## 2. `structure has no member named 'xB'`
I split `u8 padB[0x14 - 0xB]` into `xB` + `padC` in the struct the agent named, but
`func_8010908C` resolves against a **different** struct. Landmine #5's neighbourhood: with
`-lang-c` the member namespace is global-ish, so "add the field to BOSS_WORK" is not
enough — the body has to reference the struct that actually owns offset 0xB.

## 3. `conflicting types for func_80101B70`
Line 340 vs a previous declaration at line 189. The landing script inserted a body whose
signature disagrees with an existing forward declaration in the same file. **The landing
script should detect an existing declaration and reconcile or refuse** — it currently just
substitutes.

## Why this is parked rather than fixed
Three distinct failures in one batch, and the loop rule is three strikes -> park and move
on. The engine-function lane is landing +16 per build cycle; spending that budget here is
the wrong trade. Come back when the engine pool thins out.

## What is still good here
The 7 additional ST11 dispatchers (tables and offsets recorded in HANDOFF) are a separate,
cleaner batch and are NOT blocked by any of the above.
