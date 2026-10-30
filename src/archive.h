// The game's data archives (紅魔郷*.DAT, magic "PBG3"): a bit stream
// over a file, its entry table and the LZSS decoder. The code, from
// 0x43c2f0 to 0x43d158, is built with optimization, like the libraries
// after the game's code.

#pragma once

#include <windows.h>

#include "common.h"

// Reads a stream bit by bit, most significant bit first. Abstract.
class BitStream
{
public:
    BitStream()
    {
        this->Reset();
    }

    // Returns the next bit.
    virtual int ReadBit() = 0;
    // Returns the next `bits` bits.
    virtual DWORD ReadBits(int bits) = 0;
    // Returns the next byte, ignoring the bit position, or -1.
    virtual int ReadByte() = 0;
    // Moves to byte `offset`. Returns nonzero on success.
    virtual int Seek(DWORD offset) = 0;
    // Skips to the next byte boundary. Returns nonzero on success.
    virtual int AlignToByte() = 0;
    // Reads `size` bytes into `buffer` from the next byte boundary.
    virtual int Read(void *buffer, DWORD size) = 0;
    // The stream's last write time.
    virtual int GetLastWriteTime(FILETIME *time) = 0;
    virtual ~BitStream()
    {
    }

    // Starts over at bit 0.
    void Reset();
    // Reads a number stored in 1 to 4 bytes, its size in two bits first.
    DWORD ReadVarInt();
    // Reads a little-endian 32-bit number.
    DWORD ReadU32();
    // Reads a NUL-terminated string of at most `maxLength` bytes. Returns
    // nonzero if it ended in time.
    int ReadString(char *buffer, DWORD maxLength);

    // Bytes read since the start or the last Seek.
    DWORD position;
    DWORD size;
    int currentByte;
    // The next bit of currentByte.
    BYTE mask;
    // The sum of the bytes read.
    DWORD checksum;
};
ASSERT_SIZE(BitStream, 0x18);

// A Win32 file.
class File
{
public:
    File();
    ~File();

    // Opens `path` in `mode` ("r", "w" or "a"). Returns nonzero on
    // success.
    virtual int Open(char *path, char *mode);
    virtual void Close();
    virtual int Read(void *buffer, DWORD size, DWORD *read);
    virtual int Write(void *buffer, DWORD size, DWORD *written);
    // Returns the next byte, or -1.
    virtual int ReadByte();
    // Writes `c`. Returns it, or -1.
    virtual int WriteByte(int c);
    virtual int Seek(DWORD offset, DWORD origin);
    virtual DWORD Tell();
    virtual DWORD GetSize();
    // Reads the whole file, if it is at most `maxSize` bytes, into a
    // LocalAlloc'd buffer.
    virtual void *ReadAll(DWORD maxSize);

    HANDLE handle;
    // GENERIC_READ or GENERIC_WRITE.
    DWORD access;
};
ASSERT_SIZE(File, 0xc);

// A bit stream over a file opened for reading.
class FileBitStream : public BitStream, public File
{
public:
    FileBitStream();
    ~FileBitStream();

    // Opens the file at `path`. Returns nonzero on success.
    int Open(char *path);

    virtual void Close();
    virtual int ReadBit();
    virtual DWORD ReadBits(int bits);
    virtual int ReadByte();
    virtual int Seek(DWORD offset);
    virtual int AlignToByte();
    virtual int Read(void *buffer, DWORD size);
    virtual int GetLastWriteTime(FILETIME *time);
};
ASSERT_SIZE(FileBitStream, 0x24);

// Inline (~FileBitStream expands it), but filebitstream.cpp defines
// FBS_CLOSE_LATE and its own copy after Open, where the original has it.
#ifndef FBS_CLOSE_LATE
inline void FileBitStream::Close()
{
    this->File::Close();
    this->Reset();
}
#endif

// An entry of an archive's table.
struct DatArchiveEntry
{
    DWORD unk_00;
    DWORD unk_04;
    // The decoded size.
    DWORD size;
    // Where the encoded data starts; it ends where the next entry's (or
    // the table) starts.
    DWORD offset;
    // The sum of the encoded bytes.
    DWORD checksum;
    char name[0x100];
};
ASSERT_SIZE(DatArchiveEntry, 0x114);

// One of the game's data archives.
class DatArchive
{
public:
    DatArchive();
    // Calls Release.
    ~DatArchive();

    // Opens the archive at `path` and reads its entry table. Returns
    // nonzero on success.
    int Open(char *path);
    // Reads the header and the entry table. Returns nonzero on success;
    // on failure closes the stream.
    int ReadTable();
    // Returns the index of the entry named `name`, or -1.
    int FindEntry(char *name);
    // Decodes entry `entry` into a malloc'd buffer, or returns NULL.
    // `name` is unused.
    void *Decode(int entry, char *name);
    // Reads entry `entry`'s encoded data into a malloc'd buffer, its size
    // in `size` and its checksum in `checksum`.
    void *ReadEncoded(DWORD *size, DWORD *checksum, DWORD entry);
    // The decoded size of entry `entry`.
    DWORD GetEntrySize(DWORD entry);

    // Closes the archive and frees its entry table. Returns 1.
    int Release();

    FileBitStream *stream;
    // Freed by Release; never set.
    void *unk_04;
    DWORD numEntries;
    // Where the table starts, after the data.
    DWORD tableOffset;
    DatArchiveEntry *entries;
};
ASSERT_SIZE(DatArchive, 0x14);
