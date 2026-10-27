// Enemy bullets, drawn with data/etama3.anm and etama4.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"

// Runs the enemy bullets, the global at 0x5a5ff8.
class BulletManager
{
public:
    // Zeroes everything.
    void Reset();

    DWORD unk_00000[0x3d706];
};
ASSERT_SIZE(BulletManager, 0xf5c18);
extern BulletManager g_BulletManager;
