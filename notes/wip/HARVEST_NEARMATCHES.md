# Harvest near-matches — permuter last-mile candidates (2026-07-14)

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

## func_8001E4C4 (scene) — ONE register-swap from a match

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
