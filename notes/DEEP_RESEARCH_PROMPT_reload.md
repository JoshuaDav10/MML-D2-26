# Deep-research prompt — SN gcc 2.7.2 / reload live-range-split artifact

Paste the block below into deep research mode. Written 2026-07-25.

---

I am working on a **byte-exact matching decompilation** of a PlayStation 1 game
(Mega Man Legends, US, `ROCK_NEO.EXE`) built with the **PSY-Q SDK** and compiled by
**`cc1` reporting version `GNU C 2.7.2.SN32.3.7 Build 0001`** — i.e. an **SN Systems
patched gcc 2.7.2**, not stock FSF gcc. Target is MIPS R3000 (`-mcpu=3000 -O2 -G8
-msoft-float -fpcc-struct-return -mgas`). I need C source that compiles to *byte-identical*
machine code, so compiler-internal codegen decisions matter down to individual instructions.

Please research these three questions. Prioritize primary sources (source trees, repo
commits, issue threads, decomp project documentation, Discord/forum posts archived on the
web) over general explanation. I already understand the compiler theory — I need
**artifacts, links, and prior art**, not tutorials.

**QUESTION 1 — Does a reconstructed SN Systems / PSY-Q patched GCC 2.7.2 source tree exist
publicly?**
The binary I use appears to be a modern host recompile of an SN-patched 2.7.2 tree (strings
show a build path `/home/aaron/sn-2.7.2`). I want to know whether the *source* for that
patched tree is available anywhere — GitHub, GitLab, archived FTP, decomp-community mirrors,
or a documented patch set against FSF 2.7.2. Specifically of interest: whether SN modified
the register allocator or **reload** passes (`local-alloc.c`, `global.c`, `reload.c`,
`reload1.c`) versus only the MIPS backend/assembler interface. If multiple SN versions exist
(2.7.2 SN32.x), note which. Also relevant: who "aaron" is in the PSX decomp toolchain
community and whether they published the tree or a Docker/Nix recipe for it.

**QUESTION 2 — Prior art on matching a RELOAD live-range-split artifact.**
My remaining mismatch is not a source-shape problem; it is a compiler-internal artifact.
Two symptoms, which I believe are one reload episode:
  (a) The target loads a pointer into a caller-saved scratch register and then **copies** it
      into a callee-saved register (`lw $v0,0($v1)` … `addu $s1,$v0,$zero`). My build folds
      this into a single direct load (`lw $s1,0($v1)`), leaving me one instruction short.
      I proved via gcc 2.7.2 `local-alloc.c:combine_regs` that a reg-to-reg copy is deleted
      iff the source pseudo is dead at the copy, and that `cse` (running earlier) always
      makes it dead for a plain `b = a`. So no spelling of that assignment reproduces it.
  (b) The target's stack frame **reserves 8 bytes that are never read or written** — spill
      slots allocated by reload whose accesses register *inheritance* subsequently removed.
I want to know how other matching-decomp projects have reproduced these. Search
decompilation projects using the same toolchain generation — **Castlevania: Symphony of the
Night**, Spyro, Crash Bandicoot, Ape Escape, Silent Hill, Metal Gear Solid, Legend of Dragoon,
Tomb Raider, and any PSY-Q gcc 2.7.2 project — plus **decomp.me** scratches, the
**decomp-permuter** repo's issues/README, and matching-decomp guides/wikis. Concrete things
I am looking for: documented C idioms that force gcc 2.7 to emit a live-range-split copy;
techniques for making gcc reserve stack slots it never touches; any writeup on "reload
inheritance" in a matching context; and whether projects ultimately declare such artifacts
unreachable and accept the function as assembly.

**QUESTION 3 — Does another Mega Man Legends (Rockman DASH) decompilation exist?**
Any public decomp/reverse-engineering project for Mega Man Legends 1 or 2 / Rockman DASH on
PS1 (or the N64 "64" port, PSP port, or PC version) — GitHub, GitLab, decomp.me, Discord
communities, romhacking sites. If one exists, I especially want to know whether it has
matched the moji/text-renderer function at address **0x80053B40** (a ~495-instruction
per-frame font/primitive renderer using the PSX scratchpad at `0x1F800070`), or any
function with the artifacts in Question 2. Also note the project's toolchain setup and
whether their notes document register-allocation walls.

**Output format:** For each question, give a direct verdict (found / not found / partial),
then the specific links and what each contains, then a short assessment of how usable it is
for my problem. If something does not exist, say so plainly rather than substituting
adjacent material — a confident "no public SN source tree exists" is as valuable to me as
finding one.
