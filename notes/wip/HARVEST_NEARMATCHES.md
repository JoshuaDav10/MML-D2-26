# Harvest near-matches — permuter last-mile candidates (2026-07-14)

## PROVEN permuter-workdir setup recipe (2026-07-14, verified on mml_E4C4)
Per PERMUTER_GUIDE "good targets": reg-mirror / scheduling near-misses are the
RIGHT permuter targets (NOT 53B40 — that's a value-CSE wall, guide-verified futile).
To wire a near-miss (example func_8001E4C4, scene):
1. `D=tools/decomp-permuter/mml_<TAG>; mkdir $D; cp tools/decomp-permuter/mml_53B40/compile.sh $D/`
2. `printf 'func_name = "func_XXXX"\ncompiler_type = "gcc"\n' > $D/settings.toml`
3. target.s = macro include + the splat asm; assemble with the build's as flags:
   `printf '.include "macro.inc"\n' > $D/target.s; cat asm/rock_neo/nonmatchings/<mod>/func_XXXX.s >> $D/target.s`
   `mipsel-elf-as -Iinclude -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0 $D/target.s -o $D/target.o`
   (VERIFY: `objdump -d $D/target.o` insn count == the function's size, else the
   macro.inc include was missing and target.o is empty/wrong.)
4. base.c = self-contained near-match TU (typedefs + externs inlined, no #include),
   from the draft below for that function.
5. LAUNCH tracked so --stop-on-zero notifies: run `python3 permuter.py mml_<TAG>
   -j4 --stop-on-zero --best-only` AS the background command (NOT `nohup ... &`
   inside it — that detaches and kills the completion notification).
6. VERIFY base score is small/finite before trusting (mml_E4C4 base = 80).
Status: mml_E4C4 set up + running (base 80). TODO same for AE6C, F188 (good
targets); map_screen_set/F158 are borderline (held-base/CSE — may not converge).


These functions have **byte-correct logic** (verified against the target asm) and
are **1–2 instructions from a match**, blocked only on cc1 scheduling/allocation
nits. That makes them IDEAL decomp-permuter targets (the tool's actual sweet spot,
unlike the giants). Each C draft below compiled clean and produced the noted diff.
Next session: drop each into a permuter workdir, or hand-finish with the -dg lever.

## DISCOVERY (2026-07-14 Opus harvest run)

The two easy wins this session (func_80016434, func_8001D394) were **straight-line
global stores**. Every control-flow-heavy small function I then tried
(func_8001F158, func_8001E4C4, func_8001AE6C) matched on LOGIC but missed on a
single cc1 **scheduling/allocation** decision (delay-slot placement, allocno
register tie, basic-block tail ordering). **Implication for planning:** the
remaining sub-60 tier is NOT uniformly "quick." Split it: straight-line stores =
fast hand-matches; branch/loop bodies = get the logic right, then hand to the
permuter for the last instruction. Don't hand-grind the scheduling nits.

---

## func_8001E4C4 (scene) — LANDED 2026-07-14 (permuter mml_E4C4, score 0 -> 275). Was:

Logic + structure 100% match. Sole delta: `i`↔`arg1` land in swapped saved regs
(reference: `i`=$s1, `arg1`=$s2; mine: reversed). Allocno tie-break genus (same as
MojiTaskExec — equal refs, tie broken toward the param in my build). func_8001E460
is `void(s32 n, s32 on, s32 b)`.

```c
void func_8001E4C4(u8 *arg0, u8 *arg1) {
    s32 i;
    u8 c;
    for (i = 0; i < 8; i++, arg1++, arg0++) {
        c = *arg0;
        if (c == 0) break;
        if (*arg1 != 0xFF) func_8001E460(c, *arg1 & 1, (*arg1 >> 1) & 1);
    }
}
```

## func_8001AE6C (sound) — ONE instruction from a match

Volume nudge: `arg0&3` gate, `arg0&1` ? decrement-toward-0 : increment-toward-0x7F.
`extern u8 D_80098840;`. Logic verified byte-exact. Sole delta: the `v<=0` branch's
`ret=0` — reference places `move a2,zero` on a shared tail block (`.L1aee4`) reached
by `j`, with `*arg1=0` in the delay slot; cc1 (from every C form I tried: inline,
reordered, goto-shared-label) emits `ret=0` inline instead. Basic-block tail-ordering
genus. Best near-match draft (1 insn off):

```c
extern u8 D_80098840;
s32 func_8001AE6C(s32 arg0, u16 *arg1) {
    s32 ret = 0;
    if (arg0 & 3) {
        ret = 1;
        if (arg0 & 1) {
            s16 v = *arg1 - D_80098840;
            *arg1 = v;
            if (v <= 0) { *arg1 = 0; ret = 0; }
        } else {
            s16 v = *arg1 + D_80098840;
            *arg1 = v;
            if (v >= 0x7F) *arg1 = 0x7F;
        }
    }
    return ret;
}
```

## map_screen_set (sub_scrn) — logic solved, word-store scheduling hoist

Prim-fill via the `0x1F800070` scratchpad + held prim base (same genus as 53B40
render). All field values/offsets verified correct against the target. Sole residual:
cc1 hoists the `0x4` word store (`sw a0,4(a1)` = 0x2C808080) AND the `D_80098934`
load (AddPrim's arg) to EARLY (right after `p[0]`), while the target keeps both LATE
(after the 0x1c store / near AddPrim). Tried: writing the 0x4 store late in source,
aliasing it through the `s16*` base (`*(u32*)(h+2)`) to break word-store grouping —
cc1 hoists regardless. Word-store-grouping + arg-load scheduling genus → permuter
last-mile candidate. Draft (use `((s16*)p)[k]` fields + `AddPrim((u8*)D_80098934+0x78,p)`;
advance `*(u32* volatile*)0x1F800070 = p + 0xA`) in this session's git history.
NOTE: `D_80098934` is `UnkStruc_80098934*` in game.h — cast, don't redeclare.

## func_8001F158 (scene) — parked earlier this session (see activity.md 2026-07-14)

xA4 address-CSE genus; logic solved. Draft in git history.

## Sce_flag_off (scene) — logic solved, addressing-order residual (2026-07-25)
Clears bit `0x80>>(f&7)` in `Sce_flag[f>>3]`, then if `(u32)(f-0x500) < 0x20` scans two
Player_work tables (+0x454 len 0x20, then +0x450 len 3) for the value `f-0x4FF` and zeroes
the first hit in each. NOTE the pre-existing decl in include/rock_neo.h is
`unknown_t Sce_flag_off(unknown_t)` — the definition MUST return int, not void.
**cc1 SEGFAULTS if the scan loops use `break`** — use goto (see LESSONS).
Residual: the target computes `&Sce_flag[f>>3]` as base-then-index into a held register
(`lui/addiu` base, `srl` index, `addu a0,v0,a0`); every ordering tried (array index, held
pointer, explicit `base` local) permutes the operand order / register pair. Draft saved as
notes/wip/sce_flag_off_draft.c — good permuter candidate (addressing/register genus).

## func_80042154 (player) — logic solved, needs the v1=v0 copy (2026-07-25)
Symmetric accel/decel on `pl->xB4` gated by `pl->x11C & keymask`; returns 1 (key set)
or 2 (clear); clamps to +/-limit; writes step (or -step) to +0xB6; tail does
`pl->x56 = (pl->x56 + pl->xB4) & 0xFFF`. Draft: notes/wip/func_80042154_draft.c.
SOLVED so far: the target allocates a **dead 0x10 frame** (nothing is ever stored to
it) — reproduced with `u8 pad[16]; (void)&pad;` (the tooth-16 idiom; `Sce_flag_test`
uses the same trick with `u8 buf[8]`). That fixed the `addiu $sp,-0x10`.
RESIDUAL: both branches load xB4 into $v0 for the sign test but do the ARITHMETIC on
$v1 (`move v1,v0` in each branch's delay slot, then `addu v0,v1,a1` / `subu v0,v1,a1`).
That is the live-range-split copy genus (LESSONS: cse folds any plain `b = a`), plus
the shared zero-block layout. Also note `xB4` is `u16` in PL_WORK but the asm uses
`lh` -> access via `*(s16 *)&pl->xB4`; `xB6` sits in a pad array -> byte-offset cast.
Good permuter candidate.

## func_8001F828 (scene) — 5 rows, delay-slot/liveness (2026-07-25)
Twin of func_8001FA94 (same `Game_work.x52` dispatch: ==1 / <2 / ==0 / <6). Sets
D_800981BC then a second value into D_800981C2/C0/BE. Draft:
notes/wip/func_8001F828_draft.c (5 differing rows, 495->45 words exact).
Structure fully matches with the FA94 goto layout (==1 case last, x==0 out of line).
RESIDUAL (all 5 rows are one issue): the target fills the `beqz` (x==0) delay slot with
`li v0,0x32`, i.e. the value is assigned BEFORE the x==0 test. Hoisting the assignment
in C DOES fill the slot — but it makes `v` live across the Sce_flag_test call in the
zerocase arm, so cc1 moves it to $s0 and adds a save/restore (cascade to 1014 rows).
So: hoist = right instruction, wrong register; no-hoist = right register, missing fill.
Need a form where `v` is set pre-branch yet provably dead on the zerocase path.
Type changes (s32/int/u16) do not help. Permuter workdir mml_F828 running.
**Solving this likely solves func_8001FA94 too — identical shape.**
