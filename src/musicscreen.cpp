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

int MusicScreen::OnUpdate(MusicScreen *screen)
{
    int oldState = screen->state;

again:
    switch (screen->state)
    {
    case 0:
        if (screen->UpdateIntro() == 0)
        {
            break;
        }
        // The state changed: run the new one this frame.
        goto again;
    case 1:
        if (screen->UpdateList() != 0)
        {
            return 0;
        }
        break;
    }
    if (oldState != screen->state)
    {
        screen->stateFrames = 0;
    }
    else
    {
        screen->stateFrames++;
    }
    g_SpriteEngine->Step(&screen->unk_0024);
    return 1;
}
