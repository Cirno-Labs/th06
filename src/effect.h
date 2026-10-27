// Effects drawn with the stage's effect sprites (data/eff01.anm, ...).

#pragma once

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

    DWORD unk_00000[0x2];
    Effect effects[0x201];
};
ASSERT_SIZE(EffectManager, 0x2f984);
extern EffectManager g_EffectManager;
