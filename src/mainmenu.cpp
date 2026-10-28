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

int MainMenu::OnUpdate(MainMenu *menu)
{
    int i;
    Sprite *text3;
    DWORD time;
    int deltaX;
    float fps3;
    float seconds;
    short button3;
    int status;
    short pressedKey;
    BYTE *pad1;

    if (menu->unk_10ee0 < 0x10)
    {
        timeBeginPeriod(1);
        if (menu->unk_10f30 == 0)
        {
            menu->unk_10f30 = timeGetTime();
        }
        time = timeGetTime();
        timeEndPeriod(1);
        menu->unk_10f2c++;
        deltaX = time - menu->unk_10f30;
        if (deltaX >= 700)
        {
            menu->unk_10f30 = time;
            menu->unk_10f2c = 0;
        }
        else if (deltaX >= 500)
        {
            seconds = deltaX / 1000.0f;
            fps3 = menu->unk_10f2c * 1000.0f / deltaX;
            if (fps3 >= 57.0f)
            {
                menu->unk_10ee4[menu->unk_10ee0] = fps3;
                menu->unk_10ee0++;
            }
            menu->unk_10f30 = time;
            menu->unk_10f2c = 0;
        }
    }
    switch (menu->unk_81f0)
    {
    case 0:
        status = menu->Func4378a2();
        if (status == -1)
        {
            return 0;
        }
    case 1:
        menu->unk_81f8++;
        if (g_Buttons & 0xffff)
        {
            menu->unk_81f8 = 0;
        }
        if (menu->unk_81f8 >= 720)
        {
            goto demo;
        }
        if (menu->Func4379e4())
        {
            break;
        }
        menu->unk_81f8 = 0;
    case 2:
        menu->UpdateTitleMenu();
        if (g_Buttons & 0xffff)
        {
            menu->unk_81f8 = 0;
        }
        menu->unk_81f8++;
        if (menu->unk_81f8 >= 720)
        {
        demo:
            g_GameManager.unk_001c = 1;
            g_GameManager.unk_1824 = 1;
            g_GameManager.demoFrames = 0;
            g_App.gameSpeed = 1.0f;
            strcpy(g_GameManager.replayFile, "data/demo/demo00.rpy");
            g_GameManager.stage = 3;
            g_GameManager.difficulty = 3;
            g_App.scene = 2;
            return 0;
        }
        break;
    case 12:
    case 13:
    case 14:
    case 15:
        if (menu->UpdateReplayMenu())
        {
            return 0;
        }
        break;
    case 3:
        if (menu->UpdateOptions())
        {
            return 0;
        }
        break;
    case 5:
        MoveMenuCursor(menu, 11);
        for (text3 = &menu->sprites[0x22], i = 0; i < 11; i++, text3++)
        {
            HighlightMenuEntry(text3, i, menu->cursor, menu->unk_8204, menu->unk_8208, 0x73);
        }
        for (i = 0; i < 9; i++, text3++)
        {
            if (((short *)&menu->controllerMapping)[i] < 0)
            {
                text3->flags.visible = 0;
                continue;
            }
            text3->flags.visible = 1;
            HighlightMenuEntry(text3, i, menu->cursor, menu->unk_8204, menu->unk_8208, 0x73);
        }
        for (i = 0; i < 18; i++, text3++)
        {
            if (((short *)&menu->controllerMapping)[i / 2] < 0)
            {
                text3->flags.visible = 0;
                continue;
            }
            text3->flags.visible = 1;
            button3 = ((short *)&menu->controllerMapping)[i / 2];
            if (i % 2 == 0)
            {
                g_SpriteEngine->SetSprite(text3, button3 / 10 + 0x100);
            }
            else
            {
                g_SpriteEngine->SetSprite(text3, button3 % 10 + 0x100);
            }
            text3->unk_0b2 = text3->spriteIndex;
            HighlightMenuEntry(text3, i / 2, menu->cursor, menu->unk_8204, menu->unk_8208, 0x7a);
        }
        if (menu->unk_81f4 >= 0x20)
        {
            pad1 = GetPadButtons();
            for (pressedKey = 0; pressedKey < 0x20; pressedKey++)
            {
                if (pad1[pressedKey] & 0x80)
                {
                    break;
                }
            }
            if (pressedKey < 0x20 && g_Unknown478690 != pressedKey)
            {
                g_SoundPlayer.QueueSound(10, 0);
                switch (menu->cursor)
                {
                case 0:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.shootButton, 1);
                    menu->controllerMapping.shootButton = pressedKey;
                    break;
                case 1:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.bombButton, 0);
                    menu->controllerMapping.bombButton = pressedKey;
                    break;
                case 2:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.focusButton, 1);
                    menu->controllerMapping.focusButton = pressedKey;
                    break;
                case 3:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.menuButton, 0);
                    menu->controllerMapping.menuButton = pressedKey;
                    break;
                case 4:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.upButton, 0);
                    menu->controllerMapping.upButton = pressedKey;
                    break;
                case 5:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.downButton, 0);
                    menu->controllerMapping.downButton = pressedKey;
                    break;
                case 6:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.leftButton, 0);
                    menu->controllerMapping.leftButton = pressedKey;
                    break;
                case 7:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.rightButton, 0);
                    menu->controllerMapping.rightButton = pressedKey;
                    break;
                case 8:
                    ReassignButton(menu, pressedKey, menu->controllerMapping.skipButton, 0);
                    menu->controllerMapping.skipButton = pressedKey;
                    break;
                }
            }
            g_Unknown478690 = pressedKey;
            if (WAS_PRESSED(0x1001))
            {
                switch (menu->cursor)
                {
                case 9:
                {
                    ControllerMapping defaults = {0, 1, 0, -1, -1, -1, -1, -1, -1};
                    menu->controllerMapping = defaults;
                    break;
                }
                case 10:
                    menu->unk_81f0 = 3;
                    menu->unk_81f4 = 0;
                    for (pressedKey = 0; pressedKey < 0x7a; pressedKey++)
                    {
                        menu->sprites[pressedKey].pendingInterrupt = 3;
                    }
                    menu->cursor = 7;
                    g_SoundPlayer.QueueSound(0xb, 0);
                    g_ControllerMapping = menu->controllerMapping;
                    g_App.cfg.controllerMapping = menu->controllerMapping;
                    break;
                }
            }
        }
        break;
    case 6:
        if (menu->unk_81f4 == 60)
        {
            if (LoadSelectResources(menu) != 0)
            {
                LogPrintf(&g_Log, "セレクト画面の読み込みに失敗\n");
                g_App.scene = 4;
                return 0;
            }
            menu->unk_81f0 = 7;
            menu->unk_81fc = 0;
            menu->unk_8214 = menu->unk_8210;
            menu->unk_8210 = 0;
            if (g_GameManager.difficulty < 4)
            {
                for (i = 0; i < 0x7a; i++)
                {
                    menu->sprites[i].pendingInterrupt = 6;
                }
                menu->cursor = g_App.cfg.defaultDifficulty;
            }
            else
            {
                for (i = 0; i < 0x7a; i++)
                {
                    menu->sprites[i].pendingInterrupt = 0x12;
                }
                menu->cursor = 0;
            }
        }
        else
        {
            break;
        }
    case 8:
        if (menu->unk_81f4 == 36)
        {
            menu->unk_81f0 = 0;
            menu->unk_81f4 = 0;
        }
        break;
    case 7:
        text3 = &menu->sprites[0x51];
        if (g_GameManager.difficulty < 4)
        {
            MoveMenuCursor(menu, 4);
            for (i = 0; i < 4; i++, text3++)
            {
                if (i != menu->cursor)
                {
                    if (!g_App.cfg.flags.unk0)
                    {
                        text3->color = 0x60000000;
                    }
                    else
                    {
                        text3->color = 0x60ffffff;
                    }
                    text3->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
                    text3->alphaInterpTime = 0;
                }
                else
                {
                    if (!g_App.cfg.flags.unk0)
                    {
                        text3->color = 0xff000000;
                    }
                    else
                    {
                        text3->color = 0xffffffff;
                    }
                    text3->posOffset = D3DXVECTOR3(-6.0f, -6.0f, 0.0f);
                }
            }
            text3->flags.visible = 0;
        }
        else
        {
            for (i = 0; i < 4; i++, text3++)
            {
                text3->flags.visible = 0;
            }
            for (i = 4; i < 5; i++, text3++)
            {
                if (!g_App.cfg.flags.unk0)
                {
                    text3->color = 0xff000000;
                }
                else
                {
                    text3->color = 0xffffffff;
                }
                text3->posOffset = D3DXVECTOR3(-6.0f, -6.0f, 0.0f);
            }
        }
        if (WAS_PRESSED(0xa))
        {
            menu->unk_81f0 = 8;
            menu->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                menu->sprites[i].pendingInterrupt = 4;
            }
            g_SoundPlayer.QueueSound(0xb, 0);
            if (g_GameManager.difficulty < 4)
            {
                g_App.cfg.defaultDifficulty = menu->cursor;
                if (!g_GameManager.unk_1823)
                {
                    menu->cursor = 0;
                }
                else
                {
                    menu->cursor = 2;
                }
            }
            else
            {
                menu->cursor = 1;
            }
            break;
        }
        else if (WAS_PRESSED(0x1001))
        {
            menu->unk_81f0 = 9;
            menu->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                menu->sprites[i].pendingInterrupt = 7;
            }
            g_SoundPlayer.QueueSound(10, 0);
            if (g_GameManager.difficulty < 4)
            {
                text3 = &menu->sprites[menu->cursor + 0x51];
                text3->pendingInterrupt = 8;
                g_GameManager.difficulty = menu->cursor;
                menu->cursor = g_GameManager.character;
            }
            else
            {
                text3 = &menu->sprites[0x55];
                text3->pendingInterrupt = 8;
                g_GameManager.difficulty = 4;
                if (g_GameManager.IsExtraUnlocked(g_GameManager.character, 0) ||
                    g_GameManager.IsExtraUnlocked(g_GameManager.character, 1))
                {
                    menu->cursor = g_GameManager.character;
                }
                else
                {
                    menu->cursor = 1 - g_GameManager.character;
                }
            }
            g_App.cfg.defaultDifficulty = g_GameManager.difficulty;
            for (text3 = &menu->sprites[0x56], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != menu->cursor)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            break;
        }
        break;
    case 9:
        if (menu->unk_81f4 < 30)
        {
            break;
        }
        if (WAS_PRESSED_OR_HELD(0x40))
        {
            menu->cursor++;
            if (menu->cursor >= 2)
            {
                menu->cursor -= 2;
            }
            if (g_GameManager.difficulty == 4 && !g_GameManager.IsExtraUnlocked(menu->cursor, 0) &&
                !g_GameManager.IsExtraUnlocked(menu->cursor, 1))
            {
                menu->cursor--;
                if (menu->cursor < 0)
                {
                    menu->cursor += 2;
                }
                goto characterChosen;
            }
            g_SoundPlayer.QueueSound(0xc, 0);
            for (text3 = &menu->sprites[0x56], i = 0; i < 2; i++, text3++)
            {
                if (i == menu->cursor)
                {
                    text3->pendingInterrupt = 9;
                    text3++;
                    text3->pendingInterrupt = 9;
                }
                else
                {
                    text3->pendingInterrupt = 0xc;
                    text3++;
                    text3->pendingInterrupt = 0xc;
                }
            }
        }
        if (WAS_PRESSED_OR_HELD(0x80))
        {
            menu->cursor--;
            if (menu->cursor < 0)
            {
                menu->cursor += 2;
            }
            if (g_GameManager.difficulty == 4 && !g_GameManager.IsExtraUnlocked(menu->cursor, 0) &&
                !g_GameManager.IsExtraUnlocked(menu->cursor, 1))
            {
                menu->cursor++;
                if (menu->cursor >= 2)
                {
                    menu->cursor -= 2;
                }
            }
            else
            {
                g_SoundPlayer.QueueSound(0xc, 0);
                for (text3 = &menu->sprites[0x56], i = 0; i < 2; i++, text3++)
                {
                    if (i == menu->cursor)
                    {
                        text3->pendingInterrupt = 10;
                        text3++;
                        text3->pendingInterrupt = 10;
                    }
                    else
                    {
                        text3->pendingInterrupt = 0xb;
                        text3++;
                        text3->pendingInterrupt = 0xb;
                    }
                }
            }
        }
    characterChosen:
        if (WAS_PRESSED(0xa))
        {
            menu->unk_81f0 = 7;
            menu->unk_81f4 = 0;
            if (g_GameManager.difficulty < 4)
            {
                for (i = 0; i < 0x7a; i++)
                {
                    menu->sprites[i].pendingInterrupt = 6;
                }
                menu->cursor = g_App.cfg.defaultDifficulty;
            }
            else
            {
                for (i = 0; i < 0x7a; i++)
                {
                    menu->sprites[i].pendingInterrupt = 0x12;
                }
                menu->cursor = 0;
            }
            g_SoundPlayer.QueueSound(0xb, 0);
            break;
        }
        else if (WAS_PRESSED(0x1001))
        {
            menu->unk_81f0 = 11;
            menu->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                menu->sprites[i].pendingInterrupt = 0xd;
            }
            text3 = &menu->sprites[g_GameManager.difficulty + 0x51];
            text3->pendingInterrupt = 0;
            for (text3 = &menu->sprites[0x56], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != menu->cursor)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            for (text3 = &menu->sprites[0x5c], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != menu->cursor)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            g_GameManager.character = menu->cursor;
            if (g_GameManager.difficulty < 4)
            {
                menu->cursor = g_GameManager.shotType;
            }
            else if (g_GameManager.IsExtraUnlocked(g_GameManager.character, g_GameManager.shotType))
            {
                menu->cursor = g_GameManager.shotType;
            }
            else
            {
                menu->cursor = 1 - g_GameManager.shotType;
            }
            g_SoundPlayer.QueueSound(10, 0);
        }
        break;
    case 11:
        MoveMenuCursor(menu, 2);
        if (g_GameManager.difficulty == 4 && !g_GameManager.IsExtraUnlocked(g_GameManager.character, menu->cursor))
        {
            menu->cursor = 1 - menu->cursor;
        }
        for (text3 = &menu->sprites[0x5c], i = 0; i < 2; i++, text3 += 2)
        {
            text3[1].flags.colorOp = 1;
        }
        for (text3 = &menu->sprites[g_GameManager.character * 2 + 0x5c], i = 0; i < 2; i++, text3++)
        {
            text3->flags.colorOp = 1;
            text3->flags.unk0 = 1;
            if (i != menu->cursor)
            {
                if (!g_App.cfg.flags.unk0)
                {
                    text3->color = 0xa0000000;
                }
                else
                {
                    text3->color = 0xa0d0d0d0;
                }
                text3->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            }
            else
            {
                if (!g_App.cfg.flags.unk0)
                {
                    text3->color = 0xff202020;
                }
                else
                {
                    text3->color = 0xffffffff;
                }
                text3->posOffset = D3DXVECTOR3(-6.0f, -6.0f, 0.0f);
            }
        }
        if (menu->unk_81f4 < 30)
        {
            break;
        }
        if (WAS_PRESSED(0xa))
        {
            menu->unk_81f0 = 9;
            menu->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                menu->sprites[i].pendingInterrupt = 7;
            }
            for (text3 = &menu->sprites[0x5c], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != g_GameManager.character)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            text3 = &menu->sprites[g_GameManager.difficulty + 0x51];
            text3->pendingInterrupt = 0;
            g_SoundPlayer.QueueSound(0xb, 0);
            g_GameManager.shotType = menu->cursor;
            menu->cursor = g_GameManager.character;
            for (text3 = &menu->sprites[0x56], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != menu->cursor)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            break;
        }
        else if (WAS_PRESSED(0x1001))
        {
            g_GameManager.shotType = menu->cursor;
            if (!g_GameManager.unk_1823)
            {
                if (g_GameManager.difficulty < 4)
                {
                    g_GameManager.stage = 0;
                }
                else
                {
                    g_GameManager.stage = 6;
                }
            start:
                g_GameManager.lives = g_App.cfg.lives;
                g_GameManager.bombs = g_App.cfg.bombs;
                if (g_GameManager.difficulty == 4 || g_GameManager.unk_1823)
                {
                    g_GameManager.lives = 2;
                    g_GameManager.bombs = 3;
                }
                g_App.scene = 2;
                g_SoundPlayer.QueueSound(10, 0);
                g_GameManager.unk_001c = 0;
                {
                    float average;
                    float speed;

                    average = 0.0f;
                    if (menu->unk_10ee0 >= 2)
                    {
                        for (i = 0; i < menu->unk_10ee0; i++)
                        {
                            average += menu->unk_10ee4[i];
                        }
                        average /= i;
                    }
                    else
                    {
                        average = 60.0f;
                    }
                    if (average >= 155.0f)
                    {
                        speed = 0.375f;
                    }
                    else if (average >= 135.0f)
                    {
                        speed = 0.4f;
                    }
                    else if (average >= 110.0f)
                    {
                        speed = 0.5f;
                    }
                    else if (average >= 95.0f)
                    {
                        speed = 0.6f;
                    }
                    else if (average >= 87.5f)
                    {
                        speed = 0.6666667f;
                    }
                    else if (average >= 82.5f)
                    {
                        speed = 0.70588235f;
                    }
                    else if (average >= 77.5f)
                    {
                        speed = 0.75f;
                    }
                    else if (average >= 73.5f)
                    {
                        speed = 0.8f;
                    }
                    else if (average >= 68.0f)
                    {
                        speed = 0.85714287f;
                    }
                    else
                    {
                        speed = 1.0f;
                    }
                    DebugPrint2("Reflesh Rate = %f\n", 60.0f / speed);
                    g_App.gameSpeed = speed;
                }
                g_App.StopMusic();
                return 0;
            }
            menu->unk_81f0 = 17;
            menu->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                menu->sprites[i].pendingInterrupt = 0x13;
            }
            text3 = &menu->sprites[g_GameManager.difficulty + 0x51];
            text3->pendingInterrupt = 0;
            for (text3 = &menu->sprites[0x56], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != g_GameManager.character)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            for (text3 = &menu->sprites[0x5c], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != g_GameManager.character)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            menu->cursor = g_GameManager.unk_1a38;
            {
                int stages;

                stages = g_GameManager.clrds[g_GameManager.character * 2 + g_GameManager.shotType]
                                     .unk_11[g_GameManager.difficulty] > 6
                             ? 6
                             : g_GameManager.clrds[g_GameManager.character * 2 + g_GameManager.shotType]
                                   .unk_11[g_GameManager.difficulty];
                if (g_GameManager.difficulty == 0 && stages == 6)
                {
                    stages = 5;
                }
                if (menu->cursor >= stages)
                {
                    menu->cursor = 0;
                }
            }
        }
        break;
    case 17:
    {
        int stages;

        stages = g_GameManager.clrds[g_GameManager.character * 2 + g_GameManager.shotType]
                             .unk_11[g_GameManager.difficulty] > 6
                     ? 6
                     : g_GameManager.clrds[g_GameManager.character * 2 + g_GameManager.shotType]
                           .unk_11[g_GameManager.difficulty];
        if (g_GameManager.difficulty == 0 && stages == 6)
        {
            stages = 5;
        }
        MoveMenuCursor(menu, stages);
        if (menu->unk_81f4 < 30)
        {
            break;
        }
        if (WAS_PRESSED(0xa))
        {
            menu->unk_81f0 = 11;
            menu->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                menu->sprites[i].pendingInterrupt = 0xd;
            }
            text3 = &menu->sprites[g_GameManager.difficulty + 0x51];
            text3->pendingInterrupt = 0;
            for (text3 = &menu->sprites[0x56], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != g_GameManager.character)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            for (text3 = &menu->sprites[0x5c], i = 0; i < 2; i++, text3 += 2)
            {
                if (i != g_GameManager.character)
                {
                    text3[0].pendingInterrupt = 0;
                    text3[1].pendingInterrupt = 0;
                }
            }
            menu->cursor = g_GameManager.shotType;
            g_SoundPlayer.QueueSound(10, 0);
            break;
        }
        else if (WAS_PRESSED(0x1001))
        {
            g_GameManager.stage = menu->cursor;
            g_GameManager.unk_1a38 = menu->cursor;
            goto start;
        }
        break;
    }
    case 4:
        if (menu->unk_81f4 >= 60)
        {
            g_App.scene = 4;
            return 0;
        }
        break;
    case 10:
        if (menu->unk_81f4 >= 60)
        {
            g_App.scene = 6;
            return 0;
        }
        break;
    case 16:
        if (menu->unk_81f4 >= 60)
        {
            g_App.scene = 9;
            return 0;
        }
        break;
    }
    menu->unk_81f4++;
    for (i = 0; i < 0x7a; i++)
    {
        if (menu->sprites[i].HasTexture())
        {
            g_SpriteEngine->Step(&menu->sprites[i]);
        }
    }
    return 1;
}

int MoveMenuCursor(MainMenu *menu, int count)
{
    if (WAS_PRESSED_OR_HELD(0x10))
    {
        menu->cursor--;
        g_SoundPlayer.QueueSound(0xc, 0);
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
        g_SoundPlayer.QueueSound(0xc, 0);
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
    if (unk == 0 && menu->controllerMapping.shootButton == button)
    {
        menu->controllerMapping.shootButton = replacement;
    }
    if (menu->controllerMapping.bombButton == button)
    {
        menu->controllerMapping.bombButton = replacement;
    }
    if (unk == 0 && menu->controllerMapping.focusButton == button)
    {
        menu->controllerMapping.focusButton = replacement;
    }
    if (menu->controllerMapping.upButton == button)
    {
        menu->controllerMapping.upButton = replacement;
    }
    if (menu->controllerMapping.downButton == button)
    {
        menu->controllerMapping.downButton = replacement;
    }
    if (menu->controllerMapping.leftButton == button)
    {
        menu->controllerMapping.leftButton = replacement;
    }
    if (menu->controllerMapping.rightButton == button)
    {
        menu->controllerMapping.rightButton = replacement;
    }
    if (menu->controllerMapping.menuButton == button)
    {
        menu->controllerMapping.menuButton = replacement;
    }
    if (menu->controllerMapping.skipButton == button)
    {
        menu->controllerMapping.skipButton = replacement;
    }
}

void HighlightMenuEntry(Sprite *vm, int index, int cursor, D3DCOLOR selectedColor, D3DCOLOR color, int spriteOffset)
{
    if (index == cursor)
    {
        if (!g_App.HasFlag0Or8())
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
        if (!g_App.HasFlag0Or8())
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
        g_App.scene = 4;
        return -1;
    }
    if (g_App.startTime > 0)
    {
        current = timeGetTime();
        while (current - g_App.startTime >= 0 && current - g_App.startTime < 3000)
        {
            current = timeGetTime();
        }
        g_App.startTime = 0;
        g_App.PlayMusic("bgm/th06_01.mid");
    }
    for (i = 0; i < 0x7a; i++)
    {
        this->sprites[i].pendingInterrupt = 1;
        this->sprites[i].flags.colorOp = 1;
        if (!g_App.cfg.flags.unk0)
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
    if (!g_App.cfg.flags.unk0)
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

int MainMenu::UpdateTitleMenu()
{
    int i;
    Sprite *vm;

    i = MoveMenuCursor(this, 8);
    if (this->cursor == 1 && !g_GameManager.IsExtraUnlocked(0, 0) && !g_GameManager.IsExtraUnlocked(0, 1) &&
        !g_GameManager.IsExtraUnlocked(1, 0) && !g_GameManager.IsExtraUnlocked(1, 1))
    {
        this->cursor += i;
    }
    for (vm = this->sprites, i = 0; i < 8; i++, vm++)
    {
        HighlightMenuEntry(vm, i, this->cursor, 0xffff0000, 0x80300000, 0x7a);
    }
    if (this->unk_81f4 >= 20)
    {
        if (WAS_PRESSED(0x1001))
        {
            switch (this->cursor)
            {
            case 0:
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            this->unk_81f0 = 6;
            g_GameManager.unk_1823 = 0;
            if (g_GameManager.difficulty >= 4)
            {
                g_GameManager.difficulty = 1;
            }
            if (g_App.cfg.defaultDifficulty >= 4)
            {
                g_App.cfg.defaultDifficulty = 1;
            }
            this->unk_81f4 = 0;
            this->unk_81fc = 0x40000000;
            this->unk_8200 = 0xff000000;
            this->unk_820c = 0;
            this->unk_8210 = 60;
            g_SoundPlayer.QueueSound(10, 0);
            break;
            case 1:
            if (g_GameManager.IsExtraUnlocked(0, 0) || g_GameManager.IsExtraUnlocked(0, 1) ||
                g_GameManager.IsExtraUnlocked(1, 0) || g_GameManager.IsExtraUnlocked(1, 1))
            {
                for (i = 0; i < 0x7a; i++)
                {
                    this->sprites[i].pendingInterrupt = 4;
                }
                this->unk_81f0 = 6;
                g_GameManager.unk_1823 = 0;
                g_GameManager.difficulty = 4;
                this->unk_81f4 = 0;
                this->unk_81fc = 0x40000000;
                this->unk_8200 = 0xff000000;
                this->unk_820c = 0;
                this->unk_8210 = 60;
                g_SoundPlayer.QueueSound(10, 0);
            }
            else
            {
                g_SoundPlayer.QueueSound(0xb, 0);
            }
            break;
            case 2:
            g_GameManager.unk_1823 = 1;
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            this->unk_81f0 = 6;
            if (g_GameManager.difficulty >= 4)
            {
                g_GameManager.difficulty = 1;
            }
            if (g_App.cfg.defaultDifficulty >= 4)
            {
                g_App.cfg.defaultDifficulty = 1;
            }
            this->unk_81f4 = 0;
            this->unk_81fc = 0x40000000;
            this->unk_8200 = 0xff000000;
            this->unk_820c = 0;
            this->unk_8210 = 60;
            g_SoundPlayer.QueueSound(10, 0);
            break;
            case 3:
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            this->unk_81f0 = 12;
            g_GameManager.unk_1823 = 0;
            this->unk_81f4 = 0;
            this->unk_81fc = 0x40000000;
            this->unk_8200 = 0xff000000;
            this->unk_820c = 0;
            this->unk_8210 = 60;
            g_SoundPlayer.QueueSound(10, 0);
            break;
            case 4:
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            this->unk_81f0 = 10;
            this->unk_81f4 = 0;
            this->unk_81fc = 0x40000000;
            this->unk_8200 = 0xff000000;
            this->unk_820c = 0;
            this->unk_8210 = 60;
            g_SoundPlayer.QueueSound(10, 0);
            break;
            case 5:
            this->unk_81f0 = 16;
            this->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            g_SoundPlayer.QueueSound(10, 0);
            break;
            case 6:
            this->unk_81f0 = 3;
            this->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 3;
            }
            this->cursor = 0;
            this->unk_8230 = g_App.cfg.colorMode16Bits;
            this->unk_8231 = g_App.cfg.windowed;
            this->unk_8232 = g_App.cfg.frameSkip;
            g_SoundPlayer.QueueSound(10, 0);
            break;
            case 7:
            this->unk_81f0 = 4;
            this->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            g_SoundPlayer.QueueSound(0xb, 0);
            break;
            }
        }
        if (WAS_PRESSED(0x200))
        {
            this->unk_81f0 = 4;
            this->unk_81f4 = 0;
            for (i = 0; i < 0x7a; i++)
            {
                this->sprites[i].pendingInterrupt = 4;
            }
            g_SoundPlayer.QueueSound(0xb, 0);
        }
        if (WAS_PRESSED(0xa))
        {
            this->cursor = 7;
            g_SoundPlayer.QueueSound(0xb, 0);
        }
    }
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
                LogPrintf(&g_Log, "セレクト画面の読み込みに失敗\n");
                g_App.scene = 4;
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
                g_SoundPlayer.QueueSound(10, 0);
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
            g_SoundPlayer.QueueSound(0xb, 0);
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
            g_App.gameSpeed = 1.0f;
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
            g_App.scene = 2;
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
            g_SoundPlayer.QueueSound(0xb, 0);
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
        if (!g_App.HasFlag0Or8())
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
            if (!g_App.HasFlag0Or8())
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
        if (!g_App.HasFlag0Or8())
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
        if (!g_App.HasFlag0Or8())
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
        if (!g_App.cfg.flags.unk0)
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
        if (!g_App.cfg.flags.unk0)
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
        this->HighlightOption(vm, 0, i, g_App.cfg.lives);
    }
    for (i = 0; i < 4; i++, vm++)
    {
        this->HighlightOption(vm, 1, i, g_App.cfg.bombs);
    }
    for (i = 0; i < 2; i++, vm++)
    {
        this->HighlightOption(vm, 2, i, g_App.cfg.colorMode16Bits);
    }
    for (i = 0; i < 2; i++, vm++)
    {
        this->HighlightOption(vm, 4, i, g_App.cfg.playSounds);
    }
    vm = &this->sprites[0x4d];
    for (i = 0; i < 3; i++, vm++)
    {
        this->HighlightOption(vm, 3, i, g_App.cfg.musicMode);
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
                g_SoundPlayer.QueueSound(0xc, 0);
                if (g_App.cfg.lives <= 0)
                {
                    g_App.cfg.lives = 5;
                }
                g_App.cfg.lives = g_App.cfg.lives - 1;
                break;
            case 1:
                g_SoundPlayer.QueueSound(0xc, 0);
                if (g_App.cfg.bombs <= 0)
                {
                    g_App.cfg.bombs = 4;
                }
                g_App.cfg.bombs = g_App.cfg.bombs - 1;
                break;
            case 2:
                g_SoundPlayer.QueueSound(0xc, 0);
                if (g_App.cfg.colorMode16Bits <= 0)
                {
                    g_App.cfg.colorMode16Bits = 2;
                }
                g_App.cfg.colorMode16Bits = g_App.cfg.colorMode16Bits - 1;
                break;
            case 3:
                g_SoundPlayer.QueueSound(0xc, 0);
                g_App.StopMusic();
                if (g_App.cfg.musicMode <= 0)
                {
                    g_App.cfg.musicMode = 3;
                }
                g_App.cfg.musicMode = g_App.cfg.musicMode - 1;
                g_App.LoadMusic("bgm/th06_01.mid");
                g_App.PlayMusic("bgm/th06_01.mid");
                break;
            case 4:
                g_SoundPlayer.QueueSound(0xc, 0);
                if (g_App.cfg.playSounds <= 0)
                {
                    g_App.cfg.playSounds = 2;
                }
                g_App.cfg.playSounds = g_App.cfg.playSounds - 1;
                break;
            case 5:
                g_SoundPlayer.QueueSound(0xc, 0);
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
            g_SoundPlayer.QueueSound(0xb, 0);
        }
        if (WAS_PRESSED_OR_HELD(0x80))
        {
            switch (this->cursor)
            {
            case 0:
                g_SoundPlayer.QueueSound(0xc, 0);
                g_App.cfg.lives = g_App.cfg.lives + 1;
                if (g_App.cfg.lives >= 5)
                {
                    g_App.cfg.lives = 0;
                }
                break;
            case 1:
                g_SoundPlayer.QueueSound(0xc, 0);
                g_App.cfg.bombs = g_App.cfg.bombs + 1;
                if (g_App.cfg.bombs >= 4)
                {
                    g_App.cfg.bombs = 0;
                }
                break;
            case 2:
                g_SoundPlayer.QueueSound(0xc, 0);
                g_App.cfg.colorMode16Bits = g_App.cfg.colorMode16Bits + 1;
                if (g_App.cfg.colorMode16Bits >= 2)
                {
                    g_App.cfg.colorMode16Bits = 0;
                }
                break;
            case 3:
                g_SoundPlayer.QueueSound(0xc, 0);
                g_App.StopMusic();
                g_App.cfg.musicMode = g_App.cfg.musicMode + 1;
                if (g_App.cfg.musicMode >= 3)
                {
                    g_App.cfg.musicMode = 0;
                }
                g_App.LoadMusic("bgm/th06_01.mid");
                g_App.PlayMusic("bgm/th06_01.mid");
                break;
            case 4:
                g_SoundPlayer.QueueSound(0xc, 0);
                g_App.cfg.playSounds = g_App.cfg.playSounds + 1;
                if (g_App.cfg.playSounds >= 2)
                {
                    g_App.cfg.playSounds = 0;
                }
                break;
            case 5:
                g_SoundPlayer.QueueSound(0xc, 0);
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
                g_SoundPlayer.QueueSound(10, 0);
                this->controllerMapping = g_ControllerMapping;
                g_ControllerMapping.upButton = -1;
                g_ControllerMapping.downButton = -1;
                break;
            case 6:
                g_App.StopMusic();
                g_App.cfg.lives = 2;
                g_App.cfg.bombs = 3;
                g_App.cfg.musicMode = 1;
                g_App.cfg.playSounds = 1;
                g_App.cfg.defaultDifficulty = 1;
                g_App.cfg.windowed = 0;
                g_App.cfg.frameSkip = 0;
                g_App.LoadMusic("bgm/th06_01.mid");
                g_App.PlayMusic("bgm/th06_01.mid");
                break;
            case 8:
                this->unk_81f0 = 2;
                this->unk_81f4 = 0;
                for (i = 0; i < 0x7a; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                this->cursor = 6;
                g_SoundPlayer.QueueSound(0xb, 0);
                if (this->unk_8230 != g_App.cfg.colorMode16Bits ||
                    this->unk_8231 != g_App.cfg.windowed || this->unk_8232 != g_App.cfg.frameSkip)
                {
                    g_App.cfg.frameSkip = this->unk_8232;
                    g_App.cfg.windowed = this->unk_8231;
                    g_App.scene = 5;
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

    g_App.LoadArchive(3, "紅魔郷TL.dat");
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
        if (!g_App.cfg.flags.unk0)
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
        if (!g_App.cfg.flags.unk0)
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
    DebugPrint2("Debug : title 開始 VRAM = %d\n", g_App.d3dDevice->GetAvailableTextureMem());
    menu->unk_81f0 = unk != 0 ? 12 : 0;
    g_App.gameSpeed = 0.0f;
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
        g_App.LoadMusic("bgm/th06_01.mid");
    }
    g_SpriteEngine->ClearMenuScripts();
    menu->unk_81e4 = 0;
    switch (g_App.prevScene)
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
    if (!g_App.cfg.flags.unk0)
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
        if (g_App.startTime == 0)
        {
            g_App.PlayMusic("bgm/th06_01.mid");
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

    g_App.d3dDevice->ResourceManagerDiscardBytes(0);
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
