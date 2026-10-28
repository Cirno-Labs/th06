// cflags: /Os
// The /Os file with GameManager, from 0x41b1a4 at least.

#include "gamemanager.h"
#include "ascii.h"
#include "bullet.h"
#include "effect.h"
#include "enemy.h"
#include "enemyscript.h"
#include "player.h"
#include "replay.h"
#include "screeneffect.h"
#include "stage.h"
#include "unsorted.h"

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

    if (mgr->unk_1824)
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
    if (!mgr->unk_1820 && !mgr->unk_181f && !mgr->unk_1824 && WAS_PRESSED(8))
    {
        mgr->unk_181f = 1;
        g_GameManager.arena[0] = 32.0f;
        g_GameManager.arena[1] = 16.0f;
        g_GameManager.arena[2] = 384.0f;
        g_GameManager.arena[3] = 448.0f;
        g_App.redrawFrames = 3;
    }
    mgr->unk_1821 = !mgr->unk_1820 && !mgr->unk_181f ? 1 : 0;
    g_App.viewport.X = mgr->arena[0];
    g_App.viewport.Y = mgr->arena[1];
    g_App.viewport.Width = mgr->arena[2];
    g_App.viewport.Height = mgr->arena[3];
    g_App.viewport.MinZ = 0.5f;
    g_App.viewport.MaxZ = 1.0f;
    SetupCamera(0.0f);
    g_App.d3dDevice->SetViewport(&g_App.viewport);
    g_App.d3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, g_Stage.fog.color, 1.0f, 0);
    if (mgr->unk_181f == 1 || mgr->unk_181f == 2 || mgr->unk_1820)
    {
        return 3;
    }
    if (mgr->score >= 1000000000)
    {
        mgr->score = 999999990;
    }
    if (mgr->unk_0000 != mgr->score)
    {
        if (mgr->score < mgr->unk_0000)
        {
            mgr->score = mgr->unk_0000;
        }
        step = (mgr->score - mgr->unk_0000) >> 5;
        if (step >= 0x1343e)
        {
            step = 0x1343e;
        }
        else if (step < 10)
        {
            step = 10;
        }
        step -= step % 10;
        if (mgr->unk_0008 < step)
        {
            mgr->unk_0008 = step;
        }
        if (mgr->unk_0000 + mgr->unk_0008 > mgr->score)
        {
            mgr->unk_0008 = mgr->score - mgr->unk_0000;
        }
        mgr->unk_0000 += mgr->unk_0008;
        if (mgr->unk_0000 >= mgr->score)
        {
            mgr->unk_0008 = 0;
            mgr->unk_0000 = mgr->score;
        }
        if (mgr->unk_181c >= 0 && g_ExtendScores[mgr->unk_181c] <= mgr->unk_0000)
        {
            if (mgr->lives < 8)
            {
                mgr->lives++;
                g_SoundPlayer.QueueSound(0x1c, 0);
            }
            g_Gui.flags.livesChanged = 2;
            mgr->unk_181c++;
            g_GameManager.AddSubRank(200);
        }
        if (mgr->highScore < mgr->unk_0000)
        {
            mgr->highScore = mgr->unk_0000;
        }
    }
    mgr->unk_1a30++;
    return 1;
}

int GameManager::OnDraw(GameManager *mgr)
{
    if (mgr->unk_181f)
    {
        mgr->unk_181f = 2;
    }
    return 1;
}

int GameManager::AddedCallback(GameManager *mgr)
{
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
        mgr->unk_0000 = 0;
        mgr->score = 0;
        mgr->unk_0008 = 0;
        mgr->highScore = 100000;
        mgr->power = 0;
        mgr->continuesUsed = 0;
        if (mgr->stage >= 6)
        {
            mgr->difficulty = 4;
        }
        if (mgr->difficulty < 4)
        {
            mgr->unk_181c = 0;
        }
        else
        {
            mgr->unk_181c = 4;
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
            catk2->unk_06 = 0x40;
            catk2->size = 0x40;
            catk2->version = 0x10;
            catk2->index = index2;
            catk2->unk_3c = 0;
            catk2->unk_3e = 0;
        }
        data = LoadScoreDat("score.dat");
        g_GameManager.highScore = LoadHighScores(data, NULL, g_GameManager.character * 2 + g_GameManager.shotType, g_GameManager.difficulty);
        LoadSpellCards(data, mgr->catks);
        LoadClears(data, mgr->clrds);
        LoadPracticeScores(data, &mgr->pscrs[0][0][0]);
        if (mgr->unk_1823)
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
        mgr->unk_0000 = mgr->score;
        mgr->unk_0008 = 0;
    }
    mgr->subRank = 0;
    mgr->pointItems = 0;
    mgr->graze = 0;
    mgr->unk_181f = 0;
    mgr->stage++;
    if (g_GameManager.unk_001c == 0)
    {
        shot2 = g_GameManager.character * 2 + g_GameManager.shotType;
        if (!mgr->continuesUsed && mgr->clrds[shot2].unk_0c[g_GameManager.difficulty] < mgr->stage - 1)
        {
            mgr->clrds[shot2].unk_0c[g_GameManager.difficulty] = mgr->stage - 1;
        }
        if (mgr->clrds[shot2].unk_11[g_GameManager.difficulty] < mgr->stage - 1)
        {
            mgr->clrds[shot2].unk_11[g_GameManager.difficulty] = mgr->stage - 1;
        }
    }
    if (mgr->unk_1823)
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
    g_App.LoadArchive(4, "紅魔郷CM.dat");
    g_App.LoadArchive(2, "紅魔郷ST.dat");
    if (g_GameManager.unk_001c == 1)
    {
        if (ReplayManager::RegisterChain(1, g_GameManager.replayFile) != 0)
        {
            failed = TRUE;
        }
        while (g_ExtendScores[mgr->unk_181c] <= mgr->unk_0000)
        {
            mgr->unk_181c++;
        }
        mgr->minRank = g_ReplayRanks[g_GameManager.difficulty].minRank;
        mgr->maxRank = g_ReplayRanks[g_GameManager.difficulty].maxRank;
    }
    g_Rng.generated = 0;
    mgr->unk_1a2c = g_Rng.seed;
    if (Stage::RegisterChain(mgr->stage) != 0)
    {
        LogPrintf(&g_Unknown69d998, "error : 背景データの初期化に失敗しました\n");
        return -1;
    }
    if (Player::RegisterChain(0) != 0)
    {
        LogPrintf(&g_Unknown69d998, "error : プレイヤーの初期化に失敗しました\n");
        return -1;
    }
    if (BulletManager::RegisterChain("data/etama.anm") != 0)
    {
        LogPrintf(&g_Unknown69d998, "error : 敵弾の初期化に失敗しました\n");
        return -1;
    }
    if (EnemyManager::RegisterChain(g_EnemyAnmPaths[mgr->stage].anmPath, g_EnemyAnmPaths[mgr->stage].anmPath2) != 0)
    {
        LogPrintf(&g_Unknown69d998, "error : 敵の初期化に失敗しました\n");
        return -1;
    }
    if (g_EnemyScripts.Load(g_EclPaths[mgr->stage]) != 0)
    {
        LogPrintf(&g_Unknown69d998, "error : 敵頭脳の初期化に失敗しました\n");
        return -1;
    }
    if (EffectManager::RegisterChain() != 0)
    {
        LogPrintf(&g_Unknown69d998, "error : エフェクトの初期化に失敗しました\n");
        return -1;
    }
    if (Gui::RegisterChain() != 0)
    {
        LogPrintf(&g_Unknown69d998, "error : 2D表示の初期化に失敗しました\n");
        return -1;
    }
    if (g_GameManager.unk_001c == 0)
    {
        ReplayManager::RegisterChain(0, "replay/th6_00.rpy");
    }
    if (!g_GameManager.unk_1824)
    {
        g_App.LoadMidi(1, g_Stage.data->songPaths[1]);
        g_App.PlayMusic(g_Stage.data->songPaths[0]);
    }
    mgr->unk_1820 = 0;
    mgr->unk_1821 = 1;
    if (g_App.scene != 3)
    {
        g_App.achievedFrames = 0.0f;
        g_App.expectedFrames = 0.0f;
    }
    mgr->unk_002c = 0;
    mgr->score = 0;
    mgr->unk_1822 = 0;
    g_AsciiText.Initialize();
    if (failed)
    {
        g_App.scene = 1;
    }
    g_App.redrawFrames = 3;
    return 0;
}

int GameManager::RegisterChain()
{
    GameManager *mgr = &g_GameManager;

    g_GameManagerCalcChain.SetCallback((ChainCallback)GameManager::OnUpdate);
    g_GameManagerCalcChain.addedCallback = (ChainCallback)GameManager::AddedCallback;
    g_GameManagerCalcChain.deletedCallback = (ChainCallback)GameManager::DeletedCallback;
    g_GameManagerCalcChain.arg = mgr;
    mgr->unk_1a30 = 0;
    if (g_Chain.AddToCalcChain(&g_GameManagerCalcChain, 4) != 0)
    {
        return -1;
    }
    g_GameManagerDrawChain.SetCallback((ChainCallback)GameManager::OnDraw);
    g_GameManagerDrawChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_GameManagerDrawChain, 2);
    return 0;
}

int GameManager::DeletedCallback(GameManager *mgr)
{
    g_App.d3dDevice->ResourceManagerDiscardBytes(0);
    if (!g_GameManager.unk_1824)
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
    mgr->unk_1821 = 0;
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

GameManager::GameManager()
{
    memset(this, 0, sizeof(GameManager));
    this->arena[0] = 32.0f;
    this->arena[1] = 16.0f;
    this->arena[2] = 384.0f;
    this->arena[3] = 448.0f;
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
