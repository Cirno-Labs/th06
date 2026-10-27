// Replays: the inputs recorded frame by frame ("error : replay.cpp").

#pragma once

#include <windows.h>

#include "chain.h"
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
    // Removes the chain elements, frees the replay data and deletes the
    // ReplayManager. Returns 0.
    static int DeletedCallback(ReplayManager *replay);

    // The current frame.
    int frame;
    // The replay data, malloc'd.
    void *data;
    DWORD unk_08[0x10];
    // [0] is the last entry written; [stage] is where that stage's
    // record ends.
    ReplayFrame *cursors[8];
    DWORD unk_68;
    // The update element, and a draw element that may be NULL.
    ChainElem *calcChain;
    ChainElem *drawChain;
};
extern ReplayManager *g_ReplayManager;
