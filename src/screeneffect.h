// Full-screen effects such as fades, each with its own chain elements.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "timer.h"

// One effect (0x30 bytes): `type` picks its update and draw callbacks.
class ScreenEffect
{
public:
    // The update element's added callback: restarts `timer`. Returns 0.
    static int AddedCallback(ScreenEffect *effect);
    // The update element's deleted callback: removes the draw element
    // and deletes the effect. Returns 0.
    static int DeletedCallback(ScreenEffect *effect);

    int type;
    ChainElem *calcElem;
    ChainElem *drawElem;
    DWORD unk_0c;
    int alpha;
    int unk_14;
    int unk_18;
    int unk_1c;
    int unk_20;
    Timer timer;
};
ASSERT_SIZE(ScreenEffect, 0x30);
