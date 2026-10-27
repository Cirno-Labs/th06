// The file starting at 0x401000: text in the ASCII font.

#include <stdarg.h>
#include <stdio.h>

#include "ascii.h"

int AsciiText::OnDraw(AsciiText *text)
{
    text->DrawStrings();
    text->stringCount = 0;
    text->unk_6244.Func402740();
    text->unk_69bc.Func403060();
    return 1;
}

int AsciiText::OnDraw2(AsciiText *text)
{
    if (g_Unknown6c7114)
    {
        text->Func4031e0();
    }
    else
    {
        text->Func4033b0();
    }
    return 1;
}

void AsciiText::Initialize()
{
    memset(this, 0, sizeof(AsciiText));
    this->color = 0xffffffff;
    this->scaleX = 1.0f;
    this->scaleY = 1.0f;
    this->unk_0110.flags |= 0x300;
    g_SpriteEngine->InitSprite(&this->unk_0110, 0);
    g_SpriteEngine->InitSprite(&this->unk_0000, 0x20);
    this->unk_0110.pos.z = 0.1f;
    this->unk_6234 = 0;
}

int AsciiText::DeletedCallback(AsciiText *text)
{
    g_SpriteEngine->ReleaseAnm(1);
    g_SpriteEngine->ReleaseAnm(2);
    g_SpriteEngine->ReleaseAnm(3);
    return 0;
}

void AsciiText::RemoveChain()
{
    g_Chain.Remove(&g_AsciiCalcChain);
    g_Chain.Remove(&g_AsciiDrawChain);
}

void AsciiText::AddFormat(D3DXVECTOR3 *pos, char *fmt, ...)
{
    char buf[0x200];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    this->AddString(pos, buf);
    va_end(args);
}
