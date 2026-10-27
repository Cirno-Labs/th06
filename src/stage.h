// The stage: its background and its data file.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"

// Runs the stage's background, the global at 0x487b10. Its loader logs
// "ステージデータが見つかりません" ("stage data not found").
class Stage
{
public:
    // Removes the update element and the two draw elements.
    static void RemoveChain();

    DWORD unk_000[0xbd];
};
ASSERT_SIZE(Stage, 0x2f4);
extern Stage g_Stage;

// Stage's statically allocated chain elements.
extern ChainElem g_StageCalcChain;
extern ChainElem g_StageDrawChain;
extern ChainElem g_StageDrawChain2;
