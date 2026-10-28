// cflags: /Os -/Oi
// The /Os file from 0x43575d to 0x43a7a6, with the title menu.

#include "gamemanager.h"
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

void HighlightMenuEntry(Sprite *vm, int index, int cursor, D3DCOLOR selectedColor, D3DCOLOR color, int spriteOffset)
{
    if (index == cursor)
    {
        if (!g_Unknown6c6d18.HasFlag0Or8())
        {
            vm->color = selectedColor;
        }
        else
        {
            g_SpriteEngine->SetSprite(vm, vm->unk_0b2 + spriteOffset);
            vm->color = selectedColor & 0xff000000 | 0xffffff;
        }
        vm->posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
    }
    else
    {
        if (!g_Unknown6c6d18.HasFlag0Or8())
        {
            vm->color = color;
        }
        else
        {
            g_SpriteEngine->SetSprite(vm, vm->unk_0b2);
            vm->color = color & 0xff000000 | 0xffffff;
        }
        vm->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    }
}

int MainMenu::Func4378a2()
{
    DWORD current;
    int i;

    if (LoadTitleResources(this) != 0)
    {
        g_Unknown6c6d18.scene = 4;
        return -1;
    }
    if (g_Unknown6c6d18.unk_400 > 0)
    {
        current = timeGetTime();
        while (current - g_Unknown6c6d18.unk_400 >= 0 && current - g_Unknown6c6d18.unk_400 < 3000)
        {
            current = timeGetTime();
        }
        g_Unknown6c6d18.unk_400 = 0;
        g_Unknown6c6d18.PlayMusic("bgm/th06_01.mid");
    }
    for (i = 0; i < 0x7a; i++)
    {
        this->sprites[i].pendingInterrupt = 1;
        this->sprites[i].flags.colorOp = 1;
        if (!g_Unknown6c6d18.cfg.flags.unk0)
        {
            this->sprites[i].color = 0xff000000;
        }
        else
        {
            this->sprites[i].color = 0xffffffff;
        }
        this->sprites[i].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    }
    this->unk_81f0 = 1;
    return 0;
}

int MainMenu::Func4379e4()
{
    int i;

    if (this->unk_81f4 < 30)
    {
        return 1;
    }
    if (!WAS_PRESSED_OR_HELD(0x160b))
    {
        return 1;
    }
    this->unk_81f4 = 0;
    this->unk_81f0 = 2;
    for (i = 0; i < 0x7a; i++)
    {
        this->sprites[i].pendingInterrupt = 2;
    }
    if (!g_Unknown6c6d18.cfg.flags.unk0)
    {
        this->sprites[this->cursor].color = 0xffff0000;
    }
    else
    {
        this->sprites[this->cursor].color = 0xffffe0e0;
    }
    this->sprites[this->cursor].posOffset = D3DXVECTOR3(-6.0f, -6.0f, 0.0f);
    this->unk_81fc = 0;
    this->unk_8200 = 0x40000000;
    this->unk_820c = 0;
    this->unk_8210 = 60;
    return 0;
}

int LoadTitleResources(MainMenu *menu)
{
    int i;

    g_Unknown6c6d18.LoadArchive(3, "紅魔郷TL.dat");
    for (i = 0x1b; i <= 0x24; i++)
    {
        g_SpriteEngine->ReleaseAnm(i);
    }
    if (g_SpriteEngine->LoadAnm(0x15, "data/title01.anm", 0x100) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x18, "data/title02.anm", 0x11b) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x19, "data/title03.anm", 0x11f) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x1a, "data/title04.anm", 0x122) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x16, "data/title01s.anm", 0x17a) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x17, "data/title04s.anm", 0x195) != 0)
    {
        return -1;
    }
    for (i = 0; i < 0x50; i++)
    {
        g_SpriteEngine->Func4051b0(&menu->sprites[i], i + 0x100);
        menu->sprites[i].flags.unk0 = 0;
        menu->sprites[i].unk_0b2 = menu->sprites[i].spriteIndex;
        menu->sprites[i].flags.zWriteDisabled = 1;
    }
    if (g_SpriteEngine->LoadSurface(0, "data/title/title00.jpg") != 0)
    {
        return -1;
    }
    return 0;
}

int LoadReplayMenuResources(MainMenu *menu)
{
    int i;
    Sprite *vm;

    for (i = 0x15; i <= 0x1a; i++)
    {
        g_SpriteEngine->ReleaseAnm(i);
    }
    if (g_SpriteEngine->LoadSurface(0, "data/title/select00.jpg") != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x24, "data/replay00.anm", 0x160) != 0)
    {
        return -1;
    }
    for (vm = &menu->sprites[0x60], i = 0x160; i <= 0x179; i++, vm++)
    {
        g_SpriteEngine->Func4051b0(vm, i);
        vm->flags.unk0 = 0;
        vm->flags.colorOp = 1;
        if (!g_Unknown6c6d18.cfg.flags.unk0)
        {
            vm->color = 0xff000000;
        }
        else
        {
            vm->color = 0xffffffff;
        }
        vm->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        vm->unk_0b2 = vm->spriteIndex;
        vm->flags.zWriteDisabled = 1;
    }
    return 0;
}

int SetUpTitleScene(int unk)
{
    MainMenu *menu = &g_MainMenu;

    memset(menu, 0, sizeof(MainMenu));
    g_GameManager.unk_181f = 0;
    DebugPrint2("Debug : title 開始 VRAM = %d\n", g_Unknown6c6d18.d3dDevice->GetAvailableTextureMem());
    menu->unk_81f0 = unk != 0 ? 12 : 0;
    g_Unknown6c6d18.gameSpeed = 0.0f;
    menu->calcChain = g_Chain.CreateElem((ChainCallback)MainMenu::OnUpdate);
    menu->calcChain->arg = menu;
    menu->calcChain->addedCallback = (ChainCallback)MainMenu::AddedCallback;
    menu->calcChain->deletedCallback = (ChainCallback)MainMenu::DeletedCallback;
    menu->unk_81f4 = 0;
    if (g_Chain.AddToCalcChain(menu->calcChain, 2) != 0)
    {
        return -1;
    }
    menu->drawChain = g_Chain.CreateElem((ChainCallback)MainMenu::OnDraw);
    menu->drawChain->arg = menu;
    g_Chain.AddToDrawChain(menu->drawChain, 0);
    menu->unk_10f30 = 0;
    menu->unk_81f4 = 60;
    menu->unk_10f2c = 0;
    return 0;
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
    g_Chain.Remove(menu->drawChain);
    menu->drawChain = NULL;
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
