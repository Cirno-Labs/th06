// The title menu ("bgm/th06_01.mid"), the global at 0x6d46c0.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"

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

    Sprite sprites[0x7a];
    DWORD unk_81a0[0x26];
    ChainElem *calcChain;
    DWORD unk_823c[0x2328];
    void *unk_10edc;
    DWORD unk_10ee0[0x15];
};
ASSERT_SIZE(MainMenu, 0x10f34);
