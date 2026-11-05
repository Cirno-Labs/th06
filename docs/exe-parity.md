# Executable parity

`just exe` links the game from source into `build/th06.exe`, byte-for-byte
identical to the original 1.02h executable: all 512,000 bytes, SHA-1
`e61b4f4fea3802e926ef307f45166599c3e86555`, as in `orig/th06_102h.sha1`.

```sh
just check-exe            # builds the exe, then requires exact equality
just compare              # ordinary-object function comparison
just compare-exe          # detailed headers/sections/symbol report
```

`just check-exe` (decomp-tools' `project.py check-exe`) compares every
byte and the file lengths. It exits nonzero on any mismatch, including
headers, padding, tables and metadata; it does not mask addresses.
Only the PE timestamp and checksum are copied from the original; the
code, data, imports, resources and Rich header come out of the build.
Replay validation is recorded in [replay parity](replay.md).

## How it matches

Matching functions is not enough for the whole file: the linker's
layout depends on things the game's surviving code doesn't show.

- **Objects.** The source files are split as the original's objects
  were: the Rich header counts them (39 game objects), and functions,
  data and literals land in object order. Where no byte shows a
  boundary, the split is a plausible one.
- **Data.** Globals are defined in the files that own them. `.bss` is
  ordered by a hash of each name, so some globals are named for their
  slot rather than their use.
- **Dead code.** `#ifdef TH06_EXE` blocks hold functions nothing calls.
  `/OPT:REF` drops their code, but what they reference still steers the
  link: the order libraries and imports are loaded, the import count
  in the Rich header, and which copy of identical code `/OPT:ICF`
  keeps. The original had such code too; where its contents can't be
  recovered, the stand-ins say so. Changing the set of compiled
  functions can re-roll the code folding, so check `just compare-exe`
  after edits.

## Optional features

The standalone exe can be built with optional features, community
patches reimplemented in the source behind `#if FEAT_...` blocks. `FEAT`
lists them, comma-separated:

```sh
FEAT=frame_cap,english just exe wait:15 shot:title
```

`tools/build.py` passes `/DFEAT_<NAME>=1` for each one, to the exe build
only, and keeps their objects in `build/obj_<features>/`. Without `FEAT`
the exe is the vanilla game, with its objects in `build/obj_exe/`. The
DLL and `just compare` always build the vanilla code. (The exe's objects
also see `TH06_EXE`, which guards the globals the DLL takes from the
original and the exe's dead code.)

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
Its text is not in this repository. `scripts/english.py` unpacks the
installer (with 7z, without running it), applies the patch's data to
your game and writes `orig/th06e/`: the `th06e_XX.DAT` archives
(translated dialogue, stage and song titles, endings, images and
`spellcards.eng`), the patched exe for its icons and `strings.txt`, the
exe's English strings, which `scripts/build_hook.py` turns into
`build/feat/english.h`:

```sh
python3 -I scripts/english.py th06_english_patch_0.8.exe
FEAT=english just exe
```

The game then reads `th06e_XX.dat` and `th06e.cfg`, draws its text in
Arial (under Wine, map Arial to an installed font such as Nimbus Sans in
`HKCU\Software\Wine\Fonts\Replacements` if it has none), takes spell
card names from `spellcards.eng` and lays out the longer English lines
as the patch does.
