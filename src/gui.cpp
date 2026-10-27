// cflags: /Os
// The /Os file with Gui, from 0x4172d3.

#include "unsorted.h"

BOOL Gui::Func4172d3()
{
    return this->impl->unk_2424.spriteIndex >= 0 && this->impl->unk_2424.flags.unk13;
}

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

void Gui::StartMsg(int index)
{
    this->impl->Func41878b(index);
    g_Unknown6c6d18.unk_198 = 3;
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
