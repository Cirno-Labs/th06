#!/bin/sh
# Imports the trial executable into a headless Ghidra project and exports
# its function list. GHIDRA defaults to ~/opt/ghidra_*.
set -e
cd "$(dirname "$0")/../.."
GHIDRA=${GHIDRA:-$(ls -d ~/opt/ghidra_* | tail -1)}
mkdir -p build/ghidra
# Copy to an ASCII name; the original is Shift-JIS encoded.
find orig/trial013 -iname '*.exe' ! -name custom.exe \
	-exec cp {} build/th06_trial013.exe \;
"$GHIDRA/support/analyzeHeadless" build/ghidra th06_trial013 \
	-import build/th06_trial013.exe -overwrite \
	-scriptPath tools/ghidra \
	-postScript ExportFunctions.java "$PWD/build/functions_trial013.csv" \
	> build/ghidra/analyze.log 2>&1
grep REPORT build/ghidra/analyze.log
