// The stage: its background and its data file.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "timer.h"

// Runs the stage's background, the global at 0x487b10. Its loader logs
// "ステージデータが見つかりません" ("stage data not found").
class Stage
{
public:
    // Zeroes the stage, remembers `stage` and adds the update element
    // (priority 6) and the draw elements (priorities 3 and 4). Returns 0,
    // or -1 if adding fails.
    static int RegisterChain(int stage);
    static int OnUpdate(Stage *stage);
    static int OnDraw(Stage *stage);
    static int OnDraw2(Stage *stage);
    // Loads the stage's background and data.
    static int AddedCallback(Stage *stage);
    static int DeletedCallback(Stage *stage);
    // Removes the update element and the two draw elements.
    static void RemoveChain();

    DWORD unk_000;
    DWORD unk_004;
    DWORD unk_008[0x9];
    Timer timer;
    // RegisterChain's argument.
    int stage;
    DWORD unk_03c[0xae];
};
ASSERT_SIZE(Stage, 0x2f4);
extern Stage g_Stage;

// Stage's statically allocated chain elements.
extern ChainElem g_StageCalcChain;
extern ChainElem g_StageDrawChain;
extern ChainElem g_StageDrawChain2;
