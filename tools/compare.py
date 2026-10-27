"""Compares compiled functions with the original executable.

    compare.py            report every function in src/ that symbols.csv lists
    compare.py SYMBOL...  also show a disassembly diff of these functions

Relocations whose target has a known address (symbols.csv, or the same
function) are filled in and compared. The others, such as string literals,
are masked.
"""
import csv, glob, os, subprocess, sys, tempfile

import coff

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = "build/th06_102h.exe"


def compiled_functions(symbols):
    """Yields (name, bytes, mask) for each function src/ defines."""
    for path in sorted(glob.glob(os.path.join(ROOT, "build/obj/*.obj"))):
        if path.endswith("hooks.obj"):
            continue
        obj = coff.Object(path)
        for sym in obj.externals():
            if sym.section <= 0 or sym.name not in symbols:
                continue
            sec = obj.sections[sym.section - 1]
            starts = sorted(s.value for s in obj.symbols
                            if s and s.section == sym.section and s.value > sym.value)
            end = starts[0] if starts else len(sec.data)
            addr = symbols[sym.name][0]
            code = bytearray(sec.data[sym.value:end])
            mask = bytearray(len(code))
            for off, index, typ in sec.relocs:
                if not sym.value <= off < end:
                    continue
                pos = off - sym.value
                target = obj.symbols[index]
                if target.name in symbols:
                    to = symbols[target.name][0]
                elif target.section == sym.section and sym.value <= target.value < end:
                    to = addr + target.value - sym.value
                else:
                    mask[pos:pos + 4] = b"\1" * 4
                    continue
                addend = int.from_bytes(code[pos:pos + 4], "little")
                value = to + addend - (addr + pos + 4 if typ == coff.REL32 else 0)
                code[pos:pos + 4] = (value & 0xFFFFFFFF).to_bytes(4, "little")
            while len(code) > symbols[sym.name][1] and code[-1] in (0x90, 0xCC):
                code.pop()
            yield sym.name, bytes(code), bytes(mask)


def disasm(code, addr):
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(code)
        f.flush()
        out = subprocess.run(["objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel",
                              f"--adjust-vma={addr:#x}", f.name],
                             capture_output=True, text=True, check=True).stdout
    return [l.split("\t", 1)[1] for l in out.splitlines() if l[:2] == "  " and "\t" in l]


def read_symbols():
    with open(os.path.join(ROOT, "config/symbols.csv")) as f:
        return {s: (int(r["address"], 16), int(r["size"] or 0))
                for r in csv.DictReader(f) for s in r["symbol"].split()}


def results(symbols):
    """Yields (name, same, code, mask, original) for each compiled function."""
    exe = coff.Image(os.path.join(ROOT, EXE))
    for name, code, mask in compiled_functions(symbols):
        addr, size = symbols[name]
        orig = exe.read(addr, size)
        same = len(code) == size and all(m or a == b for a, b, m in zip(code, orig, mask))
        yield name, same, code, mask, orig


def main():
    os.chdir(ROOT)
    symbols = read_symbols()
    show = set(sys.argv[1:])
    matched = total = matched_bytes = 0
    for name, same, code, mask, orig in results(symbols):
        addr, size = symbols[name]
        total += 1
        matched += same
        matched_bytes += size if same else 0
        print(f"{'OK  ' if same else 'DIFF'} {addr:08x} {size:5d}/{len(code):<5d} {name}", flush=True)
        if name in show:
            # Show masked bytes as the original's so they don't show up as diffs.
            shown = bytes(b if m and i < len(orig) else a
                          for i, (a, m, b) in enumerate(zip(code, mask, orig + bytes(len(code)))))
            ours, theirs = disasm(shown, addr), disasm(orig, addr)
            with tempfile.TemporaryDirectory() as tmp:
                for fn, lines in (("original", theirs), ("compiled", ours)):
                    open(os.path.join(tmp, fn), "w").write("\n".join(lines) + "\n")
                subprocess.run(["diff", "-u", "original", "compiled"], cwd=tmp)
    print(f"{matched}/{total} functions match, {matched_bytes} bytes")
    return 0 if matched == total else 1


if __name__ == "__main__":
    sys.exit(main())
