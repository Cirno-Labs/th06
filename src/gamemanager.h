// The game in play: score, lives, stage.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"

// Runs the game in play, the global at 0x69bca0. Its added callback
// reads "score.dat" and sets up the replay ("replay/th6_00.rpy").
class GameManager
{
public:
    // The draw element's callback (the element at 0x69d740): sets
    // unk_181f to 2 while it is set. Returns 1.
    static int OnDraw(GameManager *mgr);

    DWORD unk_0000[0x606];
    BYTE unk_1818;
    BYTE unk_1819[0x6];
    BYTE unk_181f;
    BYTE unk_1820;
    // Makes DrawFps measure how far the frame rate falls short.
    BYTE unk_1821;
    BYTE unk_1822[0x2];
    DWORD unk_1824[0x83];
    int unk_1a30;
    // The stage being played: 1 to 6, 7 for the extra stage.
    int stage;
    DWORD unk_1a38;
    // The play area: x, y, width and height.
    float arena[4];
};
extern GameManager g_GameManager;
