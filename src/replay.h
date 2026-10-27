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

// Decrypts a loaded replay of `size` bytes in place and checks its
// magic, checksum and version. Returns 0, or -1 if it is not valid.
int ValidateReplay(void *data, int size);

// One stage of a replay: the game's state at its start, then the
// entries.
struct ReplayStage
{
    int unk_00;
    WORD unk_04;
    WORD unk_06;
    BYTE power;
    BYTE unk_09;
    BYTE unk_0a;
    BYTE unk_0b;
    BYTE unk_0c;
    BYTE unk_0d[0x3];
    ReplayFrame frames[1];
};

// The start of a replay ("T6RP").
struct ReplayHeader
{
    char magic[4];
    WORD version;
    BYTE shotType;
    BYTE difficulty;
    // 0x3f000318 plus the bytes from `key` on.
    int checksum;
    BYTE unk_0c[0x2];
    // The bytes after it are stored minus key, key + 7, key + 14, ...
    BYTE key;
    BYTE unk_0f[0xa];
    char name[0x17];
    // Indexed by stage; malloc'd, 0x69780 bytes each.
    ReplayStage *stages[8];
};

// Records or plays a replay; g_ReplayManager points at it while one is
// in use.
class ReplayManager
{
public:
    ReplayManager()
    {
    }

    // Ends the current stage's record with an entry at the current frame
    // and an end marker (frame 9999999), and keeps where it ends.
    static void EndStage();
    // The update callback while recording: while
    // g_GameManager.unk_1821 is set, adds an entry whenever the buttons
    // (masked with 0x1f7) change, and counts the frame. Returns 1.
    static int RecordOnUpdate(ReplayManager *replay);
    // The update callback while playing: while g_GameManager.unk_1821 is
    // set, puts the recorded buttons for the frame into g_Buttons (with
    // key repeat as in Unknown6c6d18::OnUpdate) and counts the frame.
    // Returns 1.
    static int PlayOnUpdate(ReplayManager *replay);
    // The second update callback while playing: while
    // g_GameManager.unk_1821 is set, g_Gui.Func4195a2() and
    // g_Gui.Func417344() hold, returns 6 on two frames of every three;
    // otherwise 1.
    static int PlayOnUpdate2(ReplayManager *replay);
    // The draw callback; does nothing. Returns 1.
    static int OnDraw(ReplayManager *replay);
    // The update element's added callbacks when recording and playing.
    static int RecordAddedCallback(ReplayManager *replay);
    static int PlayAddedCallback(ReplayManager *replay);

    // Creates the ReplayManager, recording (`mode` 0) or playing (1) the
    // replay at `path`, and adds its chain elements; if it exists, runs
    // the mode's added callback again. Returns 0 (the added callback's
    // result when playing again), or -1 if adding fails. Does nothing
    // when recording while the game runs below full speed.
    static int RegisterChain(int mode, char *path);
    // Removes the chain elements but the update one, frees the replay
    // data and deletes the
    // ReplayManager. Returns 0.
    static int DeletedCallback(ReplayManager *replay);

    // The current frame.
    int frame;
    // The replay data.
    ReplayHeader *data;
    // RegisterChain's arguments.
    int mode;
    char *path;
    DWORD unk_10[0xd];
    WORD unk_44;
    WORD unk_46;
    // [0] is the last entry written; [stage] is where that stage's
    // record ends.
    ReplayFrame *cursors[8];
    ChainElem *calcChain;
    ChainElem *drawChain;
    // The second update element, only when playing.
    ChainElem *calcChain2;
};
ASSERT_SIZE(ReplayManager, 0x74);
extern ReplayManager *g_ReplayManager;
