export WINEPREFIX := justfile_directory() / ".wine"
export WINEDEBUG := env("WINEDEBUG", "-all")

exe := "build/th06_102h.exe"

# List recipes
default:
    @just --list

# Compile src/ and link build/th06.dll
build:
    python3 tools/build.py

# Compare compiled functions with the original; show a diff for each SYMBOL
[positional-arguments]
compare *symbols: build _orig
    python3 tools/compare.py "$@"

# Set up build/game: game data, patched th06.exe and th06.dll
game: build _orig
    mkdir -p build/game
    cp -r orig/th06_102h/. build/game/
    rm build/game/東方紅魔郷.exe
    python3 tools/patch_exe.py {{ exe }} build/game/th06.exe
    cp build/th06.dll build/game/

# Run the patched game headless with STEPS (wait:S key:K hold:K:S shot:NAME)
[positional-arguments]
run *steps: game
    python3 tools/play.py "$@"

# Link the standalone build/th06.exe (no DLL; FEAT=a,b adds features); with STEPS, run it like `run`
[positional-arguments]
exe *steps: game
    python3 tools/build.py exe
    cp build/th06.exe build/game/th06_standalone.exe
    if [ -d orig/th06e ]; then cp orig/th06e/th06e_*.DAT build/game/; fi
    if [ $# -gt 0 ]; then python3 tools/play.py --exe th06_standalone.exe "$@"; fi

# Analyze the original with headless Ghidra; add new functions to symbols.csv
analyze: _orig
    tools/ghidra/analyze.sh
    python3 tools/seed_symbols.py

# Decompile ADDRs (default: every function) with Ghidra into build/decomp/
decompile *addrs:
    tools/ghidra/decompile.sh {{ addrs }}

# Render docs/progress.svg from symbols.csv and the build
progress: build _orig
    python3 tools/render_progress.py

# Remove build outputs (keeps the Ghidra project)
clean:
    rm -rf build/obj build/src build/game build/th06.* build/th06.*

# Verify the original and copy it to an ASCII file name
_orig:
    cd orig && sha1sum --quiet -c th06_102h.sha1
    mkdir -p build && cp orig/th06_102h/東方紅魔郷.exe {{ exe }}
