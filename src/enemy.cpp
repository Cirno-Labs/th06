// The file with EnemyManager.

#include "enemy.h"
#include "sprite.h"

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
