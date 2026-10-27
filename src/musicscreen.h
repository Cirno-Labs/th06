// The music screen: lists the game's tunes with their comments and plays
// the one picked.

#pragma once

#include "ascii.h"
#include "sprite.h"
#include "unsorted.h"

// A tune in the music screen's list.
struct MusicTune
{
    // The MIDI file; Unknown6c6d18::PlayMusic finds the wave files from it.
    char path[0x40];
    char title[0x22];
    // The comment shown for the tune, 8 lines. The screen splits each
    // line into two 32-byte halves.
    char comment[8][0x42];
};
ASSERT_SIZE(MusicTune, 0x272);

// The music screen, the global at 0x6c71f0. Runs on two chain
// elements: one updates it each frame, one draws it.
class MusicScreen
{
public:
    // The update chain element's callback: runs the current state's
    // handler and counts its frames. Returns 0 while a handler asks to
    // stop, otherwise 1.
    static int OnUpdate(MusicScreen *screen);
    // The draw chain element's callback: draws the list, with the tune
    // under the cursor highlighted, and the comment. Returns 1.
    static int OnDraw(MusicScreen *screen);
    // In state 0, waits 8 frames before moving to state 1. Returns 0.
    int UpdateIntro();
    // State 1: the list of tunes. Returns nonzero to stop.
    int UpdateList();

    unsigned char unk_0000[0x8];
    // Frames spent in `state`.
    int stateFrames;
    int state;
    // The tune under the cursor.
    int cursor;
    // The tune playing.
    int playing;
    // The first tune the list shows; it shows 10.
    int listTop;
    int tuneCount;
    MusicTune *tunes;
    Sprite unk_0024;
    // One per tune in the list.
    Sprite titles[0x20];
    // Presumably the lines of the selected tune's comment.
    Sprite comment[0x10];
};
ASSERT_SIZE(MusicScreen, 0x3434);
