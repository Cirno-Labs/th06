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
