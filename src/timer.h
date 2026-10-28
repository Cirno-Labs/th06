// Frame counters that advance at the game's speed.

#pragma once

#include "common.h"

// Counts frames. When the game runs slower than full speed, frames advance
// in fractions: `subFrame` carries the part of a frame not yet counted.
struct Timer
{
    Timer()
    {
        this->Initialize();
    }

    // Starts counting from 0.
    void Initialize();
    // Advances by `frames` frames, or by the matching fraction of them
    // while the game runs slow.
    void Add(int frames);
    // Goes back by `frames` frames, like Add.
    void Sub(int frames);
    // Sets the count to `frames`, with no fraction and no previous count
    // (-999).
    void Set(int frames)
    {
        this->current = frames;
        this->subFrame = 0.0f;
        this->previous = -999;
    }
    void operator+=(int frames)
    {
        this->Add(frames);
    }
    // Advances by one frame through Unknown6c6d18::TickTimer. Inline;
    // defined in unsorted.h.
    void Tick();
    // Tick, out of line in the Gui file, returning the new count.
    int NextTick();
    void operator++()
    {
        this->NextTick();
    }

    // The count with its fraction.
    float AsFloat()
    {
        return (float)this->current + this->subFrame;
    }
    // The count without its fraction.
    int AsFrames()
    {
        return this->current;
    }
    // Whether the count changed since the last Tick or Set.
    BOOL HasTicked()
    {
        return this->current != this->previous;
    }
    BOOL operator==(int frames)
    {
        return this->current == frames;
    }
    BOOL operator<(int frames)
    {
        return this->current < frames;
    }
    BOOL operator>=(int frames)
    {
        return this->current >= frames;
    }
    BOOL operator>(int frames)
    {
        return this->current > frames;
    }
    BOOL operator<=(int frames)
    {
        return this->current <= frames;
    }

    // `current` before the last change.
    int previous;
    float subFrame;
    int current;
};
ASSERT_SIZE(Timer, 0xc);
