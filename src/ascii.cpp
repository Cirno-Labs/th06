// The file starting at 0x401000: text in the ASCII font.

#include "ascii.h"

void AsciiText::RemoveChain()
{
    g_Chain.Remove(&g_AsciiCalcChain);
    g_Chain.Remove(&g_AsciiDrawChain);
}
