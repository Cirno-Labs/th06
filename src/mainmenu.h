// The title menu ("bgm/th06_01.mid"), the global at 0x6d46c0.

#pragma once

#include <windows.h>

#include "common.h"
#include "sprite.h"

class MainMenu
{
public:
    // Never called: the scene set-up (Func43a338) zeroes the menu with
    // memset instead.
    MainMenu();

    Sprite sprites[0x7a];
    DWORD unk_81a0[0x2365];
};
ASSERT_SIZE(MainMenu, 0x10f34);
