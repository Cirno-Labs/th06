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
    // Ends every bullet and laser: with `unk` zero they fade out, with it
    // nonzero each bullet is replaced by Func41f290(position, 6, 1).
    void Func414160(int unk);
    // Func414160(1).
    void ClearAllToItems();

    // Resets, remembers `unk`, picks g_Unknown476438's table by option
    // bit 0, and adds the update element (priority 0xb) and the draw
    // element (priority 9). Returns 0, or -1 if adding fails.
    static int RegisterChain(int unk);
    static int OnUpdate(BulletManager *mgr);
    static int OnDraw(BulletManager *mgr);
    // Loads the bullet sprites.
    static int AddedCallback(BulletManager *mgr);
    // Frees the bullet sprites, unless the scene is 3. Returns 0.
    static int DeletedCallback(BulletManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();

    DWORD unk_00000[0x3d705];
    int unk_f5c14;
};
ASSERT_SIZE(BulletManager, 0xf5c18);
extern BulletManager g_BulletManager;

// Two tables; BulletManager::RegisterChain points g_Unknown476438 at the
// first while option bit 0 is clear, otherwise at the second.
extern DWORD g_Unknown476358[0x1c];
extern DWORD g_Unknown4763c8[0x1c];
extern DWORD *g_Unknown476438;

// BulletManager's statically allocated chain elements.
extern ChainElem g_BulletCalcChain;
extern ChainElem g_BulletDrawChain;
