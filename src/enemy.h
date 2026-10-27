// Enemies, drawn with the stage's data/stgNenm.anm and stgNenm2.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "bullet.h"
#include "common.h"
#include "effect.h"
#include "sprite.h"
#include "timer.h"

// A context of an enemy's script, with its clock.
struct EnemyContext
{
    DWORD unk_00;
    Timer timer;
    DWORD unk_10[0xf];
};
ASSERT_SIZE(EnemyContext, 0x4c);

// Enemy's flags, in bytes.
struct EnemyFlags
{
    unsigned char unk0 : 6;
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
    Sprite sprite;
    Sprite sprites[8];
    EnemyContext ctx;
    EnemyContext savedCtx[8];
    DWORD unk_c3c[0xc];
    D3DXVECTOR3 pos;
    DWORD unk_c78[0x3];
    D3DXVECTOR3 velocity;
    DWORD unk_c90[0xd];
    Timer unk_cc4;
    DWORD unk_cd0[0x8];
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
    DWORD unk_00008[0x3b2];
    DWORD unk_00ed0[0x3b5ba];
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
    DWORD unk_ee5e0[0x3];
};
ASSERT_SIZE(EnemyManager, 0xee5ec);
extern EnemyManager g_EnemyManager;

// EnemyManager's statically allocated chain elements.
extern ChainElem g_EnemyCalcChain;
extern ChainElem g_EnemyDrawChain;
