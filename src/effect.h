// Effects drawn with the stage's effect sprites (data/eff01.anm, ...).

#pragma once

#include "chain.h"
#include "common.h"
#include "gamemanager.h"
#include "sprite.h"
#include "timer.h"

// One effect.
struct Effect
{
    Sprite sprite;
    D3DXVECTOR3 pos;
    DWORD unk_11c[0x6];
    // For an effect following an enemy: the enemy's position, and its
    // radius and angle around it.
    D3DXVECTOR3 unk_134;
    DWORD unk_140[0x7];
    float unk_15c;
    float unk_160;
    Timer timer;
    DWORD unk_170;
    // Called every frame while set; the effect ends when it returns
    // anything but 1.
    int (*update)(Effect *effect);
    char active;
    char unk_179;
    // Set to end an effect that followed an enemy.
    char unk_17a;
    char unk_17b;
};
ASSERT_SIZE(Effect, 0x17c);

// Runs the effects, the global at 0x487fe0. Its update callback loads
// "data/effNN.anm" for the stage.
class EffectManager
{
public:
    // Constructs the effects, then zeroes everything.
    EffectManager();

    // Zeroes everything.
    void Reset();

    // Resets and adds the update element (priority 10) and the draw
    // element (priority 8). Returns 0, or -1 if adding fails.
    static int RegisterChain();
    // Runs the active effects (only the first 0x200): their update
    // callbacks and scripts, which end them by returning nonzero, and
    // their Timers. Returns 1.
    static int OnUpdate(EffectManager *mgr);
    // Draws the active effects at their positions (only the first 0x200).
    // Returns 1.
    static int OnDraw(EffectManager *mgr);
    // Resets and loads the stage's effect sprites: data/eff01.anm to
    // eff05.anm, stage 6 reusing eff05 and stage 7 eff04. Returns 0, or
    // -1 on failure.
    static int AddedCallback(EffectManager *mgr);
    // Frees the effect sprites. Returns 0.
    static int DeletedCallback(EffectManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();
    // Starts effect `type` at `pos`.
    Effect *Func40ef50(int type, D3DXVECTOR3 *pos, int unk, D3DCOLOR color);

    DWORD unk_00000;
    // How many effects were active this frame.
    int activeCount;
    Effect effects[0x201];
};
ASSERT_SIZE(EffectManager, 0x2f984);
extern EffectManager g_EffectManager;

// EffectManager's statically allocated chain elements.
extern ChainElem g_EffectCalcChain;
extern ChainElem g_EffectDrawChain;
