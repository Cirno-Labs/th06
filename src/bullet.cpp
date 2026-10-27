// The file with BulletManager.

#include "bullet.h"
#include "unsorted.h"

void BulletManager::Reset()
{
    memset(this, 0, sizeof(BulletManager));
}

int BulletManager::DeletedCallback(BulletManager *mgr)
{
    if ((g_Unknown6c6d18.scene != 3) != 0)
    {
        g_SpriteEngine->ReleaseAnm(6);
        g_SpriteEngine->ReleaseAnm(7);
    }
    return 0;
}

void BulletManager::RemoveChain()
{
    g_Chain.Remove(&g_BulletCalcChain);
    g_Chain.Remove(&g_BulletDrawChain);
}

int BulletManager::RegisterChain(int unk)
{
    BulletManager *mgr = &g_BulletManager;

    if (!g_Unknown6c6d18.cfg.flags.unk0)
    {
        g_Unknown476438 = g_Unknown476358;
    }
    else
    {
        g_Unknown476438 = g_Unknown4763c8;
    }
    mgr->Reset();
    mgr->unk_f5c14 = unk;
    g_BulletCalcChain.SetCallback((ChainCallback)BulletManager::OnUpdate);
    g_BulletCalcChain.addedCallback = (ChainCallback)BulletManager::AddedCallback;
    g_BulletCalcChain.deletedCallback = (ChainCallback)BulletManager::DeletedCallback;
    g_BulletCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_BulletCalcChain, 0xb) != 0)
    {
        return -1;
    }
    g_BulletDrawChain.SetCallback((ChainCallback)BulletManager::OnDraw);
    g_BulletDrawChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_BulletDrawChain, 9);
    return 0;
}
