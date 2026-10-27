"""Shows where the compiler puts local variables, by name.

    locals.py NAME...

Under /Od, MSVC 7 lays out a function's locals in an order that depends
on their names: it walks a hash table of them. Names in different buckets
keep a fixed relative order whatever else is declared; names sharing a
bucket come out latest declared first. Matching a function means choosing
local names that land in the original's slots. This compiles a probe that
declares the names in the given order and prints them from [ebp-4] down.
"""
import os, re, subprocess, sys, tempfile

import build


def order(names):
    body = " ".join(f"int {n};" for n in names)
    body += " " + " ".join(f"{n} = {i + 1};" for i, n in enumerate(names))
    body += " use(" + ", ".join(names) + ");"
    with tempfile.TemporaryDirectory(dir=os.path.join(build.ROOT, "build")) as tmp:
        src = os.path.join(tmp, "probe.cpp")
        obj = os.path.join(tmp, "probe.obj")
        with open(src, "w") as f:
            f.write("void use(...);\nvoid probe()\n{\n    " + body + "\n}\n")
        subprocess.run(["wine", r"C:\msvc7\bin\cl.exe", "/nologo", "/c", *build.CFLAGS,
                        "/Fo" + build.win(os.path.relpath(obj, build.ROOT)),
                        build.win(os.path.relpath(src, build.ROOT))],
                       env=build.ENV, cwd=build.ROOT, capture_output=True, timeout=build.TIMEOUT, check=True)
        out = subprocess.run(["objdump", "-d", "-M", "intel", obj], capture_output=True, text=True).stdout
    slots = {}
    for m in re.finditer(r"mov\s+DWORD PTR \[ebp-0x([0-9a-f]+)\],0x([0-9a-f]+)", out):
        slots[names[int(m.group(2), 16) - 1]] = int(m.group(1), 16)
    return sorted(names, key=lambda n: slots[n])


def main():
    names = sys.argv[1:]
    for i, name in enumerate(order(names)):
        print(f"[ebp-{(i + 1) * 4:#x}] {name}")


if __name__ == "__main__":
    main()
