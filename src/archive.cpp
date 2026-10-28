// cflags: -/Od -/Op /O2 /Ob1
// The data archives, 0x43c6e0 to 0x43ce2e: DatArchive (with
// FileBitStream's inline destructor).

#include <stdlib.h>
#include <string.h>

#include "archive.h"

// From the DirectX SDK's dxutil.h.
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

DatArchive::DatArchive()
{
    this->tableOffset = 0;
    this->numEntries = 0;
    this->entries = NULL;
    this->stream = NULL;
    this->unk_04 = NULL;
}

int DatArchive::ReadTable()
{
    DWORD i;

    if (this->stream->ReadU32() != 0x33474250)
    {
        SAFE_DELETE(this->stream);
        return 0;
    }
    this->numEntries = this->stream->ReadVarInt();
    this->tableOffset = this->stream->ReadVarInt();
    if (!this->stream->Seek(this->tableOffset))
    {
        SAFE_DELETE(this->stream);
        return 0;
    }
    this->entries = new DatArchiveEntry[this->numEntries];
    if (this->entries == NULL)
    {
        SAFE_DELETE(this->stream);
        return 0;
    }
    for (i = 0; i < this->numEntries; i++)
    {
        this->entries[i].unk_04 = this->stream->ReadVarInt();
        this->entries[i].unk_00 = this->stream->ReadVarInt();
        this->entries[i].checksum = this->stream->ReadVarInt();
        this->entries[i].offset = this->stream->ReadVarInt();
        this->entries[i].size = this->stream->ReadVarInt();
        if (!this->stream->ReadString(this->entries[i].name, 0x100))
        {
            SAFE_DELETE(this->stream);
            SAFE_DELETE(this->entries);
            return 0;
        }
    }
    return 1;
}

int DatArchive::Release()
{
    this->tableOffset = 0;
    this->numEntries = 0;
    SAFE_DELETE(this->stream);
    SAFE_DELETE(this->entries);
    free(this->unk_04);
    return 1;
}

int DatArchive::FindEntry(char *name)
{
    DWORD i;

    for (i = 0; i < this->numEntries; i++)
    {
        if (strcmp(name, this->entries[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}

DWORD DatArchive::GetEntrySize(DWORD entry)
{
    if (entry >= this->numEntries)
    {
        return 0;
    }
    return this->entries[entry].size;
}

void *DatArchive::ReadEncoded(DWORD *size, DWORD *checksum, DWORD entry)
{
    DWORD dataSize;
    void *data;

    if (this->stream == NULL)
    {
        return NULL;
    }
    if (entry >= this->numEntries)
    {
        return NULL;
    }
    if (size == NULL)
    {
        return NULL;
    }
    if (checksum == NULL)
    {
        return NULL;
    }
    if (!this->stream->Seek(this->entries[entry].offset))
    {
        return NULL;
    }
    if (entry == this->numEntries - 1)
    {
        dataSize = this->tableOffset - this->entries[entry].offset;
    }
    else
    {
        dataSize = this->entries[entry + 1].offset - this->entries[entry].offset;
    }
    data = malloc(dataSize);
    if (data == NULL)
    {
        return NULL;
    }
    if (!this->stream->Read(data, dataSize))
    {
        free(data);
        return NULL;
    }
    *checksum = this->entries[entry].checksum;
    *size = dataSize;
    return data;
}

DatArchive::~DatArchive()
{
    this->Release();
}

int DatArchive::Open(char *path)
{
    if (!this->Release())
    {
        return 0;
    }
    this->stream = new FileBitStream();
    if (this->stream == NULL)
    {
        return 0;
    }
    if (!this->stream->Open(path))
    {
        SAFE_DELETE(this->stream);
        return 0;
    }
    return this->ReadTable();
}

// Not matching: the logic is the original's, the optimizer's register and
// stack slot choices differ.
// The encoded data's next byte, zero past its end, added to `checksum`
// (as FileBitStream reads them).
#define FETCH_BYTE() \
    { \
        currentByte = *cursor; \
        if (cursor - data >= size) \
        { \
            currentByte = 0; \
        } \
        else \
        { \
            cursor++; \
        } \
        checksum += currentByte; \
    }

// FileBitStream::ReadBit on the encoded data.
#define READ_BIT(bit) \
    { \
        if (mask == 0x80) \
        { \
            FETCH_BYTE(); \
        } \
        bit = currentByte & mask; \
        mask >>= 1; \
        if (mask == 0) \
        { \
            mask = 0x80; \
        } \
    }

// FileBitStream::ReadBits on the encoded data.
#define READ_BITS(value, bits) \
    { \
        DWORD bitMask = 1 << ((bits) - 1); \
        DWORD result = 0; \
        while (bitMask != 0) \
        { \
            if (mask == 0x80) \
            { \
                FETCH_BYTE(); \
            } \
            if (mask & currentByte) \
            { \
                result |= bitMask; \
            } \
            bitMask >>= 1; \
            mask >>= 1; \
            if (mask == 0) \
            { \
                mask = 0x80; \
            } \
        } \
        value = result; \
    }

void *DatArchive::Decode(int entry, char *name)
{
    BYTE dictionary[0x2000];
    int size;
    BYTE *out;
    BYTE *outCursor;
    BYTE *data;
    BYTE *cursor;
    DWORD checksum;
    DWORD expectedChecksum;
    DWORD currentByte;
    BYTE mask;
    DWORD dictionaryPos;
    int bit;
    DWORD value;
    DWORD offset;
    DWORD c;
    int i;

    if (entry >= this->numEntries)
    {
        return NULL;
    }
    if (this->stream == NULL)
    {
        return NULL;
    }
    size = this->entries[entry].size;
    out = (BYTE *)malloc(size);
    if (out == NULL)
    {
        return NULL;
    }
    outCursor = out;
    data = (BYTE *)this->ReadEncoded((DWORD *)&size, &expectedChecksum, entry);
    if (data == NULL)
    {
        free(out);
        return NULL;
    }
    cursor = data;
    mask = 0x80;
    checksum = 0;
    dictionaryPos = 1;
    memset(dictionary, 0, sizeof(dictionary));
    for (;;)
    {
        READ_BIT(bit);
        if (bit)
        {
            READ_BITS(value, 8);
            *outCursor++ = value;
            dictionary[dictionaryPos] = value;
            dictionaryPos = (dictionaryPos + 1) & 0x1fff;
        }
        else
        {
            READ_BITS(offset, 13);
            if (offset == 0)
            {
                break;
            }
            READ_BITS(value, 4);
            for (i = 0; i <= (int)value + 2; i++)
            {
                c = dictionary[(offset + i) & 0x1fff];
                *outCursor++ = c;
                dictionary[dictionaryPos] = c;
                dictionaryPos = (dictionaryPos + 1) & 0x1fff;
            }
        }
    }
    while (mask != 0x80)
    {
        READ_BIT(bit);
    }
    free(data);
    if (this->entries[entry].checksum != checksum)
    {
        free(out);
        return NULL;
    }
    return out;
}
