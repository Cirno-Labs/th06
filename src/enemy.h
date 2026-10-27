// Enemies, drawn with the stage's data/stgNenm.anm and stgNenm2.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "bullet.h"
#include "common.h"
#include "effect.h"
#include "enemyscript.h"
#include "sprite.h"
#include "timer.h"

// Enemy's flags, in bytes.
struct EnemyFlags
{
    // How the enemy moves: 1 by angle and speed, 2 towards
    // moveStart + moveDelta over moveDuration.
    unsigned char moveMode : 2;
    unsigned char unk2 : 4;
    // Mirrors the horizontal movement.
    unsigned char mirrored : 1;
    unsigned char unk7 : 1;
    unsigned char unk8 : 1;
    unsigned char unk9 : 2;
    unsigned char unk11 : 1;
    unsigned char unk12 : 1;
    unsigned char unk13 : 3;
    // Keeps the position within lowerBound and upperBound.
    unsigned char clampPos : 1;
    unsigned char unk17 : 7;
    BYTE unk_e53;
};

// One enemy (0xec8 bytes).
struct Enemy
{
    // Inline, but too large to expand: callers call the copy at 0x411240
    // and still reserve its inlines' slots.
    Enemy()
    {
    }

    Sprite sprite;
    Sprite sprites[8];
    EnemyScriptState ctx;
    // Contexts saved by calls.
    EnemyScriptState savedCtx[8];
    DWORD unk_c3c[0xc];
    D3DXVECTOR3 pos;
    DWORD unk_c78[0x3];
    D3DXVECTOR3 velocity;
    // Movement with moveMode 1.
    float angle;
    float angularVelocity;
    float speed;
    float acceleration;
    DWORD unk_ca0[0x3];
    // Movement with moveMode 2: from moveStart by moveDelta over
    // moveDuration frames, counted down by moveTimer.
    D3DXVECTOR3 moveDelta;
    D3DXVECTOR3 moveStart;
    Timer moveTimer;
    int moveDuration;
    DWORD unk_cd4[0x4];
    int unk_ce4;
    DWORD unk_ce8[0x2];
    Timer unk_cf0;
    DWORD unk_cfc;
    BulletShooter shooter;
    DWORD unk_d54;
    Timer unk_d58;
    BulletShooter laserShooter;
    DWORD unk_db8[0x23];
    Timer unk_e44;
    EnemyFlags flags;
    DWORD unk_e54[0x3];
    // The bounds of the position, with flags.clampPos.
    float lowerBound[2];
    float upperBound[2];
    // Effects that follow the enemy.
    Effect *effects[12];
    int effectCount;
    float unk_ea4;
    DWORD unk_ea8[0x5];
    Timer unk_ebc;

    void Func4121b0();
    // Keeps the position within the bounds, with flags.clampPos.
    void ClampPos();
    // Moves by the velocity, mirrored with flags.mirrored.
    void Move();
};
ASSERT_SIZE(Enemy, 0xec8);

// Moves the effects following `enemy` with it, widening them up to
// unk_ea4 and turning them.
void MoveEnemyEffects(Enemy *enemy);
// Releases the effects following `enemy`.
void ReleaseEnemyEffects(Enemy *enemy);

// Runs the enemies, the global at 0x4b79c8.
class EnemyManager
{
public:
    // Constructs the members and initializes.
    EnemyManager();

    // Sets everything up for a stage.
    void Initialize();

    // Initializes, remembers the stage's two enemy sprite files and adds
    // the update element (priority 9) and the draw element (priority 6).
    // Returns 0, or -1 if adding fails.
    static int RegisterChain(char *anmPath, char *anmPath2);
    static int OnUpdate(EnemyManager *mgr);
    static int OnDraw(EnemyManager *mgr);
    // Loads the enemy sprites (either path may be NULL) and picks
    // unk_ee5b8 and unk_ee5ba at random. Returns 0, or -1 if loading
    // fails.
    static int AddedCallback(EnemyManager *mgr);
    // Frees the enemy sprites. Returns 0.
    static int DeletedCallback(EnemyManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();

    // The stage's two enemy sprite files.
    char *anmPath;
    char *anmPath2;
    // The template new enemies start from, and the enemies.
    Enemy enemyTemplate;
    Enemy enemies[0x101];
    DWORD unk_ee598[0x8];
    // Below 3 and below 8.
    WORD unk_ee5b8;
    WORD unk_ee5ba;
    DWORD unk_ee5bc[0x2];
    int unk_ee5c4;
    int unk_ee5c8;
    DWORD unk_ee5cc[0x2];
    int unk_ee5d4;
    DWORD unk_ee5d8;
    int unk_ee5dc;
    Timer unk_ee5e0;
};
ASSERT_SIZE(EnemyManager, 0xee5ec);
extern EnemyManager g_EnemyManager;

// EnemyManager's statically allocated chain elements.
extern ChainElem g_EnemyCalcChain;
extern ChainElem g_EnemyDrawChain;
