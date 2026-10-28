// The game in play: score, lives, stage.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "resultscreen.h"

// Runs the game in play, the global at 0x69bca0. Its added callback
// reads "score.dat" and sets up the replay ("replay/th6_00.rpy").
class GameManager
{
public:
    // Zeroes the manager and sets the play area to (32, 16), 384 by 448.
    GameManager();

    // Whether a `width` by `height` rectangle centred on (x, y) touches
    // the play area.
    BOOL Func41b5e1(float x, float y, float width, float height);
    // Inline; `points` is an int and `score` unsigned, so even a
    // constant goes through a temporary.
    void AddScore(int points)
    {
        this->score += points;
    }
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

    // The score shown, and the score, which it counts up to.
    int unk_0000;
    unsigned int score;
    DWORD unk_0008[0x2];
    int difficulty;
    // Grazes, up to 9999 and 999999.
    int graze;
    int grazeTotal;
    DWORD unk_001c;
    // Counts the player's deaths.
    int deaths;
    // Counts the bombs used.
    int bombsUsed;
    DWORD unk_0028;
    // Gui::OnUpdate skips the interface's update while it is set.
    char unk_002c;
    BYTE unk_002d[0x3];
    // The score file's spell card, clear and practice chunks
    // (SaveScoreDat).
    Catk catks[0x40];
    Clrd clrds[4];
    Pscr pscrs[4 * 6 * 4];
    // What FireShot compares the shot tables' levels with.
    WORD power;
    WORD unk_1812;
    // Point items collected.
    WORD pointItems;
    WORD pointItemsTotal;
    BYTE unk_1818;
    // Power items collected in a row at full power, up to 30.
    char powerItemCombo;
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
    // Where the player may move: x, y, width and height.
    float playerArea[4];
    DWORD unk_1a5c[0x5];
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
