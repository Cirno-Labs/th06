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
    // Zeroes the manager and sets the play area to (32, 16), 384 by 448.
    GameManager();

    // Adds `amount` hundredths to the rank, up to maxRank.
    void AddSubRank(int amount);
    // Takes `amount` hundredths from the rank, down to minRank.
    void SubSubRank(int amount);

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
    DWORD unk_0014[0x4];
    // Counts the bombs used.
    int bombsUsed;
    DWORD unk_0028;
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
    // Lives and bombs left.
    char lives;
    char bombs;
    BYTE unk_181c;
    // The replay header keeps character * 2 + shotType.
    BYTE character;
    BYTE shotType;
    BYTE unk_181f;
    BYTE unk_1820;
    // Makes DrawFps measure how far the frame rate falls short.
    BYTE unk_1821;
    BYTE unk_1822;
    BYTE unk_1823;
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
    // A level from minRank to maxRank, moved in hundredths through
    // subRank by AddSubRank and SubSubRank.
    int rank;
    int maxRank;
    int minRank;
    int subRank;
};
ASSERT_SIZE(GameManager, 0x1a80);
extern GameManager g_GameManager;

// The 3D camera the stage backgrounds are seen through (0x69d6fc).
struct Camera
{
    // The camera's distance from the screen plane, set by SetupCamera.
    float distance;
    // Moves the point looked at from the center of the viewport.
    float offsetX;
    float offsetY;
    // Scales the camera's distance.
    float zoom;
};
extern Camera g_Camera;

// Looks at the viewport's center from a distance where a 30° field of
// view covers its height, and sets the view and projection matrices,
// with the far plane at 10000 + `extraFar`.
void SetupCamera(float extraFar);

// GameManager's statically allocated chain elements.
extern ChainElem g_GameManagerCalcChain;
extern ChainElem g_GameManagerDrawChain;
