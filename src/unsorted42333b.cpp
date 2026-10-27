// cflags: /Os -/Oi
// The file starting at 0x42333b, up to 0x425e03. Built with /Os: its
// functions are laid out without alignment.

#include "timer.h"
#include "unsorted.h"

void Timer::Initialize()
{
    this->current = 0;
    this->previous = -1;
    this->subFrame = 0.0f;
}

void Timer::Add(int frames)
{
    if (g_Unknown6c6d18.gameSpeed > 0.99f)
    {
        this->current += frames;
    }
    else if (frames < 0)
    {
        this->Sub(-frames);
    }
    else
    {
        this->previous = this->current;
        this->subFrame += g_Unknown6c6d18.frameAdvance * frames;
        while (this->subFrame >= 1.0f)
        {
            this->current++;
            this->subFrame -= 1.0f;
        }
    }
}
