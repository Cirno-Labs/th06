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
    // Sets the menu up with Func439da0 (on failure sets the scene to 4
    // and returns -1); the first time, waits until 3 seconds after the
    // game started and plays "bgm/th06_01.mid". Then resets every
    // sprite's interrupt, color and offset and sets unk_81f0. Returns 0.
    int Func4378a2();

    Sprite sprites[0x7a];
    // The selected entry.
    int cursor;
    DWORD unk_81a4[0x13];
    int unk_81f0;
    DWORD unk_81f4[0xa];
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

int Func439da0(MainMenu *menu);
