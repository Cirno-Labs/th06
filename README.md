# th06

A from-scratch matching decompilation of 東方紅魔郷 ~ the Embodiment of
Scarlet Devil (Touhou 6), built and verified on Linux through Wine.

The goal is C++ source that compiles, with the original toolchain, into a
binary that is byte-for-byte identical to the original executable.

This repository contains no game code or data. You need your own copy of
the game, see [`orig/README.md`](orig/README.md).

## Clean room

This is an independent, clean-room implementation. It is written from
the binary alone: no code, names, struct layouts or function lists are
taken from other decompilations or ports of Touhou games. Names come from
our own analysis or, for library code, from the libraries themselves.

## Status

509 of the game's 511 functions are decompiled and match the original,
240,230 of 241,285 bytes (99.56%). The two that don't match yet are
`DatArchive::Decode` and `ResultScreen::RegisterChain`. Every function
is decompiled, so the standalone exe (`just exe`) is the whole game,
built from source.

Ghidra finds 1383 functions. The game code is compiled without
optimization, so its functions start with `push ebp; mov ebp, esp`; the
statically linked D3DX8 and CRT code is optimized. That puts the game
code at `0x401000`–`0x43d158`, with the archive code last. Ghidra's
function ID recognizes the CRT from about `0x45b000` on.

```sh
just analyze   # writes build/functions_102h.csv
```

![Progress](docs/progress.svg)

`just progress` renders it: every game function, by address and as a
treemap by size, coloured by whether it is decompiled and matching.

## Target

Retail 1.02h, released 2002-12-01:

- 32-bit PE, image base `0x400000`, entry point `0x45d746`, `.text` is
  0x68a5f bytes
- `.data` is 0x2719fc bytes in memory but only 0x6000 on disk, so most of
  it is zero-initialized globals
- Imports `d3d8`, `DSOUND`, `DINPUT8`, `WINMM`, `GDI32`, `USER32` and
  `KERNEL32`. Only `Direct3DCreate8` is imported from D3D, so D3DX8 is
  statically linked.

## Toolchain

Read from the Rich header (`tools/peinfo.py`):

| Tool | Build | Objects | Probably |
| --- | --- | --- | --- |
| C++ compiler 13.00 | 9466 | 48 | Game code |
| C compiler 13.00 | 9466 | 143 | MSVC CRT |
| MASM 7.00 | 9466 | 45 | MSVC CRT |
| C++ compiler 13.00 | 8830 | 67 | Static libraries, probably D3DX8 |
| Older linker objects | 8755 | 10 | Static libraries |
| Import library 7.00 | 9210 | 8 | DirectX SDK import libraries |
| Linker 7.00 | 9466 | | |

This means Visual C++ .NET 2002 (MSVC 7.0, `cl` 13.00.9466) with the
DirectX 8 SDK. Only the roughly 48 C++ objects at build 9466 need to be
decompiled. The CRT and the SDK libraries are linked as-is.

## Workflow

Everything runs headless, so it works over SSH. `just` lists the recipes.

1. Analyze the binary with Ghidra in headless mode (`just analyze`).
   New functions are added to `config/symbols.csv` with placeholder names
   (`Func43be50`); CRT functions keep the names Ghidra's function ID
   gives them.
2. Write C++ for a function in `src/`, in the style of 2002-era code.
   `just decompile [ADDR...]` writes Ghidra's pseudocode to
   `build/decomp/<address>.c` to read next to the disassembly (all
   functions take about 80 seconds).
3. Record the function and every original function and global it
   uses in [`config/symbols.csv`](config/symbols.csv): address, size, our
   name, and the mangled name, which the link error for an unresolved
   external gives.
4. Compare (`just compare`): builds with `cl.exe` under Wine and
   checks each function against the original byte for byte. Calls and
   global addresses are resolved through `symbols.csv` and compared too;
   only references to unnamed data such as string literals are masked.
   `just compare SYMBOL` shows a disassembly diff.
5. Play (`just run`): runs the original game with the new code hooked
   in, headless under Xvfb, to check that the code behaves correctly as
   well as matching. Steps drive it and take screenshots, e.g.
   `just run wait:12 key:Escape key:z wait:2 shot:menu` writes
   `build/shots/menu.png`.

6. Standalone (`just exe`): links the same objects, without the
   hook DLL, into `build/th06.exe`, a game that needs nothing from the
   original executable. It links the real D3DX8, the static CRT and the
   SDK import libraries, and carries the original's icon and header
   fields. `just exe STEPS` runs it like `just run`.

### Optional features

The standalone exe can be built with optional features, community
patches reimplemented in the source behind `#if FEAT_...` blocks. `FEAT`
lists them, comma-separated:

```sh
FEAT=frame_cap,english just exe wait:15 shot:title
```

`tools/build.py` passes `/DFEAT_<NAME>=1` for each one, to the exe build
only, and keeps their objects in `build/obj_<features>/`. Without `FEAT`
the exe is the vanilla game. The DLL and `just compare` always build the
vanilla code.

`frame_cap` is VsyncPatch (`vpatch_th06.dll`, rev4, from
`VsyncPatch.zip` at <https://maribelhearn.com/faq/eosd>) with the
settings of the `vpatch.ini` it ships with. The game no longer waits
for the display's vsync: frames are timed with the performance counter
at 60 per second (240 while Ctrl is held in a replay, 30 with Shift),
each is presented at its deadline, and the next starts only as early as
recent frames needed to be ready by their own, which keeps input lag
low. Frame skip is off and the frame rate shown is measured at each
present. `src/exe/framecap.cpp` has the limiter; the changes to the game
are under `#if FEAT_FRAME_CAP`.

`english` is the static English patch, `th06e.exe` from Touhou Wiki's
English patch v0.8 for 1.02h (`th06_english_patch_0.8.exe`, linked from
<https://en.touhouwiki.net/wiki/Embodiment_of_Scarlet_Devil/English_patch>).
Its text is not in this repository. `tools/english.py` unpacks the
installer (with 7z, without running it), applies the patch's data to
your game and writes `orig/th06e/`: the `th06e_XX.DAT` archives
(translated dialogue, stage and song titles, endings, images and
`spellcards.eng`), the patched exe for its icons and `strings.txt`, the
exe's English strings, which `tools/build.py` turns into
`build/feat/english.h`:

```sh
python3 -I tools/english.py th06_english_patch_0.8.exe
FEAT=english just exe
```

The game then reads `th06e_XX.dat` and `th06e.cfg`, draws its text in
Arial (under Wine, map Arial to an installed font such as Nimbus Sans in
`HKCU\Software\Wine\Fonts\Replacements` if it has none), takes spell
card names from `spellcards.eng` and lays out the longer English lines
as the patch does.

### Hooking

The decompiled code runs inside the original game. `tools/patch_exe.py` adds
one import, `th06.dll`, to a copy of the executable. The DLL is
built from the same objects that `just compare` checks. On load it
overwrites each decompiled function in the original with a jump to the
new code.

Calls from new code to functions that are not decompiled yet go through
generated thunks (`push address; ret`) to the original. Globals are
absolute symbols at their original addresses. Both are generated from
`symbols.csv`. The DLL has no C runtime of its own: CRT functions belong
in `symbols.csv` too, so the game and the new code share one heap.

## Layout

```
orig/     your game files (not tracked) and their checksums
src/      decompiled source; src/dll/ is the hook DLL's own code,
          src/exe/ the game's data and globals for the standalone exe
          and its optional features' code
config/   symbols.csv: address, size, name and mangled symbol
tools/    build, compare, exe patcher, symbol seeding, Ghidra scripts,
          the English patch's data (english.py, pbg3.py)
build/    outputs; build/game/ is a runnable copy of the patched game
```

## Requirements

- Wine (32-bit), Python 3, [just](https://github.com/casey/just),
  binutils (`objdump`), 7-Zip
- For `just run`: Xvfb, xdotool, ImageMagick, 32-bit Mesa OpenGL
  (`libgl1-mesa-dri:i386`, `libglx-mesa0:i386`) and the `ja_JP.UTF-8`
  locale, without which the game can't find its Shift-JIS named data
  files
- Visual C++ .NET 2002, the Windows XP SP2 Platform SDK and the
  DirectX 8.0 SDK (bring your own copies)
- Ghidra (for analysis, optional)

## Legal

Touhou Project and 東方紅魔郷 are by ZUN / Team Shanghai Alice. This is an
unofficial fan project. It contains no assets or code from the original
game, and it is not affiliated with or endorsed by ZUN or Team Shanghai
Alice.

## License

The source in this repository is licensed under the GNU General Public
License, version 3 only (`GPL-3.0-only`), see [`LICENSE`](LICENSE). The
license covers our code, not the original game or its data.
