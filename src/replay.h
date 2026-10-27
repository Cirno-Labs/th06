// Replays: the inputs recorded frame by frame ("error : replay.cpp").

#pragma once

#include <windows.h>

#include "common.h"

// One recorded change of the inputs.
struct ReplayFrame
{
    int frame;
    WORD input;
    WORD unk_06;
};

// Records or plays a replay; g_ReplayManager points at it while one is
// in use.
class ReplayManager
{
public:
    // Ends the current stage's record with an entry at the current frame
    // and an end marker (frame 9999999), and keeps where it ends.
    static void EndStage();
    // The update callback while recording: while
    // g_GameManager.unk_1821 is set, adds an entry whenever the buttons
    // (masked with 0x1f7) change, and counts the frame. Returns 1.
    static int RecordOnUpdate(ReplayManager *replay);

    // The current frame.
    int frame;
    DWORD unk_04[0x11];
    // [0] is the last entry written; [stage] is where that stage's
    // record ends.
    ReplayFrame *cursors[8];
};
extern ReplayManager *g_ReplayManager;
