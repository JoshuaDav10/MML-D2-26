# Submodule patch persistence — Brief 1 report

**Branch:** `infra/submodule-patches` (off `dev`)  
**Date:** 2026-07-05  
**Status:** **PARTIAL** — insurance patches + local `mml-local` commits done; **GitHub fork/push blocked** (expired `gh` token).

## What was saved

| Submodule | Upstream base | `mml-local` commit | Insurance patch |
|-----------|---------------|-------------------|-----------------|
| `tools/maspx` | `6610273` (mkst/maspsx) | `7511db8` | `maspx-local.patch` |
| `tools/asm-differ` | `ae40866` (simonlindholm/asm-differ) | `11e6d8c` | `asm-differ-local.patch` |

Committed submodule diffs are **byte-identical** to the insurance `.patch` files (verified with `diff -u`).

### Patch contents

- **maspx** — `ASPSX_REGFIRST_SYM_PREFIXES` reg-first `addu $at` ordering + bare-constant load-delay nop fix (`maspsx/__init__.py`, +47/−3 lines).
- **asm-differ** — Python 3.14 `ast.Num` → `ast.Constant` in `eval_()` (`diff.py`, +2/−2 lines).

## Blocker

`gh repo fork` and `gh repo create` both return **Forbidden** (`gh auth status` reports invalid token). Fork URLs do not exist yet:

- `https://github.com/JoshuaDav10/maspsx` — not found
- `https://github.com/JoshuaDav10/asm-differ` — not found

Per `notes/DELEGATION_QUEUE.md` Brief 1: do not vendor or improvise past fork creation. **Finish steps below require `gh auth refresh -h github.com` first.**

## Finish steps (after re-auth)

```bash
# 1. Fork upstream (or create empty repos and force-push mml-local)
gh auth refresh -h github.com
gh repo fork mkst/maspsx --default-branch-only
gh repo fork simonlindholm/asm-differ --default-branch-only

# 2. Push mml-local branches to forks
cd tools/maspx
git remote set-url origin https://github.com/JoshuaDav10/maspsx.git
git push -u origin mml-local:main   # or push mml-local and pin superproject to that SHA

cd ../asm-differ
git remote set-url origin https://github.com/JoshuaDav10/asm-differ.git
git push -u origin mml-local:main

# 3. Superproject: update .gitmodules URLs + pin SHAs, commit
cd ../..
# edit .gitmodules → JoshuaDav10 fork URLs
git add .gitmodules tools/maspx tools/asm-differ
git commit -m "Pin maspx/asm-differ submodules to MML fork commits."

# 4. Destruction rehearsal
git submodule sync
git submodule update --init --checkout
grep ASPSX_REGFIRST_SYM_PREFIXES tools/maspx/maspsx/__init__.py
grep -n 'ast.Constant' tools/asm-differ/diff.py
```

Target `.gitmodules` URLs after fork:

```
[submodule "tools/maspx"]
    url = https://github.com/JoshuaDav10/maspsx.git
[submodule "tools/asm-differ"]
    url = https://github.com/JoshuaDav10/asm-differ.git
```

## Verification completed (local)

### 1. Submodule working trees clean

```
$ git -C tools/maspx status
On branch mml-local
nothing to commit, working tree clean

$ git -C tools/asm-differ status
On branch mml-local
nothing to commit, working tree clean
```

### 2. Destruction rehearsal

**NOT RUN** — requires fork URLs + superproject pin (step 3 above).

### 3. Full clean build

```
$ rm -rf build && make CPP=cpp
# (exit 0)

$ make CPP=cpp check_rock_neo_only
OK

$ make CPP=cpp check_overlays
205/205 overlays OK
```

### 4. diff.py spot check

```
$ ./diff.py Sound_call
# (runs without "--base-shift" parse error; exit 0)
```

### 5. Fresh-checkout rehearsal

**NOT RUN** — blocked on fork pin; rely on steps 1–4 until forks land.
