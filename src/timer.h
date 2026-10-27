// Frame counters that advance at the game's speed.

#pragma once

#include "common.h"

// Counts frames. When the game runs slower than full speed, frames advance
// in fractions: `subFrame` carries the part of a frame not yet counted.
struct Timer
{
    // Starts counting from 0.
    void Initialize();
    // Advances by `frames` frames, or by the matching fraction of them
    // while the game runs slow.
    void Add(int frames);
    // Goes back by `frames` frames, like Add.
    void Sub(int frames);

    // `current` before the last change.
    int previous;
    float subFrame;
    int current;
};
ASSERT_SIZE(Timer, 0xc);
