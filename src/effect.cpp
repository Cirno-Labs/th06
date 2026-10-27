// The file with EffectManager, up to 0x40f7c0 at least.

#include "effect.h"

EffectManager::EffectManager()
{
    this->Reset();
}

void EffectManager::Reset()
{
    memset(this, 0, sizeof(EffectManager));
}
