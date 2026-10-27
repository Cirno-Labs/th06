// The file starting at 0x401000: text in the ASCII font.

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
