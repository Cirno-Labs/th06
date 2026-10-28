// The file starting at 0x401000: text in the ASCII font.

#include <stdarg.h>
#include <stdio.h>

#include "ascii.h"
#include "unsorted.h"

AsciiText::AsciiText()
{
}

int PauseMenu::UpdatePauseMenu()
{
    int i;

    if (WAS_PRESSED(8))
    {
        this->state = 3;
        for (i = 0; i < 6; i++)
        {
            if (this->sprites[i].flags.unk0)
            {
                this->sprites[i].pendingInterrupt = 2;
            }
        }
        this->stateFrames = 0;
        this->background.pendingInterrupt = 1;
    }
    if (WAS_PRESSED(0x200))
    {
        this->state = 6;
        for (i = 0; i < 6; i++)
        {
            if (this->sprites[i].flags.unk0)
            {
                this->sprites[i].pendingInterrupt = 2;
            }
        }
        this->stateFrames = 0;
    }
    switch (this->state)
    {
    case 0:
        for (i = 0; i < 6; i++)
        {
            g_SpriteEngine->StartScript(&this->sprites[i], i + 2);
        }
        for (i = 0; i < 3; i++)
        {
            this->sprites[i].pendingInterrupt = 1;
        }
        this->state++;
        this->stateFrames = 0;
        if (g_App.lockableBackBuffer)
        {
            g_SpriteEngine->SetPendingCopy(3, 0x20, 0x10, 0x180, 0x1c0);
            g_SpriteEngine->StartScript(&this->background, 0x718);
            this->background.pos.x = 32.0f;
            this->background.pos.y = 16.0f;
            this->background.pos.z = 0.0f;
        }
    case 1:
        this->sprites[1].color = 0xffff8080;
        this->sprites[2].color = 0x80808080;
        this->sprites[1].scaleY = 1.7f;
        this->sprites[1].scaleX = 1.7f;
        this->sprites[2].scaleY = 1.5f;
        this->sprites[2].scaleX = 1.5f;
        this->sprites[1].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->sprites[2].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->stateFrames >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->state = 2;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 3; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                this->state = 3;
                this->stateFrames = 0;
                this->background.pendingInterrupt = 1;
            }
        }
        break;
    case 2:
        this->sprites[1].color = 0x80808080;
        this->sprites[2].color = 0xffff8080;
        this->sprites[1].scaleY = 1.5f;
        this->sprites[1].scaleX = 1.5f;
        this->sprites[2].scaleY = 1.7f;
        this->sprites[2].scaleX = 1.7f;
        this->sprites[1].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        this->sprites[2].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        if (this->stateFrames >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->state = 1;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 3; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                for (i = 3; i < 6; i++)
                {
                    this->sprites[i].pendingInterrupt = 1;
                }
                this->state = 5;
                this->stateFrames = 0;
            }
        }
        break;
    case 3:
        if (this->stateFrames >= 20)
        {
            this->state = 0;
            g_GameManager.isPaused = 0;
            for (i = 0; i < 6; i++)
            {
                this->sprites[i].ClearUnk0();
            }
        }
        break;
    case 4:
        this->sprites[4].color = 0xffff8080;
        this->sprites[5].color = 0x80808080;
        this->sprites[4].scaleY = 1.7f;
        this->sprites[4].scaleX = 1.7f;
        this->sprites[5].scaleY = 1.5f;
        this->sprites[5].scaleX = 1.5f;
        this->sprites[4].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->sprites[5].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->stateFrames >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->state = 5;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 3; i < 6; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                this->state = 6;
                this->stateFrames = 0;
            }
        }
        break;
    case 5:
        this->sprites[4].color = 0x80808080;
        this->sprites[5].color = 0xffff8080;
        this->sprites[4].scaleY = 1.5f;
        this->sprites[4].scaleX = 1.5f;
        this->sprites[5].scaleY = 1.7f;
        this->sprites[5].scaleX = 1.7f;
        this->sprites[4].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        this->sprites[5].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        if (this->stateFrames >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->state = 4;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 3; i++)
                {
                    this->sprites[i].pendingInterrupt = 1;
                }
                for (i = 3; i < 6; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                this->state = 2;
                this->stateFrames = 0;
            }
        }
        break;
    case 6:
        if (this->stateFrames >= 20)
        {
            this->state = 0;
            g_GameManager.isPaused = 0;
            g_App.scene = 1;
            for (i = 0; i < 6; i++)
            {
                this->sprites[i].ClearUnk0();
            }
        }
        break;
    }
    for (i = 0; i < 6; i++)
    {
        g_SpriteEngine->Step(&this->sprites[i]);
    }
    if (g_App.lockableBackBuffer)
    {
        g_SpriteEngine->Step(&this->background);
    }
    this->stateFrames++;
    return 0;
}

void PauseMenu::DrawPauseMenu()
{
    int i;

    if (g_GameManager.isPaused)
    {
        g_App.viewport.X = g_GameManager.arena[0];
        g_App.viewport.Y = g_GameManager.arena[1];
        g_App.viewport.Width = g_GameManager.arena[2];
        g_App.viewport.Height = g_GameManager.arena[3];
        g_App.d3dDevice->SetViewport(&g_App.viewport);
        if (g_App.lockableBackBuffer && this->state != 0)
        {
            Sprite sprite = this->background;

            sprite.flags.zWriteDisabled = 1;
            g_SpriteEngine->Draw(&sprite);
        }
        for (i = 0; i < 6; i++)
        {
            if (this->sprites[i].flags.unk0)
            {
                g_SpriteEngine->Draw(&this->sprites[i]);
            }
        }
    }
}

int PauseMenu::UpdateContinueMenu()
{
    int i;

    if (g_GameManager.isInPractice)
    {
        g_GameManager.isInContinueMenu = 0;
        g_GameManager.guiScore = g_GameManager.score;
        g_App.scene = 7;
        return 1;
    }
    if (g_GameManager.isInReplay)
    {
        g_GameManager.isInContinueMenu = 0;
        g_App.scene = 8;
        g_GameManager.guiScore = g_GameManager.score;
        return 1;
    }
    if (g_GameManager.continuesUsed >= 3 || g_GameManager.difficulty >= 4)
    {
        g_GameManager.isInContinueMenu = 0;
        g_App.scene = 7;
        g_GameManager.guiScore = g_GameManager.score;
        return 1;
    }
    switch (this->state)
    {
    case 0:
        if (this->stateFrames == 0)
        {
            for (i = 0; i < 4; i++)
            {
                if (i < 2)
                {
                    g_SpriteEngine->StartScript(&this->sprites[i], i + 8);
                }
                else
                {
                    g_SpriteEngine->StartScript(&this->sprites[i], i + 4);
                }
                this->sprites[i].pendingInterrupt = 1;
            }
            if (g_App.lockableBackBuffer)
            {
                g_SpriteEngine->SetPendingCopy(3, 0x20, 0x10, 0x180, 0x1c0);
                g_SpriteEngine->StartScript(&this->background, 0x718);
                this->background.pos.x = 32.0f;
                this->background.pos.y = 16.0f;
                this->background.pos.z = 0.0f;
            }
        }
        if (this->stateFrames > 8)
        {
            break;
        }
        this->state += 2;
        this->stateFrames = 0;
    case 1:
        this->sprites[2].color = 0xffff8080;
        this->sprites[3].color = 0x80808080;
        this->sprites[2].scaleY = 1.7f;
        this->sprites[2].scaleX = 1.7f;
        this->sprites[3].scaleY = 1.5f;
        this->sprites[3].scaleX = 1.5f;
        this->sprites[2].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->sprites[3].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->stateFrames >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->state = 2;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 4; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                this->state = 3;
                this->background.pendingInterrupt = 1;
                this->stateFrames = 0;
            }
        }
        break;
    case 2:
        this->sprites[3].color = 0xffff8080;
        this->sprites[2].color = 0x80808080;
        this->sprites[2].scaleY = 1.5f;
        this->sprites[2].scaleX = 1.5f;
        this->sprites[3].scaleY = 1.7f;
        this->sprites[3].scaleX = 1.7f;
        this->sprites[3].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->sprites[2].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->stateFrames >= 30)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->state = 1;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 4; i++)
                {
                    this->sprites[i].pendingInterrupt = 2;
                }
                this->state = 4;
                this->stateFrames = 0;
            }
        }
        break;
    case 4:
        if (this->stateFrames >= 20)
        {
            this->state = 0;
            this->stateFrames = 0;
            g_GameManager.isInContinueMenu = 0;
            g_App.scene = 7;
            for (i = 0; i < 4; i++)
            {
                this->sprites[i].ClearUnk0();
            }
            g_GameManager.guiScore = g_GameManager.score;
            return 0;
        }
        break;
    case 3:
        if (this->stateFrames >= 30)
        {
            this->state = 0;
            this->stateFrames = 0;
            g_GameManager.isInContinueMenu = 0;
            for (i = 0; i < 4; i++)
            {
                this->sprites[i].ClearUnk0();
            }
            g_GameManager.continuesUsed++;
            g_GameManager.guiScore = g_GameManager.continuesUsed;
            g_GameManager.scoreIncrement = 0;
            g_GameManager.score = g_GameManager.guiScore;
            g_GameManager.lives = g_App.startCfg.lives;
            g_GameManager.bombs = g_App.startCfg.bombs;
            g_GameManager.graze = 0;
            g_GameManager.pointItems = 0;
            g_GameManager.power = 0;
            g_GameManager.nextExtend = 0;
            g_Gui.flags.livesChanged = 2;
            g_Gui.flags.bombsChanged = 2;
            g_Gui.flags.grazeChanged = 2;
            g_Gui.flags.pointsChanged = 2;
            g_Gui.flags.powerChanged = 2;
            return 0;
        }
    }
    for (i = 0; i < 4; i++)
    {
        g_SpriteEngine->Step(&this->sprites[i]);
    }
    if (g_App.lockableBackBuffer)
    {
        g_SpriteEngine->Step(&this->background);
    }
    this->stateFrames++;
    return 0;
}

void PauseMenu::DrawContinueMenu()
{
    int i;

    if (g_GameManager.isInContinueMenu)
    {
        g_App.viewport.X = g_GameManager.arena[0];
        g_App.viewport.Y = g_GameManager.arena[1];
        g_App.viewport.Width = g_GameManager.arena[2];
        g_App.viewport.Height = g_GameManager.arena[3];
        g_App.d3dDevice->SetViewport(&g_App.viewport);
        if (g_App.lockableBackBuffer && (this->state != 0 || this->stateFrames > 2))
        {
            g_SpriteEngine->Draw(&this->background);
        }
        if (this->state == 1 || this->state == 2)
        {
            this->sprites[4] = this->sprites[1];
            this->sprites[4].pos.x += 8.0f * this->sprites[4].scaleX;
            this->sprites[4].spriteData = &g_SpriteEngine->sprites[0x1e - g_GameManager.continuesUsed];
            g_SpriteEngine->Draw(&this->sprites[4]);
        }
        for (i = 0; i < 4; i++)
        {
            if (this->sprites[i].flags.unk0)
            {
                g_SpriteEngine->Draw(&this->sprites[i]);
            }
        }
    }
}

int AsciiText::OnUpdate(AsciiText *text)
{
    int i;
    AsciiPopup *entry;

    if (!g_GameManager.isPaused && !g_GameManager.isInContinueMenu)
    {
        for (entry = text->popups, i = 0; i < 0x203; i++, entry++)
        {
            if (!entry->active)
            {
                continue;
            }
            entry->pos.y -= 0.5f * g_App.frameAdvance;
            entry->timer.Tick();
            if ((entry->timer.current > 60) != 0)
            {
                entry->active = 0;
            }
        }
    }
    else if (g_GameManager.isPaused)
    {
        text->pauseMenu.UpdatePauseMenu();
    }
    if (g_GameManager.isInContinueMenu)
    {
        text->continueMenu.UpdateContinueMenu();
    }
    return 1;
}

int AsciiText::RegisterChain()
{
    AsciiText *text = &g_AsciiText;

    g_AsciiCalcChain.SetCallback((ChainCallback)AsciiText::OnUpdate);
    g_AsciiCalcChain.addedCallback = (ChainCallback)AsciiText::AddedCallback;
    g_AsciiCalcChain.deletedCallback = (ChainCallback)AsciiText::DeletedCallback;
    g_AsciiCalcChain.arg = text;
    if (g_Chain.AddToCalcChain(&g_AsciiCalcChain, 1) != 0)
    {
        return -1;
    }
    g_AsciiDrawChain.SetCallback((ChainCallback)AsciiText::OnDraw);
    g_AsciiDrawChain.arg = text;
    g_Chain.AddToDrawChain(&g_AsciiDrawChain, 0xf);
    g_AsciiDrawChain2.SetCallback((ChainCallback)AsciiText::OnDraw2);
    g_AsciiDrawChain2.arg = text;
    g_Chain.AddToDrawChain(&g_AsciiDrawChain2, 10);
    return 0;
}

void AsciiText::DrawStrings()
{
    float advance;
    int idx;
    AsciiString *string;
    BYTE *c;
    int clipped = 1;
    D3DXVECTOR3 pos;

    string = this->strings;
    this->glyph.flags.unk0 = 1;
    this->glyph.flags.anchor = 3;
    for (idx = 0; idx < this->stringCount; idx++, string++)
    {
        this->glyph.pos = string->pos;
        c = (BYTE *)string->text;
        this->glyph.scaleX = string->scaleX;
        this->glyph.scaleY = string->scaleY;
        advance = 14.0f * string->scaleX;
        if (clipped != string->inArena)
        {
            clipped = string->inArena;
            if (clipped != 0)
            {
                g_App.viewport.X = g_GameManager.arena[0];
                g_App.viewport.Y = g_GameManager.arena[1];
                g_App.viewport.Width = g_GameManager.arena[2];
                g_App.viewport.Height = g_GameManager.arena[3];
                g_App.d3dDevice->SetViewport(&g_App.viewport);
            }
            else
            {
                g_App.viewport.X = 0;
                g_App.viewport.Y = 0;
                g_App.viewport.Width = 640;
                g_App.viewport.Height = 480;
                g_App.d3dDevice->SetViewport(&g_App.viewport);
            }
        }
        while (*c != 0)
        {
            if (*c == '\n')
            {
                this->glyph.pos.y += 16.0f * string->scaleY;
                this->glyph.pos.x = string->pos.x;
            }
            else if (*c == ' ')
            {
                this->glyph.pos.x += advance;
            }
            else
            {
                if (string->highlight == 0)
                {
                    this->glyph.spriteData = &g_SpriteEngine->sprites[*c - 0x15];
                    this->glyph.color = string->color;
                }
                else
                {
                    this->glyph.spriteData = &g_SpriteEngine->sprites[*c + 0x61];
                    this->glyph.color = 0xffffffff;
                }
                g_SpriteEngine->Draw(&this->glyph);
                this->glyph.pos.x += advance;
            }
            c++;
        }
    }
}

int AsciiText::OnDraw(AsciiText *text)
{
    text->DrawStrings();
    text->stringCount = 0;
    text->pauseMenu.DrawPauseMenu();
    text->continueMenu.DrawContinueMenu();
    return 1;
}

int AsciiText::OnDraw2(AsciiText *text)
{
    if (g_App.hardwareTnL)
    {
        text->DrawPopups();
    }
    else
    {
        text->DrawPopups2();
    }
    return 1;
}

int AsciiText::AddedCallback(AsciiText *text)
{
    if (g_SpriteEngine->LoadAnm(1, "data/ascii.anm", 0) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(2, "data/asciis.anm", 0x77) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(3, "data/capture.anm", 0x718) != 0)
    {
        return -1;
    }
    text->Initialize();
    return 0;
}

void AsciiText::DrawPopups()
{
    AsciiPopup *p = this->popups;
    int count;
    int i;
    BYTE *digitPtr;
    D3DXVECTOR3 unused;

    g_App.viewport.X = g_GameManager.arena[0];
    g_App.viewport.Y = g_GameManager.arena[1];
    g_App.viewport.Width = g_GameManager.arena[2];
    g_App.viewport.Height = g_GameManager.arena[3];
    g_App.d3dDevice->SetViewport(&g_App.viewport);
    for (i = 0; i < 0x203; i++, p++)
    {
        if (!p->active)
        {
            continue;
        }
        this->digit.pos.x = p->pos.x - (p->digitCount << 2);
        this->digit.pos.y = p->pos.y;
        this->digit.color = p->color;
        digitPtr = p->digits + p->digitCount - 1;
        for (count = p->digitCount; count > 0; count--)
        {
            this->digit.spriteData = &g_SpriteEngine->sprites[*digitPtr];
            if (*digitPtr >= 10)
            {
                // The minus sign, stretched.
                this->digit.matrix._11 = 0.1875f;
                this->digit.matrix._22 = 0.03125f;
                g_SpriteEngine->Draw3DRounded(&this->digit);
                this->digit.matrix._11 = 0.03125f;
                this->digit.matrix._22 = 0.03125f;
            }
            else
            {
                g_SpriteEngine->Draw3DRounded(&this->digit);
            }
            this->digit.pos.x += 8.0f;
            digitPtr--;
        }
    }
}

void AsciiText::DrawPopups2()
{
    AsciiPopup *p = this->popups;
    int count;
    int i;
    BYTE *digitPtr;
    D3DXVECTOR3 unused;

    g_App.viewport.X = g_GameManager.arena[0];
    g_App.viewport.Y = g_GameManager.arena[1];
    g_App.viewport.Width = g_GameManager.arena[2];
    g_App.viewport.Height = g_GameManager.arena[3];
    g_App.d3dDevice->SetViewport(&g_App.viewport);
    for (i = 0; i < 0x203; i++, p++)
    {
        if (!p->active)
        {
            continue;
        }
        this->digit.pos.x = p->pos.x - (p->digitCount << 2);
        this->digit.pos.y = p->pos.y;
        this->digit.color = p->color;
        digitPtr = p->digits + p->digitCount - 1;
        for (count = p->digitCount; count > 0; count--)
        {
            this->digit.spriteData = &g_SpriteEngine->sprites[*digitPtr];
            if (*digitPtr >= 10)
            {
                // The minus sign, stretched.
                this->digit.matrix._11 = 0.1875f;
                this->digit.matrix._22 = 0.03125f;
                g_SpriteEngine->Draw(&this->digit);
                this->digit.matrix._11 = 0.03125f;
                this->digit.matrix._22 = 0.03125f;
            }
            else
            {
                g_SpriteEngine->Draw3DRounded(&this->digit);
            }
            this->digit.pos.x += 8.0f;
            digitPtr--;
        }
    }
}

int AsciiText::DeletedCallback(AsciiText *text)
{
    g_SpriteEngine->ReleaseAnm(1);
    g_SpriteEngine->ReleaseAnm(2);
    g_SpriteEngine->ReleaseAnm(3);
    return 0;
}

void AsciiText::RemoveChain()
{
    g_Chain.Remove(&g_AsciiCalcChain);
    g_Chain.Remove(&g_AsciiDrawChain);
}

void AsciiText::AddString(D3DXVECTOR3 *pos, char *text)
{
    AsciiString *str;

    if (this->stringCount >= 0x100)
    {
        return;
    }
    str = &this->strings[this->stringCount];
    this->stringCount++;
    strcpy(str->text, text);
    str->pos = *pos;
    str->color = this->color;
    str->scaleX = this->scaleX;
    str->scaleY = this->scaleY;
    str->inArena = this->inArena;
    if (g_App.HasFlag0Or8())
    {
        str->highlight = this->highlight;
    }
    else
    {
        str->highlight = 0;
    }
}

void AsciiText::AddFormat(D3DXVECTOR3 *pos, char *fmt, ...)
{
    char buf[0x200];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    this->AddString(pos, buf);
    va_end(args);
}

void AsciiText::AddPopup(D3DXVECTOR3 *pos, int value, D3DCOLOR color)
{
    AsciiPopup *popup;
    int n;

    if (this->nextPopup >= 0x200)
    {
        this->nextPopup = 0;
    }
    popup = &this->popups[this->nextPopup];
    popup->active = 1;
    n = 0;
    if (value >= 0)
    {
        while (value != 0)
        {
            popup->digits[n] = value % 10;
            n++;
            value /= 10;
        }
    }
    else
    {
        popup->digits[n] = 10;
        n++;
    }
    if (n == 0)
    {
        popup->digits[n] = 0;
        n++;
    }
    popup->digitCount = n;
    popup->color = color;
    popup->timer.Set(0);
    popup->pos = *pos;
    this->nextPopup++;
}

void AsciiText::AddPopup2(D3DXVECTOR3 *pos, int value, D3DCOLOR color)
{
    AsciiPopup *popup;
    int n;

    if (this->nextPopup2 >= 3)
    {
        this->nextPopup2 = 0;
    }
    popup = &this->popups[this->nextPopup2 + 0x200];
    popup->active = 1;
    n = 0;
    if (value >= 0)
    {
        while (value != 0)
        {
            popup->digits[n] = value % 10;
            n++;
            value /= 10;
        }
    }
    else
    {
        popup->digits[n] = 10;
        n++;
    }
    if (n == 0)
    {
        popup->digits[n] = 0;
        n++;
    }
    popup->digitCount = n;
    popup->color = color;
    popup->timer.Set(0);
    popup->pos = *pos;
    this->nextPopup2++;
}

// Resets the sprite: no rotation, a scale of 1, no interpolations, white,
// the identity matrix, flags unk0 and visible.
void Sprite::Initialize()
{
    this->rotationX = this->rotationY = this->rotationZ = this->rotationSpeedX = this->rotationSpeedY =
        this->rotationSpeedZ = this->scaleTargetY = this->scaleTargetX = this->uvScrollX = this->uvScrollY = 0.0f;
    this->scaleY = this->scaleX = 1.0f;
    this->alphaInterpTime = this->scaleInterpTime = 0;
    this->color = 0xffffffff;
    D3DXMatrixIdentity(&this->matrix);
    // A 16-bit store: the flags' low half set to unk0 | visible, the rest
    // kept.
    *(WORD *)&this->flags = 3;
    this->autoRotate = 0;
    this->pendingInterrupt = 0;
    this->posInterpTime = 0;
    this->scriptTime.Initialize();
}
