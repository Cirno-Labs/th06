// The stage: its background and its data file.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"
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
    // Resets, loads the files g_StageFiles gives for the stage and sets
    // the fog up. Returns 0, or -1 if loading fails.
    static int AddedCallback(Stage *stage);
    // Loads the background sprites from `anmPath` and the data from
    // `dataPath`. Returns 0, or -1 on failure.
    int Load(char *anmPath, char *dataPath);
    // Frees the background sprites and the data. Returns 0.
    static int DeletedCallback(Stage *stage);
    // Removes the update element and the two draw elements.
    static void RemoveChain();

    // One Sprite per object sprite in the data, malloc'd.
    Sprite *sprites;
    // The stage data file, as loaded.
    void *data;
    DWORD unk_008[0x5];
    Timer unk_01c;
    int unk_028;
    Timer timer;
    // RegisterChain's argument.
    int stage;
    D3DXVECTOR3 unk_03c;
    // D3DRS_FOGSTART, D3DRS_FOGEND and D3DRS_FOGCOLOR.
    float fogNear;
    float fogFar;
    D3DCOLOR fogColor;
    DWORD unk_054[0x6];
    int unk_06c;
    DWORD unk_070[0x4];
    int unk_080;
    DWORD unk_084[0x89];
    BYTE unk_2a8;
    BYTE unk_2a9[0x3];
    D3DXVECTOR3 unk_2ac;
    D3DXVECTOR3 unk_2b8;
    int unk_2c4;
    Timer unk_2c8;
    DWORD unk_2d4[0x8];
};
ASSERT_SIZE(Stage, 0x2f4);
extern Stage g_Stage;

// The background sprites and data file of each stage, at 0x4760e0;
// stage 0 has "dummy" ones.
struct StageFiles
{
    char *anmPath;
    char *dataPath;
};
extern StageFiles g_StageFiles[8];

// Stage's statically allocated chain elements.
extern ChainElem g_StageCalcChain;
extern ChainElem g_StageDrawChain;
extern ChainElem g_StageDrawChain2;
