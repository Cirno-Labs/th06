// The file with ScreenEffect, from 0x42f6d0.

#include "screeneffect.h"

int ScreenEffect::AddedCallback(ScreenEffect *effect)
{
    effect->timer.Set(0);
    return 0;
}

int ScreenEffect::DeletedCallback(ScreenEffect *effect)
{
    effect->calcElem->deletedCallback = NULL;
    g_Chain.Remove(effect->drawElem);
    effect->drawElem = NULL;
    delete effect;
    effect = NULL;
    return 0;
}
