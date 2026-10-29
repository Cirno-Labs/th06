"""Reads and writes the game's data archives (紅魔郷*.DAT, "PBG3").

The format, from DatArchive and BitStream in src/: a bit stream, most
significant bit first. The header is the magic, the entry count and the
table's offset (variable-length numbers); each entry's data is LZSS
encoded (a 13-bit dictionary position and 4-bit length, or a literal
byte) and starts on a byte boundary; the table follows the data.
"""
import struct

MAGIC = 0x33474250
WINDOW = 0x2000


class BitReader:
    def __init__(self, data, pos=0):
        self.data, self.pos, self.mask, self.byte = data, pos, 0x80, 0

    def bit(self):
        if self.mask == 0x80:
            self.byte = self.data[self.pos] if self.pos < len(self.data) else 0
            self.pos += 1
        b = self.byte & self.mask
        self.mask = self.mask >> 1 or 0x80
        return 1 if b else 0

    def bits(self, n):
        v = 0
        for _ in range(n):
            v = v << 1 | self.bit()
        return v

    def varint(self):
        size = self.bit() << 1 | self.bit()
        return self.bits(8 * (size + 1))

    def string(self):
        out = bytearray()
        while (c := self.bits(8)) != 0:
            out.append(c)
        return bytes(out)


class BitWriter:
    def __init__(self):
        self.out, self.byte, self.n = bytearray(), 0, 0

    def bits(self, v, n):
        for i in range(n - 1, -1, -1):
            self.byte = self.byte << 1 | (v >> i & 1)
            self.n += 1
            if self.n == 8:
                self.out.append(self.byte)
                self.byte, self.n = 0, 0

    def varint(self, v):
        size = 0 if v < 1 << 8 else 1 if v < 1 << 16 else 2 if v < 1 << 24 else 3
        self.bits(size, 2)
        self.bits(v, 8 * (size + 1))

    def string(self, s):
        for c in s + b"\0":
            self.bits(c, 8)

    def flush(self):
        if self.n:
            self.bits(0, 8 - self.n)
        return bytes(self.out)


def decode(data):
    r = BitReader(data)
    dic = bytearray(WINDOW)
    pos, out = 1, bytearray()
    while True:
        if r.bit():
            c = r.bits(8)
            out.append(c)
            dic[pos] = c
            pos = (pos + 1) & (WINDOW - 1)
        else:
            offset = r.bits(13)
            if offset == 0:
                return bytes(out)
            for i in range(r.bits(4) + 3):
                c = dic[(offset + i) & (WINDOW - 1)]
                out.append(c)
                dic[pos] = c
                pos = (pos + 1) & (WINDOW - 1)


def encode(data):
    """Greedy LZSS with hash chains: matches of 3 to 18 bytes from the
    last WINDOW - 1 bytes."""
    w = BitWriter()
    heads, n, i = {}, len(data), 0
    while i < n:
        best_len, best_at = 0, 0
        key = data[i:i + 3]
        if len(key) == 3:
            for at in reversed(heads.get(key, ())):
                if i - at >= WINDOW - 1:
                    break
                if (at + 1) % WINDOW == 0:
                    continue  # position 0 would read as the end marker
                l = 3
                while l < 18 and i + l < n and data[at + l] == data[i + l]:
                    l += 1
                if l > best_len:
                    best_len, best_at = l, at
                    if l == 18:
                        break
        step = best_len if best_len >= 3 else 1
        if best_len >= 3:
            # The dictionary position of data[k] is (k + 1) % WINDOW.
            w.bits(0, 1)
            w.bits((best_at + 1) & (WINDOW - 1), 13)
            w.bits(best_len - 3, 4)
        else:
            w.bits(1, 1)
            w.bits(data[i], 8)
        for k in range(i, min(i + step, n - 2)):
            chain = heads.setdefault(data[k:k + 3], [])
            chain.append(k)
            if len(chain) > 64:
                del chain[:32]
        i += step
    w.bits(0, 1)
    w.bits(0, 13)
    return w.flush()


def read(path):
    """Returns [(name, unk_04, unk_00, data)] of the archive at `path`."""
    with open(path, "rb") as f:
        raw = f.read()
    r = BitReader(raw)
    magic = bytes(r.bits(8) for _ in range(4))
    if struct.unpack("<I", magic)[0] != MAGIC:
        raise ValueError(f"{path}: not a PBG3 archive")
    count, table = r.varint(), r.varint()
    r = BitReader(raw, table)
    rows = []
    for _ in range(count):
        unk_04, unk_00, checksum, offset, size = (r.varint() for _ in range(5))
        rows.append((r.string(), unk_04, unk_00, checksum, offset, size))
    out = []
    for k, (name, unk_04, unk_00, checksum, offset, size) in enumerate(rows):
        end = rows[k + 1][4] if k + 1 < count else table
        enc = raw[offset:end]
        if sum(enc) != checksum:
            raise ValueError(f"{path}: {name!r}: bad checksum")
        data = decode(enc)
        if len(data) != size:
            raise ValueError(f"{path}: {name!r}: size {len(data)} != {size}")
        out.append((name, unk_04, unk_00, data))
    return out


def write(path, entries):
    """Writes [(name, unk_04, unk_00, data)] as an archive."""
    body, rows = bytearray(), []
    header = 4 + 2 * 5  # magic and two varints of at most 5 bytes
    for name, unk_04, unk_00, data in entries:
        enc = encode(data)
        rows.append((name, unk_04, unk_00, sum(enc), header + len(body), len(data)))
        body += enc
    table = header + len(body)
    h = BitWriter()
    for c in struct.pack("<I", MAGIC):
        h.bits(c, 8)
    h.varint(len(entries))
    h.varint(table)
    head = h.flush().ljust(header, b"\0")
    t = BitWriter()
    for name, unk_04, unk_00, checksum, offset, size in rows:
        for v in (unk_04, unk_00, checksum, offset, size):
            t.varint(v)
        t.string(name)
    with open(path, "wb") as f:
        f.write(head + body + t.flush())
