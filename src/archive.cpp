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
