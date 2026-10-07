# th06

A from-scratch matching decompilation of 東方紅魔郷 ~ the Embodiment of
Scarlet Devil (Touhou 6), built and verified on Linux through Wine.

The goal is C++ source that compiles, with the original toolchain, into a
binary that is byte-for-byte identical to the original executable.

This repository contains no game code or data. You need your own copy of
the game, see [`orig/README.md`](orig/README.md).

## Status

Nothing has been decompiled yet. The first target is the freely distributed
trial (0.13), which is used to bring up the toolchain and workflow before
moving to the retail 1.02h build.

Initial Ghidra analysis of the trial finds 1229 functions. Those from
`0x446ead` onwards are the statically linked MSVC CRT, identified by
Ghidra's function ID. The 857 functions in `0x401000`–`0x446ead`
(about 232 KB) are game code, plus whatever compiler helpers and D3DX8
code is mixed in. For example, `0x401000` is MSVC's
`vector constructor iterator`.

```sh
tools/ghidra/analyze.sh   # writes build/functions_trial013.csv
```

## Targets

| Build | Date | `.text` | Notes |
| --- | --- | --- | --- |
| Trial 0.13 | 2002-08-25 | 0x53cf0 bytes | Bring-up target, free download |
| Retail 1.02h | 2002-12-01 | TBD | Final target |

Trial 0.13 at a glance:

- 32-bit PE, image base `0x400000`, entry point `0x448c06`
- `.data` is 0x27201c bytes in memory but only 0x7000 on disk, so most of
  it is zero-initialized globals
- Imports `d3d8`, `DSOUND` (by ordinal), `DINPUT8`, `WINMM`, `GDI32`,
  `USER32` and `KERNEL32`. Only `Direct3DCreate8` is imported from D3D,
  so D3DX8 is most likely statically linked.

## Toolchain

Read from the Rich header of the trial executable (`tools/peinfo.py`):

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

Everything runs headless, so it works over SSH:

1. Analyze the binary with Ghidra in headless mode
   (`analyzeHeadless`), applying MSVC 7 function signatures so library
   code is labeled and can be ignored.
2. Record each game function in [`config/functions.csv`](config/functions.csv)
   (address, size, name, source file, status).
3. Write C++ for the function in `src/`, in the style of 2002-era
   code.
4. Build with `cl.exe` under Wine in a repo-local `WINEPREFIX`.
5. Compare the compiled function's bytes against the original and
   iterate until they match.

## Layout

```
orig/     your game files (not tracked) and their checksums
src/      decompiled source
config/   function list and match status
tools/    scripts (header inspection, build, compare)
```

## Requirements

- Wine
- Visual C++ .NET 2002 (bring your own copy)
- DirectX 8 SDK
- Python 3
- Ghidra (for analysis, optional)

## Legal

Touhou Project and 東方紅魔郷 are by ZUN / Team Shanghai Alice. This is an
unofficial fan project. It contains no assets or code from the original
game, and it is not affiliated with or endorsed by ZUN or Team Shanghai
Alice.
