// The music screen: lists the game's tunes with their comments and plays
// the one picked.

#pragma once

#include "sprite.h"

// The music screen, the global at 0x6c71f0. Runs on two chain
// elements: one updates it each frame, one draws it.
class MusicScreen
{
public:
    // In state 0, waits 8 frames before moving to state 1. Returns 0.
    int UpdateIntro();

    unsigned char unk_0000[0x8];
    // Frames spent in `state`.
    int stateFrames;
    int state;
    unsigned char unk_0010[0x14];
    Sprite unk_0024;
    // One per tune in the list.
    Sprite titles[0x20];
    // Presumably the lines of the selected tune's comment.
    Sprite comment[0x10];
};
ASSERT_SIZE(MusicScreen, 0x3434);
