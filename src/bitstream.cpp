// cflags: -/Od -/Op /O2 /Ob1
// BitStream, 0x43c2f0 to 0x43c409.

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
