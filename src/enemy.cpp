// The file with EnemyManager.

#include "enemy.h"
#include "sprite.h"
#include "unsorted.h"

int EnemyManager::DeletedCallback(EnemyManager *mgr)
{
    g_SpriteEngine->ReleaseAnm(9);
    g_SpriteEngine->ReleaseAnm(8);
    return 0;
}

void EnemyManager::RemoveChain()
{
    g_Chain.Remove(&g_EnemyCalcChain);
    g_Chain.Remove(&g_EnemyDrawChain);
}

void ReleaseEnemyEffects(Enemy *enemy)
{
    int i;

    for (i = 0; i < enemy->effectCount; i++)
    {
        if (enemy->effects[i] == NULL)
        {
            continue;
        }
        enemy->effects[i]->unk_17a = 1;
        enemy->effects[i] = NULL;
    }
    enemy->effectCount = 0;
}

int EnemyManager::RegisterChain(char *anmPath, char *anmPath2)
{
    EnemyManager *mgr = &g_EnemyManager;

    mgr->Initialize();
    mgr->anmPath = anmPath;
    mgr->anmPath2 = anmPath2;
    g_EnemyCalcChain.SetCallback((ChainCallback)EnemyManager::OnUpdate);
    g_EnemyCalcChain.addedCallback = (ChainCallback)EnemyManager::AddedCallback;
    g_EnemyCalcChain.deletedCallback = (ChainCallback)EnemyManager::DeletedCallback;
    g_EnemyCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_EnemyCalcChain, 9) != 0)
    {
        return -1;
    }
    g_EnemyDrawChain.SetCallback((ChainCallback)EnemyManager::OnDraw);
    g_EnemyDrawChain.arg = mgr;
    if (g_Chain.AddToDrawChain(&g_EnemyDrawChain, 6) != 0)
    {
        return -1;
    }
    return 0;
}

int EnemyManager::AddedCallback(EnemyManager *mgr)
{
    DWORD *unused = mgr->unk_00ed0;

    if (mgr->anmPath != NULL && g_SpriteEngine->LoadAnm(8, mgr->anmPath, 0x100) != 0)
    {
        return -1;
    }
    if (mgr->anmPath2 != NULL && g_SpriteEngine->LoadAnm(9, mgr->anmPath2, 0x100) != 0)
    {
        return -1;
    }
    mgr->unk_ee5b8 = g_Rng.GetRandomU16InRange(3);
    mgr->unk_ee5ba = g_Rng.GetRandomU16InRange(8);
    mgr->unk_ee5c8 = 0;
    mgr->unk_ee5dc = 0;
    return 0;
}
