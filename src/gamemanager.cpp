// cflags: /Os
// The /Os file with GameManager, from 0x41b1a4 at least.

#include "gamemanager.h"
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
            g_Unknown6c6d18.scene = 1;
        }
        mgr->demoFrames++;
        if (mgr->demoFrames == 3600)
        {
            ScreenEffect::StartEffect(2, 120, 0, 0, 0);
        }
        if (mgr->demoFrames >= 3720)
        {
            g_Unknown6c6d18.scene = 1;
        }
    }
    if (!mgr->unk_1820 && !mgr->unk_181f && !mgr->unk_1824 && WAS_PRESSED(8))
    {
        mgr->unk_181f = 1;
        g_GameManager.arena[0] = 32.0f;
        g_GameManager.arena[1] = 16.0f;
        g_GameManager.arena[2] = 384.0f;
        g_GameManager.arena[3] = 448.0f;
        g_Unknown6c6d18.unk_198 = 3;
    }
    mgr->unk_1821 = !mgr->unk_1820 && !mgr->unk_181f ? 1 : 0;
    g_Unknown6c6d18.viewport.X = mgr->arena[0];
    g_Unknown6c6d18.viewport.Y = mgr->arena[1];
    g_Unknown6c6d18.viewport.Width = mgr->arena[2];
    g_Unknown6c6d18.viewport.Height = mgr->arena[3];
    g_Unknown6c6d18.viewport.MinZ = 0.5f;
    g_Unknown6c6d18.viewport.MaxZ = 1.0f;
    SetupCamera(0.0f);
    g_Unknown6c6d18.d3dDevice->SetViewport(&g_Unknown6c6d18.viewport);
    g_Unknown6c6d18.d3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, g_Stage.fog.color, 1.0f, 0);
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
                g_Unknown6d3f50.QueueSound(0x1c, 0);
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
    g_Unknown6c6d18.d3dDevice->ResourceManagerDiscardBytes(0);
    if (!g_GameManager.unk_1824)
    {
        g_Unknown6c6d18.StopMusic();
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

void SetupCamera(float extraFar)
{
    float midX;
    float halfHeight;
    float aspectXY;
    float fovAngle;
    float zDist;

    midX = (float)g_Unknown6c6d18.viewport.Width / 2.0f;
    halfHeight = (float)g_Unknown6c6d18.viewport.Height / 2.0f;
    aspectXY = (float)g_Unknown6c6d18.viewport.Width / (float)g_Unknown6c6d18.viewport.Height;
    fovAngle = 0.5235988f;
    zDist = halfHeight / tanf(fovAngle / 2.0f);
    // A block of its own: its vectors come after the floats on the stack.
    {
        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
        D3DXVECTOR3 lookAt(midX + g_Camera.offsetX, -halfHeight + g_Camera.offsetY, 0.0f);
        D3DXVECTOR3 from(midX, -halfHeight, -zDist * g_Camera.zoom);
        D3DXMatrixLookAtLH(&g_Unknown6c6d18.viewMatrix, &from, &lookAt, &up);
        g_Camera.distance = fabsf(zDist);
        D3DXMatrixPerspectiveFovLH(&g_Unknown6c6d18.projectionMatrix, fovAngle, aspectXY, 100.0f, 10000.0f + extraFar);
        g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_VIEW, &g_Unknown6c6d18.viewMatrix);
        g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_PROJECTION, &g_Unknown6c6d18.projectionMatrix);
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
