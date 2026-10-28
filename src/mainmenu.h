// The title menu ("bgm/th06_01.mid"), the global at 0x6d46c0.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "replay.h"
#include "sprite.h"
#include "unsorted.h"

class MainMenu
{
public:
    // The update element's callback.
    static int OnUpdate(MainMenu *menu);
    // The update element's added callback: loads the title music, puts
    // the cursor on the entry the previous scene came from, sets the
    // colors, reloads the clears and practice scores from "score.dat"
    // and fades in (playing the music the first time). Returns 0.
    static int AddedCallback(MainMenu *menu);
    // The draw element's callback.
    static int OnDraw(MainMenu *menu);
    // Never called: the scene set-up (SetUpTitleScene) zeroes the menu with
    // memset instead.
    MainMenu();

    // Releases the menu's sprite files (slots 0x1b to 0x24 and those
    // of Func43a70a) and surface 0, forgets their scripts, removes the
    // calc element and frees unk_10edc. Returns 0.
    static int DeletedCallback(MainMenu *menu);
    // Sets the menu up with LoadTitleResources (on failure sets the scene to 4
    // and returns -1); the first time, waits until 3 seconds after the
    // game started and plays "bgm/th06_01.mid". Then resets every
    // sprite's interrupt, color and offset and sets unk_81f0. Returns 0.
    int Func4378a2();
    // After 30 frames, on a press of a confirming button (0x160b):
    // moves to state 2, interrupts every sprite with 2 and marks the
    // entry at the cursor. Returns 0 then, else 1.
    int Func4379e4();
    // In state 17 (practice), lists the practice high scores of the
    // stages the shot type reached on this difficulty, fading in over
    // 30 frames, the one at the cursor brighter. Returns 0.
    int DrawPracticeScores();
    // In the replay menu (states 13 to 15), lists the page of 15 replays
    // around the cursor (unk_81e8), the one at the cursor highlighted, and
    // in state 15 the picked replay's slowdown and the score each stage
    // ended with, the stage at `cursor` highlighted. Returns 0.
    int DrawReplayMenu();

    Sprite sprites[0x7a];
    // The selected entry.
    int cursor;
    DWORD unk_81a4[0x10];
    int unk_81e4;
    // The replay menu's cursor and the number of replays listed.
    int unk_81e8;
    int unk_81ec;
    int unk_81f0;
    // Counts the frames in the current state (unk_81f0).
    int unk_81f4;
    DWORD unk_81f8;
    int unk_81fc;
    D3DCOLOR unk_8200;
    // The colors HighlightMenuEntry gives the selected entry and the
    // others.
    D3DCOLOR unk_8204;
    D3DCOLOR unk_8208;
    int unk_820c;
    int unk_8210;
    int unk_8214;
    DWORD unk_8218;
    // The button mapping the options screen edits.
    ControllerMapping controllerMapping;
    short unk_822e;
    DWORD unk_8230;
    // The update element (priority 2, with the added and deleted
    // callbacks) and the draw element (priority 0), created by
    // SetUpTitleScene; DeletedCallback removes the draw element.
    ChainElem *calcChain;
    ChainElem *drawChain;
    DWORD unk_823c[0x1e00];
    // The replay menu's entries: the number shown for each, and the
    // replays' headers.
    char replayLabels[60][8];
    ReplayHeader replays[60];
    // The replay picked in the replay menu.
    ReplayHeader *unk_10edc;
    DWORD unk_10ee0[0x12];
    int unk_10f28;
    int unk_10f2c;
    int unk_10f30;
};
ASSERT_SIZE(MainMenu, 0x10f34);
extern MainMenu g_MainMenu;
// The names the replay menu shows for stages, shot types and difficulties.
extern char *g_MenuStageNames[7];
extern char *g_MenuShotTypeNames[4];
extern char *g_MenuDifficultyNames[5];

// Moves the menu's cursor up or down (wrapping among `count` entries)
// when up or down is pressed or repeats, with sound 12. Returns -1 for
// up, 1 for down, else 0.
int MoveMenuCursor(MainMenu *menu, int count);

// Gives every button of menu->controllerMapping assigned `button` the
// value `replacement`, skipping entries 0 and 2 when `unk` is set.
void ReassignButton(MainMenu *menu, short button, short replacement, int unk);

// Highlights menu entry sprite `vm` (number `index`) when it is the
// one at `cursor`: `selectedColor` and a (-4, -4) offset, else `color`
// and no offset. Where colors are not blended (HasFlag0Or8), it shows
// sprite unk_0b2 + `spriteOffset` (selected) or unk_0b2 in white
// instead, keeping the color's alpha.
void HighlightMenuEntry(Sprite *vm, int index, int cursor, D3DCOLOR selectedColor, D3DCOLOR color, int spriteOffset);

// Loads the title's archive ("紅魔郷TL.dat"), sprites (slots 0x15 to
// 0x1a, after freeing 0x1b to 0x24) and background
// ("data/title/title00.jpg") and starts scripts 0x100 to 0x14f on the
// first 0x50 sprites. Returns 0, or -1 if a file fails to load.
int LoadTitleResources(MainMenu *menu);

// Frees the title's sprites (slots 0x15 to 0x1a) and loads the replay
// menu's background ("data/title/select00.jpg") and sprites (slot 0x24),
// starting scripts 0x160 to 0x179 on sprites 0x60 on. Returns 0, or -1
// if a file fails to load.
int LoadReplayMenuResources(MainMenu *menu);

// Frees the title's sprites (slots 0x15 to 0x1a) and loads the
// difficulty and character select's background and sprites (slots 0x1b
// to 0x23), starting scripts 0x150 to 0x15f on sprites 0x50 on. Returns
// 0, or -1 if a file fails to load.
int LoadSelectResources(MainMenu *menu);
