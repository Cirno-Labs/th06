// The file starting at 0x401000: text in the ASCII font.

#include <stdarg.h>
#include <stdio.h>

#include "ascii.h"
#include "unsorted.h"

int AsciiText::OnUpdate(AsciiText *text)
{
    int i;
    Unknown7134 *entry;

    if (!g_Unknown69d4bf && !g_Unknown69d4c0)
    {
        for (entry = text->unk_7134, i = 0; i < 0x203; i++, entry++)
        {
            if (!entry->active)
            {
                continue;
            }
            entry->unk_0c -= 0.5f * g_Unknown6c6d18.frameAdvance;
            entry->timer.Tick();
            if ((entry->timer.current > 60) != 0)
            {
                entry->active = 0;
            }
        }
    }
    else if (g_Unknown69d4bf)
    {
        text->unk_6244.Func401b80();
    }
    if (g_Unknown69d4c0)
    {
        text->unk_69bc.Func402870();
    }
    return 1;
}

int AsciiText::RegisterChain()
{
    AsciiText *text = &g_AsciiText;

    g_AsciiCalcChain.SetCallback((ChainCallback)AsciiText::OnUpdate);
    g_AsciiCalcChain.addedCallback = (ChainCallback)AsciiText::AddedCallback;
    g_AsciiCalcChain.deletedCallback = (ChainCallback)AsciiText::DeletedCallback;
    g_AsciiCalcChain.arg = text;
    if (g_Chain.AddToCalcChain(&g_AsciiCalcChain, 1) != 0)
    {
        return -1;
    }
    g_AsciiDrawChain.SetCallback((ChainCallback)AsciiText::OnDraw);
    g_AsciiDrawChain.arg = text;
    g_Chain.AddToDrawChain(&g_AsciiDrawChain, 0xf);
    g_AsciiDrawChain2.SetCallback((ChainCallback)AsciiText::OnDraw2);
    g_AsciiDrawChain2.arg = text;
    g_Chain.AddToDrawChain(&g_AsciiDrawChain2, 10);
    return 0;
}

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

int AsciiText::AddedCallback(AsciiText *text)
{
    if (g_SpriteEngine->LoadAnm(1, "data/ascii.anm", 0) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(2, "data/asciis.anm", 0x77) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(3, "data/capture.anm", 0x718) != 0)
    {
        return -1;
    }
    text->Initialize();
    return 0;
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

void AsciiText::AddString(D3DXVECTOR3 *pos, char *text)
{
    AsciiString *str;

    if (this->stringCount >= 0x100)
    {
        return;
    }
    str = &this->strings[this->stringCount];
    this->stringCount++;
    strcpy(str->text, text);
    str->pos = *pos;
    str->color = this->color;
    str->scaleX = this->scaleX;
    str->scaleY = this->scaleY;
    str->unk_5c = this->unk_6230;
    if (g_Unknown6c6d18.HasFlag0Or8())
    {
        str->unk_58 = this->unk_6234;
    }
    else
    {
        str->unk_58 = 0;
    }
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
