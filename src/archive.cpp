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
