"""Adds every function from the Ghidra analysis to config/symbols.csv.

    seed_symbols.py [build/functions_102h.csv]

New rows get a placeholder name, FuncXXXXXX. In the library code after
the game code, names that Ghidra's function ID recognized are kept; in
the game code they are false positives. Existing rows are never changed.
"""
import csv, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME_END = 0x43C2F0  # unoptimized game code ends here; D3DX8 and the CRT follow


def main():
    os.chdir(ROOT)
    src = sys.argv[1] if len(sys.argv) > 1 else "build/functions_102h.csv"
    path = "config/symbols.csv"
    rows = {int(r["address"], 16): r for r in csv.DictReader(open(path))}
    added = 0
    for f in csv.DictReader(open(src)):
        addr = int(f["address"], 16)
        if addr in rows:
            continue
        name = f["name"]
        if addr < GAME_END or name.startswith("FUN_"):
            name = f"Func{addr:x}"
        rows[addr] = {"address": f"0x{addr:08x}", "size": f["size"], "name": name, "symbol": ""}
        added += 1
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, ["address", "size", "name", "symbol"], lineterminator="\n")
        w.writeheader()
        for addr in sorted(rows):
            w.writerow({k: rows[addr].get(k) or "" for k in w.fieldnames})
    print(f"{added} functions added to {path}, {len(rows)} rows")


if __name__ == "__main__":
    main()
