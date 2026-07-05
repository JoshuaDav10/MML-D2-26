# m2c drafts (100–199 insn tier)

UNTRUSTED rough decompiles for the medium-function tier. Never copy into
`src/` without byte-match iteration (`tools/bytecmp.sh`).

## How produced

- m2c pinned at `94098d4de68c2fcc13fb8cf1096a1520eb171abe` (`tools/m2c/PINNED_COMMIT`)
- Target list: active stubs with 100–199 asm comment lines
  (`python3 tools/census.py`, cpp-based — not naive INCLUDE_ASM grep)
- Context: `cpp` on `src/rock_neo/<tu>.c` with `-DPERMUTER` (stubs stripped)
- Decompile: `tools/m2c/m2c.py -t mipsel-gcc-c --context … -f <func> <asm>`

## Regenerate one

```bash
source .venv/bin/activate
python3 tools/m2c/generate_drafts.py --func func_800411DC
```

## Regenerate tier

```bash
python3 tools/m2c/generate_drafts.py
```

## Status (61 functions)

| insns | function | TU | status |
|------:|----------|-----|--------|
| 197 | `func_800411DC` | `player.c` | ok |
| 193 | `func_8005A874` | `moji.c` | m2c-errored |
| 193 | `func_8005B98C` | `moji.c` | m2c-errored |
| 190 | `func_8004174C` | `player.c` | ok |
| 189 | `func_800587F8` | `moji.c` | m2c-errored |
| 183 | `func_80063058` | `debug.c` | ok |
| 181 | `func_80016160` | `game.c` | m2c-errored |
| 181 | `func_80040818` | `player.c` | ok |
| 181 | `func_80063604` | `debug.c` | ok |
| 180 | `func_80063334` | `debug.c` | ok |
| 180 | `func_800638D8` | `debug.c` | ok |
| 178 | `func_8003EEC0` | `player.c` | ok |
| 177 | `func_8003C4EC` | `player.c` | ok |
| 172 | `func_8001B89C` | `cd.c` | m2c-errored |
| 170 | `func_8003E860` | `player.c` | m2c-errored |
| 166 | `func_80040B68` | `player.c` | ok |
| 165 | `func_8003C88C` | `player.c` | ok |
| 161 | `func_8003E224` | `player.c` | m2c-errored |
| 161 | `func_8005B5D8` | `moji.c` | m2c-errored |
| 159 | `func_800598C8` | `moji.c` | m2c-errored |
| 157 | `main` | `main.c` | ok |
| 154 | `func_80054BEC` | `moji.c` | m2c-errored |
| 151 | `func_8003C07C` | `player.c` | ok |
| 151 | `func_800414F0` | `player.c` | ok |
| 148 | `func_80041B8C` | `player.c` | ok |
| 145 | `Sce_flag_on` | `scene.c` | ok |
| 142 | `func_8001CD60` | `cd.c` | m2c-errored |
| 142 | `func_8005F3D0` | `sub_scrn.c` | ok |
| 140 | `func_8005FBB4` | `sub_scrn.c` | m2c-errored |
| 139 | `func_8005BCE4` | `moji.c` | m2c-errored |
| 138 | `func_8003DA34` | `player.c` | m2c-errored |
| 137 | `Sub_screen_gauge_set` | `sub_scrn.c` | ok |
| 137 | `func_8005A634` | `moji.c` | m2c-errored |
| 135 | `Sub_screen_basic_param_set` | `sub_scrn.c` | ok |
| 134 | `func_8003F5D8` | `player.c` | ok |
| 133 | `func_80016798` | `game.c` | m2c-errored |
| 133 | `func_800169AC` | `game.c` | m2c-errored |
| 133 | `func_8001AC58` | `sound.c` | ok |
| 132 | `func_8003BE6C` | `player.c` | ok |
| 132 | `func_80055868` | `moji.c` | m2c-errored |
| 130 | `func_8003E4A8` | `player.c` | m2c-errored |
| 127 | `Map_screen_task` | `sub_scrn.c` | ok |
| 123 | `Sub_screen_status_calc` | `sub_scrn.c` | ok |
| 119 | `func_8001D078` | `cd.c` | m2c-errored |
| 119 | `func_80059B44` | `moji.c` | m2c-errored |
| 118 | `func_80014394` | `Code800133D8.c` | ok |
| 118 | `func_8005FDE4` | `sub_scrn.c` | m2c-errored |
| 116 | `func_800592FC` | `moji.c` | m2c-errored |
| 115 | `func_8001AA8C` | `sound.c` | ok |
| 113 | `Sub_screen_rb_parts_calc` | `sub_scrn.c` | ok |
| 112 | `func_8005F210` | `sub_scrn.c` | m2c-errored |
| 111 | `Sub_screen_gauge_set2` | `sub_scrn.c` | ok |
| 111 | `func_8003E068` | `player.c` | ok |
| 110 | `func_8001F3C8` | `scene.c` | ok |
| 109 | `func_80019DE0` | `sound.c` | ok |
| 108 | `func_8003E6B0` | `player.c` | ok |
| 106 | `func_80057E50` | `moji.c` | m2c-errored |
| 105 | `func_8001E068` | `scene.c` | ok |
| 105 | `idk_Init_system_maybe` | `main.c` | ok |
| 104 | `func_80059728` | `moji.c` | m2c-errored |
| 100 | `func_80012C80` | `main.c` | ok |

**Summary:** 35 ok, 26 m2c-errored/context-missing (total 61 = tier census count).
