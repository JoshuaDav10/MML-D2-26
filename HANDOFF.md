# HANDOFF — MML Decomp session state (281 matched — authoritative)

> **READ THE DENOMINATOR.** 281 / **484** C-mapped functions = 58.1% is the number this
> project has always quoted. It is NOT mission completion. 635 more game functions
> (56,999 instructions) are still linked as raw asm and were never split into a C TU.
> Against the actual executable: **281 / 1119 = 25.1% by function, ~7.1% by volume.**
> Quote both. (2026-07-26 adversarial audit; `tools/check_docs.sh` now enforces it.)

> **53B40 giant push (2026-07-25 Opus) — VERDICT REACHED, count unchanged at 276.**
> Draft improved **75 -> 70** positional rows (495/495 parity) and the *original* wall is
> SOLVED: frame fossil closed (prologue byte-identical), and the callee-saved allocation
> now matches the target on **7 of 8 registers**. But a deeper blocker was identified and
> proven: the target's `addu $s1,$v0,$zero` at the render site is a **reload live-range
> split**, and the 8 reserved-but-untouched frame bytes (0x20-0x27) are the *same* reload
> episode. Proof by reading `local-alloc.c:combine_regs` — a reg-to-reg copy is deleted iff
> the source pseudo is dead, and `cse` (running earlier) always makes it dead for a plain
> `b = a`. Confirmed empirically twice. **Scoped claim:** no *spelling of that assignment*
> can produce it; a structurally different whole function still might.
> **~20 levers tried and closed — teeth 10-15 in `notes/wip/GCC_SOURCE_PROGRESS.md`. DO NOT
> RETRY ANY OF THEM.** Durable rules extracted to LESSONS.md (stack slots follow declaration
> order; constants are rematerialized not spilled — REG_EQUIV; the combine_regs copy rule).
> Two bases banked: `53b40_draft_permbest.c` (70 rows, uses a `volatile` as padding that
> buys the correct 0x50 frame) and `53b40_draft_honest.c` (96 rows, semantically clean,
> frame 0x48). **RECOMMENDATION: stop source-form work on 53B40.** Remaining options are
> (1) study reload1.c inheritance, (2) accept the near-match, (3) accept-as-asm. Redirect to
> BB4C or the small-function lanes, where levers still work.

> **53B40 session (2026-07-19/20 Fable, max-effort single-target):** count
> UNCHANGED at **274** (audit_count.sh verified at start AND after the
> macro.inc fix; 53B40 NOT yet landed — no fake progress). But the function
> moved from 426 mismatched words to **40/495 structurally-unaligned**, with
> every lever banked in `notes/wip/53b40_draft.c` + GCC_SOURCE_PROGRESS.md
> teeth 6-9. Highlights: THE $s7/$fp swap is SOLVED (the intruder was the
> hoisted &D_800BB9C8 — killed by a per-iteration `last` local); preheader
> is EXACT (tst/setflag/fff body-top locals); TWO semantic bugs in all prior
> drafts found (inner-loop glyph arms inverted for op<0x84; D494 result is
> discarded, not stored); divisions are MOJI_TASK* pointer subtraction
> (exact-div 0x1A1F58D1 mflo, NOT signed /0xC4); DRAWCTX is
> `{pad[0x70]; u32 x70[3]; u32 x7C;}` indexed x3D (dc block-locals per site);
> SetDrawMode takes 5 args. **TOOLCHAIN BUG FIXED**: include/macro.inc's li
> macro dropped low halves of big constants (first hit: 0x86186187) — fixed +
> full audit + 205/205 overlays re-verified. decomp-permuter re-seeded (base
> score 2595→sub-1600 and falling; earlier 9745 stall was a lever-less base +
> a compile.sh cwd bug breaking gprel's census — both fixed, see
> PERMUTER_GUIDE.md). A detached permuter (-j8 --stop-on-zero) may still be
> running in tools/decomp-permuter/; harvest `mml_53B40/output-*` (lowest
> score), verify via `CPP=cpp tools/bytecmp.sh func_80053B40 <source.c>`,
> land via the prepared tree-adapted TU recipe in GCC_SOURCE_PROGRESS.md
> tooth 9 (needs (u8)x3F/(s8)x71 casts + DRAWCTX+externs in moji.c). Landing
> gate: audit_count.sh must print 275.

> **[SUPERSEDED — see the title line for the current count]** Harvest update
> (dated 2026-07-14 in-file; the count-scandal it describes is actually 2026-07-19 per
> git 959c5ed). As-written-then [SUPERSEDED]: AUTHORITATIVE count = **276 matched**
> (`census.py --matched` after `rm -rf build`; hash OK + raw `cmp` identical).
> Genuine new this session: **+3** — func_80016434 (game), func_8001D394 (cd), func_8001A63C (sound),
> both real stubs I wrote C for and mutation-tested. Mission: 276/484 = **57.0%**
> by function count, ~17.6% by volume.
> ⚠️ **COUNT-CORRECTION / GOTCHA:** I earlier over-claimed 280 by "un-gating"
> func_800155A4/8005EC80 (+3 more). PHANTOM: game.c AND sub_scrn.c carry
> `#define ACCEPT_REORDERING_BULLSHIT` (since commit 957191c) so ALL their
> `#ifndef ACCEPT_REORDERING_BULLSHIT` guards ALREADY take the body branch — those
> bodies were matching all along; un-gating is a no-op. The naive `grep
> INCLUDE_ASM` stub census can't see the define and overcounts. **Trust
> `census.py --matched` only.** The prior HANDOFF "276" was also an overcount. Overlays CONFIRMED 205/205 done
> (expedition complete; the "19 missing" note was stale). Banked the held-pointer
> idiom (LESSONS) — the lever for the BB4C addressing-crux. 3 near-matches
> (logic-solved, 1–2 insns off on cc1 scheduling) queued for the permuter in
> `notes/wip/HARVEST_NEARMATCHES.md`. DISCOVERY: remaining sub-60 fns split into
> straight-line stores (fast hand-match) vs branch/loop bodies (scheduling-hard →
> route to permuter). 53B40 permuter re-seeded with the levers (stalled at 9745,
> stopped — needs the -dg $s7 hand-fix, then permuter last-mile).

> **Latest (2026-07-14 Opus+Fable session):** GCC-source expedition Phase 1
> **ANSWERED** — the 53B40/BB4C loop-hoist genus is **C-REACHABLE, not a toolchain
> wall**. Fable read `move_movables` in the FSF gcc-2.7.2 source, cross-checked it
> against the real cc1-27 `-dL` dump (no SN divergence in the loop pass), extracted
> the exact hoist formula (now in `COMPILER_IDIOMS.md §4`), and compile-verified two
> C levers that stop the bad constant-hoist. Full trail: `notes/wip/GCC_SOURCE_PROGRESS.md`
> (teeth 1–4). **Next on 53B40:** apply the 3 verified edits to
> `tools/decomp-permuter/mml_53B40/base.c` (reused-`pp` local for 0x1F800070;
> pre-loop `setflag=0x40000000` local; extern `D_800BB9C8`=Moji_work+0x310) + settle
> `$fp`→`$s7`, then permute/diff to a byte match. Match NOT yet hash-verified.
> Also: cc1-27 is SN-patched `2.7.2.SN32.3.7` (FSF source = mechanism baseline only;
> ground truth = the `-dL` dump). 276 matched (unchanged that session). [SUPERSEDED]
>
> **Prior (2026-07-12 Opus session) [SUPERSEDED — 5 of these were phantom un-gates,
> see the 2026-07-19 correction]:** [SUPERSEDED] 276 matched (sub_scrn sort vein +4 →
> func_8001F6C4 scene +1). decomp-permuter FIXED & working (`pip install toml`
> + patchasm build/test.s guard) — see `notes/PERMUTER_GUIDE.md`. New
> `notes/PARTS_LIST.md` (tooling inventory). Expedition plan:
> `notes/wip/GCC_SOURCE_EXPEDITION.md`. A transcript-archive + two-book textbook
> lives in the separate `MML-decomp-journal` repo (private, GitHub JoshuaDav10).

> **Read this first.** You are (probably) Claude Fable in Claude Code, resuming a
> Mega Man Legends (PSX) matching decompilation. This file + `CLAUDE.md`
> (auto-loaded) + `notes/LESSONS.md` are your complete context. The older
> `FABLE_HANDOFF.md` / `MML_DECOMP_CHECKPOINT.md` are historical (Stage 0/1);
> this file supersedes their "what to do next" sections.
>
> **Reference corpus**: `notes/DOCUMENT_INDEX.md` maps an external doc corpus
> at `notes/reference/` (symlink): MIPS R3000 manual, nocash PSXSPX hardware
> spec, PSYQ SDK docs, PSX assembly notes, MML-specific notes. Index-driven
> lookup only — pull the file/line-range you need, never whole docs into
> context. (Prior sessions missed this; don't repeat that.)

## State (all verified, none of this is aspirational)

- **Branch: `dev`** — ALL commits go here. NEVER commit to or merge `main`;
  only the user promotes to main. Push target: `origin` (JoshuaDav10/MML-D2-26).
- **The build matches byte-for-byte**.
  `make CPP=cpp check_rock_neo_only` prints OK; also verifiable with
  `cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe` (raw byte compare).
- **Matched: 281** (~20% of instruction volume; 203 active stubs, 484 total).
  AUTHORITATIVE source is `tools/audit_count.sh` (clean rebuild + hash + raw cmp +
  census) — never a grep, never a stale note here. Run `tools/check_docs.sh` to verify
  this file has not drifted. See `progress.md`.
  - 2026-07-12 resume: salvaged stranded work from the dead agent branches —
    moji func_80058CC8 (codex-moji dc5aee9) landed + hash-gated (259); both
    giants' advanced scratch drafts pulled into notes/wip (see giant status).
    Three agent-* worktrees pruned; branches kept as archives.

## THREE BACKGROUND STREAMS IN FLIGHT (launched 2026-07-12) — check first on resume

Background agents die with the session, but each is worktree-isolated, commits
frequently to a FIXED branch, and maintains a `_PROGRESS.md` resume doc. If this
session ended mid-flight, DON'T restart them blind — read each branch + doc:

| Branch | Task | Baseline → target | Resume doc / draft |
|---|---|---|---|
| `bg/bb4c-endgame` | BB4C reg/sched endgame (func_8001BB4C, cd.c) | 319 → 0 hard mismatches | `notes/wip/BB4C_PROGRESS.md`, `bb4c_draft_v2.c` |
| `bg/moji-harvest` | small moji.c stub harvest (50 left) | +N matches | `notes/wip/MOJI_HARVEST_PROGRESS.md` |
| `bg/53b40-reconcile` | reconcile func_80053B40 426-mismatch, push→0 | explain 426, then →0 | `notes/wip/53B40_PROGRESS.md`, `53b40_draft.c` |

Rules the agents follow: worktree-isolated, NEVER push, NEVER touch dev/main,
stay in their file lane. Landing to dev is FOREGROUND-only: cherry-pick a
bytecmp/hash-clean result, clean-rebuild hash-gate + mutation test, then commit.
Verify BB4C/53B40 with `CPP=cpp tools/bytecmp.sh <func> <draft>`. To continue a
stream yourself: `git worktree add /tmp/<name> <branch>` and iterate there, or
cherry-pick its landable commits.
- Agent worktrees from the 07-06 sessions were audited + pruned 2026-07-11:
  ALL content was already salvaged into dev (BB4C baseline = 806/809 words
  @ 461 hard mismatches in notes/wip/bb4c_draft_v2.c; cbase-local iteration
  already tried, net-neutral). Branches `worktree-agent-*` kept as archives.
- **Trust matches only after a CLEAN rebuild** (`touch src/rock_neo/*.c &&
  rm -f build/rock_neo.elf` before `make`). This session found a prior
  "match" (func_800605DC) that never actually compiled — the stale stub
  object stayed linked and the hash passed on it. Clean-rebuild + the
  liveness mutation test are the only guards. See LESSONS.md 2026-07-05
  (Opus) and progress.md "Last verified build".
- **The pipeline can emit $gp-relative (sdata) access** via
  `tools/gprel.py` (between maspsx and patchasm); as of 2026-07-05 it also
  gp-rewrites refs to small `.comm` symbols (tentative definitions such as
  moji.c's `u8 Moji_flag[8];` — the COMMON is intentional, splat carves the
  symbol out of the extracted data). Read LESSONS.md §2 before declaring
  any extern.
- Overlays: MEASURED 2026-07-05 — 186/205 CDDATA/DAT .BIN files already
  build byte-identical; 19 lack configs entirely. The old "ST** don't
  link" note was stale. Plan: `notes/OVERLAY_EXPEDITION.md` (delegable).

## What was accomplished in the 2026-07-06 PM session (most recent)

Resumed from a mid-flight context transfer (3 workstreams). Landed **11 moji.c
script-opcode matches**, all clean-rebuilt + full-binary sha1 OK + committed to
dev (232→244 counting the prior session's func_80054A04): func_8005497C,
func_800564C8, func_80056558, func_80056610, func_800566CC, func_80055438,
func_80057184, func_80055A78, func_80055B14, func_8005721C, func_800545C8.

- **Reusable CALL-opcode template** drove most of them: `base = m->x44; if
  (base) MojiTaskExec(no, base, op); else MojiTaskExec(no, D_8008CACC[idx],
  0xFF); script += 3`. Variants remap the op index through a table (`D_800BE2F8`
  or a u8 array inside `Player_work` at +0x450/+0x454) and fold an op bias (−1)
  into the null-arm table base (`D_8008CAC8` = `D_8008CACC−4`).
- **Idioms pinned** (also in activity.md / LESSONS-worthy): `(s8)m->x71` forces
  the signed `lb`; a remap table *inside* `Player_work` needs Game_work-style
  struct-member access to emit per-site `%hi/%lo(Player_work+off)` instead of a
  hoisted base pointer; `m->x48 = m->script2 = …` store order (script2 first)
  is load-bearing (func_80055438); explicit `& 0xFFFF` into an `int` (not a
  `u16` var) pins the mask at the store not the call (func_80055B14); if/else
  arm order decides fall-through vs branched block (func_800545C8 `(s16)x4<=0`).
- **PARKED near-match: func_80056778** — cc1 hoists the join-block flags const
  `lui 0x402000` into the `bnez` delay slot where the original keeps a `nop`.
  Same delay-slot/const-hoist genus as the giants; left INCLUDE_ASM. Draft +
  diagnosis in the git history of this session (not committed to the tree).
- **Both background giants advanced and salvaged into `notes/wip/`** (main tree
  byte-identical, moji.c untouched by them): see the giant-status section below.

## What was accomplished in the 2026-07-05 DAY session

Batches 10-13, all clean-rebuilt + hash-gated + mutation-tested + pushed to
dev (commits 615f06d..b5762e0). 22 new matches (187 → 209), ~12.6% volume.

1. **Batch 10 (4)**: finished the overnight in-flight batch — scene
   func_8001DE84 (NEW IDIOM: goto-shared return label defeats cc1's
   setcc/sltu tail collapse) + func_8001F740, main func_8001319C
   (left-assoc pointer arithmetic pins addu order), debug func_800629F0
   (NEW IDIOM: neighbor-symbol STORES — `((u16*)&Scene_work)[-26]` — pin a
   fn-table load AND defeat arg anchor-CSE).
2. **Batch 11 (7)**: moji func_800594CC/54798/55344/55C1C, player
   func_80041E90, scene func_8001E390/F580. PARKED func_80041EF4 with an
   RTL-verified open problem: cc1 elides the original's andi 0xFFFF pair
   (combine's nonzero_bits proves every source shape ≤0xFFFF).
3. **Batch 12 (6)**: cd CdControl/CdControlB retry twins, moji
   func_80057A24/58788 (magic-multiply slot index), sound func_8001A0A8,
   scene func_8001E460. **CAUGHT A FALSE PASS**: cd.c had a stale
   conflicting declaration; the TU failed to compile, the pipeline wrote a
   PARTIAL .o, and check printed OK on the stale exe — a build-runner
   subagent even reported it as a pass. Error-grep make output YOURSELF.
4. **Batch 13 (5)**: player func_8003F224/40224 (goto-return trick
   generalized; `and` operand order follows source order — new knob),
   scene func_8001D8C0, moji func_80054ADC/55BB0 (D_8008AAC0 table pair).
5. PARKED with findings this session: func_80041EF4 (andi elision),
   func_80019918 (FC50 cross-jump family), func_8001A6DC + func_80013578
   (register-birth / scheduling-slot genus).

## What was accomplished in the 2026-07-05 OVERNIGHT autonomous session

Batches 4-8b, all clean-rebuilt + hash-gated + mutation-tested + pushed to dev
(commits 2791c0c..c3c6807). 46 new matches (136 → 182), crossing 10% volume.

1. **Batch 4 (3)**: main func_80012E10/func_80012FEC, sound func_80019FB4.
   New idiom: same symbol-indexed slot read twice → cc1 CSEs the address;
   alias the second read through a NEIGHBOR symbol constant-index
   (`((u8 **)D_801F8114)[(n << 5) + 2]`) — distinct RTL, identical relocs.
2. **Batch 5 (5)**: player func_80040380/40710/40AEC, sound Sound_call,
   cd func_8001D7E4. func_80042044 PARKED: register-birth-order mismatch
   (k must land in $v0, ret in $t0; pass-through (pl,arg1,arg2) →
   func_80042154 proven — gets ret→$t0 — but k still steals $a0).
3. **Batch 6 (4)**: scene func_8001FD3C/FD90, sound Sound_call2 (q++ pins
   tail store order — LESSONS), sub_scrn func_8005EC34. PARKED as a family:
   func_8001FC50/FDE4 + already-known FCA4 (cc1 jump-canonicalization:
   cross-jumped constant legs / inverted beq→store; no C shape found).
4. **Batch 7 (9)**: un-gated the pre-patchasm ACCEPT_REORDERING_BULLSHIT
   drafts in sub_scrn.c (sorts, cancel/shift checks, rb_parts_set, EC80,
   60DB8). Only real fix: MojiTaskExec's -1 arg needs a K&R declaration
   (ANSI u8 prototype truncated it at the call site).
5. **Batch 8 (1)**: func_800600CC via raw-address Moji_flag derefs (gp-vs-
   lui is per-FUNCTION here) + tools/maspx patch: bare-CONSTANT-address
   loads now get load-delay nops like bare-symbol loads.
6. **Batch 8b (consistency)**: the K&R decl conflicted with the ANSI
   definition — moji.c had been failing to compile with the hash passing on
   a stale object. Definition now K&R (byte-identical .text); from-scratch
   `rm -rf build` rebuild: zero errors, hash OK. LESSON: grep make output
   for errors before trusting the hash.
   Sub_screen_rb_parts_calc's USE_OG_COMPILER draft tried: 6 insns short,
   re-gated with note.

## What was accomplished in the 2026-07-05 Fable MojiTaskExec session

1. **MojiTaskExec MATCHED** (136 total, ~6.3% volume) — the 133-line
   script-VM task-slot initializer, biggest single match yet. Clean rebuild
   → hash OK → cmp byte-identical → mutation test (x3D 3→4 failed the check;
   restored, OK). The prep notes from last session were accurate; final
   signature is `s32 MojiTaskExec(s32 no, u8 *script_base, u8 op)` — note
   op is **u8**, and that is register-allocation-load-bearing (below).
2. **New matching technique — allocno-priority forensics via cc1 -dl/-dg**:
   the last blocker was `no` vs the 0x40000 mask constant mirrored across
   $s2/$s3. That's gcc 2.7 global.c priority `floor_log2(refs)*refs/live_len`
   tying EXACTLY (12/88 == 3/22); ties go to the lower pseudo (params win).
   The u8 op param's QImode entry copy adds 1 insn to no's live length and
   breaks the tie the original way. Dumps + mechanism in LESSONS.md
   "2026-07-05 (Fable, MojiTaskExec)". Statement-split perturbations do NOT
   work (cse folds them before flow counts refs/lengths).
3. **moji.h struct filled in**: u16 x4/x6/x8/xA/xC/xE row, u16 x38, u8 x3A,
   x3E s8→u8, u8* x48, u8 x73, u8 x7D/x7E/x7F, x78 s8→u8 (asm stores +0x80
   via addiu 0x80 — signed s8 would emit addiu -128, different bytes).
   `extern MOJI_TASK Moji_work[]` (0x800BB6B8, stride 0xC4) now in moji.h;
   slot 4 fields written as `Moji_work[4].flags`/`.xC2` reloc to the original
   D_800BB9C8/D_800BBA8A bytes. All prior moji matches revalidated by the
   clean-rebuild hash after the struct changes.
4. Whole function drafted in the scratch-TU pipeline (cpp|cc1|maspsx|gprel|
   patchasm + normalizing stream diff) — the tree got exactly one edit and
   matched on the first in-tree build.

## Strategy for the medium/large phase (agreed with user 2026-07-05)

The quick-win era is ending. Remaining 266 stubs by size: 115 under 60
insns, 60 at 60-99, 67 at 100-199, 33 at 200+. The plan:

1. **Finish the sub-60 tier first** (~115 fns) — still cheap, still feeds
   struct knowledge. Batch cadence as now.
2. **60-199 tier: hunt FAMILIES, not sizes** — pick functions sharing
   vocabulary (moji table dispatchers, player state siblings, cd queue
   users). Use tools/m2ctx.py + m2c for rough drafts on anything >100
   insns; drafts are never trusted, only iterated against the byte diff.
3. **200+ tier (33 fns): one per session, moji first** — MOJI_TASK is the
   best-typed struct and moji owns 5 of the 12 biggest (func_80053B40,
   func_80059E74, func_8005ACA8, func_80057FF8, func_80056820, each
   340-520 insns). Each is a MojiTaskExec-scale focused effort.
   **Biggest overall: cd/func_8001BB4C (~860 insns)** — the CD
   streaming/loader state machine; save it for late, after its smaller
   cd siblings firm up the command/queue structs.
4. Parked families (jump-canonicalization, register-birth, andi-elision)
   stay parked until a future match reveals the missing shape.

## Overlay expedition (independent track, delegable)

REMEASURED 2026-07-05: the old "overlays don't link" claim was stale.
186/205 CDDATA/DAT .BIN files already rebuild byte-identical; the real
job is writing configs for the 19 missing files (fonts/maps/screens +
six ST/EXIT code overlays) and adding a 205-file check target. Full
phased plan, hard rules, and an unfakeable verification protocol:
**`notes/OVERLAY_EXPEDITION.md`** — written for delegation to a weaker
agent on the `overlay-expedition` branch.

## Giant-function status (two multi-session decomps in flight)

- **func_8001BB4C (cd.c, 845 insns)** — the CD loader. Structure fully solved:
  805–806/809 words, all control flow / CD_WORK union / three-pointer addressing
  correct. Blocked at **319 hard mismatches / 811 words** (2026-07-12 confirmed).
  NOT a register/scheduling fight — bg/bb4c-endgame DISPROVED that via -dg: all 7
  callee-saved regs ($s0-$s6, pseudos 72-78 → hw 16-22) held exactly as the
  original. The 319 is a **2-word instruction surplus** cascading every downstream
  branch displacement + reg number. Both surplus words are the PROJECT'S PARKED
  GENERA, not new problems:
    (1) ADDRESSING-CRUX (states 2 & 4): draft emits `r->x`/`r->y` as an absolute
        store (`sh $v1, D_800...`); reference reuses the held `$s1`=r pointer
        (`addu $a0,$s1,$zero; sh $v1,0($a0)`). cc1 won't reuse the pointer.
    (2) JUMP-CANONICALIZATION (state 1 tail): reference gets a delay-slot store by
        holding `&D_800C5608` in a base reg (`bnez tail; sw` in slot; `j tail0`);
        draft stores first then `beqz` + extra `j` — cc1 constant-folds the
        pointer form back to absolute. Same genus as scene's parked FC50 family.
  Count-neutral fidelity fixes already landed in the draft (case-4 r->y absolute,
  case-2/4 branch senses → reference goto-out `bne`). DEFERRED: an unsigned-mask
  srl fix that is proven-required but spikes the count to 419 until word-parity is
  reached — apply only AFTER the 2-word surplus closes. Full aligner method + the
  two precise blockers in notes/wip/BB4C_PROGRESS.md; branch bg/bb4c-endgame
  (commit 0b41228). Earlier history: 510→461 ($s7 hoist fix)→319 (case-0 temp-ptr).
  Scratch draft: `notes/wip/bb4c_draft_v2.c`; forensics:
  `notes/wip/BB4C_ANALYSIS.md`. First hard mismatch is a branch displacement
  from a small instruction-count deficit; verify with
  `CPP=cpp tools/bytecmp.sh func_8001BB4C notes/wip/bb4c_draft_v2.c`. Do NOT land
  into cd.c until 0 hard mismatches + clean rebuild. **No live agent — the
  461-mismatch draft in notes/wip/bb4c_draft_v2.c is the current baseline; this
  is the single best target for a fresh Fable session (structure done, only a
  register/scheduling finish remains).**
- **func_80053B40 (moji.c, 522 insns)** — text/font renderer. Body logic
  verified byte-exact (the /0x15 glyph-cell divide-magic + *0xC UV block).
  Blocked on **callee-saved allocation**: cc1's `loop.c move_movables` ranks the
  `0x1F800070` scratchpad address (4 in-loop uses) above the `0x40000000`
  constant (2 uses) and steals its saved-reg slot; the original keeps
  `0x1F800070` inline (`lui/ori` at all 4 sites). Next-session plan (make the 3
  Map_prim_ptr accesses non-CSE-mergeable / -dg ranking-demotion) is in
  `notes/wip/53B40_ANALYSIS.md`; scratch draft `notes/wip/53b40_draft.c`. moji.h
  already exposes xB8/xBA for it. Land via the foreground (moji.c ownership).
  - 2026-07-12 RECONCILED (bg/53b40-reconcile, notes/wip/53B40_PROGRESS.md):
    the 426 is a CASCADE, not a regression — the eba6103 salvage actually
    IMPROVED it (455→426 via the Moji_flag u8[8]+`*(u32*)` gp idiom). 426 is the
    correct baseline; keep the current draft. The earlier "$fp / 6th saved reg"
    theory was DISPROVEN via -dg: both draft and reference save exactly $s0-$s7
    +$ra. The whole 426 is ONE fault — a 1-for-1 swap of which constant holds
    $s7: draft hoists 0x1F800070 into $s4 (wrong) and leaves 0x40000000 as inline
    `li` ×2; reference hoists 0x40000000 into $s7 and keeps 0x1F800070 as fresh
    `lui/ori` at all 4 sites. The frame 0x40-vs-0x50 delta is downstream spill
    slots, not an extra reg. Fix the swap → the cascade (frame + every stack
    offset) collapses at once.
    BLOCKER (loop.c move_movables, -dL): 0x1F800070's 3 in-loop uses CSE-merge
    into one savings-3 movable that clears the hoist threshold (3); the original
    never forms that movable (kept inline), so its threshold stays 2 and the
    2-use 0x40000000 hoists into $s7 instead. 6 source forms tried to break the
    CSE-merge of the 3 OTPTR (0x1F800070) address computations (plain/volatile/
    memory-clobber asm, distinct ptr types, volatile locals, same-addr-diff-mode)
    — all failed; gcc-2.7 merges an identical constant address into one pseudo at
    -O2. PLATEAUED at 426. **This is the prime Fable-escalation candidate** (a
    tightly-scoped movable-ranking problem; same genus as BB4C). Full forensics +
    next hypotheses in notes/wip/53B40_PROGRESS.md; branch bg/53b40-reconcile.

## Where to pick up next (day session's view)

0. **Continue the moji CALL-opcode harvest** — the template above cracks these
   fast. Remaining sibling stubs in the +0x56xxx / +0x57xxx range that call
   MojiTaskExec or advance script/script2. Read the just-matched func_800564C8/
   56558/56610/566CC/55438/55A78/57184 as templates before drafting.
1. Smallest remaining stubs are now ≥35 insns: get the list with the
   preprocessor-based stub census (grep `.include` after cpp — a NAIVE
   grep of INCLUDE_ASM overcounts ifdef'd-out lines):
   `for f in src/rock_neo/*.c; do cpp -Iinclude <defines> $f | grep -o
   'nonmatchings/...' ; done` then wc -l the .s files.
2. PARKED with findings (don't re-grind blind): func_80042044 +
   func_8001A6DC + func_80013578 (register-birth/scheduling genus),
   func_800199A4 + func_80019918 — CRACKED 2026-07-11 by a harvest agent
   with the switch recipe (goto-chain + range-collapse), landed + hash-gated;
   cd func_8001CF98 — MATCHED 2026-07-06 (in-body local assignment resolves
   the load-duplication + index-CSE conflict; see LESSONS);
   cd func_8001BB4C (THE 845-insn CD loader — full analysis + m2c draft +
   VERIFIED field widths in notes/wip/BB4C_ANALYSIS.md; multi-session, tables
   carve like scene's. Command entry = all s32 words + data[0x100]; RECT all
   s16; the work struct @D_800C5604 is a common header + PER-COMMAND-TYPE UNION
   from +0x14 on — SAME address is byte in type-7, word in type-8. Union arms
   NOW FULLY ENUMERATED per inner state (see "UNION ARM ENUMERATION" in the
   analysis doc): 5618 is word/signed-word/u8 across states 4/6/8; 5620 is the
   u8 gate but read wide in state 2. Recommended C: flat s32 CD_WORK struct with
   (u8)/(u8*) casts at the byte sites, formal union as fallback. NEXT: declare
   CMD_ENT + CD_WORK in cd.h, translate the m2c draft, bytecmp-iterate),
   func_80041EF4
   (andi elision — RTL
   verified), Sub_screen_rb_parts_calc (draft 6 insns short).
   JUMP-TABLE INFRA COMPLETE + multi-table proven 2026-07-06: scene
   func_8001FCE4/F8DC/F9AC all matched; all 3 scene tables emitted by
   scene.c.o(.rodata) at 0x800108C0 (function order, contiguous). Carve
   recipe + multi-table CONTIGUITY constraint + the case-0 || ternary
   register trick are in LESSONS.md. splat yamls carry [0x10C0, .rodata,
   scene]; asm/ + rock_neo.ld edits are LOCAL-ONLY (gitignored). 25 more
   table functions reachable, incl. cd/func_8001BB4C (~845 insns, 3 tables).
3. Scratch-TU tooling from this session (recreate if scratchpad is gone):
   tryfn.sh (pipeline to .o + objdump) and bytecmp.sh (assembles a draft,
   extracts the function words from objdump, byte-compares against the
   splat .s comment words — flags relocs for eyeballing). bytecmp is the
   workhorse: 0 hard mismatches + sane relocs ⇒ land it in the tree.
   Both derive from Makefile line 266's pipeline.
4. When a callee-saved register mirror resists the usual knobs, go straight
   to the -dl dump arithmetic (LESSONS.md) instead of blind permutation.
5. Before defining a former stub, grep the TU for pre-existing guessed
   declarations of it (batch 12's false pass came from one).

## What was accomplished in the 2026-07-05 Fable session (audit+harvest)

1. **Audited the Opus session** (independent from-scratch `rm -rf build`
   rebuild → `cmp` byte-identical; fresh mutation tests; recount) — all of
   Opus's claims held, including the func_800605DC stale-object diagnosis.
2. **12 more matches (135 total)** in two clean-rebuilt batches:
   player func_800405F4; sound func_80019A34/func_8001A1FC/func_8001A238;
   moji func_800553A8/553F0/555F4/57BB4/58740; sub_scrn Map_screen_init;
   cd func_8001CB30; main func_80012424.
3. **One function re-stubbed as an open problem**: scene func_8001FCA4 — an
   ==5 leg inside a ternary chain; cc1 canonicalizes ==/!= ternaries so the
   un-inverted beq form of the original is unreachable from every form tried
   (~12). Full notes in LESSONS.md "OPEN PROBLEM" and activity.md. Don't
   re-grind it blind; revisit when another ==-in-chain function matches.
4. **New idioms in LESSONS.md**: separate-offset-variable loops keep the
   symbol-indexed lui/$at load form (vs strength-reduced pointer march);
   `((u8*)sym)[k]` selects lbu per-site without a second extern; raw
   (unmasked) param compares mean an int-typed param in the source.
5. New typed fields: MOJI_TASK.x44 (u8* jump-offset table base) + xC2
   (u16 index); PL_WORK.x128/x12A (key masks).

## What was accomplished in the 2026-07-05 Opus session

1. **6 more matches (123 total)**, all clean-rebuild + hash verified:
   Code800133D8 func_800133D8, cd func_8001B858 (fn-table dispatch),
   moji func_80055C80 / func_800576C4 (script2 stack push) / func_80057B70,
   sound func_800199F8. Next stubs start at 23 asm lines.
2. **Caught + fixed a stale-object fake match**: func_800605DC (claimed last
   session) never compiled (a `void` vs sub_scrn.h `unknown_t` conflict); the
   stale stub object had kept the hash green. Forced clean recompile exposed
   it; header prototype fixed; now genuinely verified. This is why the state
   line above stresses clean rebuilds.
3. **New idiom**: a callee's declared return type steers the CALLER's register
   allocation even when the result is unused (func_80057B70 — declaring
   func_80043294 `s32` reserves $v0 across the call). See LESSONS.md.
4. **One symbol, two per-TU views**: Code800133D8_work is an s16-x0 struct in
   game.c but a local `s32[3]` array in Code800133D8.c (three-word zero).

## What was accomplished in the OVERNIGHT 2026-07-05 session

1. **18 more matches (117 total)** in three hash-verified batches across
   moji/scene/cd/main/sound/sub_scrn. All ≤22-line stubs are now exhausted
   up through ~22 asm lines; next targets start at 23 lines
   (`wc -l` the stub list per the inner loop below).
2. **gprel.py `.comm` support** — moji.c's `u8 Moji_flag[8];` tentative
   definition (COMMON on purpose; splat carved 0x80098A58 out of the data)
   now gets its refs gp-rewritten like census-approved externs. Unblocked
   func_80057DB8 and any future Moji_flag-touching moji function.
3. **New idioms in LESSONS.md "2026-07-05 (overnight)"**: compute-into-locals
   at the load site; alias-forcing read via `&D_80098199 - 1`; ternary
   chains land in $v0 (unlike single ternaries); call-result variable reuse;
   parenthesization steering addu order; and "a 4-byte data shift can be a
   short FUNCTION, not a COMMON leak — check the function diff first".
4. **The scratch-TU brute-force loop is now the workhorse**: pipe variants
   through `cpp|cc1|maspsx` directly (see the perm scripts pattern in this
   session's history / LESSONS night-additions). Most functions this session
   were matched in the scratchpad BEFORE touching the tree.
5. New typed fields: MOJI_TASK.xBC (u16, cleared by several opcode
   handlers), MOJI_TASK.x7C (u8 index into the D_8008AB08 fn table).

## What was accomplished in the LATE-NIGHT 2026-07-04 session

1. **17 more matches (99 total)** across moji/player/scene/cd/main/sound.
   New typed knowledge: SCENE_WORK struct (include/rock_neo/scene.h),
   PL_WORK key-mask fields (x11C/x11E vs x138/x13E) + x9/xA state fields,
   MOJI_TASK second call stack (stack2[8] @0x4C, u16 xC0).
2. **Sce_flag_test matched** — the scene-flag bit reader (many callers
   ahead). Its three tricks (dead 8-byte local array for the empty frame,
   param reassignment for in-place srl, mask-as-local so the load schedules
   mid-computation) are in LESSONS.md "late night additions".
3. Player key-test family (func_800406A8/DC, func_80040B34) share one
   shape — more siblings likely nearby in player.c.

## What was accomplished in the NIGHT 2026-07-04 session

1. **18 more matches (82 total)** — all ≤14-instruction functions across
   main/sound/cd/Code800133D8/scene/moji, hash-verified + mutation-tested.
   Highlights: func_80012F78 (first newly-written gp-relative C),
   the CD_CMD command-queue writer family (new struct in cd.c),
   MOJI_TASK script call stack discovered (stack[8] @0x18, u16 sp @0xBE;
   func_80054AB4 is the script "return" opcode).
2. **New matching idioms** in LESSONS.md "night additions": scalar-pointer
   vs array decl by use count, volatile for reload-after-store,
   ternary-vs-if/else register choice, local init order, and the
   brute-force-orderings-in-a-scratch-TU tactic (fast, decisive).
3. gprel.py now drops ALL small `.extern`s (COMMON-leak proofing).

## What was accomplished in the LATE 2026-07-04 session

1. **Salvaged an uncommitted, unverified WIP batch** left in the tree (it did
   NOT build to a match — reminder: never leave unverified WIP uncommitted
   and unlabeled; stash or commit-to-a-branch with a WIP marker instead).
2. **Discovered and fixed the sdata/gp-relative pipeline gap** — the single
   most important structural fix since the patchasm reorder pass. The C
   pipeline could never emit $gp-relative access (GAS -G0 expanded every
   small-extern ref to lui/$at; cc1's `.extern sym,size` leaked COMMON
   symbols that silently shifted the data segment). New `tools/gprel.py`
   stage (maspsx → gprel → patchasm) fixes this via a census of `%gp_rel(`
   in the extracted asm. Full mechanism: LESSONS.md §2 (rewritten — the old
   §2 was aspirational and wrong).
3. **11 new matches** (main func_80012FA4/FC8; moji func_80054410, 5457C,
   54B88, 5563C, 56128, 57124, 58C08; scene func_8001DDC0 — first
   gp-relative match; sound func_8001B2F0), hash-verified + mutation-tested.
4. **Recount**: 7 upstream game.c functions were compiled+matching all along
   but uncounted → true total 64 matched / 420 active stubs. Counting method
   in LESSONS.md ("Counting matched functions").
5. **Volume metric**: ~841 of ~31,300 instructions matched (~2.7% of the
   main exe's mapped function volume; 64/475 = 13.5% by function count).

## What was accomplished in the earlier 2026-07-04 session (chronological)

1. **Diff loop works**: `./diff.py <func>` (root symlink → tools/asm-differ).
   Needed: watchdog+levenshtein (now in requirements.txt) and a **local patch
   in the asm-differ submodule** (Python 3.14: `ast.Num`→`ast.Constant`,
   ~line 1012 in eval_()). ⚠️ That patch is UNCOMMITTABLE (submodule) — if
   diff.py fails with "Failed to parse --base-shift", reapply it (SETUP.md
   "Stage 1 fixes" has details).
2. **Killed the blocker that stopped upstream**: cc1-27 at -O2 defers all C
   bodies to end-of-TU after the INCLUDE_ASM asm blocks (upstream's
   `ACCEPT_REORDERING_BULLSHIT`). `tools/patchasm.py` now has a reorder pass
   (moves each deferred `.ent/.end` body to its in-place `.globl` marker).
   Real C now works anywhere in a file. Limitation: no `static` functions.
3. **Matched 30 functions** across moji/scene/cd/debug/sound/sub_scrn/
   Code800133D8. Introduced `MOJI_TASK` struct (include/rock_neo/moji.h) —
   field 0x14 is `u8* script`; the tiny moji funcs are script-opcode handlers.
4. **Wrote the knowledge base**: `notes/LESSONS.md` = compiler idioms proven
   against the hash (gp-vs-lui extern rules, swap idiom, aliasing double-loads,
   scheduler interleaving). READ IT before writing any C.
5. **Proved anti-hallucination ritual to the user** (they are — rightly —
   wary from a past attempt): mutation test = break a matched function,
   check MUST fail, restore, OK. Progress is gated on the byte compare, never
   on "looks right". Keep it that way and keep saying so honestly.

## The inner loop (per function / small batch)

```bash
source .venv/bin/activate                      # ALWAYS first (rabbitizer error = forgot)
# pick target: wc -l asm/rock_neo/nonmatchings/*/*.s | sort -n
cat asm/rock_neo/nonmatchings/<file>/<func>.s  # read the asm
# write C in src/rock_neo/<file>.c replacing the INCLUDE_ASM line
rm -f build/rock_neo.elf && make CPP=cpp build_rock_neo_only
make CPP=cpp check_rock_neo_only               # OK = matched
# mismatch? head -30 build/rock_neo.diff isolates the function;
#   ./diff.py --format plain <func> for instruction-level view; iterate.
git add -A src/rock_neo include/rock_neo progress.md activity.md && git commit  # on dev
```
Update progress.md counts + activity.md line per batch. Commit message style:
see `git log` on dev.

## Critical gotchas (cost real time; do not rediscover)

- `.c.o` files are make *intermediate* files: after editing pipeline tools
  (patchasm.py etc.), `touch src/rock_neo/*.c && rm -f build/rock_neo.elf`,
  else make silently reuses stale output.
- NEVER `make clean` (deletes asm/ + assets/ → full re-extract). `rm -rf build` only.
- `CPP=cpp` on every make invocation.
- Extern declarations: read the target asm FIRST — `($gp)` access ⇒ plain
  scalar extern; `lui/%lo` ⇒ unsized array extern (`extern u8 foo[];`).
  Getting this wrong flips addressing mode and mismatches. (LESSONS.md §2.)
- moji.h's `Moji_flag` is `u8[8]` (matches moji.c definition) — don't "fix" it back to u32.

## Where to pick up (in order of value)

1. **Continue small-function harvest**: `wc -l asm/rock_neo/nonmatchings/*/*.s
   | sort -n` — everything ≤25 lines is quick wins; many moji script handlers
   share MOJI_TASK vocabulary.
2. DONE (late 2026-07-04 session): main.c func_80012FA4/FC8, the moji
   script handlers (func_80054410/5457C/54B88/5563C/56128/57124/58C08),
   scene func_8001DDC0, sound func_8001B2F0 — all hash-verified.
   Many remaining small funcs touch sdata globals; gprel.py now makes those
   matchable. All trivial 8-line (jr ra/nop) stubs are exhausted —
   everything remaining requires actually reading the asm.
3. **Medium functions (25–80 lines)**: consider installing m2c
   (github.com/matt-kempster/m2c) for draft C — `tools/m2ctx.py` already
   exists for generating its context. Drafts are never trusted, only iterated
   against the diff.
4. Big moji handlers (~500 lines) only after the struct fills in more.

## Delegation & token hygiene

- `.claude/agents/build-runner.md` + `function-mapper.md` (both Haiku) exist —
  route builds/greps there when output would bloat context; they return
  summaries. Keep C-matching reasoning in the main thread.
- Don't paste whole asm files into context for big functions; read the span.
- The user clears context between sessions deliberately — everything worth
  knowing must land in this file, LESSONS.md, progress.md, or commits.

## Verification contract with the user (do not weaken it)

- "Matched" claims ONLY after `check_rock_neo_only` prints OK in that session.
- The user can independently run: `make CPP=cpp check_rock_neo_only` and
  `cmp disks/us/ROCK_NEO.EXE build/rock_neo.exe`.
- If asked whether progress is real, offer the mutation test, don't just assert.
