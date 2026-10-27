// The file with ScreenEffect, from 0x42f6d0.

#include "screeneffect.h"

int ScreenEffect::AddedCallback(ScreenEffect *effect)
{
    effect->timer.Set(0);
    return 0;
}
