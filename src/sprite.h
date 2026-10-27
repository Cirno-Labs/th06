// Sprites animated by ANM scripts.

#pragma once

#include "common.h"

// The state of one sprite's animation script. Constructed at 0x403720,
// which sets up its Timers.
class Sprite
{
public:
    Sprite();

    unsigned char unk_000[0x110];
};
ASSERT_SIZE(Sprite, 0x110);

// Runs and draws Sprites.
class SpriteEngine
{
public:
    // Empty, and defined out of line: `delete` calls it, then operator
    // delete.
    ~SpriteEngine();

    // Runs a sprite's script for one frame.
    int Step(Sprite *vm);
};
extern SpriteEngine *g_SpriteEngine;
