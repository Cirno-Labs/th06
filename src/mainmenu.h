// The title menu ("bgm/th06_01.mid"), the global at 0x6d46c0.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"
#include "unsorted.h"

class MainMenu
{
public:
    // Never called: the scene set-up (Func43a338) zeroes the menu with
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

    Sprite sprites[0x7a];
    // The selected entry.
    int cursor;
    DWORD unk_81a4[0x13];
    int unk_81f0;
    // Counts the frames in the current state (unk_81f0).
    int unk_81f4;
    DWORD unk_81f8;
    int unk_81fc;
    D3DCOLOR unk_8200;
    DWORD unk_8204[0x2];
    int unk_820c;
    int unk_8210;
    DWORD unk_8214[0x2];
    // The button mapping the options screen edits.
    ControllerMapping controllerMapping;
    short unk_822e;
    DWORD unk_8230[0x2];
    ChainElem *calcChain;
    DWORD unk_823c[0x2328];
    void *unk_10edc;
    DWORD unk_10ee0[0x15];
};
ASSERT_SIZE(MainMenu, 0x10f34);

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
