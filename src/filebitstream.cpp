// cflags: -/Od -/Op /O2 /Ob1
// FileBitStream, 0x43c410 to 0x43c76e: its own object in the original,
// which starts with BitStream's inline destructors (/O2 emits the inline
// functions a function needs before it). The constructor is out of
// line; Close's copy comes after Open (see archive.h). The destructor is
// out of line too and defined here, after the constructor: cl then
// emits it at the end, between ReadByte's adjustor thunk and the scalar
// deleting destructor, where the original has it (an inline one comes
// after the scalar deleting destructor).

#define FBS_CLOSE_LATE
#include "archive.h"

FileBitStream::FileBitStream()
{
}

FileBitStream::~FileBitStream()
{
    this->Close();
}

int FileBitStream::Open(char *path)
{
    this->Close();
    this->Reset();
    if (!this->File::Open(path, "r"))
    {
        return 0;
    }
    this->size = GetFileSize(this->handle, NULL);
    return 1;
}

inline void FileBitStream::Close()
{
    this->File::Close();
    this->Reset();
}

int FileBitStream::ReadBit()
{
    int bit;

    if (this->handle == NULL)
    {
        return 0;
    }
    if (this->mask == 0x80)
    {
        this->currentByte = this->File::ReadByte();
        if (this->currentByte == -1)
        {
            return 0;
        }
        this->position++;
        this->checksum += this->currentByte;
    }
    bit = this->currentByte & this->mask;
    this->mask >>= 1;
    if (this->mask == 0)
    {
        this->mask = 0x80;
    }
    return bit != 0;
}

DWORD FileBitStream::ReadBits(int bits)
{
    DWORD bit = 1 << (bits - 1);
    DWORD result = 0;

    if (this->handle == NULL)
    {
        return 0;
    }
    while (bit != 0)
    {
        if (this->mask == 0x80)
        {
            this->currentByte = this->File::ReadByte();
            if (this->currentByte == -1)
            {
                return 0;
            }
            this->position++;
            this->checksum += this->currentByte;
        }
        if (this->mask & this->currentByte)
        {
            result |= bit;
        }
        bit >>= 1;
        this->mask >>= 1;
        if (this->mask == 0)
        {
            this->mask = 0x80;
        }
    }
    return result;
}

int FileBitStream::ReadByte()
{
    if (this->position < this->size)
    {
        this->position++;
    }
    return this->File::ReadByte();
}

int FileBitStream::Seek(DWORD offset)
{
    if (offset >= this->size)
    {
        return 0;
    }
    if (!this->AlignToByte())
    {
        return 0;
    }
    if (!this->File::Seek(offset, FILE_BEGIN))
    {
        return 0;
    }
    this->position = offset;
    this->checksum = 0;
    return 1;
}

int FileBitStream::AlignToByte()
{
    if (this->handle == NULL)
    {
        return 0;
    }
    while (this->mask != 0x80)
    {
        this->ReadBit();
    }
    return 1;
}

int FileBitStream::Read(void *buffer, DWORD size)
{
    DWORD read;

    this->AlignToByte();
    return this->File::Read(buffer, size, &read);
}

int FileBitStream::GetLastWriteTime(FILETIME *time)
{
    if (this->handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return GetFileTime(this->handle, NULL, NULL, time);
}
