# Rebuilding a Lost Blueprint
### How (and why) we're reconstructing the source code of a 1997 PlayStation game — explained for humans

---

## Chapter 1: The Cake Without the Recipe

In 1997, a team at Capcom wrote a video game called Mega Man Legends. They
wrote it in a programming language called **C** — thousands of lines of
human-readable instructions, full of names and comments and structure. Think
of that C code as a **recipe**: "cream the butter and sugar, fold in the
flour, bake at 350°."

But a PlayStation can't read recipes. So the team fed their C code through a
**compiler** — a translation machine that converts human-readable recipes
into **machine code**: the raw numeric instructions a CPU actually executes.
Machine code is the **finished cake**. It's what shipped on the disc. It
works perfectly — but you can't easily see how it was made.

Then, like almost every game studio of that era, the recipe was lost.
Companies rarely archived source code back then. The cake survives on
millions of discs; the recipe is gone.

**This project bakes the recipe back into existence.** We are writing new C
code — a new recipe — and we keep adjusting it until it produces *exactly*
the same cake. Not a similar cake. Not a cake that tastes the same. A cake
that is identical down to every last crumb, every air bubble, every
molecule. That discipline is called **matching decompilation**, and the
"every molecule" part is what makes it special.

### Why "identical" is the whole point

If you reverse-engineer a game loosely — "eh, this function probably adds
the score" — you end up with code that *might* be right. Might. Software is
unforgiving: one wrong assumption and the game crashes in some dungeon three
hours in.

Matching decompilation removes "might" entirely. Our rule: compile our
recipe with **the same 1997 compiler** the original team used, and compare
the output to the original game file **byte by byte** — all 825,344 bytes of
it. If even one byte differs, our recipe is wrong somewhere. If every byte
matches, our C code is *provably* a faithful reconstruction — the machine
itself vouches for it. No trust, no opinions, no "looks right to me."

It's like claiming you've reverse-engineered Coca-Cola's formula. Anyone can
*claim* their cola tastes the same. We're doing something stricter: our cola
is chemically indistinguishable, molecule for molecule, under a mass
spectrometer. At that point it's not an imitation. It *is* the formula.

---

## Chapter 2: The Cast of Characters

Before the process, meet the tools. Each has a job in the kitchen.

**The original binary (`ROCK_NEO.EXE`)** — the sacred cake. Extracted from a
legally-owned game disc. We never modify it. It is the answer key.

**The disassembler ("splat")** — machine code is numbers, and numbers are
brutal to read. A disassembler converts them into **assembly language**:
still extremely low-level, but at least written in words. If machine code is
a cake, assembly is an X-ray of the cake — you can see every layer and
bubble, but it still doesn't tell you the recipe. Splat X-rayed the whole
game and carved it into **475 functions** (a function = one self-contained
mini-program, like "make the frosting" or "check if the player pressed
jump").

**The 1997 compiler (`cc1-27`)** — the *original oven*. This matters
enormously. Two different ovens bake the same recipe into slightly different
cakes. A modern compiler translates C differently than a 1997 compiler did —
different instruction choices, different ordering. To match byte-for-byte,
we must use the very oven the original bakers used. We have it; it still runs.

**The diff tool (`asm-differ`)** — a side-by-side comparator. Left column:
the original function's assembly. Right column: what *our* C compiled into.
Every line that differs gets highlighted. It's the mass spectrometer readout
showing exactly which molecules are off.

**The hash check** — the final judge. After every change, we rebuild the
*entire game* and compare it to the original. One command, one verdict:
`OK` or failure. There is no partial credit and no way to sweet-talk it.

---

## Chapter 3: The Placeholder Trick (or: How to Rebuild a Bridge While Driving On It)

Here's a puzzle. The game only works as a *whole* — all 475 functions
linked together at exact addresses in memory, like 475 bricks mortared into
a wall. You can't rebuild one brick and leave a hole where the others go;
the wall wouldn't stand, and we'd have no way to test anything.

The solution is elegant: **placeholders**. At the start, our "source code"
is 475 stubs that each say: *"I haven't decompiled this function yet — just
copy the original assembly bytes here."* Build all 475 stubs and you get...
a perfect copy of the original game. Boring, but crucial: **the wall stands
from day one**, and it's provably identical to the original wall.

Now the real work: we pick ONE brick. We study its X-ray (the assembly),
write C code we believe produces that exact brick, and swap it in — real C
replacing the placeholder. Rebuild the whole wall. Compare to the original:

- **Every byte identical?** Our brick is perfect. It stays. Progress: +1.
- **Any byte different?** Our recipe for that brick is off. The diff tool
  shows exactly which instructions came out wrong, we adjust the C, try again.

Repeat 475 times. At the end, every placeholder is gone, the wall is still
byte-for-byte identical — and now we possess the complete blueprint.

The count of real bricks only moves in one direction, because every swap is
verified before it's kept. We call this **the ratchet** — like a socket
wrench that can tighten but physically cannot loosen. (Git, our version
control system, enforces this: every verified state is snapshotted forever.
Work happens on a scratch copy called the `dev` branch; the pristine `main`
branch only advances when a human decides the work is confirmed.)

---

## Chapter 4: One Brick, Start to Finish

Let's walk through a real one — the very first function we matched.

**Step 1: Read the X-ray.** The assembly for `func_800542FC`:

```
lw    $v0, 0x14($a0)     # load the word stored 0x14 bytes into some object
addiu $v0, $v0, 1        # add 1 to it
sw    $v0, 0x14($a0)     # store it back
```

**Step 2: Play detective.** Someone hands you: "walk into the room, pick up
the notebook on the third shelf, add one tally mark, put it back." What was
the *original English instruction*? Probably just: "add a tally to the
notebook." In C:

```c
void func_800542FC(THING *t) {
    t->counter += 1;
}
```

Short — but not obvious. Assembly never says "counter." It says "the thing
0x14 bytes from the start." Recovering *meaning* from *mechanics* is the
detective work. (Later, a neighboring function read *through* this value —
proof it was actually a pointer to a script, not a counter. We renamed it.
The bytes didn't change; our understanding did.)

**Step 3: Bake and compare.** Compile, rebuild the whole game, run the
check. If the oven produces the exact three instructions above, in that
order, using those exact registers — `OK`.

**Step 4: When it's NOT ok — the fun part.** Later we hit a function that
swaps two bytes in an array. Our first attempt was *logically perfect* —
same behavior, guaranteed — but the check FAILED. Why? We'd written:

> "copy A into cup one, copy B into cup two, pour cup one into B, pour cup
> two into A"

...while the original programmer wrote the textbook swap:

> "copy A into a spare cup, pour B into A, pour the spare cup into B"

Same result — but the 1997 oven arranges its mixing bowls (CPU **registers**)
differently for each phrasing. The diff showed the same instructions with
registers mirrored, we recognized the signature, rephrased, rebuilt: `OK`.

This is the deep lesson of matching decompilation: **we're not just
recovering what the code does — we're recovering how it was *phrased*.**
The compiler is so deterministic that the finished cake carries a
fingerprint of the recipe's exact wording. Byte-matching lets us read
phrasing *through* the compilation process. It borders on archaeology:
sometimes you can even tell where the original programmer copy-pasted.

---

## Chapter 5: The Bug That Killed the Last Expedition

A previous team attempted this exact project and abandoned it, leaving
frustrated comments in the code. Their blocker — which we solved on day one
— is worth understanding, because it's a perfect engineering story.

The 1997 compiler has a quirk: when a file mixes real C with placeholders,
it bakes all the real C **last**, dumping those functions at the *end* of
the output instead of where they belong. Imagine a printer that prints all
your inserted photos on the final pages instead of where you placed them.
Every brick you decompiled would land at the wrong address in the wall —
instant mismatch, always. The previous team concluded you couldn't mix real
C with placeholders at all, which made incremental progress nearly
impossible. Expedition over.

Our fix: the quirky printer at least leaves a **bookmark** ("photo #3 goes
here") at each correct spot. So we wrote a small program that runs after the
compiler and re-inserts every photo at its bookmark before the final
assembly step. Twenty lines of Python, verified against the full-game byte
check. The wall stopped scrambling, and the entire placeholder strategy
became viable. Total cost: one evening. The absence of that one insight
ended the previous project.

---

## Chapter 6: Trust Nothing, Verify Everything

An uncomfortable truth: this project uses an AI to write the C code, and
AIs can be *confidently wrong* — they can produce plausible-looking code
and sincerely report success. A previous AI-assisted attempt failed exactly
this way. So this project is architected such that **honesty is enforced by
physics, not personality**:

1. **The byte check can't be charmed.** "Matched" is only claimed after the
   whole-game comparison passes. The comparison is a dumb, incorruptible
   machine — it compares 825,344 bytes and says yes or no.
2. **The human can verify independently.** Two commands, runnable anytime,
   requiring zero trust in the AI: one rebuilds and checks; one (`cmp`)
   compares our game file to the original byte-for-byte with no cleverness
   involved at all.
3. **The mutation test.** Paranoid that the C is just decoration and the
   build is secretly still using placeholders? Sabotage one matched
   function — change a single `+1` to `+2` — and rebuild. The check MUST
   fail. Restore it; the check passes again. That single flipping character
   proves the C is *live* in the binary. We ran this demonstration for real.

The one thing bytes can't verify is *storytelling*: when we name a variable
`script` or call a function a "text handler," that's interpretation, and it
can be wrong (ours already was once, and got corrected). Names are
hypotheses. Bytes are facts. The project keeps the two clearly separated.

---

## Chapter 7: Getting Smarter as We Go

Rebuilding brick #400 should be much faster than brick #40, for three
compounding reasons:

**Structure discovery.** The game's data lives in **structs** — labeled
compartment boxes. Assembly only ever says "the byte 0x72 compartments deep."
Every time we prove what a compartment means, we write it into a shared
header file, and *every* future function touching that box gets the answer
for free. Knowledge that compiles is knowledge that can't be forgotten.

**A lessons journal.** Every compiler quirk proven against the byte check
(the swap phrasing, the printer bug, how global variables are addressed)
goes into a running document. It's the lab notebook: the next session — or
the next AI instance, which starts with total amnesia — reads it and
inherits every hard-won lesson without re-fighting for it.

**Division of labor.** Reading a diff and intuiting "the original programmer
phrased this as a do-while" takes the sharpest reasoning available. Running
builds and counting stubs does not. The workflow routes grunt work to
cheaper helpers and saves the expensive thinking for the one task that
needs it. A head chef doesn't wash the dishes.

---

## Chapter 8: What "Done" Means

- **Milestone 1 — the main executable fully matched.** All 475 functions
  converted from placeholder to proven C. The engine of the game, in
  readable, verified source form. This is the current campaign. (As of this
  writing: 32 down, 443 to go — about 0.5% of the instructions, but the
  hardest *setup* problems are behind us, and the pace compounds.)
- **Milestone 2 — the level files.** ~150 additional mini-programs (one per
  game area) that don't even build yet. A separate expedition.
- **Milestone 3 — the payoff.** With real source code, the game can be
  studied, fixed, modified, and *ported* — imagine it running natively on a
  modern PC, at any resolution, because the blueprint exists again. This is
  why matching decompilations of other games (Mario 64, Ocarina of Time)
  changed their communities forever: the recipe, once recovered, belongs to
  everyone who owns the cake.

And each step of the way, one dumb, beautiful, incorruptible test tells us
we're really there: **every byte, exactly where the original bakers left it,
twenty-nine years ago.**
