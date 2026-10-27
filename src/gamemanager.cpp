// cflags: /Os
// The /Os file with GameManager, from 0x41b1a4 at least.

#include "gamemanager.h"
#include "bullet.h"
#include "effect.h"
#include "enemy.h"
#include "enemyscript.h"
#include "player.h"
#include "unsorted.h"

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
    Func404650();
    BulletManager::RemoveChain();
    Player::RemoveChain();
    EnemyManager::RemoveChain();
    g_EnemyScripts.Release();
    EffectManager::RemoveChain();
    Gui::RemoveChain();
    Func42aab0();
    mgr->unk_1821 = 0;
    g_AsciiText.Initialize();
    return 0;
}
