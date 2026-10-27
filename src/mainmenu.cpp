// cflags: /Os
// The /Os file from 0x435670 to 0x43a7a6, with the title menu.

#include "mainmenu.h"
#include "unsorted.h"

MainMenu::MainMenu()
{
}

void Func43a70a()
{
    int i;

    for (i = 21; i <= 27; i++)
    {
        g_SpriteEngine->ReleaseAnm(i);
    }
}
