// The game's global objects. Built into the standalone exe only: the DLL
// build uses the original's, which its own static initializers construct.
//
// They are defined in the order the original constructs them (its
// initializers run file by file in link order), so constructors and the
// destructors registered with atexit run in the same order. The original
// also constructs a few globals that no game code uses (a Timer and three
// ChainElems); they are left out. Its unused TextFont is in chain.cpp.

#include "ascii.h"
#include "bullet.h"
#include "chain.h"
#include "effect.h"
#include "enemy.h"
#include "enemyscript.h"
#include "features.h"
#include "gamemanager.h"
#include "item.h"
#include "mainmenu.h"
#include "player.h"
#include "replay.h"
#include "sprite.h"
#include "stage.h"
#include "unsorted.h"

// ascii.cpp
AsciiText g_AsciiText;
ChainElem g_AsciiCalcChain;
ChainElem g_AsciiDrawChain;
ChainElem g_AsciiDrawChain2;

// stage.cpp
Stage g_Stage;
ChainElem g_StageCalcChain;
ChainElem g_StageDrawChain;
ChainElem g_StageDrawChain2;

// enemyscript.cpp
EnemyScripts g_EnemyScripts;
int g_ShotTypeOperand;
float g_PlayerDistance;
float g_PlayerAngle;
float g_StarAngles[6];
D3DXVECTOR3 g_StarFrom;
D3DXVECTOR3 g_StarTo;

// effect.cpp
EffectManager g_EffectManager;
ChainElem g_EffectCalcChain;
ChainElem g_EffectDrawChain;

// enemy.cpp
ChainElem g_EnemyCalcChain;
ChainElem g_EnemyDrawChain;
EnemyManager g_EnemyManager;

// bullet.cpp
ChainElem g_BulletCalcChain;
ChainElem g_BulletDrawChain;
BulletManager g_BulletManager;

// gui.cpp
ChainElem g_GuiCalcChain;
ChainElem g_GuiDrawChain;
Gui g_Gui;

// gamemanager.cpp
GameManager g_GameManager;
ChainElem g_GameManagerCalcChain;
ChainElem g_GameManagerDrawChain;
Camera g_Camera;

// chain.cpp and the input code
Chain g_Chain;
Rng g_Rng;
DatArchive **g_Archives;
WORD g_Buttons;
WORD g_PrevButtons;
WORD g_ButtonRepeat;
WORD g_ButtonFrames;
WORD g_ShotHeldFrames;
BYTE g_PadButtons[0x80];
JOYCAPSA g_JoyCaps;
int g_FileSize;

// The log. The original's Log has an inline constructor: it empties the
// buffer and logs this header line first.
Log g_Log;

static int InitLog()
{
    g_Log.cursor = g_Log.buffer;
    *g_Log.cursor = '\0';
    g_Log.hasError = 0;
    LogPrintf(&g_Log, "東方動作記録 --------------------------------------------- \n");
    return 0;
}
static int s_LogInitialized = InitLog();

IDirect3DSurface8 *g_TextSurface;

// item.cpp
ItemManager g_ItemManager;

// The main window and the game object (zeroed by an inline constructor in
// the original, as static storage is anyway).
GameWindow g_GameWindow;
HANDLE g_AppMutex;
App g_App;

// player.cpp
Player g_Player;

// replay.cpp, sound.cpp
ReplayManager *g_ReplayManager;
SoundPlayer g_SoundPlayer;

// spriteengine.cpp
SpriteEngine *g_SpriteEngine;
VertexTex1DiffuseXyzrwh g_DiffuseQuad[4];
VertexTex1Xyzrwh g_Quad[4];
VertexTex1DiffuseXyz g_DiffuseQuadXyz[4];

// mainmenu.cpp
MainMenu g_MainMenu;

#if FEAT_ENGLISH
char *g_EnglishSpellCards;
#endif
