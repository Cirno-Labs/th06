// cflags: /Os
// The /Os file with GameManager, from 0x41b1a4 at least.

#include "gamemanager.h"
#include "ascii.h"
#include "bullet.h"
#include "effect.h"
#include "enemy.h"
#include "enemyscript.h"
#include "features.h"
#include "player.h"
#include "replay.h"
#include "screeneffect.h"
#include "stage.h"
#include "unsorted.h"

// The game's data, as the original defines it here. The DLL build uses
// the original's copies instead.
#ifdef TH06_EXE
unsigned int g_ExtendScores[] = {
    10000000, 20000000, 40000000, 60000000, 1900000000,
};

GameManager g_GameManager;
ChainElem g_GameManagerCalcChain;
ChainElem g_GameManagerDrawChain;
Camera g_Camera;
#endif // TH06_EXE

BOOL GameManager::IsInPlayArea(float x, float y, float width, float height)
{
    if (x + width / 2.0f < 0.0f)
    {
        return FALSE;
    }
    if (x - width / 2.0f > g_GameManager.arena[2])
    {
        return FALSE;
    }
    if (y + height / 2.0f < 0.0f)
    {
        return FALSE;
    }
    if (y - height / 2.0f > g_GameManager.arena[3])
    {
        return FALSE;
    }
    return TRUE;
}

int GameManager::OnUpdate(GameManager *mgr)
{
    unsigned int step;

    if (mgr->isInDemo)
    {
        if (WAS_PRESSED(0xffff))
        {
            g_App.scene = 1;
        }
        mgr->demoFrames++;
        if (mgr->demoFrames == 3600)
        {
            ScreenEffect::StartEffect(2, 120, 0, 0, 0);
        }
        if (mgr->demoFrames >= 3720)
        {
            g_App.scene = 1;
        }
    }
    if (!mgr->isInContinueMenu && !mgr->isPaused && !mgr->isInDemo && WAS_PRESSED(8))
    {
        mgr->isPaused = 1;
        g_GameManager.arena[0] = 32.0f;
        g_GameManager.arena[1] = 16.0f;
        g_GameManager.arena[2] = 384.0f;
        g_GameManager.arena[3] = 448.0f;
        g_App.redrawFrames = 3;
    }
    mgr->isPlaying = !mgr->isInContinueMenu && !mgr->isPaused ? 1 : 0;
    g_App.viewport.X = mgr->arena[0];
    g_App.viewport.Y = mgr->arena[1];
    g_App.viewport.Width = mgr->arena[2];
    g_App.viewport.Height = mgr->arena[3];
    g_App.viewport.MinZ = 0.5f;
    g_App.viewport.MaxZ = 1.0f;
    SetupCamera(0.0f);
    g_App.d3dDevice->SetViewport(&g_App.viewport);
    g_App.d3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, g_Stage.fog.color, 1.0f, 0);
    if (mgr->isPaused == 1 || mgr->isPaused == 2 || mgr->isInContinueMenu)
    {
        return 3;
    }
    if (mgr->score >= 1000000000)
    {
        mgr->score = 999999990;
    }
    if (mgr->guiScore != mgr->score)
    {
        if (mgr->score < mgr->guiScore)
        {
            mgr->score = mgr->guiScore;
        }
        step = (mgr->score - mgr->guiScore) >> 5;
        if (step >= 0x1343e)
        {
            step = 0x1343e;
        }
        else if (step < 10)
        {
            step = 10;
        }
        step -= step % 10;
        if (mgr->scoreIncrement < step)
        {
            mgr->scoreIncrement = step;
        }
        if (mgr->guiScore + mgr->scoreIncrement > mgr->score)
        {
            mgr->scoreIncrement = mgr->score - mgr->guiScore;
        }
        mgr->guiScore += mgr->scoreIncrement;
        if (mgr->guiScore >= mgr->score)
        {
            mgr->scoreIncrement = 0;
            mgr->guiScore = mgr->score;
        }
        if (mgr->nextExtend >= 0 && g_ExtendScores[mgr->nextExtend] <= mgr->guiScore)
        {
            if (mgr->lives < 8)
            {
                mgr->lives++;
                g_SoundPlayer.QueueSound(0x1c, 0);
            }
            g_Gui.flags.livesChanged = 2;
            mgr->nextExtend++;
            g_GameManager.AddSubRank(200);
        }
        if (mgr->highScore < mgr->guiScore)
        {
            mgr->highScore = mgr->guiScore;
        }
    }
    mgr->stageFrames++;
    return 1;
}

int GameManager::OnDraw(GameManager *mgr)
{
    if (mgr->isPaused)
    {
        mgr->isPaused = 2;
    }
    return 1;
}

int GameManager::RegisterChain()
{
    GameManager *mgr = &g_GameManager;

    g_GameManagerCalcChain.SetCallback((ChainCallback)GameManager::OnUpdate);
    g_GameManagerCalcChain.addedCallback = (ChainCallback)GameManager::AddedCallback;
    g_GameManagerCalcChain.deletedCallback = (ChainCallback)GameManager::DeletedCallback;
    g_GameManagerCalcChain.arg = mgr;
    mgr->stageFrames = 0;
    if (g_Chain.AddToCalcChain(&g_GameManagerCalcChain, 4) != 0)
    {
        return -1;
    }
    g_GameManagerDrawChain.SetCallback((ChainCallback)GameManager::OnDraw);
    g_GameManagerDrawChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_GameManagerDrawChain, 2);
    return 0;
}

int GameManager::AddedCallback(GameManager *mgr)
{
#ifdef TH06_EXE
    // The original's are statics of this function.
    static char *g_EclPaths[8] = {
        "dummy",
        "data/ecldata1.ecl",
        "data/ecldata2.ecl",
        "data/ecldata3.ecl",
        "data/ecldata4.ecl",
        "data/ecldata5.ecl",
        "data/ecldata6.ecl",
        "data/ecldata7.ecl",
    };
    static EnemyAnmPaths g_EnemyAnmPaths[8] = {
        {"dummy", "dummy"},
        {"data/stg1enm.anm", "data/stg1enm2.anm"},
        {"data/stg2enm.anm", "data/stg2enm2.anm"},
        {"data/stg3enm.anm", NULL},
        {"data/stg4enm.anm", NULL},
        {"data/stg5enm.anm", "data/stg5enm2.anm"},
        {"data/stg6enm.anm", "data/stg6enm2.anm"},
        {"data/stg7enm.anm", "data/stg7enm2.anm"},
    };
    static RankSettings g_ReplayRanks[5] = {
        {16, 12, 20},
        {16, 10, 32},
        {16, 10, 32},
        {16, 10, 32},
        {16, 14, 18},
    };
    static RankSettings g_Ranks[5] = {
        {16, 12, 20},
        {16, 10, 32},
        {16, 10, 32},
        {16, 10, 32},
        {16, 14, 18},
    };
#endif // TH06_EXE
    BOOL failed = FALSE;
    Catk *catk2;
    int index2;
    unsigned int m2;
    ScoreDat *data;
    int shot2;

    g_App.d3dDevice->ResourceManagerDiscardBytes(0);
    if (g_App.scene != 3)
    {
        g_App.startCfg.bombs = g_GameManager.bombs;
        g_App.startCfg.lives = g_GameManager.lives;
        mgr->arena[0] = 32.0f;
        mgr->arena[1] = 16.0f;
        mgr->arena[2] = 384.0f;
        mgr->arena[3] = 448.0f;
        mgr->playerArea[0] = 8.0f;
        mgr->playerArea[1] = 16.0f;
        mgr->playerArea[2] = 368.0f;
        mgr->playerArea[3] = 416.0f;
        mgr->progressFrames = 0;
        mgr->guiScore = 0;
        mgr->score = 0;
        mgr->scoreIncrement = 0;
        mgr->highScore = 100000;
        mgr->power = 0;
        mgr->continuesUsed = 0;
        if (mgr->stage >= 6)
        {
            mgr->difficulty = 4;
        }
        if (mgr->difficulty < 4)
        {
            mgr->nextExtend = 0;
        }
        else
        {
            mgr->nextExtend = 4;
        }
        g_GameManager.powerItemCombo = 0;
        mgr->rank = 8;
        mgr->grazeTotal = 0;
        mgr->pointItemsTotal = 0;
        for (catk2 = mgr->catks, index2 = 0; index2 < 0x40; index2++, catk2++)
        {
            for (m2 = 0; m2 < 0x20; m2++)
            {
                ((WORD *)catk2)[m2] = g_Rng.GetRandomU16();
            }
            catk2->magic = 0x4b544143;
            catk2->size2 = 0x40;
            catk2->size = 0x40;
            catk2->version = 0x10;
            catk2->index = index2;
            catk2->attempts = 0;
            catk2->captures = 0;
        }
        data = LoadScoreDat("score.dat");
        g_GameManager.highScore = LoadHighScores(data, NULL, g_GameManager.character * 2 + g_GameManager.shotType, g_GameManager.difficulty);
        LoadSpellCards(data, mgr->catks);
        LoadClears(data, mgr->clrds);
        LoadPracticeScores(data, &mgr->pscrs[0][0][0]);
        if (mgr->isInPractice)
        {
            g_GameManager.highScore = mgr->pscrs[g_GameManager.character * 2 + g_GameManager.shotType][g_GameManager.stage][g_GameManager.difficulty].score;
        }
        ReleaseScoreDat(data);
        mgr->rank = g_Ranks[g_GameManager.difficulty].rank;
        mgr->minRank = g_Ranks[g_GameManager.difficulty].minRank;
        mgr->maxRank = g_Ranks[g_GameManager.difficulty].maxRank;
        mgr->deaths = 0;
        mgr->bombsUsed = 0;
        mgr->spellCardsCaptured = 0;
    }
    else
    {
        mgr->guiScore = mgr->score;
        mgr->scoreIncrement = 0;
    }
    mgr->subRank = 0;
    mgr->pointItems = 0;
    mgr->graze = 0;
    mgr->isPaused = 0;
    mgr->stage++;
    if (g_GameManager.isInReplay == 0)
    {
        shot2 = g_GameManager.character * 2 + g_GameManager.shotType;
        if (!mgr->continuesUsed && mgr->clrds[shot2].stagesClearedNoContinue[g_GameManager.difficulty] < mgr->stage - 1)
        {
            mgr->clrds[shot2].stagesClearedNoContinue[g_GameManager.difficulty] = mgr->stage - 1;
        }
        if (mgr->clrds[shot2].stagesCleared[g_GameManager.difficulty] < mgr->stage - 1)
        {
            mgr->clrds[shot2].stagesCleared[g_GameManager.difficulty] = mgr->stage - 1;
        }
    }
    if (mgr->isInPractice)
    {
        switch (mgr->stage)
        {
        case 1:
            break;
        case 2:
            mgr->power = 0x40;
            break;
        default:
            mgr->power = 0x80;
            break;
        }
    }
#if FEAT_ENGLISH
    g_App.LoadArchive(4, "th06e_CM.dat");
    g_App.LoadArchive(2, "th06e_ST.dat");
#else
    g_App.LoadArchive(4, "紅魔郷CM.dat");
    g_App.LoadArchive(2, "紅魔郷ST.dat");
#endif
    if (g_GameManager.isInReplay == 1)
    {
        if (ReplayManager::RegisterChain(1, g_GameManager.replayFile) != 0)
        {
            failed = TRUE;
        }
        while (g_ExtendScores[mgr->nextExtend] <= mgr->guiScore)
        {
            mgr->nextExtend++;
        }
        mgr->minRank = g_ReplayRanks[g_GameManager.difficulty].minRank;
        mgr->maxRank = g_ReplayRanks[g_GameManager.difficulty].maxRank;
    }
    g_Rng.generated = 0;
    mgr->rngSeed = g_Rng.seed;
    if (Stage::RegisterChain(mgr->stage) != 0)
    {
        LogPrintf(&g_Log, "error : 背景データの初期化に失敗しました\n");
        return -1;
    }
    if (Player::RegisterChain(0) != 0)
    {
        LogPrintf(&g_Log, "error : プレイヤーの初期化に失敗しました\n");
        return -1;
    }
    if (BulletManager::RegisterChain("data/etama.anm") != 0)
    {
        LogPrintf(&g_Log, "error : 敵弾の初期化に失敗しました\n");
        return -1;
    }
    if (EnemyManager::RegisterChain(g_EnemyAnmPaths[mgr->stage].anmPath, g_EnemyAnmPaths[mgr->stage].anmPath2) != 0)
    {
        LogPrintf(&g_Log, "error : 敵の初期化に失敗しました\n");
        return -1;
    }
    if (g_EnemyScripts.Load(g_EclPaths[mgr->stage]) != 0)
    {
        LogPrintf(&g_Log, "error : 敵頭脳の初期化に失敗しました\n");
        return -1;
    }
    if (EffectManager::RegisterChain() != 0)
    {
        LogPrintf(&g_Log, "error : エフェクトの初期化に失敗しました\n");
        return -1;
    }
    if (Gui::RegisterChain() != 0)
    {
        LogPrintf(&g_Log, "error : 2D表示の初期化に失敗しました\n");
        return -1;
    }
    if (g_GameManager.isInReplay == 0)
    {
        ReplayManager::RegisterChain(0, "replay/th6_00.rpy");
    }
    if (!g_GameManager.isInDemo)
    {
        g_App.LoadMidi(1, g_Stage.data->songPaths[1]);
        g_App.PlayMusic(g_Stage.data->songPaths[0]);
    }
    mgr->isInContinueMenu = 0;
    mgr->isPlaying = 1;
    if (g_App.scene != 3)
    {
        g_App.achievedFrames = 0.0f;
        g_App.expectedFrames = 0.0f;
    }
    mgr->isFrozen = 0;
    mgr->score = 0;
    mgr->isGameCleared = 0;
    g_AsciiText.Initialize();
    if (failed)
    {
        g_App.scene = 1;
    }
    g_App.redrawFrames = 3;
    return 0;
}

int GameManager::DeletedCallback(GameManager *mgr)
{
    g_App.d3dDevice->ResourceManagerDiscardBytes(0);
    if (!g_GameManager.isInDemo)
    {
        g_App.StopMusic();
    }
    Stage::RemoveChain();
    BulletManager::RemoveChain();
    Player::RemoveChain();
    EnemyManager::RemoveChain();
    g_EnemyScripts.Release();
    EffectManager::RemoveChain();
    Gui::RemoveChain();
    ReplayManager::EndStage();
    mgr->isPlaying = 0;
    g_AsciiText.Initialize();
    return 0;
}

void GameManager::RemoveChain()
{
    g_Chain.Remove(&g_GameManagerCalcChain);
    g_Chain.Remove(&g_GameManagerDrawChain);
}

void SetupFixedCamera(float extraFar)
{
    float midX;
    float halfHeight;
    float aspectXY;
    float fovAngle;
    float zDist;

    midX = (float)g_App.viewport.Width / 2.0f;
    halfHeight = (float)g_App.viewport.Height / 2.0f;
    aspectXY = (float)g_App.viewport.Width / (float)g_App.viewport.Height;
    fovAngle = 0.5235988f;
    zDist = halfHeight / tanf(fovAngle / 2.0f);
    {
        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
        D3DXVECTOR3 lookAt(midX, -halfHeight, 0.0f);
        D3DXVECTOR3 from(midX, -halfHeight, -zDist);
        D3DXMatrixLookAtLH(&g_App.viewMatrix, &from, &lookAt, &up);
        g_Camera.distance = fabsf(zDist);
        D3DXMatrixPerspectiveFovLH(&g_App.projectionMatrix, fovAngle, aspectXY, 100.0f, 10000.0f + extraFar);
        g_App.d3dDevice->SetTransform(D3DTS_VIEW, &g_App.viewMatrix);
        g_App.d3dDevice->SetTransform(D3DTS_PROJECTION, &g_App.projectionMatrix);
    }
}

void SetupCamera(float extraFar)
{
    float midX;
    float halfHeight;
    float aspectXY;
    float fovAngle;
    float zDist;

    midX = (float)g_App.viewport.Width / 2.0f;
    halfHeight = (float)g_App.viewport.Height / 2.0f;
    aspectXY = (float)g_App.viewport.Width / (float)g_App.viewport.Height;
    fovAngle = 0.5235988f;
    zDist = halfHeight / tanf(fovAngle / 2.0f);
    // A block of its own: its vectors come after the floats on the stack.
    {
        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
        D3DXVECTOR3 lookAt(midX + g_Camera.offsetX, -halfHeight + g_Camera.offsetY, 0.0f);
        D3DXVECTOR3 from(midX, -halfHeight, -zDist * g_Camera.zoom);
        D3DXMatrixLookAtLH(&g_App.viewMatrix, &from, &lookAt, &up);
        g_Camera.distance = fabsf(zDist);
        D3DXMatrixPerspectiveFovLH(&g_App.projectionMatrix, fovAngle, aspectXY, 100.0f, 10000.0f + extraFar);
        g_App.d3dDevice->SetTransform(D3DTS_VIEW, &g_App.viewMatrix);
        g_App.d3dDevice->SetTransform(D3DTS_PROJECTION, &g_App.projectionMatrix);
    }
}

void GameManager::AddSubRank(int amount)
{
    this->subRank += amount;
    while (this->subRank >= 100)
    {
        this->rank++;
        this->subRank -= 100;
    }
    if (this->rank > this->maxRank)
    {
        this->rank = this->maxRank;
    }
}

void GameManager::SubSubRank(int amount)
{
    this->subRank -= amount;
    while (this->subRank < 0)
    {
        this->rank--;
        this->subRank += 100;
    }
    if (this->rank < this->minRank)
    {
        this->rank = this->minRank;
    }
}

GameManager::GameManager()
{
    memset(this, 0, sizeof(GameManager));
    this->arena[0] = 32.0f;
    this->arena[1] = 16.0f;
    this->arena[2] = 384.0f;
    this->arena[3] = 448.0f;
}
