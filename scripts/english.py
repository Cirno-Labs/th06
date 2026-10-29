"""Builds the English patch's files for the standalone exe's FEAT_ENGLISH.

    python3 -I scripts/english.py INSTALLER [GAME [OUT]]

INSTALLER is the Touhou Wiki English patch v0.8 for 1.02h,
th06_english_patch_0.8.exe (an NSIS installer). GAME is the original
game (default orig/th06_102h), OUT where the results go (default
orig/th06e, untracked like everything else in orig/).

The installer is only unpacked (with 7z), never run. Its patch is data:
byte changes to the exe (exe_patch_1.02h.txt), translated dialogue
(translations.pickle), replacement images and texts, spellcards.eng and,
in its patcher's bytecode, the stage and song titles. This applies them
the way its patcher does and writes:

    th06e_CM.DAT ... th06e_TL.DAT   the patched archives
    th06e.exe                       the patched exe (for its resources)
    strings.txt                     the exe's English strings, NAME=text

scripts/build_hook.py turns strings.txt into build/feat/english.h.
"""
import hashlib, io, os, pickle, re, shutil, struct, subprocess, sys, tempfile, zipfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pbg3

EXE = "東方紅魔郷.exe"
EXE_SHA1 = "e61b4f4fea3802e926ef307f45166599c3e86555"
ARCHIVES = ["CM", "ED", "IN", "MD", "ST", "TL"]
INSERT_INTO = "IN"  # loaded when the game reads spellcards.eng
IMAGE_BASE = 0x400000  # the exe's sections are at file offset + this

# The exe's strings the patch translates, by their address in the patched
# exe, and the shot type names' pointer table.
STRINGS = [
    ("BOMB_REIMU_A", 0x469A60),
    ("BOMB_REIMU_B", 0x469A7C),
    ("BOMB_MARISA_A", 0x46A420),
    ("BOMB_MARISA_B", 0x46A450),
    ("WINDOW_TITLE", 0x46B1F8),
    ("UNKNOWN_NAME", 0x46BCDC),
]
SHOT_NAMES = ("SHOT_REIMU_A", "SHOT_REIMU_B", "SHOT_MARISA_A", "SHOT_MARISA_B")
SHOT_NAMES_TABLE = 0x4784D8


def unpack(installer, into):
    subprocess.run(["7z", "x", "-y", "-o" + into, installer], check=True, stdout=subprocess.DEVNULL)
    return os.path.join(into, "$PLUGINSDIR")


def patch_exe(text, exe):
    """Applies an exe_patch text: sections of "OFFSET: OLD NEW" lines (OLD
    may be ?? for bytes past the original's end)."""
    out = bytearray(exe)
    for line in text.splitlines():
        m = re.match(r"([0-9A-F]{8}): (\?\?|[0-9A-F]{2}) ([0-9A-F]{2})$", line.strip())
        if not m:
            continue
        offset, old, new = int(m[1], 16), m[2], int(m[3], 16)
        if offset >= len(out):
            out += bytes(offset + 1 - len(out))
        if old != "??" and out[offset] != int(old, 16):
            sys.exit(f"exe patch: byte {offset:#x} is {out[offset]:02x}, not {old}")
        out[offset] = new
    return bytes(out)


def c_string(exe, address):
    offset = address - IMAGE_BASE
    return exe[offset:exe.index(b"\0", offset)]


def strings(exe):
    out = [(name, c_string(exe, address)) for name, address in STRINGS]
    pointers = struct.unpack_from("<4I", exe, SHOT_NAMES_TABLE - IMAGE_BASE)
    out += [(name, c_string(exe, p)) for name, p in zip(SHOT_NAMES, pointers)]
    for name, value in out:
        if b"\n" in value or b"=" in name.encode():
            sys.exit(f"string {name} can't go in strings.txt")
    return b"".join(name.encode() + b"=" + value + b"\n" for name, value in out)


class Screen:
    """shared.screen.screen: a dialogue screen (file, type 0 for text and 1
    for the boss's title, section, lines)."""


class Translation:
    """shared.screen.translation: from_screen, to_screen."""


class Unpickler(pickle.Unpickler):
    """Loads translations.pickle, which holds only these two classes."""

    def find_class(self, module, name):
        if (module, name) == ("shared.screen", "screen"):
            return Screen
        if (module, name) == ("shared.screen", "translation"):
            return Translation
        raise pickle.UnpicklingError(f"unexpected {module}.{name}")


def load_translations(path):
    """Returns {(file, type, Japanese lines): [(section, English lines)]}."""
    with open(path, "rb") as f:
        items = Unpickler(f, encoding="bytes").load()
    table = {}
    for t in items:
        src, dst = t.__dict__[b"from_screen"].__dict__, t.__dict__[b"to_screen"].__dict__
        key = (src[b"file"].decode(), src[b"type"], tuple(src[b"lines"]))
        table.setdefault(key, []).append((src[b"section"], dst[b"lines"]))
    return table


def patch_msg(name, data, table):
    """Translates a message file (MsgFile: a count, script offsets, then
    the scripts' instructions). A screen is the text (opcode 3) or title
    (opcode 8) lines shown together: a new one starts at line 0 or after a
    wait (opcode 4). Its lines are replaced; extra ones are added after
    its last line, missing ones dropped."""
    count = struct.unpack_from("<i", data)[0]
    offsets = struct.unpack_from(f"<{count}i", data, 4)
    order = sorted(set(offsets), key=offsets.index)
    scripts, missing = {}, 0
    for offset in order:
        section = offsets.index(offset)
        instrs, p = [], offset
        while True:
            time, op, size = struct.unpack_from("<HBB", data, p)
            instrs.append([time, op, data[p + 4:p + 4 + size]])
            p += 4 + size
            if op == 0:
                break
        screens, waited = [], False
        for i, (time, op, args) in enumerate(instrs):
            if op == 4:
                waited = True
            if op in (3, 8):
                if struct.unpack_from("<h", args, 2)[0] == 0 or waited or not screens or screens[-1][0] != op:
                    screens.append((op, []))
                screens[-1][1].append(i)
                waited = False
        replace = {}
        for op, lines in screens:
            japanese = tuple(instrs[i][2][4:].split(b"\0")[0] for i in lines)
            found = table.get((name, 0 if op == 3 else 1, japanese))
            if not found:
                missing += 1
                continue
            english = next((l for s, l in found if s == section), found[0][1])
            who = instrs[lines[0]][2][:2]
            new = []
            for k, line in enumerate(english):
                time = instrs[lines[min(k, len(lines) - 1)]][0]
                args = who + struct.pack("<h", k) + line + b"\0"
                new.append([time, op, args.ljust((len(args) + 3) & ~3, b"\0")])
            replace[lines[-1]] = new
            for i in lines[:-1]:
                replace[i] = []
        out = []
        for i, instr in enumerate(instrs):
            out += replace.get(i, [instr])
        scripts[offset] = b"".join(struct.pack("<HBB", t, op, len(a)) + a for t, op, a in out)
    if missing:
        print(f"  {name}: {missing} screens without a translation")
    body, moved, at = b"", {}, 4 + 4 * count
    for offset in order:
        moved[offset] = at + len(body)
        body += scripts[offset]
    return struct.pack(f"<i{count}i", count, *(moved[o] for o in offsets)) + body


def read_marshal(data):
    """Reads a Python 2.4 marshal stream (a .pyc after its 8-byte header)
    as nested tuples and dicts; code objects become dicts."""
    pos, interned = 0, []

    def obj():
        nonlocal pos
        t = chr(data[pos])
        pos += 1
        if t in "0N":
            return None
        if t in "FT":
            return t == "T"
        if t == "i":
            pos += 4
            return struct.unpack_from("<i", data, pos - 4)[0]
        if t in "stu":
            n = struct.unpack_from("<i", data, pos)[0]
            pos += 4 + n
            s = data[pos - n:pos]
            if t == "t":
                interned.append(s)
            return s
        if t == "R":
            pos += 4
            return interned[struct.unpack_from("<i", data, pos - 4)[0]]
        if t in "([":
            n = struct.unpack_from("<i", data, pos)[0]
            pos += 4
            return tuple(obj() for _ in range(n))
        if t == "c":
            pos += 16
            code = {k: obj() for k in ("code", "consts", "names", "varnames", "freevars", "cellvars",
                                       "filename", "name")}
            pos += 4
            obj()  # lnotab
            return code
        raise ValueError(f"marshal type {t!r} at {pos - 1}")

    return obj()


def stage_titles(library):
    """The stage and song titles std_patcher writes, from the constants of
    its bytecode: per stage a (title, song, song) and the spaces before
    each, which stand in for centering and right alignment."""
    with zipfile.ZipFile(library) as z:
        code = read_marshal(z.read("th06/std_patcher.pyc")[8:])
    consts = code["consts"]
    titles = [c for c in consts if isinstance(c, tuple) and len(c) == 3 and all(isinstance(x, bytes) for x in c)]
    spaces = [c for c in consts if isinstance(c, tuple) and len(c) == 3 and all(isinstance(x, int) for x in c)]
    if len(titles) != 7 or len(spaces) != 7:
        sys.exit("std_patcher.pyc: unexpected constants")
    return list(zip(titles, spaces))


def patch_std(name, data, titles):
    """Writes a stage's name and first two song names (0x80 bytes each from
    0x10), each after its spaces."""
    (title, song1, song2), (s0, s1, s2) = titles[int(name[5]) - 1]
    out = bytearray(data)
    for at, text, spaces in ((0x10, title, s0), (0x90, song1, s1), (0x110, song2, s2)):
        text = b" " * spaces + text
        if len(text) >= 0x80:
            sys.exit(f"{name}: title too long")
        out[at:at + 0x80] = text.ljust(0x80, b"\0")
    return bytes(out)


def rot13(data):
    """The installer stores the endings ROT13-encoded."""
    out = bytearray(data)
    for i, c in enumerate(out):
        base = 0x41 if 0x41 <= c <= 0x5A else 0x61 if 0x61 <= c <= 0x7A else None
        if base is not None:
            out[i] = base + (c - base + 13) % 26
    return bytes(out)


def main():
    if not 2 <= len(sys.argv) <= 4:
        sys.exit(__doc__)
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    installer = os.path.abspath(sys.argv[1])
    game = sys.argv[2] if len(sys.argv) > 2 else os.path.join(root, "orig/th06_102h")
    out = sys.argv[3] if len(sys.argv) > 3 else os.path.join(root, "orig/th06e")
    with open(os.path.join(game, EXE), "rb") as f:
        exe = f.read()
    if hashlib.sha1(exe).hexdigest() != EXE_SHA1:
        sys.exit(f"{EXE} is not 1.02h")
    os.makedirs(out, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        files = unpack(installer, tmp)
        patch = os.path.join(files, "th06")
        with open(os.path.join(patch, "exe_patch_1.02h.txt"), encoding="latin-1") as f:
            english_exe = patch_exe(f.read(), exe)
        with open(os.path.join(out, "th06e.exe"), "wb") as f:
            f.write(english_exe)
        with open(os.path.join(out, "strings.txt"), "wb") as f:
            f.write(strings(english_exe))

        table = load_translations(os.path.join(patch, "translations.pickle"))
        titles = stage_titles(os.path.join(files, "library.zip"))
        replace = {}
        for sub in ("replace_files", "replace_files/full"):
            d = os.path.join(patch, sub)
            for n in os.listdir(d):
                if os.path.isfile(os.path.join(d, n)):
                    replace[n.encode()] = os.path.join(d, n)
        inserts = os.path.join(patch, "insert_files")
        for tag in ARCHIVES:
            src = os.path.join(game, f"紅魔郷{tag}.DAT")
            print(f"th06e_{tag}.DAT")
            entries = []
            for name, unk_04, unk_00, data in pbg3.read(src):
                text = name.decode("cp932")
                if name in replace:
                    with open(replace.pop(name), "rb") as f:
                        data = f.read()
                    if text.endswith(".end"):
                        data = rot13(data)
                elif text.startswith("msg") and text.endswith(".dat"):
                    data = patch_msg(text, data, table)
                elif text.startswith("stage") and text.endswith(".std"):
                    data = patch_std(text, data, titles)
                entries.append((name, unk_04, unk_00, data))
            if tag == INSERT_INTO:
                for n in sorted(os.listdir(inserts)):
                    with open(os.path.join(inserts, n), "rb") as f:
                        entries.append((n.encode(), entries[-1][1], entries[-1][2], f.read()))
            pbg3.write(os.path.join(out, f"th06e_{tag}.DAT"), entries)
        if replace:
            print("not in any archive:", ", ".join(n.decode() for n in replace))
    print("wrote", out)


if __name__ == "__main__":
    main()
