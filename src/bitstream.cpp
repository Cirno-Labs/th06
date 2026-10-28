// cflags: -/Od -/Op /O2 /Ob1
// The bit streams, 0x43c2f0 to 0x43c6d8: BitStream and FileBitStream.

#include "archive.h"

void BitStream::Reset()
{
    this->mask = 0x80;
    this->position = 0;
    this->size = 0;
    this->currentByte = 0;
    this->checksum = 0;
}

DWORD BitStream::ReadVarInt()
{
    DWORD result = 0;
    DWORD size = 0;
    DWORD bit;

    if (this->ReadBit())
    {
        size = 2;
    }
    if (this->ReadBit())
    {
        size |= 1;
    }
    switch (size)
    {
    case 0:
        bit = 0x80;
        break;
    case 1:
        bit = 0x8000;
        break;
    case 2:
        bit = 0x800000;
        break;
    case 3:
        bit = 0x80000000;
        break;
    default:
        goto done;
    }
    do
    {
        if (this->ReadBit())
        {
            result |= bit;
        }
        bit >>= 1;
    } while (bit != 0);
done:
    return result;
}

DWORD BitStream::ReadU32()
{
    DWORD a = this->ReadBits(8);
    DWORD b = (this->ReadBits(8) << 8) + a;
    DWORD c = (this->ReadBits(8) << 16) + b;
    return (this->ReadBits(8) << 24) + c;
}

int BitStream::ReadString(char *buffer, DWORD maxLength)
{
    DWORD i;

    if (buffer == NULL)
    {
        return 0;
    }
    for (i = 0; i < maxLength; i++)
    {
        buffer[i] = this->ReadBits(8);
        if (buffer[i] == '\0')
        {
            return 1;
        }
    }
    return 0;
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
