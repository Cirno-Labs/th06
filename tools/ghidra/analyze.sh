#!/bin/sh
# Imports the 1.02h executable (copied to build/ by `just analyze`) into a
# headless Ghidra project and exports its function list. GHIDRA defaults to
# ~/opt/ghidra_*.
set -e
cd "$(dirname "$0")/../.."
GHIDRA=${GHIDRA:-$(ls -d ~/opt/ghidra_* | tail -1)}
mkdir -p build/ghidra
"$GHIDRA/support/analyzeHeadless" build/ghidra th06_102h \
	-import build/th06_102h.exe -overwrite \
	-scriptPath tools/ghidra \
	-postScript ExportFunctions.java "$PWD/build/functions_102h.csv" \
	> build/ghidra/analyze.log 2>&1
grep REPORT build/ghidra/analyze.log
