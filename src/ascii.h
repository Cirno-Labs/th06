// Text drawn in the ASCII font (data/ascii.anm).

#pragma once

#include <d3dx8.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"

// Queues strings to draw in the ASCII font, the global at 0x47b900. Its
// code starts at 0x401000 and loads "data/ascii.anm".
class AsciiText
{
public:
    // Adds AsciiText's update and draw chain elements. Returns 0, or -1
    // if adding fails.
    static int RegisterChain();
    // The second draw element's callback (priority 10): runs Func4031e0
    // while g_Unknown6c7114 is set, otherwise Func4033b0. Returns 1.
    static int OnDraw2(AsciiText *text);
    void Func4031e0();
    void Func4033b0();

    // The update element's deleted callback: frees the ANM files the
    // added callback loaded. Returns 0.
    static int DeletedCallback(AsciiText *text);
    // Removes AsciiText's update element and its first draw element;
    // the second draw element stays.
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

extern BYTE g_Unknown6c7114;

// AsciiText's chain elements, statically allocated: the update element,
// and draw elements at priorities 0xf and 10.
extern ChainElem g_AsciiCalcChain;
extern ChainElem g_AsciiDrawChain;
extern ChainElem g_AsciiDrawChain2;
