// Enemies, drawn with the stage's data/stgNenm.anm and stgNenm2.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"

// Runs the enemies, the global at 0x4b79c8.
class EnemyManager
{
public:
    // Frees the enemy sprites. Returns 0.
    static int DeletedCallback(EnemyManager *mgr);

    // The stage's two enemy sprite files.
    char *anmPath;
    char *anmPath2;
    DWORD unk_00008[0x3b979];
};
ASSERT_SIZE(EnemyManager, 0xee5ec);
extern EnemyManager g_EnemyManager;
