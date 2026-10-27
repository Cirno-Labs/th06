// cflags: /Os -/Oi
// Part of the /Os file starting at 0x42333b.

#include "musicscreen.h"

int MusicScreen::UpdateIntro()
{
    if (this->stateFrames >= 8)
    {
        this->state = 1;
    }
    return 0;
}
