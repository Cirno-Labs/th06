// Enemies, drawn with the stage's data/stgNenm.anm and stgNenm2.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"

// Runs the enemies, the global at 0x4b79c8.
class EnemyManager
{
public:
    // Sets everything up for a stage.
    void Initialize();

    // Initializes, remembers the stage's two enemy sprite files and adds
    // the update element (priority 9) and the draw element (priority 6).
    // Returns 0, or -1 if adding fails.
    static int RegisterChain(char *anmPath, char *anmPath2);
    static int OnUpdate(EnemyManager *mgr);
    static int OnDraw(EnemyManager *mgr);
    // Loads the enemy sprites.
    static int AddedCallback(EnemyManager *mgr);
    // Frees the enemy sprites. Returns 0.
    static int DeletedCallback(EnemyManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();

    // The stage's two enemy sprite files.
    char *anmPath;
    char *anmPath2;
    DWORD unk_00008[0x3b979];
};
ASSERT_SIZE(EnemyManager, 0xee5ec);
extern EnemyManager g_EnemyManager;

// EnemyManager's statically allocated chain elements.
extern ChainElem g_EnemyCalcChain;
extern ChainElem g_EnemyDrawChain;
