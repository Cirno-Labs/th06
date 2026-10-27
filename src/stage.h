// The stage: its background and its data file.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"
#include "timer.h"

// A run of quads in the stage data, ended by one with a negative type.
struct StageQuad
{
    short type;
    // The offset of the next quad.
    short size;
    short script;
    // Set by Stage::Load: the quad's Sprite in Stage::sprites.
    short spriteIndex;
    // Type 0's position in the object, and size; a size of 0 keeps the
    // sprite's.
    D3DXVECTOR3 pos;
    float width;
    float height;
};

// An object of the stage data: its quads start at +0x1c.
struct StageObject
{
    BYTE unk_00[0x2];
    // The background layer (0 to 3) it is drawn on.
    char layer;
    BYTE unk_03;
    D3DXVECTOR3 pos;
    // The bounding box DrawLayer tests against the viewport.
    D3DXVECTOR3 size;
    StageQuad quads[1];
};

// A placement of an object; a negative `id` ends the list.
struct StageInstance
{
    short id;
    short unk_02;
    D3DXVECTOR3 pos;
};

// The start of the stage data file.
struct StageData
{
    short objectCount;
    short quadCount;
    // Offsets from the start of the file.
    int unk_04;
    int unk_08;
    // Then, at 0x490, the objects' offsets.
};

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
    // Sets the fog, then draws layers 0 and 1 unless unk_080 is above 1
    // or g_Gui.Func4172d3() is set. Returns 1.
    static int OnDraw(Stage *stage);
    // Draws the objects on `layer` whose bounding box shows on screen,
    // relative to the camera at unk_03c. Returns 0.
    int DrawLayer(int layer);
    // Draws layers 2 and 3 under the same conditions as OnDraw, with a
    // rectangle fading in over unk_084's 60 frames while unk_080 is 1,
    // then unk_088 while unk_080 is set, and leaves the fog at 1000 to
    // 2000. Returns 1.
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
    StageData *data;
    int quadCount;
    int objectCount;
    // In `data`: the objects, and the header's two offsets as pointers.
    StageObject **objects;
    StageInstance *instances;
    void *unk_018;
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
    DWORD unk_070[0x3];
    // Makes OnDraw set the fog color again.
    char fogColorChanged;
    char unk_07d[0x3];
    int unk_080;
    int unk_084;
    Sprite unk_088;
    DWORD unk_198[0x44];
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
