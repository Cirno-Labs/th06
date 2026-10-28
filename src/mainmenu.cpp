// cflags: /Os
// The /Os file from 0x43575d to 0x43a7a6, with the title menu.

#include "mainmenu.h"
#include "unsorted.h"

MainMenu::MainMenu()
{
}

int MoveMenuCursor(MainMenu *menu, int count)
{
    if (WAS_PRESSED_OR_HELD(0x10))
    {
        menu->cursor--;
        g_Unknown6d3f50.QueueSound(0xc, 0);
        if (menu->cursor < 0)
        {
            menu->cursor = count - 1;
        }
        if (menu->cursor >= count)
        {
            menu->cursor = 0;
        }
        return -1;
    }
    if (WAS_PRESSED_OR_HELD(0x20))
    {
        menu->cursor++;
        g_Unknown6d3f50.QueueSound(0xc, 0);
        if (menu->cursor < 0)
        {
            menu->cursor = count - 1;
        }
        if (menu->cursor >= count)
        {
            menu->cursor = 0;
        }
        return 1;
    }
    return 0;
}

void ReassignButton(MainMenu *menu, short button, short replacement, int unk)
{
    if (unk == 0 && menu->controllerMapping.unk_00[0] == button)
    {
        menu->controllerMapping.unk_00[0] = replacement;
    }
    if (menu->controllerMapping.unk_00[1] == button)
    {
        menu->controllerMapping.unk_00[1] = replacement;
    }
    if (unk == 0 && menu->controllerMapping.unk_00[2] == button)
    {
        menu->controllerMapping.unk_00[2] = replacement;
    }
    if (menu->controllerMapping.unk_00[4] == button)
    {
        menu->controllerMapping.unk_00[4] = replacement;
    }
    if (menu->controllerMapping.unk_00[5] == button)
    {
        menu->controllerMapping.unk_00[5] = replacement;
    }
    if (menu->controllerMapping.unk_00[6] == button)
    {
        menu->controllerMapping.unk_00[6] = replacement;
    }
    if (menu->controllerMapping.unk_00[7] == button)
    {
        menu->controllerMapping.unk_00[7] = replacement;
    }
    if (menu->controllerMapping.unk_00[3] == button)
    {
        menu->controllerMapping.unk_00[3] = replacement;
    }
    if (menu->controllerMapping.unk_00[8] == button)
    {
        menu->controllerMapping.unk_00[8] = replacement;
    }
}

int MainMenu::DeletedCallback(MainMenu *menu)
{
    int i;

    g_Unknown6c6d18.d3dDevice->ResourceManagerDiscardBytes(0);
    Func43a70a();
    for (i = 0x1b; i <= 0x24; i++)
    {
        g_SpriteEngine->ReleaseAnm(i);
    }
    g_SpriteEngine->ReleaseSurface(0);
    g_SpriteEngine->ClearMenuScripts();
    g_Chain.Remove(menu->calcChain);
    menu->calcChain = NULL;
    MemFree(menu->unk_10edc);
    return 0;
}

void Func43a70a()
{
    int i;

    for (i = 21; i <= 27; i++)
    {
        g_SpriteEngine->ReleaseAnm(i);
    }
}
