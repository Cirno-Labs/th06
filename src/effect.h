// Effects drawn with the stage's effect sprites (data/eff01.anm, ...).

#pragma once

#include "chain.h"
#include "common.h"
#include "sprite.h"
#include "timer.h"

// One effect.
struct Effect
{
    Sprite sprite;
    DWORD unk_110[0x15];
    Timer timer;
    DWORD unk_170[0x3];
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
    static int OnUpdate(EffectManager *mgr);
    static int OnDraw(EffectManager *mgr);
    // Resets and loads the stage's effect sprites: data/eff01.anm to
    // eff05.anm, stage 6 reusing eff05 and stage 7 eff04. Returns 0, or
    // -1 on failure.
    static int AddedCallback(EffectManager *mgr);
    // Frees the effect sprites. Returns 0.
    static int DeletedCallback(EffectManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();

    DWORD unk_00000[0x2];
    Effect effects[0x201];
};
ASSERT_SIZE(EffectManager, 0x2f984);
extern EffectManager g_EffectManager;

// The stage being played, at 0x69d6d4: 1 to 6, 7 for the extra stage.
extern int g_Stage;

// EffectManager's statically allocated chain elements.
extern ChainElem g_EffectCalcChain;
extern ChainElem g_EffectDrawChain;
