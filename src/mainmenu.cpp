// cflags: /Os -/Oi
// The /Os file from 0x43575d to 0x43a7a6, with the title menu.

#include <direct.h>
#include <stdio.h>
#include <string.h>

#include "ascii.h"
#include "gamemanager.h"
#include "mainmenu.h"
#include "screeneffect.h"
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

int MainMenu::UpdateReplayMenu()
{
    Sprite *text;
    int i;
    HANDLE findHandle;
    int replayCount;
    ReplayHeader *data;
    char path[0x40];
    WIN32_FIND_DATAA fd;

    switch (this->unk_81f0)
    {
    case 12:
        if (this->unk_81f4 == 60)
        {
            if (LoadReplayMenuResources(this) != 0)
            {
                LogPrintf(&g_Unknown69d998, "セレクト画面の読み込みに失敗\n");
                g_Unknown6c6d18.scene = 4;
                return 0;
            }
            replayCount = 0;
            for (i = 0; i < 15; i++)
            {
                sprintf(path, "./replay/th6_%.2d.rpy", i + 1);
                data = (ReplayHeader *)LoadFileData(path, 1);
                if (data == NULL)
                {
                    continue;
                }
                if (!ValidateReplay(data, g_FileSize))
                {
                    this->replays[replayCount] = *data;
                    strcpy(this->replayPaths[replayCount], path);
                    sprintf(this->replayLabels[replayCount], "No.%.2d", i + 1);
                    replayCount++;
                }
                free(data);
            }
            _mkdir("./replay");
            _chdir("./replay");
            findHandle = FindFirstFileA("th6_ud????.rpy", &fd);
            if (findHandle != INVALID_HANDLE_VALUE)
            {
                for (i = 0; i < 45; i++)
                {
                    data = (ReplayHeader *)LoadFileData(fd.cFileName, 1);
                    if (data == NULL)
                    {
                        continue;
                    }
                    if (!ValidateReplay(data, g_FileSize))
                    {
                        this->replays[replayCount] = *data;
                        sprintf(this->replayPaths[replayCount], "./replay/%s", fd.cFileName);
                        sprintf(this->replayLabels[replayCount], "User ");
                        replayCount++;
                    }
                    free(data);
                    if (!FindNextFileA(findHandle, &fd))
                    {
                        break;
                    }
                }
            }
            FindClose(findHandle);
            _chdir("../");
            this->unk_81ec = replayCount;
            this->unk_81fc = 0;
            this->unk_8214 = this->unk_8210;
            this->unk_8210 = 0;
            this->unk_81f0 = 13;
            for (text = this->sprites, i = 0; i < 0x7a; i++, text++)
            {
                text->pendingInterrupt = 15;
            }
            this->cursor = 0;
            break;
        }
        break;
    case 14:
        if (this->unk_81f4 == 36)
        {
            this->unk_81f0 = 0;
            this->unk_81f4 = 0;
        }
        break;
    case 13:
        if (this->unk_81f4 < 40)
        {
            break;
        }
        if (this->unk_81ec != 0)
        {
            MoveMenuCursor(this, this->unk_81ec);
            this->unk_81e8 = this->cursor;
            if (WAS_PRESSED(0x1001))
            {
                this->unk_81f0 = 15;
                for (text = &this->sprites[0x61], i = 0; i < 0x19; i++, text++)
                {
                    text->pendingInterrupt = 0x11;
                }
                text = &this->sprites[this->unk_81e8 + 0x63];
                text->pendingInterrupt = 0x10;
                this->unk_81f4 = 0;
                this->cursor = 0;
                g_Unknown6d3f50.QueueSound(10, 0);
                this->unk_10edc = (ReplayHeader *)LoadFileData(this->replayPaths[this->unk_81e8], 1);
                ValidateReplay(this->unk_10edc, g_FileSize);
                for (i = 0; i < 7; i++)
                {
                    if (this->unk_10edc->stages[i + 1] != NULL)
                    {
                        this->unk_10edc->stages[i + 1] =
                            (ReplayStage *)((int)this->unk_10edc + (int)this->unk_10edc->stages[i + 1]);
                    }
                }
                while (this->replays[this->unk_81e8].stages[this->cursor + 1] == NULL)
                {
                    this->cursor++;
                    if (this->cursor >= 7)
                    {
                        return 0;
                    }
                }
            }
        }
        if (WAS_PRESSED(0xa))
        {
            this->unk_81f0 = 14;
            this->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            g_Unknown6d3f50.QueueSound(0xb, 0);
            this->cursor = 0;
            break;
        }
        break;
    case 15:
        if (this->unk_81f4 < 40)
        {
            break;
        }
        i = MoveMenuCursor(this, 7);
        if (i < 0)
        {
            while (this->replays[this->unk_81e8].stages[this->cursor + 1] == NULL)
            {
                this->cursor--;
                if (this->cursor < 0)
                {
                    this->cursor = 6;
                }
            }
        }
        else if (i > 0)
        {
            while (this->replays[this->unk_81e8].stages[this->cursor + 1] == NULL)
            {
                this->cursor++;
                if (this->cursor >= 7)
                {
                    this->cursor = 0;
                }
            }
        }
        if (WAS_PRESSED(0x1001) && &this->unk_10edc[this->cursor].stages[1] != NULL)
        {
            g_GameManager.unk_001c = 1;
            g_Unknown6c6d18.gameSpeed = 1.0f;
            strcpy(g_GameManager.replayFile, this->replayPaths[this->unk_81e8]);
            g_GameManager.difficulty = this->unk_10edc->difficulty;
            g_GameManager.character = this->unk_10edc->shotType / 2;
            g_GameManager.shotType = this->unk_10edc->shotType % 2;
            i = 0;
            while (this->unk_10edc->stages[i + 1] == NULL)
            {
                i++;
            }
            g_GameManager.lives = this->unk_10edc->stages[i + 1]->lives;
            g_GameManager.bombs = this->unk_10edc->stages[i + 1]->bombs;
            MemFree(this->unk_10edc);
            this->unk_10edc = NULL;
            g_GameManager.stage = this->cursor;
            g_Unknown6c6d18.scene = 2;
            return 1;
        }
        if (WAS_PRESSED(0xa))
        {
            MemFree(this->unk_10edc);
            this->unk_10edc = NULL;
            this->unk_81f0 = 13;
            this->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            g_Unknown6d3f50.QueueSound(0xb, 0);
            this->unk_81f0 = 13;
            for (text = this->sprites, i = 0; i < 0x7a; i++, text++)
            {
                text->pendingInterrupt = 15;
            }
            this->cursor = this->unk_81e8;
        }
        break;
    }
    return 0;
}

int MainMenu::DrawReplayMenu()
{
    Sprite *text;
    int i;
    int top;

    text = &this->sprites[0x62];
    g_AsciiText.AddFormat(&text->pos, "No.   Name      Date     Player   Rank");
    i = this->unk_81e8 - this->unk_81e8 % 15;
    for (top = i; i < top + 15; i++)
    {
        if (i >= this->unk_81ec)
        {
            break;
        }
        text++;
        if (!g_Unknown6c6d18.HasFlag0Or8())
        {
            if (i == this->unk_81e8)
            {
                g_AsciiText.SetColor(0xffff8080);
            }
            else
            {
                g_AsciiText.SetColor(0xff808080);
            }
        }
        else
        {
            g_AsciiText.SetUnk6234(i == this->unk_81e8);
            if (i == this->unk_81e8)
            {
                g_AsciiText.SetColor(0xffffffff);
            }
            else
            {
                g_AsciiText.SetColor(0xff808080);
            }
        }
        g_AsciiText.AddFormat(&text->pos, "%s %8s  %8s %7s  %7s", this->replayLabels[i], this->replays[i].name,
                              this->replays[i].date, g_MenuShotTypeNames[this->replays[i].shotType],
                              g_MenuDifficultyNames[this->replays[i].difficulty]);
    }
    if (this->unk_81f0 == 15 && this->unk_10edc != NULL)
    {
        g_AsciiText.SetColor(0xffffffff);
        g_AsciiText.SetUnk6234(0);
        text = &this->sprites[0x61];
        g_AsciiText.AddFormat(&text->pos, "       %2.3f%%", this->unk_10edc->slowdown);
        text = &this->sprites[0x72];
        g_AsciiText.AddFormat(&text->pos, "Stage  LastScore");
        for (i = 0; i < 7; i++)
        {
            text++;
            if (!g_Unknown6c6d18.HasFlag0Or8())
            {
                if (i == this->cursor)
                {
                    g_AsciiText.SetColor(0xffff8080);
                }
                else
                {
                    g_AsciiText.SetColor(0xff808080);
                }
            }
            else
            {
                g_AsciiText.SetUnk6234(i == this->cursor);
                if (i == this->cursor)
                {
                    g_AsciiText.SetColor(0xffffffff);
                }
                else
                {
                    g_AsciiText.SetColor(0xff808080);
                }
            }
            if (this->unk_10edc->stages[i + 1] != NULL)
            {
                g_AsciiText.AddFormat(&text->pos, "%s %9d", g_MenuStageNames[i], this->unk_10edc->stages[i + 1]->score);
            }
            else
            {
                g_AsciiText.AddFormat(&text->pos, "%s ---------", g_MenuStageNames[i]);
            }
        }
    }
    g_AsciiText.SetColor(0xffffffff);
    g_AsciiText.SetUnk6234(0);
    return 0;
}

void MainMenu::HighlightOption(Sprite *vm, int row, int value, int current)
{
    if (value != current)
    {
        if (!g_Unknown6c6d18.HasFlag0Or8())
        {
            vm->color = 0x80202050;
        }
        else
        {
            g_SpriteEngine->SetSprite(vm, vm->unk_0b2);
        }
        vm->scaleX = 1.0f;
        vm->scaleY = 1.0f;
        vm->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        if (!g_Unknown6c6d18.HasFlag0Or8())
        {
            vm->color = 0x800000ff;
        }
        else if (vm->unk_0b2 < 0x122)
        {
            g_SpriteEngine->SetSprite(vm, vm->unk_0b2 + 0x7a);
        }
        else
        {
            g_SpriteEngine->SetSprite(vm, vm->unk_0b2 + 0x73);
        }
        vm->posOffset = D3DXVECTOR3(-2.0f, -2.0f, 0.0f);
    }
    if (row != this->cursor)
    {
        if (!g_Unknown6c6d18.cfg.flags.unk0)
        {
            vm->color = vm->color & 0xffffff | 0x80000000;
        }
        else
        {
            vm->color = vm->color & 0xffffff | 0x80000000;
        }
        vm->posOffset += D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        if (!g_Unknown6c6d18.cfg.flags.unk0)
        {
            vm->color = vm->color & 0xffffff | 0xff000000;
        }
        else
        {
            vm->color = vm->color & 0xffffff | 0xff000000;
        }
        vm->posOffset += D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
    }
}

int MainMenu::UpdateOptions()
{
    int i;
    Sprite *vm;

    MoveMenuCursor(this, 9);
    vm = &this->sprites[8];
    for (i = 0; i < 9; i++)
    {
        if (i >= 5 && i <= 7)
        {
            this->HighlightOption(&this->sprites[i + 0x43], i, i, this->cursor);
        }
        else
        {
            this->HighlightOption(vm, i, i, this->cursor);
            vm++;
        }
    }
    for (i = 0; i < 5; i++, vm++)
    {
        this->HighlightOption(vm, 0, i, g_Unknown6c6d18.cfg.lives);
    }
    for (i = 0; i < 4; i++, vm++)
    {
        this->HighlightOption(vm, 1, i, g_Unknown6c6d18.cfg.bombs);
    }
    for (i = 0; i < 2; i++, vm++)
    {
        this->HighlightOption(vm, 2, i, g_Unknown6c6d18.cfg.colorMode16Bits);
    }
    for (i = 0; i < 2; i++, vm++)
    {
        this->HighlightOption(vm, 4, i, g_Unknown6c6d18.cfg.playSounds);
    }
    vm = &this->sprites[0x4d];
    for (i = 0; i < 3; i++, vm++)
    {
        this->HighlightOption(vm, 3, i, g_Unknown6c6d18.cfg.musicMode);
    }
    vm = &this->sprites[0x4b];
    for (i = 0; i < 2; i++, vm++)
    {
        this->HighlightOption(vm, 5, i, this->unk_8231);
    }
    if (this->unk_81f4 >= 0x20)
    {
        if (WAS_PRESSED_OR_HELD(0x40))
        {
            switch (this->cursor)
            {
            case 0:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                if (g_Unknown6c6d18.cfg.lives <= 0)
                {
                    g_Unknown6c6d18.cfg.lives = 5;
                }
                g_Unknown6c6d18.cfg.lives = g_Unknown6c6d18.cfg.lives - 1;
                break;
            case 1:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                if (g_Unknown6c6d18.cfg.bombs <= 0)
                {
                    g_Unknown6c6d18.cfg.bombs = 4;
                }
                g_Unknown6c6d18.cfg.bombs = g_Unknown6c6d18.cfg.bombs - 1;
                break;
            case 2:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                if (g_Unknown6c6d18.cfg.colorMode16Bits <= 0)
                {
                    g_Unknown6c6d18.cfg.colorMode16Bits = 2;
                }
                g_Unknown6c6d18.cfg.colorMode16Bits = g_Unknown6c6d18.cfg.colorMode16Bits - 1;
                break;
            case 3:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                g_Unknown6c6d18.StopMusic();
                if (g_Unknown6c6d18.cfg.musicMode <= 0)
                {
                    g_Unknown6c6d18.cfg.musicMode = 3;
                }
                g_Unknown6c6d18.cfg.musicMode = g_Unknown6c6d18.cfg.musicMode - 1;
                g_Unknown6c6d18.LoadMusic("bgm/th06_01.mid");
                g_Unknown6c6d18.PlayMusic("bgm/th06_01.mid");
                break;
            case 4:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                if (g_Unknown6c6d18.cfg.playSounds <= 0)
                {
                    g_Unknown6c6d18.cfg.playSounds = 2;
                }
                g_Unknown6c6d18.cfg.playSounds = g_Unknown6c6d18.cfg.playSounds - 1;
                break;
            case 5:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                if (this->unk_8231 <= 0)
                {
                    this->unk_8231 = 2;
                }
                this->unk_8231 = this->unk_8231 - 1;
                break;
            }
        }
        if (WAS_PRESSED(0xa))
        {
            this->cursor = 8;
            g_Unknown6d3f50.QueueSound(0xb, 0);
        }
        if (WAS_PRESSED_OR_HELD(0x80))
        {
            switch (this->cursor)
            {
            case 0:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                g_Unknown6c6d18.cfg.lives = g_Unknown6c6d18.cfg.lives + 1;
                if (g_Unknown6c6d18.cfg.lives >= 5)
                {
                    g_Unknown6c6d18.cfg.lives = 0;
                }
                break;
            case 1:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                g_Unknown6c6d18.cfg.bombs = g_Unknown6c6d18.cfg.bombs + 1;
                if (g_Unknown6c6d18.cfg.bombs >= 4)
                {
                    g_Unknown6c6d18.cfg.bombs = 0;
                }
                break;
            case 2:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                g_Unknown6c6d18.cfg.colorMode16Bits = g_Unknown6c6d18.cfg.colorMode16Bits + 1;
                if (g_Unknown6c6d18.cfg.colorMode16Bits >= 2)
                {
                    g_Unknown6c6d18.cfg.colorMode16Bits = 0;
                }
                break;
            case 3:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                g_Unknown6c6d18.StopMusic();
                g_Unknown6c6d18.cfg.musicMode = g_Unknown6c6d18.cfg.musicMode + 1;
                if (g_Unknown6c6d18.cfg.musicMode >= 3)
                {
                    g_Unknown6c6d18.cfg.musicMode = 0;
                }
                g_Unknown6c6d18.LoadMusic("bgm/th06_01.mid");
                g_Unknown6c6d18.PlayMusic("bgm/th06_01.mid");
                break;
            case 4:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                g_Unknown6c6d18.cfg.playSounds = g_Unknown6c6d18.cfg.playSounds + 1;
                if (g_Unknown6c6d18.cfg.playSounds >= 2)
                {
                    g_Unknown6c6d18.cfg.playSounds = 0;
                }
                break;
            case 5:
                g_Unknown6d3f50.QueueSound(0xc, 0);
                this->unk_8231 = this->unk_8231 + 1;
                if (this->unk_8231 >= 2)
                {
                    this->unk_8231 = 0;
                }
                break;
            }
        }
        if (WAS_PRESSED(0x1001))
        {
            switch (this->cursor)
            {
            case 7:
                this->unk_81f0 = 5;
                this->unk_81f4 = 0;
                for (i = 0; i < 0x7a; i++)
                {
                    this->sprites[i].pendingInterrupt = 5;
                }
                this->cursor = 0;
                g_Unknown6d3f50.QueueSound(10, 0);
                this->controllerMapping = g_ControllerMapping;
                g_ControllerMapping.unk_00[4] = -1;
                g_ControllerMapping.unk_00[5] = -1;
                break;
            case 6:
                g_Unknown6c6d18.StopMusic();
                g_Unknown6c6d18.cfg.lives = 2;
                g_Unknown6c6d18.cfg.bombs = 3;
                g_Unknown6c6d18.cfg.musicMode = 1;
                g_Unknown6c6d18.cfg.playSounds = 1;
                g_Unknown6c6d18.cfg.unk_1d = 1;
                g_Unknown6c6d18.cfg.windowed = 0;
                g_Unknown6c6d18.cfg.frameSkip = 0;
                g_Unknown6c6d18.LoadMusic("bgm/th06_01.mid");
                g_Unknown6c6d18.PlayMusic("bgm/th06_01.mid");
                break;
            case 8:
                this->unk_81f0 = 2;
                this->unk_81f4 = 0;
                for (i = 0; i < 0x7a; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                this->cursor = 6;
                g_Unknown6d3f50.QueueSound(0xb, 0);
                if (this->unk_8230 != g_Unknown6c6d18.cfg.colorMode16Bits ||
                    this->unk_8231 != g_Unknown6c6d18.cfg.windowed || this->unk_8232 != g_Unknown6c6d18.cfg.frameSkip)
                {
                    g_Unknown6c6d18.cfg.frameSkip = this->unk_8232;
                    g_Unknown6c6d18.cfg.windowed = this->unk_8231;
                    g_Unknown6c6d18.scene = 5;
                    return 1;
                }
                break;
            }
        }
    }
    return 0;
}

int MainMenu::DrawPracticeScores()
{
    int i;
    DWORD fade;
    int n2;
    int si3;
    D3DXVECTOR3 pos;

    if (this->unk_81f0 == 17)
    {
        pos.x = 320.0f;
        pos.y = 200.0f;
        pos.z = 0.0f;
        fade = this->unk_81f4 < 30 ? this->unk_81f4 * 255 / 30 : 255;
        si3 = g_GameManager.character * 2 + g_GameManager.shotType;
        n2 = g_GameManager.clrds[si3].unk_11[g_GameManager.difficulty] > 6
                    ? 6
                    : g_GameManager.clrds[si3].unk_11[g_GameManager.difficulty];
        if (g_GameManager.difficulty == 0 && n2 == 6)
        {
            n2 = 5;
        }
        for (i = 0; i < n2; i++)
        {
            if (i == this->cursor)
            {
                g_AsciiText.color = fade << 24 | 0xc0f0f0;
            }
            else
            {
                g_AsciiText.color = (fade >> 1) << 24 | 0x80c0c0;
            }
            g_AsciiText.AddFormat(&pos, "STAGE %d  %.9d", i + 1, g_GameManager.pscrs[si3][i][g_GameManager.difficulty].score);
            pos.y += 24.0f;
        }
        g_AsciiText.color = 0xffffffff;
    }
    return 0;
}

int MainMenu::OnDraw(MainMenu *menu)
{
    Sprite *vm;
    int i;
    float rect[4];
    DWORD alpha;

    vm = menu->sprites;
    rect[0] = 0.0f;
    rect[1] = 0.0f;
    rect[2] = 640.0f;
    rect[3] = 480.0f;
    if (menu->unk_81f0 == 0)
    {
        return 1;
    }
    g_SpriteEngine->ClearTexture();
    g_SpriteEngine->CopySurfaceToBackBuffer(0, 0, 0, 0, 0);
    if (menu->unk_8210 != 0)
    {
        if (menu->unk_820c < menu->unk_8210)
        {
            menu->unk_820c++;
        }
        alpha = ((menu->unk_8200 & 0xff000000) >> 24) - ((menu->unk_81fc & 0xff000000) >> 24);
        DrawColorRect(rect, (alpha * menu->unk_820c / menu->unk_8210 + ((menu->unk_81fc & 0xff000000) >> 24)) << 24 |
                                menu->unk_8200 & 0xffffff);
    }
    else if (menu->unk_820c != 0)
    {
        menu->unk_820c--;
        alpha = ((menu->unk_8200 & 0xff000000) >> 24) - ((menu->unk_81fc & 0xff000000) >> 24);
        DrawColorRect(rect, (alpha * menu->unk_820c / menu->unk_8214 + ((menu->unk_81fc & 0xff000000) >> 24)) << 24 |
                                menu->unk_8200 & 0xffffff);
    }
    for (i = 0; i < 0x62; i++, vm++)
    {
        if (vm->HasTexture())
        {
            D3DXVECTOR3 savedPos = vm->pos;
            vm->pos += vm->posOffset;
            g_SpriteEngine->DrawRotated(vm);
            vm->pos = savedPos;
        }
    }
    switch (menu->unk_81f0)
    {
    case 13:
    case 14:
    case 15:
        menu->DrawReplayMenu();
        break;
    }
    menu->DrawPracticeScores();
    return 1;
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

int LoadSelectResources(MainMenu *menu)
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
    if (g_SpriteEngine->LoadAnm(0x1b, "data/select01.anm", 0x150) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x1c, "data/select02.anm", 0x153) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x1d, "data/select03.anm", 0x15a) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x1e, "data/select04.anm", 0x15c) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x1f, "data/select05.anm", 0x155) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x20, "data/slpl00a.anm", 0x156) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x21, "data/slpl00b.anm", 0x157) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x22, "data/slpl01a.anm", 0x158) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x23, "data/slpl01b.anm", 0x159) != 0)
    {
        return -1;
    }
    for (vm = &menu->sprites[0x50], i = 0x150; i <= 0x15f; i++, vm++)
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

int MainMenu::AddedCallback(MainMenu *menu)
{
    ScoreDat *scoreDat;

    if (!g_GameManager.unk_1824)
    {
        g_Unknown6c6d18.LoadMusic("bgm/th06_01.mid");
    }
    g_SpriteEngine->ClearMenuScripts();
    menu->unk_81e4 = 0;
    switch (g_Unknown6c6d18.prevScene)
    {
    case 2:
    case 3:
    case 7:
        menu->cursor = g_GameManager.difficulty == 4;
        break;
    case 6:
        menu->cursor = 4;
        break;
    case 9:
        menu->cursor = 5;
        break;
    default:
        menu->cursor = 0;
        break;
    }
    if (g_GameManager.unk_1823)
    {
        menu->cursor = 2;
    }
    g_GameManager.unk_1823 = 0;
    if (!g_Unknown6c6d18.cfg.flags.unk0)
    {
        menu->unk_8208 = 0x80004000;
        menu->unk_8204 = 0xff008000;
    }
    else
    {
        menu->unk_8208 = 0x80ffffff;
        menu->unk_8204 = 0xffffffff;
    }
    menu->unk_81fc = 0;
    menu->unk_8200 = 0x40000000;
    menu->unk_820c = 0;
    menu->unk_8210 = 0;
    menu->unk_10f28 = 0x10;
    menu->unk_10edc = NULL;
    scoreDat = LoadScoreDat("score.dat");
    LoadClears(scoreDat, g_GameManager.clrds);
    LoadPracticeScores(scoreDat, &g_GameManager.pscrs[0][0][0]);
    ReleaseScoreDat(scoreDat);
    if (!g_GameManager.unk_1824)
    {
        if (g_Unknown6c6d18.unk_400 == 0)
        {
            g_Unknown6c6d18.PlayMusic("bgm/th06_01.mid");
            ScreenEffect::StartEffect(0, 120, 0xffffff, 0, 0);
        }
        else
        {
            ScreenEffect::StartEffect(0, 200, 0xffffff, 0, 0);
        }
    }
    g_GameManager.unk_1824 = 0;
    g_GameManager.demoFrames = 0;
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

BOOL GameManager::IsExtraUnlocked(int character, int shot)
{
    return this->clrds[character * 2 + shot].unk_0c[1] == 99 || this->clrds[character * 2 + shot].unk_0c[2] == 99 ||
                   this->clrds[character * 2 + shot].unk_0c[3] == 99
               ? TRUE
               : FALSE;
}
