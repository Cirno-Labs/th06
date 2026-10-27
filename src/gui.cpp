// cflags: /Os
// The /Os file with Gui, from 0x4172d3.

#include "unsorted.h"

void Gui::Func417314()
{
    this->impl->unk_206e = 1;
}

void Gui::Func41732c()
{
    this->impl->unk_217e = 1;
}

BOOL Gui::Func417344()
{
    return this->impl->unk_2bd8;
}

BOOL Gui::Func419572()
{
    if (this->impl->unk_2bd4 > 0)
    {
        return FALSE;
    }
    return this->impl->unk_253c >= 0;
}

BOOL Gui::Func4195a2()
{
    return this->impl->unk_253c >= 0;
}
