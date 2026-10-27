// cflags: /Os -/Oi
// The file starting at 0x42333b, up to 0x425e03. Built with /Os: its
// functions are laid out without alignment.

#include "timer.h"

void Timer::Initialize()
{
    this->current = 0;
    this->previous = -1;
    this->subFrame = 0.0f;
}
