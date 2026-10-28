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
