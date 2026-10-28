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
    // Type 1's update callback: shakes the play area, moving its edges
    // in at random by from shakeFrom to shakeTo pixels over `duration`
    // frames, unless g_GameManager.unk_002c is set. Returns 0 when done
    // (with the play area back in place), else 1.
    static int OnUpdateShake(ScreenEffect *effect);
    // Type 2's update callback: fades `alpha` from 0 to 255 over
    // `duration` frames. Returns 0 when done, else 1.
    static int OnUpdateFadeOut(ScreenEffect *effect);
    // Type 0's draw callback: covers the whole screen in `color` at
    // `alpha`. Returns 1.
    static int OnDrawScreen(ScreenEffect *effect);
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
    union
    {
        // Types 0 and 2: the color faded.
        D3DCOLOR color;
        // Type 1: how far the play area shakes at the start, and at the
        // end.
        int shakeFrom;
    };
    int shakeTo;
    int unk_20;
    Timer timer;
};
ASSERT_SIZE(ScreenEffect, 0x30);
