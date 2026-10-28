// cflags: -/Od -/Op /O2 /Ob1
// The file class, 0x43ce40 to 0x43d158.

#include "archive.h"

File::File()
{
    this->handle = INVALID_HANDLE_VALUE;
    this->access = 0;
}
