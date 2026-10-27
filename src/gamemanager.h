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
    // Adds the update element (priority 4) and the draw element
    // (priority 2). Returns 0, or -1 if adding fails.
    static int RegisterChain();
    static int OnUpdate(GameManager *mgr);
    // Loads the scores and sets the game up.
    static int AddedCallback(GameManager *mgr);
    // Stops the music unless unk_1824 is set, removes the game's chain
    // elements and resets the ASCII text. Returns 0.
    static int DeletedCallback(GameManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();
    // The draw element's callback: sets unk_181f to 2 while it is set.
    // Returns 1.
    static int OnDraw(GameManager *mgr);

    int unk_0000;
    int unk_0004;
    DWORD unk_0008[0x2];
    int difficulty;
    DWORD unk_0014[0x6];
    // Gui::OnUpdate skips the interface's update while it is set.
    char unk_002c;
    BYTE unk_002d[0x3];
    DWORD unk_0030[0x5f8];
    // What FireShot compares the shot tables' levels with.
    WORD power;
    WORD unk_1812;
    WORD unk_1814;
    WORD unk_1816;
    BYTE unk_1818;
    BYTE unk_1819;
    BYTE unk_181a;
    BYTE unk_181b;
    BYTE unk_181c;
    // The replay header keeps character * 2 + shotType.
    BYTE character;
    BYTE shotType;
    BYTE unk_181f;
    BYTE unk_1820;
    // Makes DrawFps measure how far the frame rate falls short.
    BYTE unk_1821;
    BYTE unk_1822[0x2];
    BYTE unk_1824;
    BYTE unk_1825[0x3];
    DWORD unk_1828[0x81];
    WORD unk_1a2c;
    WORD unk_1a2e;
    int unk_1a30;
    // The stage being played: 1 to 6, 7 for the extra stage.
    int stage;
    DWORD unk_1a38;
    // The play area: x, y, width and height.
    float arena[4];
    DWORD unk_1a4c[0x9];
    int unk_1a70;
};
extern GameManager g_GameManager;

// GameManager's statically allocated chain elements.
extern ChainElem g_GameManagerCalcChain;
extern ChainElem g_GameManagerDrawChain;
