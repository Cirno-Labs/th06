// The file starting at 0x401000: text in the ASCII font.

#include <stdarg.h>
#include <stdio.h>

#include "ascii.h"
#include "unsorted.h"

AsciiText::AsciiText()
{
}

int Unknown4010e0::UpdatePauseMenu()
{
    int i;

    if (WAS_PRESSED(8))
    {
        this->unk_000 = 3;
        for (i = 0; i < 6; i++)
        {
            if (this->unk_008[i].flags.unk0)
            {
                this->unk_008[i].pendingInterrupt = 2;
            }
        }
        this->unk_004 = 0;
        this->unk_668.pendingInterrupt = 1;
    }
    if (WAS_PRESSED(0x200))
    {
        this->unk_000 = 6;
        for (i = 0; i < 6; i++)
        {
            if (this->unk_008[i].flags.unk0)
            {
                this->unk_008[i].pendingInterrupt = 2;
            }
        }
        this->unk_004 = 0;
    }
    switch (this->unk_000)
    {
    case 0:
        for (i = 0; i < 6; i++)
        {
            g_SpriteEngine->StartScript(&this->unk_008[i], i + 2);
        }
        for (i = 0; i < 3; i++)
        {
            this->unk_008[i].pendingInterrupt = 1;
        }
        this->unk_000++;
        this->unk_004 = 0;
        if (g_App.lockableBackBuffer)
        {
            g_SpriteEngine->SetPendingCopy(3, 0x20, 0x10, 0x180, 0x1c0);
            g_SpriteEngine->StartScript(&this->unk_668, 0x718);
            this->unk_668.pos.x = 32.0f;
            this->unk_668.pos.y = 16.0f;
            this->unk_668.pos.z = 0.0f;
        }
    case 1:
        this->unk_008[1].color = 0xffff8080;
        this->unk_008[2].color = 0x80808080;
        this->unk_008[1].scaleY = 1.7f;
        this->unk_008[1].scaleX = 1.7f;
        this->unk_008[2].scaleY = 1.5f;
        this->unk_008[2].scaleX = 1.5f;
        this->unk_008[1].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->unk_008[2].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->unk_004 >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->unk_000 = 2;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 3; i++)
                {
                    this->unk_008[i].pendingInterrupt = 2;
                }
                this->unk_000 = 3;
                this->unk_004 = 0;
                this->unk_668.pendingInterrupt = 1;
            }
        }
        break;
    case 2:
        this->unk_008[1].color = 0x80808080;
        this->unk_008[2].color = 0xffff8080;
        this->unk_008[1].scaleY = 1.5f;
        this->unk_008[1].scaleX = 1.5f;
        this->unk_008[2].scaleY = 1.7f;
        this->unk_008[2].scaleX = 1.7f;
        this->unk_008[1].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        this->unk_008[2].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        if (this->unk_004 >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->unk_000 = 1;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 3; i++)
                {
                    this->unk_008[i].pendingInterrupt = 2;
                }
                for (i = 3; i < 6; i++)
                {
                    this->unk_008[i].pendingInterrupt = 1;
                }
                this->unk_000 = 5;
                this->unk_004 = 0;
            }
        }
        break;
    case 3:
        if (this->unk_004 >= 20)
        {
            this->unk_000 = 0;
            g_GameManager.unk_181f = 0;
            for (i = 0; i < 6; i++)
            {
                this->unk_008[i].ClearUnk0();
            }
        }
        break;
    case 4:
        this->unk_008[4].color = 0xffff8080;
        this->unk_008[5].color = 0x80808080;
        this->unk_008[4].scaleY = 1.7f;
        this->unk_008[4].scaleX = 1.7f;
        this->unk_008[5].scaleY = 1.5f;
        this->unk_008[5].scaleX = 1.5f;
        this->unk_008[4].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->unk_008[5].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->unk_004 >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->unk_000 = 5;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 3; i < 6; i++)
                {
                    this->unk_008[i].pendingInterrupt = 2;
                }
                this->unk_000 = 6;
                this->unk_004 = 0;
            }
        }
        break;
    case 5:
        this->unk_008[4].color = 0x80808080;
        this->unk_008[5].color = 0xffff8080;
        this->unk_008[4].scaleY = 1.5f;
        this->unk_008[4].scaleX = 1.5f;
        this->unk_008[5].scaleY = 1.7f;
        this->unk_008[5].scaleX = 1.7f;
        this->unk_008[4].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        this->unk_008[5].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        if (this->unk_004 >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->unk_000 = 4;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 3; i++)
                {
                    this->unk_008[i].pendingInterrupt = 1;
                }
                for (i = 3; i < 6; i++)
                {
                    this->unk_008[i].pendingInterrupt = 2;
                }
                this->unk_000 = 2;
                this->unk_004 = 0;
            }
        }
        break;
    case 6:
        if (this->unk_004 >= 20)
        {
            this->unk_000 = 0;
            g_GameManager.unk_181f = 0;
            g_App.scene = 1;
            for (i = 0; i < 6; i++)
            {
                this->unk_008[i].ClearUnk0();
            }
        }
        break;
    }
    for (i = 0; i < 6; i++)
    {
        g_SpriteEngine->Step(&this->unk_008[i]);
    }
    if (g_App.lockableBackBuffer)
    {
        g_SpriteEngine->Step(&this->unk_668);
    }
    this->unk_004++;
    return 0;
}

void Unknown4010e0::Func402740()
{
    int i;

    if (g_GameManager.unk_181f)
    {
        g_App.viewport.X = g_GameManager.arena[0];
        g_App.viewport.Y = g_GameManager.arena[1];
        g_App.viewport.Width = g_GameManager.arena[2];
        g_App.viewport.Height = g_GameManager.arena[3];
        g_App.d3dDevice->SetViewport(&g_App.viewport);
        if (g_App.lockableBackBuffer && this->unk_000 != 0)
        {
            Sprite sprite = this->unk_668;

            sprite.flags.zWriteDisabled = 1;
            g_SpriteEngine->Draw(&sprite);
        }
        for (i = 0; i < 6; i++)
        {
            if (this->unk_008[i].flags.unk0)
            {
                g_SpriteEngine->Draw(&this->unk_008[i]);
            }
        }
    }
}

int Unknown4010e0::UpdateContinueMenu()
{
    int i;

    if (g_GameManager.unk_1823)
    {
        g_GameManager.unk_1820 = 0;
        g_GameManager.unk_0000 = g_GameManager.score;
        g_App.scene = 7;
        return 1;
    }
    if (g_GameManager.unk_001c)
    {
        g_GameManager.unk_1820 = 0;
        g_App.scene = 8;
        g_GameManager.unk_0000 = g_GameManager.score;
        return 1;
    }
    if (g_GameManager.continuesUsed >= 3 || g_GameManager.difficulty >= 4)
    {
        g_GameManager.unk_1820 = 0;
        g_App.scene = 7;
        g_GameManager.unk_0000 = g_GameManager.score;
        return 1;
    }
    switch (this->unk_000)
    {
    case 0:
        if (this->unk_004 == 0)
        {
            for (i = 0; i < 4; i++)
            {
                if (i < 2)
                {
                    g_SpriteEngine->StartScript(&this->unk_008[i], i + 8);
                }
                else
                {
                    g_SpriteEngine->StartScript(&this->unk_008[i], i + 4);
                }
                this->unk_008[i].pendingInterrupt = 1;
            }
            if (g_App.lockableBackBuffer)
            {
                g_SpriteEngine->SetPendingCopy(3, 0x20, 0x10, 0x180, 0x1c0);
                g_SpriteEngine->StartScript(&this->unk_668, 0x718);
                this->unk_668.pos.x = 32.0f;
                this->unk_668.pos.y = 16.0f;
                this->unk_668.pos.z = 0.0f;
            }
        }
        if (this->unk_004 > 8)
        {
            break;
        }
        this->unk_000 += 2;
        this->unk_004 = 0;
    case 1:
        this->unk_008[2].color = 0xffff8080;
        this->unk_008[3].color = 0x80808080;
        this->unk_008[2].scaleY = 1.7f;
        this->unk_008[2].scaleX = 1.7f;
        this->unk_008[3].scaleY = 1.5f;
        this->unk_008[3].scaleX = 1.5f;
        this->unk_008[2].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->unk_008[3].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->unk_004 >= 4)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->unk_000 = 2;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 4; i++)
                {
                    this->unk_008[i].pendingInterrupt = 2;
                }
                this->unk_000 = 3;
                this->unk_668.pendingInterrupt = 1;
                this->unk_004 = 0;
            }
        }
        break;
    case 2:
        this->unk_008[3].color = 0xffff8080;
        this->unk_008[2].color = 0x80808080;
        this->unk_008[2].scaleY = 1.5f;
        this->unk_008[2].scaleX = 1.5f;
        this->unk_008[3].scaleY = 1.7f;
        this->unk_008[3].scaleX = 1.7f;
        this->unk_008[3].posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
        this->unk_008[2].posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        if (this->unk_004 >= 30)
        {
            if (WAS_PRESSED(0x10) || WAS_PRESSED(0x20))
            {
                this->unk_000 = 1;
            }
            if (WAS_PRESSED(1))
            {
                for (i = 0; i < 4; i++)
                {
                    this->unk_008[i].pendingInterrupt = 2;
                }
                this->unk_000 = 4;
                this->unk_004 = 0;
            }
        }
        break;
    case 4:
        if (this->unk_004 >= 20)
        {
            this->unk_000 = 0;
            this->unk_004 = 0;
            g_GameManager.unk_1820 = 0;
            g_App.scene = 7;
            for (i = 0; i < 4; i++)
            {
                this->unk_008[i].ClearUnk0();
            }
            g_GameManager.unk_0000 = g_GameManager.score;
            return 0;
        }
        break;
    case 3:
        if (this->unk_004 >= 30)
        {
            this->unk_000 = 0;
            this->unk_004 = 0;
            g_GameManager.unk_1820 = 0;
            for (i = 0; i < 4; i++)
            {
                this->unk_008[i].ClearUnk0();
            }
            g_GameManager.continuesUsed++;
            g_GameManager.unk_0000 = g_GameManager.continuesUsed;
            g_GameManager.unk_0008 = 0;
            g_GameManager.score = g_GameManager.unk_0000;
            g_GameManager.lives = g_App.startCfg.lives;
            g_GameManager.bombs = g_App.startCfg.bombs;
            g_GameManager.graze = 0;
            g_GameManager.pointItems = 0;
            g_GameManager.power = 0;
            g_GameManager.unk_181c = 0;
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
        g_SpriteEngine->Step(&this->unk_008[i]);
    }
    if (g_App.lockableBackBuffer)
    {
        g_SpriteEngine->Step(&this->unk_668);
    }
    this->unk_004++;
    return 0;
}

void Unknown4010e0::Func403060()
{
    int i;

    if (g_GameManager.unk_1820)
    {
        g_App.viewport.X = g_GameManager.arena[0];
        g_App.viewport.Y = g_GameManager.arena[1];
        g_App.viewport.Width = g_GameManager.arena[2];
        g_App.viewport.Height = g_GameManager.arena[3];
        g_App.d3dDevice->SetViewport(&g_App.viewport);
        if (g_App.lockableBackBuffer && (this->unk_000 != 0 || this->unk_004 > 2))
        {
            g_SpriteEngine->Draw(&this->unk_668);
        }
        if (this->unk_000 == 1 || this->unk_000 == 2)
        {
            this->unk_008[4] = this->unk_008[1];
            this->unk_008[4].pos.x += 8.0f * this->unk_008[4].scaleX;
            this->unk_008[4].spriteData = &g_SpriteEngine->sprites[0x1e - g_GameManager.continuesUsed];
            g_SpriteEngine->Draw(&this->unk_008[4]);
        }
        for (i = 0; i < 4; i++)
        {
            if (this->unk_008[i].flags.unk0)
            {
                g_SpriteEngine->Draw(&this->unk_008[i]);
            }
        }
    }
}

int AsciiText::OnUpdate(AsciiText *text)
{
    int i;
    AsciiPopup *entry;

    if (!g_GameManager.unk_181f && !g_GameManager.unk_1820)
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
    else if (g_GameManager.unk_181f)
    {
        text->unk_6244.UpdatePauseMenu();
    }
    if (g_GameManager.unk_1820)
    {
        text->unk_69bc.UpdateContinueMenu();
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
    this->unk_0000.flags.unk0 = 1;
    this->unk_0000.flags.anchor = 3;
    for (idx = 0; idx < this->stringCount; idx++, string++)
    {
        this->unk_0000.pos = string->pos;
        c = (BYTE *)string->text;
        this->unk_0000.scaleX = string->scaleX;
        this->unk_0000.scaleY = string->scaleY;
        advance = 14.0f * string->scaleX;
        if (clipped != string->unk_5c)
        {
            clipped = string->unk_5c;
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
                this->unk_0000.pos.y += 16.0f * string->scaleY;
                this->unk_0000.pos.x = string->pos.x;
            }
            else if (*c == ' ')
            {
                this->unk_0000.pos.x += advance;
            }
            else
            {
                if (string->unk_58 == 0)
                {
                    this->unk_0000.spriteData = &g_SpriteEngine->sprites[*c - 0x15];
                    this->unk_0000.color = string->color;
                }
                else
                {
                    this->unk_0000.spriteData = &g_SpriteEngine->sprites[*c + 0x61];
                    this->unk_0000.color = 0xffffffff;
                }
                g_SpriteEngine->Draw(&this->unk_0000);
                this->unk_0000.pos.x += advance;
            }
            c++;
        }
    }
}

int AsciiText::OnDraw(AsciiText *text)
{
    text->DrawStrings();
    text->stringCount = 0;
    text->unk_6244.Func402740();
    text->unk_69bc.Func403060();
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
        this->unk_0110.pos.x = p->pos.x - (p->digitCount << 2);
        this->unk_0110.pos.y = p->pos.y;
        this->unk_0110.color = p->color;
        digitPtr = p->digits + p->digitCount - 1;
        for (count = p->digitCount; count > 0; count--)
        {
            this->unk_0110.spriteData = &g_SpriteEngine->sprites[*digitPtr];
            if (*digitPtr >= 10)
            {
                // The minus sign, stretched.
                this->unk_0110.matrix._11 = 0.1875f;
                this->unk_0110.matrix._22 = 0.03125f;
                g_SpriteEngine->Draw3DRounded(&this->unk_0110);
                this->unk_0110.matrix._11 = 0.03125f;
                this->unk_0110.matrix._22 = 0.03125f;
            }
            else
            {
                g_SpriteEngine->Draw3DRounded(&this->unk_0110);
            }
            this->unk_0110.pos.x += 8.0f;
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
        this->unk_0110.pos.x = p->pos.x - (p->digitCount << 2);
        this->unk_0110.pos.y = p->pos.y;
        this->unk_0110.color = p->color;
        digitPtr = p->digits + p->digitCount - 1;
        for (count = p->digitCount; count > 0; count--)
        {
            this->unk_0110.spriteData = &g_SpriteEngine->sprites[*digitPtr];
            if (*digitPtr >= 10)
            {
                // The minus sign, stretched.
                this->unk_0110.matrix._11 = 0.1875f;
                this->unk_0110.matrix._22 = 0.03125f;
                g_SpriteEngine->Draw(&this->unk_0110);
                this->unk_0110.matrix._11 = 0.03125f;
                this->unk_0110.matrix._22 = 0.03125f;
            }
            else
            {
                g_SpriteEngine->Draw3DRounded(&this->unk_0110);
            }
            this->unk_0110.pos.x += 8.0f;
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
    str->unk_5c = this->unk_6230;
    if (g_App.HasFlag0Or8())
    {
        str->unk_58 = this->unk_6234;
    }
    else
    {
        str->unk_58 = 0;
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
