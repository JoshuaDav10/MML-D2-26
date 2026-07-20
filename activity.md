# Activity Log

## 2026-07-14 (Opus) — 2nd permuter win
- **func_80062C6C (debug) MATCHED** (275->276) — audit_count.sh census 276, hash OK,
  raw cmp identical, mutation-tested. 2nd true permuter win (register-mirror, like
  E4C4). Lever: init `i=0` before `fp=D_8008DBE0`. Debug fn-table dispatch loop.

## 2026-07-14 (Opus) — first permuter near-miss win
- **func_8001E4C4 (scene) MATCHED** (274→275) — AUTHORITATIVE: audit_count.sh
  census 274→275, full hash OK, raw cmp identical, mutation-tested. FIRST win of
  the "hand near-match → permuter finishes" lane: hand-work reached a 1-diff
  register-mirror (allocno tie, unforceable by hand); permuter (mml_E4C4, a "good
  target" per the guide) hit score 0 at ~19k iters; landed + full-audit-verified.
  Winning lever: `long c;` + redundant `if(i){c=*arg0;}else{c=*arg0;}`. Proves the
  Lane-B strategy. Watcher/notify caveat learned: launch permuter AS the tracked
  background command (not `nohup&` inside it) or --stop-on-zero won't notify.

## 2026-07-14 (Opus) — harvest continued (post-correction, disciplined)
- **func_8001A63C (sound) MATCHED** — AUTHORITATIVE: census 273→274, full hash OK,
  raw cmp identical, mutation-tested. Magic-div-by-12 volume lookup. Genuine +1.
  Levers: pos branch = fall-through; table lookup before the shift; `a1=a1*v0`
  reassign to reuse $a1 for mflo. Caught a subtle trap: census showed 274 while
  the FULL hash still FAILED (a $a1-vs-$a2 register nit in the tail) — census
  reads per-.o and misses link/register nits. Only `audit_count.sh` (hash) is the
  real gate. Fixed the reg nit → hash OK. Baseline this session was 271
  (authoritative); genuine matches = func_80016434, func_8001D394, func_8001A63C.

## 2026-07-14 (Opus) — harvest restart + overlay/expedition reconciliation
- ⚠️ **COUNT CORRECTION**: I briefly claimed func_800155A4/func_8005EC80 (+3 more)
  as new matches (278→280→283). RETRACTED — all PHANTOM. game.c AND sub_scrn.c
  carry `#define ACCEPT_REORDERING_BULLSHIT` at the top (since commit 957191c),
  so every `#ifndef ACCEPT_REORDERING_BULLSHIT` guard ALREADY compiles the body
  branch. Those bodies were matching all along; "un-gating" them is a no-op that
  doesn't change the binary or the count. AUTHORITATIVE `census.py --matched` =
  273 (verified: rm -rf build, hash OK, raw cmp identical). Genuine new this
  session = 2: func_80016434 (game) + func_8001D394 (cd) — real INCLUDE_ASM
  stubs I wrote C for + mutation-tested. LESSON: never count from `grep
  INCLUDE_ASM` (blind to the define); only `census.py --matched` is truth.
- **func_8001D394 MATCHED** (277→278), clean rebuild + sha1 OK + mutation test.
  cd.c CD-volume re-arm. NEW IDIOM (LESSONS): assign a symbol address to a local
  pointer (`u8 *p = D_800AD140;`) and pass `p + off` — forces cc1 to hold the
  base in a register (`addu a0,v1,0x14`) rather than constant-fold `SYM+off` into
  an absolute `&D_800AD154`. Directly relevant to the BB4C addressing-crux genus.
- **func_80016434 MATCHED** (276→277), clean rebuild + full-binary sha1 OK +
  mutation test (0x18→0x19 broke the hash; restore → OK). 27-insn game.c global
  initializer gated on `if (arg0 == 2)`. Trap avoided: `D_800C356F` was already
  declared `u8[]` in rock_neo.h (reused it as `[0]`, didn't redefine); grep the
  TU for pre-existing decls BEFORE adding externs (batch-12 false-pass lesson).
- **Overlays confirmed DONE**: 205/205 CDDATA/DAT .BIN byte-identical on dev
  (all 19 formerly-"missing" files present + cmp-clean). OVERLAY_EXPEDITION.md
  was stale; marked COMPLETE. No agent needed — checked before acting.
- **53B40 expedition tooth 5**: reproduced + re-verified the 3 levers (worker's
  scratch died with its session), found prior permuter runs used a lever-less
  base, re-seeded it, re-launched permuter. Residual = $fp-vs-$s7 allocno only.
- **func_8001F158 PARKED** (scene) with findings — don't re-grind blind. Logic
  fully solved (held ptr `p = Scene_work.xA4`; advance `xA4 = p+8`; copy `p[8]`→x8,
  `p[9]`→x9 with `x18/x1C/x10` zeroing on inequality). SCENE_WORK struct verified
  correct via cc1-27 offsetof probe (x8@0x8, xA4@0xA4, size 0xA8). Sole blocker:
  cc1 CSE's `&Scene_work.xA4` into ONE held register (read+write via `0(v1)`);
  the target re-materializes the xA4 address ABSOLUTELY at both the read and the
  write (`lw a0,%lo(...)` then `sw v0,%lo(...)`), keeping only the xA4 VALUE in a0.
  Reading x8 early into a local didn't break the address CSE. This is the
  register/addressing-allocation genus (same family as BB4C/53B40). Revisit with
  the permuter or a -dg allocno pass; the draft is in this session's git history.
- **Harvest run (build-runner-offload → sync builds)**: 3 more small functions
  attempted — func_8001E4C4 (scene), func_8001AE6C (sound), func_8001F158 (scene).
  ALL solved on LOGIC, ALL missed on a single cc1 scheduling/allocation nit
  (allocno reg-tie / delay-slot / basic-block tail ordering). Saved as 1–2-insn
  permuter last-mile candidates in notes/wip/HARVEST_NEARMATCHES.md.
  DISCOVERY: the easy wins (16434, D394) were straight-line stores; control-flow
  small fns are scheduling-hard. Plan split: hand-match straight-line stores;
  route branch/loop near-matches to the permuter. Don't hand-grind the nits.
- **Mission %**: 278/497 = 55.9% by function count; ~17.6% by instruction volume.

## 2026-07-12 (Opus) — scene momentum + permuter fix
- **func_8001F6C4 MATCHED** (275→276), clean rebuild + full-binary sha1 OK +
  mutation test (`p[f+1]`→`p[f+2]` broke the hash; restore → OK). 28-insn scene
  fn. Knobs: (1) a call with no arg-setup taking a "leftover" a0 that differs per
  path = one reused local `a` passed as the arg; (2) a "+0xC whole-data shift"
  that was NOT a COMMON leak — the body was 3 insns too long (data follows text);
  (3) hoisting `s32 f = D_800C356F[0];` above the `if` fills the bne delay slot
  with `sll` and drops the 3 extra insns; (4) byte-in-bss syms via lui/%lo want
  the array decl (`extern u8 D_800C356E[]`). See LESSONS.
- **Permuter unblocked**: `pip install toml` + fixed tools/patchasm.py crash
  (build/test.s debug write from a permuter workdir). Added notes/PERMUTER_GUIDE.md.
  53B40 confirmed a dead permuter target (best 8535/base 11395 over 2820 iters —
  value-CSE wall).

## 2026-07-12 (Opus) — sub_scrn sort vein
- **sub_scrn 271→275** (f2e025e). Sub_screen_sort_{attack,energy,range,rapid}
  activated from parked drafts (stripped ACCEPT_REORDERING_BULLSHIT guards).
  Verified full-binary sha1 OK after a forced-clean rebuild (rm'd sub_scrn.c.o).
  NOTE: census.py under-counted (270) — it reads the built .c.o intermediate, so
  always rebuild before trusting census. Ground truth is the hash.

## 2026-07-12 (Opus) — moji harvest resume (target: reliable wins)
- **func_80053AA4 MATCHED** (267→268), clean rebuild + full-binary sha1 OK +
  mutation test (xC2 0xFF→0xFE broke the hash; restore → OK). The kill-one-slot
  handler: `m=&Moji_work[no]` (u8 no), cond `func_8001D494(0,1,0)` on the
  0x40000 flag, `m->xC2=0xFF; m->flags=0`, then clear a Moji_flag bitset.
- **IDIOM (new knob):** an AND-of-3 where two operands are a literal mask and a
  runtime value gets reassociated by cc1 into `f & (lit & rt)` (combines the two
  masks first). To pin the original's `(f & lit) & rt` accumulator form, split
  off a temporary: `f = *(u32*)Moji_flag & 0xBFC1FFFF; ... = f & ~(0x08000000>>no);`.
  First attempt (single expression) mismatched ONLY on this grouping.
- **func_80053788 MATCHED** (268→269), clean rebuild + sha1 OK + mutation test
  (x6=no→0 broke it; restore → OK). moji reset-all: Cd_read_comb(0x1E),
  func_8001D7E4(), loop over Moji_work[0..4] clearing flags/script2/x6/x48,
  then D_80098B2C/Moji_flag/D_80098960 = 0.
  - **IDIOM (addressing mix):** the loop's `script2` (0x6C) store compiled to an
    absolute `%lo(Moji_work+0x6C)($at)` under plain `Moji_work[no].script2 = 0`,
    but the original holds a base pointer `m = &Moji_work[no]` for THAT store
    only (`sw zero,0x6c(v0)`) while flags/x6/x48 stay array-absolute. Writing
    exactly that mix (`m->script2` + `Moji_work[no].field` for the rest) matched.
  - Header had a guessed `s32 func_80053788()`; kept the def `s32 ...(void)` with
    no return (byte-identical to void) to avoid touching unknown callers.
- **func_80057C2C MATCHED** (269→270), clean rebuild + sha1 OK + mutation test
  (terminator 8→9 broke it; restore → OK). Bit-scan of D_800BE3DB (MSB-first)
  builds the D_800BE2F8 index list + `8` terminator, then a CALL-op dispatch on
  `m->script2[count]`. Took 7 evidence-driven diff iterations; each fixed a real
  difference. Idioms proven:
  - count++ is CONDITIONAL, so gcc can't strength-reduce `D_800BE2F8[count]` —
    the walking pointer must be EXPLICIT in source (`*p++ = i`).
  - `u32 i` (not s32) → `sltiu` for the `i < 8` bound (s32 gives `slti`).
  - `u8 *q = &D_800BE3DB; *q` — the write through `*p++` aliases it, forcing cc1
    to hold &D_800BE3DB in a reg and reload the value each iter (matches `lbu 0(t1)`).
  - if/else with ONE tail `return 1` (not an early `return`) → cc1 shares the
    return and fills the `script2` store into the branch delay slot.
  - **preheader scheduling knob:** the hoisted `0x80` mask kept landing on the
    wrong side of the walking-ptr init. Fix: make it a named local `s32 mask`
    and place it IN the for-init between `i` and `p`
    (`for (i=0, mask=0x80, p=D_800BE2F8; ...)`) — for-init comma order maps to
    preheader instruction order.
- **PARKED func_80057924** (left INCLUDE_ASM): logic fully solved and verified via
  diff (two-arm func_80043294 call: fall-through `-(x7D-x7C)*10`, else
  `-zennyCount`; then `x7C=x7D; script2+=2; return 1`). Blocked because the
  original holds `&Game_work.x7D` in a SECOND callee-saved reg ($s0) across the
  call and reloads x7D through it per-arm, while my clean C loads x7D fresh at
  the join. This is the hold-a-struct-field-base-pointer-across-a-call genus —
  same family as BB4C's addressing-crux. Not worth grinding; revisit if that
  genus gets a general idiom.
- **Sub_screen_cancel_check MATCHED** (270→271), clean rebuild + sha1 OK +
  mutation test. This was a PRE-WRITTEN draft parked behind `#ifdef
  ACCEPT_REORDERING_BULLSHIT` (from before the patchasm reorder pass existed) —
  it compiles-matches as-is now. Just un-ifdef'd it. **Lead: audit the other
  ACCEPT_REORDERING_BULLSHIT-parked drafts — some may now match for free.**
- Filed the deep-research FINDINGS doc for the two giant blockers (53B40 CSE-hoist
  / BB4C pointer-fold) — notes/RESEARCH_FINDINGS_gcc272_idioms.md, candidate-only,
  kept out of LESSONS.md until hash-gated (commit 62512b6).

## 2026-07-06 (Fable, session resume) — moji small-opcode harvest (10 matches)
- Landed 10 moji.c script-opcode handlers, each full-binary sha1 OK:
  func_8005497C, func_800564C8, func_80056558, func_80056610, func_800566CC,
  func_80055438, func_80057184, func_80055A78, func_80055B14, func_8005721C.
- Reusable CALL-opcode template: base=m->x44; if(base) MojiTaskExec(no, base,
  op) else MojiTaskExec(no, D_8008CACC[idx], 0xFF); script += 3. Variants remap
  the op index through a table (D_800BE2F8, or Player_work+0x450/0x454) and the
  null-arm folds an op bias (e.g. -1) into the table base (D_8008CAC8=CACC-4).
- Idioms pinned this session: (s8)m->x71 forces signed lb; the u16-remap-inside-
  Player_work needs struct-member access (Game_work-style) to emit per-site
  %hi/%lo(Player_work+off) instead of a hoisted base ptr; `m->x48 = m->script2
  = ...` store order (script2 first) load-bearing in func_80055438; explicit
  `& 0xFFFF` into an int (not a u16 var) pins the mask at the store in 55B14.
- Near-match parked: func_80056778 — cc1 hoists the join-block `lui` (flags
  const 0x402000) into the bnez delay slot; original keeps a nop there.
  Same delay-slot/const-hoist genus as the giants. Left as INCLUDE_ASM.
- Salvaged both dead background giants' work into notes/wip/: BB4C cbase-local
  iteration (461 mismatches, 806/809 words) and the 53B40 draft + root-cause
  (loop.c move_movables use-count ranking hoists 0x1F800070 over 0x40000000).

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

## 2026-07-05 — Fable (day session, batch 10)
- Finished the batch the overnight session left in flight: 4 more matches
  (191 total, ~11.1% volume): scene func_8001DE84 + func_8001F740,
  main func_8001319C, debug func_800629F0.
- func_8001DE84: cc1 collapses a pure `if (x&8) return 1; return 0;` tail
  into sltu (setcc). The matching form shares the return-0 via a goto label
  from the earlier leg — the extra inbound edge blocks the store-flag
  transformation (LESSONS).
- func_800629F0: original stores Debug_work.x4/.x6 BEFORE the fn-table
  load, no address CSE with the &Debug_work call arg. Raw-address stores
  get hoisted past (constant vs symbol disambiguates); same-symbol field
  stores get anchor-CSE'd with the arg. Fix: store through the NEIGHBOR
  symbol `((u16 *)&Scene_work)[-26/-25]` (Scene_work = Debug_work+0x38) —
  symbol MEM pins the load, distinct symbol defeats the anchor (LESSONS).
- func_8001319C: `p[i + 0x71]` mirrored the addu (index first);
  `*(p + i + 0x71)` (left-assoc) keeps the pointer first.
- Verification: clean rebuild hash OK; 3 mutation tests (one per touched
  TU) each failed the check; restored, final rebuild OK.

## 2026-07-05 — Fable (day session, batch 11)
- 7 more matches (198 total, ~11.7% volume): moji func_800594CC /
  func_80054798 / func_80055344 / func_80055C1C, player func_80041E90,
  scene func_8001E390 / func_8001F580.
- Three matched on the first scratch draft (the MOJI_TASK vocabulary is
  paying off); func_80055C1C needed the Opus callee-return-type idiom
  (func_80039E18 declared s32 reserves $v0, return-1 fills the delay slot);
  func_8001F580 needed a distinct u16 keep copy of its u16 param (s32 copy
  gets merged back into the param's register, one callee-save short).
- PL_WORK x56/x10C/x116/x126 typed from the asm.
- PARKED func_80041EF4 (player): the original ends with two andi 0xFFFF
  truncations of lhu-loaded values feeding func_80041F54's 4th arg; cc1
  proves nonzero_bits ≤ 0xFFFF through every source shape tried (~12:
  locals s32/u16/s16, param reassignment, single vs cross-jumped double
  call sites, explicit casts/masks) and elides them (2 insns short).
  RTL confirmed: the zero_extends exist in .jump, die in combine. Needs a
  def combine can't trace — none found. Revisit if another andi-keeper
  matches.
- PARKED func_80019918 (sound): FC50/FCA4 jump-canonicalization family
  (legs 2+3 assign 7, legs 1+4 assign 0x39 — cc1 cross-jumps + inverts).
- PARKED func_8001A6DC (sound): lerp helper; register-birth problem — cc1
  shares $s0 between param n and call-result a (24 vs 26 insns), original
  keeps a in $s2. -dl shows a's copy scheduled after the n+1 arg setup,
  making ranges disjoint. Same genus as func_80042044.
- func_8001A0A8 matched in scratch; landing with batch 12.
- Verification: clean rebuild hash OK (after fixing an Sce_flag_on decl
  conflict with rock_neo.h's unknown_t prototypes — scratch TUs must
  mirror the real include chain); mutation test per touched TU.

## 2026-07-05 — Fable (day session, batch 12)
- 6 more matches (204 total, ~12.2% volume): sound func_8001A0A8, cd
  func_8001D254 + func_8001D2BC (CdControl/CdControlB retry twins), moji
  func_80057A24 + func_80058788, scene func_8001E460.
- func_80058788 is the Moji task-slot bit clear: pointer-diff m - Moji_work
  becomes the 0x1A1F58D1 magic-multiply division by 0xC4;
  Moji_flag3 &= ~(0x20000 << n); compute-into-locals order (f before n)
  pins the constant lui pair.
- CAUGHT A FALSE PASS: cd.c had a pre-existing `void func_8001D254(s32,
  s32, u8*)` declaration conflicting with the new definition; the TU failed
  to compile, the pipeline still wrote a partial .o, and check printed OK
  on the stale exe. The build-runner report even rationalized it as a pass.
  Direct rebuild exposed it; decl unified to (u8, u8*, u8*) — caller bytes
  unchanged (hash-verified). Reinforces: grep make output for errors
  YOURSELF before trusting OK.
- PARKED func_80013578 (Code800133D8): all 25 insns correct but one
  scheduling slot off — the area_no/x82 sbs sink below the call arg's la;
  ~10 shapes tried (store orders, volatile, fn-ptr local, K&R). The
  cross-jumped direct-stores if/else (func_8001997C idiom) got the value
  leg right; only the sb/la interleave differs.
- Mutation tests: 4/4 (one per TU) failed the check while mutated;
  restored; final clean rebuild 0 errors, OK, cmp byte-identical.

## 2026-07-05 — Fable (day session, batch 13)
- 5 more matches (209 total, ~12.6% volume): player func_8003F224 +
  func_80040224, scene func_8001D8C0, moji func_80054ADC + func_80055BB0.
- The DE84 goto-shared-return trick generalized: func_8003F224's tail
  `if (k & x140) return 0; return 1;` collapses to sltiu unless the
  return-1 is a label also reached from the entry test.
- New reliable knob: `and` operand mirror flips with source operand order
  (`pl->x13E & k` ↔ `k & pl->x13E`) — used three times this batch.
- moji x7C dispatch pair typed: D_8008AAC0 is a second fn table alongside
  D_8008AB08; 54ADC caches script2[1] in a local (no reload after the sb),
  55BB0 reads m->x7C from the field (reload) — the asm tells you which.
- Mutation tests 3/3 failed while mutated; final rebuild 0 errors, OK,
  cmp byte-identical.

## 2026-07-05 — Fable (day session 2, batch 14)
- 6 more matches (215 total, ~13.2% volume): moji func_80058EA0, scene
  func_8001E3F0 + func_8001EB98, cd func_8001D324, player func_8003FDA8 +
  func_80041A44. All scratch-matched via bytecmp.sh before touching the
  tree; every diff word was a reloc before landing.
- func_80058EA0 is func_80058788's twin + a D_80098851=0xFF gp-store; the
  new store's STATEMENT POSITION had to be after the script2 store (before
  the flags store it perturbs the whole schedule/allocation).
- func_8001EB98: two new-knob composite — dead u8[8] frame local (frame
  0x28 not 0x20) + one reused u32 temp with in-place reassign shift
  (t = p[0]; t <<= 2) + cast byte-offset deref keeps the second
  Scene_work.x24 access symbol-indexed in $v1. SCENE_WORK.x24 typed
  (u8* slot array). GAME_WORK.x60 typed (u16, clamped accumulator).
- func_80041A44: assigning the result var only AFTER the compare legs
  (else-if ladder, early `return 0xB` literal) keeps it dead across the
  lbu temps so it lands in $v0 and the delay-slot li's rematerialize it —
  the v1+move shape from any live-across form is 2 insns long.
- func_8003FDA8: preloading pl->x11C into a local hoists the lhu above
  the first branch (scheduler can't cross blocks on its own).
- Mutation tests 2/2 (cd, player) failed the check while mutated;
  restored; final clean rebuild 0 errors, OK, cmp byte-identical.

## 2026-07-05 — Fable (day session 2, batch 15)
- 6 more matches (221 total, ~13.8% volume): moji MojiTaskKill +
  func_80054804 + func_800570B0, cd func_8001CC08, scene func_8001DFEC,
  main func_800122D0. All scratch-matched first.
- CAUGHT ANOTHER FALSE PASS at land time: MojiTaskKill defined void but
  moji.h declares s32 — moji.c failed to compile, stale objects kept the
  hash green. The error-grep of make output caught it (the check alone
  would not have). Definition flipped to s32, bytes identical.
- func_8001DFEC: writing e as an INTEGER add ((u8*)(n*0x14 + (s32)st[1]))
  keeps the scaled index in $rs of the addu AND forces the arg-copy shape
  (pointer-typed adds canonicalize ptr-first and coalesce e into $a0,
  going one insn short). Copy-statement-before-|= gives the right
  schedule; the sll into a fresh reg needs k live past it (statement
  order, not a cast trick).
- func_800570B0 is func_80055344 with a Moji_flag[0]==s[1] gate: the
  c1/c2/key/flag INIT ORDER (key fourth, flag fifth) is what lands
  key=$a0/flag=$v1; other orders mirror them.
- func_8001CC08: u8 param + K&R definition (cd.c's existing unprototyped
  forward decl), masked at use per the MojiTaskExec rule.
- Mutation tests 2/2 failed while mutated; final rebuild 0 errors, OK,
  cmp byte-identical.

## 2026-07-05 — Fable (day session 2, batch 16)
- 2 more matches (223 total, ~14.0% volume): player func_800400B8, game
  func_800164B4.
- NEW IDIOM: a `switch` on a byte field keeps the ==0 leg's beqz
  UN-INVERTED with the case body out of line (beqz/beq/j ladder); the
  equivalent if/else-if emitted bnez with the body inline — first tool
  that beats the jump-canonicalization on an equality leg. Try switch on
  the parked func_8001FCA4 family next session.
- Cursor's tools/bytecmp.sh (Brief 4) used for both — works as specified.
- Also this session: audited+merged Cursor's Briefs 2/4/5 outputs onto dev
  (from-scratch build: 205/205 overlays OK), added Briefs 6/7 to the
  delegation queue. Brief 1 remains fork/push/repoint (gh auth now works).
- Mutation test failed while mutated; restored, 0 errors, OK,
  cmp byte-identical.

## 2026-07-06 — Fable (session start: audit Cursor's FCA4 + Briefs 6/7)
- Audited Cursor's match/fca4-switch branch: func_8001FCA4 matched via
  switch on Game_work.x52 (the batch-16 switch discovery applied to the
  parked jump-canonicalization problem). Independent clean rebuild →
  0 errors, hash OK, cmp byte-identical; mutation test (0x86→0x88) failed
  the check while mutated; restored OK. ff-merged to dev. 224 matched,
  ~14.0% volume, 260 active stubs.
- Cursor had not updated progress/activity/LESSONS — done now; the switch
  idiom is written up in LESSONS.md (2026-07-06 section) with a retry list
  for the rest of the family (FC50, FDE4, 199A4, 19918).
- Spot-checked Briefs 6/7 on dev: census.py --matched reproduces
  484 = 224 matched + 260 stubs; xref.py regenerates notes/XREF.md clean
  (stub count refreshed 261→260 in this commit).

## 2026-07-06 — Fable (batch 17: switch family cleanup)
- 2 more matches (226 total, ~14.2% volume): scene func_8001FC50 +
  func_8001FDE4 — the rest of the scene jump-canonicalization family,
  both via the switch idiom. FC50 needed two refinements (explicit case 0
  to force the case tree; fallthrough-into-default for the shared 0x81
  leg); FDE4 matched on the first switch draft. Both matched in the
  scratch TU (bytecmp) before touching the tree; first in-tree build OK.
- 2/2 mutation tests failed while mutated; restored, 0 errors, hash OK,
  cmp byte-identical. Remaining parked in the family: sound-side
  func_800199A4/func_80019918 (retry with switch next).

## 2026-07-06 — Fable (jump-table infrastructure + FCE4)
- JUMP-TABLE INFRASTRUCTURE LANDED: C switches can now emit their rodata
  jump tables and the build places them byte-identically. Proven on
  func_8001FCE4 (227 total, first-of-71-tables function): 800.rodata.s
  split at ROM 0x1120/0x1140, scene.c.o(.rodata) slotted between them in
  rock_neo.ld, splat yamls updated to match. Full recipe in LESSONS.md.
- Verified: clean rebuild 0 errors, hash OK, cmp byte-identical; TWO
  mutation tests (case value = code bytes; case remap = table bytes only)
  both failed while mutated; restored OK. Map confirms .rodata at
  0x80010920 size 0x20 from scene.c.o.
- This unblocks the 28 parked jump-table functions (31 tables), including
  the scene F8DC/F9AC siblings and — eventually — cd/func_8001BB4C
  (~845 insns, 3 tables), the binary's biggest function.

## 2026-07-06 — Fable (multi-table proof: scene F8DC + F9AC)
- 2 more matches (229 total, ~14.4% volume): scene func_8001F8DC +
  func_8001F9AC. MULTI-TABLE carve proven: all three scene jump tables
  (jtbl_800108C0/800108F0/80010920) now emitted by scene.c.o(.rodata),
  contiguous at 0x800108C0 in function order, whole binary byte-identical.
- Key discovery: case-0 flag test written as `|| ternary` (not if/else)
  keeps the result pseudo in $v0. if/else makes cc1 precompute the
  else-value and invert the branch, extending the pseudo's live range
  across a Sce_flag_test $v0 return → .greg `conflicts: 2` → forced to
  $v1, mirroring every value store. Found via cc1 -dg greg dump.
- Verified: clean rebuild 0 errors, hash OK, cmp byte-identical; mutation
  tests on a case VALUE (F8DC code) and a case-to-body REMAP (F9AC table)
  both failed while mutated; restored, OK. Multi-table constraint confirmed
  (contiguous matched tables per TU) and written to LESSONS.

## 2026-07-06 — Fable (cd sibling campaign toward BB4C)
- 1 match (230 total, ~14.4% volume): cd func_8001CAAC (CD command-queue
  dequeue/shift-down). KEY: field-by-field struct copy (not `*dst=dst[1]`)
  reproduces the original's two-IV cursor (a1=&dst->xC). Clean rebuild 0
  errors, hash OK, cmp byte-identical; mutation test (cmd<-arg0) failed
  while mutated; restored OK.
- Scoped the path to cd/func_8001BB4C (~845 insns, the binary's biggest):
  only 3 of its callees remain stubs — CAAC (done), CB7C, CF98. Both CB7C
  and CF98 drafted + analyzed to <=2 shape issues each; PARKED with precise
  findings in LESSONS (CB7C: cross-block delay-slot fill + address-CSE for
  D_800AD142; CF98: counter/ptr register order + arg*12 index order).
  These are the next targets before BB4C itself.

## 2026-07-06 — Opus (CF98 matched; census reconciled to 232)
- 1 match (232 total, census.py --matched authoritative: 232 / 252 stubs /
  484): cd func_8001CF98 (CD arm). Resolved the parked load-duplication:
  declare `s32 v;` and ASSIGN it in the body (not at declaration) — collapses
  the [arg][0] load to ONE reg reused for both the D_80098A7C store and the
  CdIntToPos arg0, WHILE keeping the arg*12 index CSE'd in a callee-saved reg
  across the call for the later [arg][1] read. Hoisting the init breaks the
  index CSE (re-derives arg*12 twice); writing the expr twice splits the
  value into two regs. Both knobs needed — see LESSONS. Clean rebuild 0
  errors, hash OK, cmp byte-identical; mutation (D_80098B42 7->8) failed while
  mutated, restored OK. THIRD (last remaining drafted) BB4C callee done.
- Count reconciliation: the prior CB7C entry below claimed "232 total" — that
  was an off-by-one (it was 231 after CB7C). census confirms 232 only now,
  after CF98.

## 2026-07-06 — Fable (overnight: CB7C matched, CF98 parked, BB4C started)
- 1 match (231 total): cd func_8001CB7C (CD retry re-arm). D_800AD142
  array-decl for the single-materialized-address RMW; D_8009896C=0 before
  the |= for the schedule. Clean rebuild 0 errors, hash OK, cmp identical;
  mutation test failed while mutated; restored OK.
- func_8001CF98 PARKED after full structural analysis: KEY find is
  CdIntToPos is the PSYQ 2-arg CdIntToPos(int, CdlLOC*) — the &D_80098814
  before the call is its CdlLOC out-param. Remaining: cc1 double-emits the
  [arg][1] load/store around CdIntToPos (needs CSE-dump). Signature known
  (void(s32)), so BB4C proceeds.
- Both remaining BB4C callees now either matched (CB7C) or signature-known
  (CF98 = void(s32)). Starting func_8001BB4C (~845 insns).

## 2026-07-06 — Fable (overnight, cont.): BB4C foundation laid
- Started func_8001BB4C (845 insns, biggest in the binary). Generated an
  m2c draft (notes/wip/bb4c_m2c_draft.c) using the combined asm+3-tables
  input (notes/wip/bb4c_combined.s), mapped the ENTIRE control flow, and
  wrote notes/wip/BB4C_ANALYSIS.md: the three nested jump-table switches
  (outer state machine on D_800989C4 / inner on D_800989C8 / innermost on
  cmd->type), both work-struct layouts (D_800B5DB0 command entries stride
  0x800; the D_800C5604 work area with the s2/s3/s4 pointer trio), the
  PSYQ signatures, and a step-by-step matching plan.
- Deliberately NOT landing a partial draft in-tree (can't hash-gate a
  non-matching function; project rule). BB4C is a multi-session decomp;
  this session laid the full resumable foundation. Its three tables
  (jtbl_80010244/25C/284) are contiguous in 800.rodata.s → one cd.c.o
  rodata carve, mirroring the proven scene multi-table recipe.

## 2026-07-11 — Fable (session restart: stock-take + 557B8 matched)
- Took stock after the 3-workstream session: TaskList empty (background agents
  died with the session); audited all three agent worktrees against dev.
  VERDICT: everything already banked — a46's ba01152 (BB4C 805/809, ~510 hard)
  is superseded by dev's 806/809 @ 461 (commits 09e0770 + 1017bbc); aee's
  dirty bb4c_draft_v2.c is the already-salvaged cbase iteration (net-neutral);
  ac12's three 53B40 commits are byte-identical to dev's notes/wip files.
  Worktrees pruned; branches kept as archival refs.
- **func_800557B8 MATCHED (246 total)** — the uncommitted `&&`-shape draft
  left in moji.c was 2 insns long / wrong layout. Fixed via scratch bytecmp in
  3 iterations: s16 local (frame 0x18→0x20 + lh/addu copy), `||` condition
  with expire as then-arm, direct returns (ret local cost a $a1→$v0 copy).
  Clean rebuild + sha1 OK + mutation test (cnt-2 broke it, restore OK).
  New idioms recorded in LESSONS.md 2026-07-11.
- progress.md census header refreshed (was stale at 232): 246 matched /
  238 active stubs / 484; moji 94/145.

## 2026-07-11 — Fable (cont.): salvaged stopped agents; sound pair landed (248)
- The user inadvertently stopped the three relaunched background agents.
  Salvage sweep: harvest worktree had func_80019918 + func_800199A4
  BYTECMP-VERIFIED (the parked jump-canon family, cracked via the switch
  recipe from LESSONS 2026-07-06). Cherry-picked to dev, clean rebuild,
  sha1 OK, mutation test (0x39→0x3A broke it, restore OK) → 248 matched.
- BB4C agent left an uncommitted in-flight iteration (u32 xm mask locals +
  D_800C5608 extern) — saved to notes/wip/bb4c_inflight_xm.diff for the
  relaunched agent to evaluate; 53B40 agent's worktree was unchanged.
- All three agents relaunched with updated prompts.

## 2026-07-11 — Fable (gate): 4 player key-test siblings landed (252)
- Harvest agent (resumed, context intact): func_8004030C, func_800404FC,
  func_80040574, func_80040630 BYTECMP-VERIFIED in its worktree; cherry-picked
  to dev, clean rebuild, sha1 OK, mutation test (x16B bit 2→4 broke it,
  restore OK). PL_WORK gains x130/x132 (key masks), x16B (flag byte),
  x44A/x44B (hold counter) — all layout-neutral padding carves.

## 2026-07-11 — Fable (gate): 6 more player functions landed (258)
- Harvest batch 2: func_800403D4/40468 (dual-mask k-select), weapon stat-table
  family func_80040FA8/41044/410D4/41158 (D_80089A84 stride-12, goto-shared
  return-1). Cherry-picked, clean rebuild, sha1 OK, mutation test (0xC0→0xC1
  broke it, restore OK). PL_WORK gains x12C/x12E/x13A/x13C/x100/x171/x174.

## 2026-07-12 — Opus (resume/salvage): moji func_80058CC8 landed (259)
- Recovered a stranded bytecmp-verified match from the dead codex-moji agent's
  branch (dc5aee9): func_80058CC8 = script-call-stack push (m->stack[m->xBE++]
  = script+3) then Sce_flag_test-gated entry select (D_8008BAA4[k] vs D_8008CE58).
  Cherry-picked onto dev, CLEAN rebuild, sha1 OK, mutation test (xBE++ → xBE+=2
  broke it, restore OK). Needs externs D_8008BAA4[]/D_8008CE58 + sce.h include.
- Also salvaged the two giants' advanced scratch drafts from the dead agents'
  branches into notes/wip/ (BB4C 22266e1, 53B40 eba6103); mismatch counts
  re-verified by bytecmp (numbers recorded in the salvage commit, not trusting
  the stale analysis-doc headers).

## 2026-07-12 — Opus (background streams): 8 moji matches + giant forensics (267)
- bg/moji-harvest (background Opus, worktree): 8 moji.c matches landed via
  foreground re-gate (moji 95→103, stubs 50→42): func_8005A598, func_80058C28,
  func_80057744, func_8005958C+func_80059660 (twins), func_80058DEC,
  func_800548C4, func_800563A8. Clean rebuild sha1 OK + mutation test on
  func_800563A8. 2 attempted+reverted (func_80057924, func_80057C2C — cc1
  scheduling). LESSON: a normalizing asm-differ can show CLEAN while the hash
  FAILS — gate on the byte check only (now in COMPILER_IDIOMS §0).
- bg/53b40-reconcile (done): 426 is a CASCADE not a regression; $fp theory
  disproven; single $s7-swap fault (movable-ranking). Landed to dev.
- bg/bb4c-endgame (done): 319 reframed — NOT a regalloc fight; 2-word surplus in
  the addressing-crux + jump-canonicalization parked genera. Landed to dev.
- Built notes/COMPILER_IDIOMS.md (portable gcc-2.7.2 cookbook) +
  RESEARCH_BRIEF_gcc272_idioms.md. Fable launched on the 53B40 constant-CSE
  blocker (bg/53b40-fable, in flight).
