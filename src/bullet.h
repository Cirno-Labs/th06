// Enemy bullets, drawn with data/etama3.anm and etama4.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"
#include "timer.h"

// Runs the enemy bullets, the global at 0x5a5ff8.
// One enemy bullet.
struct Bullet
{
    DWORD unk_000[0x158];
    D3DXVECTOR3 pos;
    DWORD unk_56c[0x14];
    WORD unk_5bc;
    // 0 unused; 5 fading out.
    WORD state;
    DWORD unk_5c0;
};
ASSERT_SIZE(Bullet, 0x5c4);

// One enemy laser: a beam from `pos` along `angle`, between startOffset
// and endOffset.
struct Laser
{
    DWORD unk_000[0x88];
    D3DXVECTOR3 pos;
    float angle;
    float startOffset;
    float endOffset;
    DWORD unk_238[0x7];
    int unk_254;
    int active;
    Timer timer;
    DWORD unk_268;
    // 2 and up: ending.
    BYTE state;
    BYTE unk_26d[0x3];
};
ASSERT_SIZE(Laser, 0x270);

class BulletManager
{
public:
    // Zeroes everything.
    void Reset();
    // Ends every bullet and laser: with `toItems` zero they fade out;
    // with it nonzero bullets turn into items of type 6, as do points
    // every 32 along each laser.
    void ClearAll(int toItems);
    // ClearAll(1).
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

    DWORD unk_00000[0x1580];
    Bullet bullets[0x280];
    Laser lasers[0x40];
    DWORD unk_f5c00[0x5];
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
