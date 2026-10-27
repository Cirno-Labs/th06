// The file with EffectManager, up to 0x40f7c0 at least.

#include "effect.h"
#include "unsorted.h"

EffectManager::EffectManager()
{
    this->Reset();
}

void EffectManager::Reset()
{
    memset(this, 0, sizeof(EffectManager));
}

int EffectManager::RegisterChain()
{
    EffectManager *mgr = &g_EffectManager;

    mgr->Reset();
    g_EffectCalcChain.SetCallback((ChainCallback)EffectManager::OnUpdate);
    g_EffectCalcChain.addedCallback = (ChainCallback)EffectManager::AddedCallback;
    g_EffectCalcChain.deletedCallback = (ChainCallback)EffectManager::DeletedCallback;
    g_EffectCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_EffectCalcChain, 10) != 0)
    {
        return -1;
    }
    g_EffectDrawChain.SetCallback((ChainCallback)EffectManager::OnDraw);
    g_EffectDrawChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_EffectDrawChain, 8);
    return 0;
}

int EffectManager::DeletedCallback(EffectManager *mgr)
{
    g_SpriteEngine->ReleaseAnm(0xb);
    return 0;
}

void EffectManager::RemoveChain()
{
    g_Chain.Remove(&g_EffectCalcChain);
    g_Chain.Remove(&g_EffectDrawChain);
}

int EffectManager::AddedCallback(EffectManager *mgr)
{
    mgr->Reset();
    switch (g_GameManager.stage)
    {
    case 0:
    case 1:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff01.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 2:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff02.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 3:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff03.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 4:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff04.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 5:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff05.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 6:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff05.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 7:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff04.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    }
    return 0;
}

int EffectManager::OnDraw(EffectManager *mgr)
{
    Effect *eff = mgr->effects;
    int i;

    for (i = 0; i < 0x200; i++, eff++)
    {
        if (!eff->active)
        {
            continue;
        }
        eff->sprite.pos = eff->pos;
        g_SpriteEngine->Func433150(&eff->sprite);
    }
    return 1;
}

int EffectManager::OnUpdate(EffectManager *mgr)
{
    Effect *eff = mgr->effects;
    int i;

    mgr->activeCount = 0;
    for (i = 0; i < 0x200; i++, eff++)
    {
        if (!eff->active)
        {
            continue;
        }
        mgr->activeCount++;
        if (eff->update != NULL && eff->update(eff) != 1)
        {
            eff->active = 0;
        }
        if (g_SpriteEngine->Step(&eff->sprite) != 0)
        {
            eff->active = 0;
        }
        eff->timer.Tick();
    }
    return 1;
}
