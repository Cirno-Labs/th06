"""Compiles src/ with the original compiler and links the hook DLL.

Every symbol in config/symbols.csv that src/ defines is hooked: the DLL
overwrites the original function with a jump to the new one. Every other
symbol becomes an absolute symbol at its original address, so new code
calls and reads the original game.
"""
import csv, glob, os, struct, subprocess, sys

import coff

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJ = "build/obj"
DLL = "build/th06.dll"
CFLAGS = ["/Od", "/Ob1"]
TIMEOUT = 60  # cl spins forever after a fatal error

ENV = dict(
    os.environ,
    WINEPREFIX=os.environ.get("WINEPREFIX", os.path.join(ROOT, ".wine")),
    WINEDEBUG=os.environ.get("WINEDEBUG", "-all"),
    INCLUDE=r"C:\dx8\include;C:\msvc7\include;C:\psdk\Include",
    LIB=r"C:\dx8\lib;C:\msvc7\lib;C:\psdk\Lib",
)


def win(path):
    return path.replace("/", "\\")


def tool(name, *args):
    cmd = ["wine", rf"C:\msvc7\bin\{name}.exe", "/nologo", *args]
    try:
        r = subprocess.run(cmd, env=ENV, timeout=TIMEOUT, capture_output=True, text=True, errors="replace")
    except subprocess.TimeoutExpired:
        sys.exit(f"{name} timed out (fatal error?): {' '.join(args)}")
    out = "\n".join(l for l in r.stdout.splitlines() if l.strip() and not l.endswith(".cpp"))
    if out:
        print(out)
    if r.returncode:
        sys.exit(r.returncode)


def read_symbols():
    with open("config/symbols.csv") as f:
        return {r["symbol"]: int(r["address"], 16) for r in csv.DictReader(f) if r["symbol"]}


def compile_all():
    os.makedirs(OBJ, exist_ok=True)
    headers = max((os.path.getmtime(h) for h in glob.glob("src/**/*.h", recursive=True)), default=0)
    objs = []
    for src in sorted(glob.glob("src/**/*.cpp", recursive=True)):
        obj = os.path.join(OBJ, os.path.relpath(src, "src").replace("/", "_")[:-4] + ".obj")
        objs.append(obj)
        if os.path.exists(obj) and os.path.getmtime(obj) > max(os.path.getmtime(src), headers):
            continue
        print("cl", src)
        tool("cl", "/c", *CFLAGS, *file_flags(src), "/Isrc", "/Fo" + win(obj), win(src))
    return objs


def file_flags(src):
    """Extra flags for one file, from a `// cflags: ...` first line."""
    with open(src) as f:
        first = f.readline()
    return first.split(":", 1)[1].split() if first.startswith("// cflags:") else []


def write_hooks(objs, symbols):
    """Writes the hook table and the symbols that point into the original.

    The linker rejects calls to absolute symbols, so each original function
    that new code calls gets a thunk instead: push <address>; ret. It
    leaves every register intact, whatever the calling convention.
    """
    defined, undefined, called = set(), set(), set()
    for path in objs:
        obj = coff.Object(path)
        for s in obj.externals():
            (defined if s.section > 0 else undefined).add(s.name)
        for sec in obj.sections:
            called.update(obj.symbols[i].name for _, i, t in sec.relocs if t == coff.REL32)
    hooks = sorted((symbols[n], n) for n in defined & symbols.keys())
    external = {n: a for n, a in symbols.items() if n in undefined - defined}

    table, table_relocs = bytearray(), []
    for addr, name in hooks:
        table_relocs.append((len(table) + 4, name, coff.DIR32))
        table += struct.pack("<II", addr, 0)
    table += bytes(8)
    thunks, entry = bytearray(), {"_g_Hooks": (1, 0)}
    for name, addr in external.items():
        if name in called:
            entry[name] = (2, len(thunks))
            thunks += struct.pack("<BIB", 0x68, addr, 0xC3).ljust(8, b"\xcc")
    absolute = {n: a for n, a in external.items() if n not in entry}

    path = os.path.join(OBJ, "hooks.obj")
    coff.write_object(path, [(".rdata", coff.RDATA, bytes(table), table_relocs),
                             (".text", coff.TEXT, bytes(thunks), [])],
                      entry, [n for _, n in hooks], absolute)
    print(f"{len(hooks)} hooked, {len(external)} used from the original")
    return path


def main():
    os.chdir(ROOT)
    objs = compile_all()
    hooks = write_hooks(objs, read_symbols())
    tool("link", "/DLL", "/NODEFAULTLIB", "/ENTRY:DllMain", "/BASE:0x30000000",
         "/MAP:" + win(DLL[:-4] + ".map"), "/OUT:" + win(DLL),
         *map(win, objs), win(hooks), "kernel32.lib", "user32.lib")
    print("linked", DLL)


if __name__ == "__main__":
    main()
