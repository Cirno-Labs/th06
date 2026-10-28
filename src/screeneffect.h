// Full-screen effects such as fades, each with its own chain elements.

#pragma once

#include <windows.h>
#include <d3d8.h>

#include "chain.h"
#include "common.h"
#include "timer.h"

// One effect (0x30 bytes): `type` picks its update and draw callbacks.
class ScreenEffect
{
public:
    // Creates an effect of `type` with its chain elements.
    static ScreenEffect *Func42fd30(int type, int a, int b, int c, int d);
    // The update element's added callback: restarts `timer`. Returns 0.
    static int AddedCallback(ScreenEffect *effect);
    // The update element's deleted callback: removes the draw element
    // and deletes the effect. Returns 0.
    static int DeletedCallback(ScreenEffect *effect);
    // Type 0's update callback: fades `alpha` from 255 to 0 over
    // `duration` frames. Returns 0 when done, else 1.
    static int OnUpdateFadeIn(ScreenEffect *effect);
    // Type 2's update callback: fades `alpha` from 0 to 255 over
    // `duration` frames. Returns 0 when done, else 1.
    static int OnUpdateFadeOut(ScreenEffect *effect);
    // Type 2's draw callback: covers the play area in `color` at `alpha`.
    // Returns 1.
    static int OnDrawPlayArea(ScreenEffect *effect);

    int type;
    ChainElem *calcElem;
    ChainElem *drawElem;
    DWORD unk_0c;
    int alpha;
    // How many frames the effect lasts.
    int duration;
    D3DCOLOR color;
    int unk_1c;
    int unk_20;
    Timer timer;
};
ASSERT_SIZE(ScreenEffect, 0x30);
