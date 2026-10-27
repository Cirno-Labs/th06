// Text drawn in the ASCII font (data/ascii.anm).

#pragma once

#include <d3dx8.h>

#include "common.h"

// Queues strings to draw in the ASCII font, the global at 0x47b900. Its
// code starts at 0x401000 and loads "data/ascii.anm".
class AsciiText
{
public:
    // Removes AsciiText's two chain elements.
    static void RemoveChain();

    // Queues a string at `pos` in the current color. Up to 256 strings
    // a frame; more are dropped.
    void AddString(D3DXVECTOR3 *pos, char *text);
    // Like AddString, for a printf-style text of up to 512 bytes.
    void AddFormat(D3DXVECTOR3 *pos, char *fmt, ...);

    unsigned char unk_0000[0x6224];
    // The color of the strings queued next.
    D3DCOLOR color;
};
extern AsciiText g_AsciiText;
