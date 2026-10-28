// cflags: /Os
// The /Os file with Gui, from 0x4172d3.

#include <stdio.h>
#include <string.h>

#include "ascii.h"
#include "gamemanager.h"
#include "stage.h"
#include "unsorted.h"

BOOL Gui::Func4172d3()
{
    return this->impl->unk_2424.spriteIndex >= 0 && this->impl->unk_2424.flags.stopped;
}

void Gui::Func417314()
{
    this->impl->unk_1fe4.pendingInterrupt = 1;
}

void Gui::Func41732c()
{
    this->impl->unk_20f4.pendingInterrupt = 1;
}

BOOL Gui::Func417344()
{
    return this->impl->msg.skippable;
}

void Gui::ShowBonusPopup(int bonus)
{
    this->impl->bonusPopup.pos = D3DXVECTOR3(416.0f, 32.0f, 0.0f);
    this->impl->bonusPopup.shown = 1;
    this->impl->bonusPopup.timer.Set(0);
    this->impl->bonusPopup.value = bonus;
}

void Gui::ShowFullPowerPopup(int unk)
{
    this->impl->fullPowerPopup.pos = D3DXVECTOR3(416.0f, 232.0f, 0.0f);
    this->impl->fullPowerPopup.shown = 1;
    this->impl->fullPowerPopup.timer.Set(0);
    this->impl->fullPowerPopup.value = unk;
}

void Gui::Func417458(int value)
{
    this->impl->unk_2c24.pos = D3DXVECTOR3(224.0f, 16.0f, 0.0f);
    this->impl->unk_2c24.shown = 1;
    this->impl->unk_2c24.timer.Set(0);
    this->impl->unk_2c24.value = value;
}

int Gui::OnUpdate(Gui *gui)
{
    if (g_GameManager.isFrozen)
    {
        return 1;
    }
    gui->UpdateInterface();
    gui->impl->RunMsg();
    return 1;
}

int Gui::OnDraw(Gui *gui)
{
    D3DXVECTOR3 cursor;
    char buf[32];

    g_App.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    if (gui->impl->stageClearShown != 0)
    {
        cursor.x = 74.0f;
        cursor.y = 128.0f;
        cursor.z = 0.0f;
        g_AsciiText.color = 0xffffff40;
        if (g_GameManager.stage < 6)
        {
            g_AsciiText.AddFormat(&cursor, "Stage Clear\n\n");
        }
        else
        {
            g_AsciiText.AddFormat(&cursor, "All Clear!\n\n");
        }
        cursor.y += 32.0f;
        g_AsciiText.color = 0xffffffff;
        g_AsciiText.AddFormat(&cursor, "Stage * 1000 = %5d\n", g_GameManager.stage * 1000);
        cursor.y += 16.0f;
        g_AsciiText.color = 0xffe0e0ff;
        g_AsciiText.AddFormat(&cursor, "Power *  100 = %5d\n", g_GameManager.power * 100);
        cursor.y += 16.0f;
        g_AsciiText.color = 0xffd0d0ff;
        g_AsciiText.AddFormat(&cursor, "Graze *   10 = %5d\n", g_GameManager.graze * 10);
        cursor.y += 16.0f;
        g_AsciiText.color = 0xffff8080;
        g_AsciiText.AddFormat(&cursor, "    * Point Item %3d\n", g_GameManager.pointItems);
        if (g_GameManager.stage >= 6)
        {
            cursor.y += 16.0f;
            g_AsciiText.color = 0xffffff80;
            g_AsciiText.AddFormat(&cursor, "Player    = %8d\n", g_GameManager.lives * 3000000);
            cursor.y += 16.0f;
            g_AsciiText.AddFormat(&cursor, "Bomb      = %8d\n", g_GameManager.bombs * 1000000);
        }
        cursor.y += 32.0f;
        switch (g_GameManager.difficulty)
        {
        case 0:
            g_AsciiText.color = 0xffff8080;
            g_AsciiText.AddFormat(&cursor, "Easy Rank      * 0.5\n");
            break;
        case 1:
            g_AsciiText.color = 0xffff8080;
            g_AsciiText.AddFormat(&cursor, "Normal Rank    * 1.0\n");
            break;
        case 2:
            g_AsciiText.color = 0xffff8080;
            g_AsciiText.AddFormat(&cursor, "Hard Rank      * 1.2\n");
            break;
        case 3:
            g_AsciiText.color = 0xffff8080;
            g_AsciiText.AddFormat(&cursor, "Lunatic Rank   * 1.5\n");
            break;
        case 4:
            g_AsciiText.color = 0xffff8080;
            g_AsciiText.AddFormat(&cursor, "Extra Rank     * 2.0\n");
            break;
        }
        cursor.y += 16.0f;
        if (g_GameManager.difficulty < 4 && !g_GameManager.isInPractice)
        {
            switch (g_App.startCfg.lives)
            {
            case 3:
                g_AsciiText.color = 0xffff8080;
                g_AsciiText.AddFormat(&cursor, "Player Penalty * 0.5\n");
                cursor.y += 16.0f;
                break;
            case 4:
                g_AsciiText.color = 0xffff8080;
                g_AsciiText.AddFormat(&cursor, "Player Penalty * 0.2\n");
                cursor.y += 16.0f;
                break;
            }
        }
        g_AsciiText.color = 0xffffffff;
        g_AsciiText.AddFormat(&cursor, "Total     = %8d", gui->impl->stageClearTotal);
        g_AsciiText.color = 0xffffffff;
    }
    gui->impl->DrawMsg();
    gui->DrawBanners();
    gui->DrawHud();
    g_AsciiText.inArena = 1;
    if (gui->impl->bonusPopup.shown)
    {
        g_AsciiText.color = 0xffffff80;
        g_AsciiText.AddFormat(&gui->impl->bonusPopup.pos, "BONUS %8d", gui->impl->bonusPopup.value);
        g_AsciiText.color = 0xffffffff;
    }
    if (gui->impl->fullPowerPopup.shown)
    {
        g_AsciiText.color = 0xffc0b0ff;
        g_AsciiText.AddFormat(&gui->impl->fullPowerPopup.pos, "Full Power Mode!!", gui->impl->fullPowerPopup.value);
        g_AsciiText.color = 0xffffffff;
    }
    if (gui->impl->unk_2c24.shown)
    {
        g_AsciiText.color = 0xffff0000;
        gui->impl->unk_2c24.pos.x = (384.0f - strlen("Spell Card Bonus!") * 16.0f) / 2.0f + 32.0f;
        gui->impl->unk_2c24.pos.y = 80.0f;
        g_AsciiText.AddFormat(&gui->impl->unk_2c24.pos, "Spell Card Bonus!");
        gui->impl->unk_2c24.pos.y += 16.0f;
        sprintf(buf, "+%d", gui->impl->unk_2c24.value);
        gui->impl->unk_2c24.pos.x = (384.0f - strlen(buf) * 32.0f) / 2.0f + 32.0f;
        g_AsciiText.scaleX = 2.0f;
        g_AsciiText.scaleY = 2.0f;
        g_AsciiText.color = 0xffff8080;
        g_AsciiText.AddString(&gui->impl->unk_2c24.pos, buf);
        g_AsciiText.scaleX = 1.0f;
        g_AsciiText.scaleY = 1.0f;
        g_AsciiText.color = 0xffffffff;
    }
    g_AsciiText.inArena = 0;
    g_App.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    return 1;
}

void Gui::ShowBombName(int sprite, char *name)
{
    g_SpriteEngine->StartScript(&this->impl->unk_1dc4, 0x4a1);
    g_SpriteEngine->SetSprite(&this->impl->unk_1dc4, sprite);
    g_SpriteEngine->StartScript(&this->impl->unk_1fe4, 0x706);
    g_SpriteEngine->RenderText(&this->impl->unk_1fe4, 0xf0f0ff, 0, name);
    this->unk_08 = strlen(name) * 15 / 2.0f + 16.0f;
    g_App.redrawFrames = 3;
    g_SoundPlayer.QueueSound(14, 0);
}

void Gui::Func417bfd(int sprite, char *text)
{
    g_SpriteEngine->StartScript(&this->impl->unk_1ed4, 0x4a3);
    g_SpriteEngine->SetSprite(&this->impl->unk_1ed4, sprite + 0x4a8);
    g_SpriteEngine->StartScript(&this->impl->unk_20f4, 0x707);
    g_SpriteEngine->RenderTextRightAligned(&this->impl->unk_20f4, 0xfff0f0, 0, text);
    this->unk_0c = strlen(text) * 15 / 2.0f + 16.0f;
    g_SoundPlayer.QueueSound(14, 0);
}

int Gui::LoadResources()
{
    int i;

    if ((g_App.scene != 3) != 0)
    {
        memset(this->impl, 0, sizeof(GuiImpl));
        if (g_SpriteEngine->LoadAnm(0xd, "data/front.anm", 0x600) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0xe, "data/loading.anm", 0x619) != 0)
        {
            return -1;
        }
        this->impl->unk_2424.spriteIndex = -1;
        switch (g_GameManager.character)
        {
        case 0:
            if (g_SpriteEngine->LoadAnm(0xf, "data/face00a.anm", 0x4a0) != 0)
            {
                return -1;
            }
            if (g_SpriteEngine->LoadAnm(0x10, "data/face00b.anm", 0x4a2) != 0)
            {
                return -1;
            }
            if (g_SpriteEngine->LoadAnm(0x11, "data/face00c.anm", 0x4a4) != 0)
            {
                return -1;
            }
            break;
        case 1:
            if (g_SpriteEngine->LoadAnm(0xf, "data/face01a.anm", 0x4a0) != 0)
            {
                return -1;
            }
            if (g_SpriteEngine->LoadAnm(0x10, "data/face01b.anm", 0x4a2) != 0)
            {
                return -1;
            }
            if (g_SpriteEngine->LoadAnm(0x11, "data/face01c.anm", 0x4a4) != 0)
            {
                return -1;
            }
            break;
        }
    }
    else
    {
        g_SpriteEngine->StartScript(&this->impl->unk_2424, 0x619);
        this->impl->unk_2424.pendingInterrupt = 1;
    }
    switch (g_GameManager.stage)
    {
    case 1:
        if (g_SpriteEngine->LoadAnm(0x12, "data/face03a.anm", 0x4a8) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x13, "data/face03b.anm", 0x4aa) != 0)
        {
            return -1;
        }
        if (this->LoadMsg("data/msg1.dat") != 0)
        {
            return -1;
        }
        break;
    case 2:
        if (g_SpriteEngine->LoadAnm(0x12, "data/face05a.anm", 0x4a8) != 0)
        {
            return -1;
        }
        if (this->LoadMsg("data/msg2.dat") != 0)
        {
            return -1;
        }
        break;
    case 3:
        if (g_SpriteEngine->LoadAnm(0x12, "data/face06a.anm", 0x4a8) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x13, "data/face06b.anm", 0x4aa) != 0)
        {
            return -1;
        }
        if (this->LoadMsg("data/msg3.dat") != 0)
        {
            return -1;
        }
        break;
    case 4:
        if (g_SpriteEngine->LoadAnm(0x12, "data/face08a.anm", 0x4a8) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x13, "data/face08b.anm", 0x4aa) != 0)
        {
            return -1;
        }
        if (this->LoadMsg("data/msg4.dat") != 0)
        {
            return -1;
        }
        break;
    case 5:
        if (g_SpriteEngine->LoadAnm(0x12, "data/face09a.anm", 0x4a8) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x13, "data/face09b.anm", 0x4aa) != 0)
        {
            return -1;
        }
        if (this->LoadMsg("data/msg5.dat") != 0)
        {
            return -1;
        }
        break;
    case 6:
        if (g_SpriteEngine->LoadAnm(0x12, "data/face09b.anm", 0x4a8) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x13, "data/face10a.anm", 0x4aa) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x14, "data/face10b.anm", 0x4ab) != 0)
        {
            return -1;
        }
        if (this->LoadMsg("data/msg6.dat") != 0)
        {
            return -1;
        }
        break;
    default:
        if (g_SpriteEngine->LoadAnm(0x12, "data/face08a.anm", 0x4a8) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x13, "data/face12a.anm", 0x4aa) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x14, "data/face12b.anm", 0x4ab) != 0)
        {
            return -1;
        }
        if (this->LoadMsg("data/msg7.dat") != 0)
        {
            return -1;
        }
        break;
    }
    if ((g_App.scene != 3) != 0)
    {
        for (i = 0; i < 0x1a; i++)
        {
            g_SpriteEngine->StartScript(&this->impl->unk_0000[i], i + 0x600);
        }
    }
    this->bossPresent = 0;
    this->impl->bossBarState = 0;
    this->bossHealth = 0.0f;
    this->bossHealthShown = 0.0f;
    g_SpriteEngine->StartScript(&this->impl->unk_1dc4, 0x4a1);
    g_SpriteEngine->StartScript(&this->impl->unk_1ed4, 0x4a3);
    g_SpriteEngine->StartScript(&this->impl->unk_1fe4, 0x706);
    g_SpriteEngine->StartScript(&this->impl->unk_20f4, 0x707);
    g_SpriteEngine->StartScript(&this->impl->unk_2204, 0x617);
    g_SpriteEngine->StartScript(&this->impl->unk_2314, 0x618);
    this->impl->unk_1dc4.scriptPos = NULL;
    this->impl->unk_1fe4.scriptPos = NULL;
    this->impl->unk_1ed4.scriptPos = NULL;
    this->impl->unk_20f4.scriptPos = NULL;
    this->impl->unk_1dc4.flags.unk0 = 0;
    this->impl->unk_1fe4.flags.unk0 = 0;
    this->impl->unk_1ed4.flags.unk0 = 0;
    this->impl->unk_20f4.flags.unk0 = 0;
    this->impl->unk_1fe4.unk_10c = 15;
    this->impl->unk_1fe4.unk_10d = 15;
    this->impl->unk_20f4.unk_10c = 15;
    this->impl->unk_20f4.unk_10d = 15;
    g_SpriteEngine->StartScript(&this->impl->unk_1ba4, 0x700);
    g_SpriteEngine->StartScript(&this->impl->unk_1cb4, 0x701);
    g_SpriteEngine->RenderTextCentered(&this->impl->unk_1ba4, 0xe0ffff, 0, g_Stage.data->name);
    this->impl->unk_1cb4.unk_10c = 16;
    this->impl->unk_1cb4.unk_10d = 16;
    g_SpriteEngine->RenderTextRightAligned(&this->impl->unk_1cb4, 0xe0ffff, 0, "♪%s", g_Stage.data->songNames[0]);
    this->impl->msg.scriptIndex = -1;
    this->impl->stageClearShown = 0;
    this->impl->bonusPopup.shown = 0;
    this->impl->fullPowerPopup.shown = 0;
    this->impl->unk_2c24.shown = 0;
    this->flags.livesChanged = 2;
    this->flags.bombsChanged = 2;
    this->flags.grazeChanged = 2;
    this->flags.pointsChanged = 2;
    this->flags.powerChanged = 2;
    return 0;
}

int Gui::LoadMsg(char *path)
{
    int i;

    this->FreeMsgFile();
    this->impl->msg.msgFile = (MsgFile *)LoadFileData(path, 0);
    if (this->impl->msg.msgFile == NULL)
    {
        LogPrintf(&g_Log, "error : メッセージファイル %s が読み込めませんでした\n", path);
        return -1;
    }
    this->impl->msg.scriptIndex = -1;
    this->impl->msg.instr = NULL;
    for (i = 0; i < this->impl->msg.msgFile->count; i++)
    {
        this->impl->msg.msgFile->scripts[i] = (char *)this->impl->msg.msgFile->scripts[i] + (int)this->impl->msg.msgFile;
    }
    return 0;
}

void Gui::FreeMsgFile()
{
    if (this->impl->msg.msgFile != NULL)
    {
        MemFree(this->impl->msg.msgFile);
        this->impl->msg.msgFile = NULL;
    }
}

void Gui::StartMsg(int index)
{
    this->impl->StartMsgScript(index);
    g_App.redrawFrames = 3;
}

void GuiImpl::StartMsgScript(int index)
{
    MsgFile *msgFile;

    if (this->msg.msgFile->count <= index)
    {
        return;
    }
    msgFile = this->msg.msgFile;
    memset(&this->msg, 0, sizeof(GuiMsg));
    this->msg.scriptIndex = index;
    this->msg.msgFile = msgFile;
    this->msg.instr = (MsgInstr *)this->msg.msgFile->scripts[index];
    this->msg.text[0].scriptIndex = -1;
    this->msg.text[1].scriptIndex = -1;
    this->msg.fontSize = 15;
    this->msg.textColors[0] = 0xe8f0ff;
    this->msg.textColors[1] = 0xffe8f0;
    this->msg.shadowColors[0] = 0;
    this->msg.shadowColors[1] = 0;
    this->msg.skippable = 1;
    if (g_GameManager.stage == 6 && (index == 0 || index == 10))
    {
        g_SpriteEngine->LoadAnm(0xb, "data/eff06.anm", 0x2b3);
    }
    else if (g_GameManager.stage == 7 && (index == 0 || index == 10))
    {
        g_SpriteEngine->LoadAnm(0xb, "data/eff07.anm", 0x2b3);
        g_SpriteEngine->LoadAnm(0x12, "data/face12c.anm", 0x4a8);
    }
}

int GuiImpl::RunMsg()
{
    MsgArgs *args;

    if (this->msg.scriptIndex < 0)
    {
        return -1;
    }
    if (this->msg.unk_6a0 > 0)
    {
        this->msg.unk_6a0--;
    }
    if (this->msg.skippable && (g_Buttons & 0x100))
    {
        this->msg.timer.Set(this->msg.instr->time);
    }
    while (this->msg.timer >= this->msg.instr->time)
    {
        switch (this->msg.instr->opcode)
        {
        case 0:
            this->msg.scriptIndex = -1;
            return -1;
        case 1:
            args = &this->msg.instr->args;
            g_SpriteEngine->StartScript(&this->msg.portraits[args->who], args->index + (args->who != 0 ? 0x4a2 : 0x4a0));
            break;
        case 2:
            args = &this->msg.instr->args;
            g_SpriteEngine->SetSprite(&this->msg.portraits[args->who], args->index + (args->who != 0 ? 0x4a8 : 0x4a0));
            break;
        case 3:
            args = &this->msg.instr->args;
            if (args->index == 0 && this->msg.text[1].scriptIndex >= 0)
            {
                g_SpriteEngine->RenderText(&this->msg.text[1], this->msg.textColors[args->who], this->msg.shadowColors[args->who], " ");
            }
            g_SpriteEngine->StartScript(&this->msg.text[args->index], args->index + 0x702);
            this->msg.text[args->index].unk_10d = this->msg.fontSize;
            this->msg.text[args->index].unk_10c = this->msg.text[args->index].unk_10d;
            g_SpriteEngine->RenderText(&this->msg.text[args->index], this->msg.textColors[args->who], this->msg.shadowColors[args->who], args->text);
            this->msg.waitFrames = 0;
            break;
        case 4:
            if ((!this->msg.skippable || !(g_Buttons & 0x100)) &&
                (!(g_Buttons & 1) || (g_Buttons & 1) == (g_PrevButtons & 1) || this->msg.waitFrames < 8))
            {
                if (this->msg.waitFrames >= this->msg.instr->i)
                {
                    break;
                }
                this->msg.waitFrames++;
                goto stepSprites;
            }
            break;
        case 5:
            args = &this->msg.instr->args;
            if (args->who < 2)
            {
                this->msg.portraits[args->who].pendingInterrupt = args->value;
            }
            else
            {
                this->msg.text[args->who - 2].pendingInterrupt = args->value;
            }
            break;
        case 6:
            this->msg.unk_6a0++;
            break;
        case 7:
            g_SpriteEngine->StartScript(&this->unk_1cb4, 0x701);
            this->unk_1cb4.unk_10c = 16;
            this->unk_1cb4.unk_10d = 16;
            g_SpriteEngine->RenderTextRightAligned(&this->unk_1cb4, 0xe0ffff, 0, "♪%s", g_Stage.data->songNames[this->msg.instr->i]);
            if (g_App.PlayMidi(this->msg.instr->i) != 0)
            {
                g_App.PlayMusic(g_Stage.data->songPaths[this->msg.instr->i]);
            }
            break;
        case 8:
            args = &this->msg.instr->args;
            g_SpriteEngine->StartScript(&this->msg.unk_45c[args->index], args->index + 0x704);
            g_SpriteEngine->RenderTextRightAligned(&this->msg.unk_45c[args->index], this->msg.textColors[args->who], this->msg.shadowColors[args->who], args->text);
            this->msg.waitFrames = 0;
            break;
        case 9:
            this->stageClearShown = 1;
            if (g_GameManager.stage < 6)
            {
                g_SpriteEngine->StartScript(&this->unk_2424, 0x619);
            }
            else
            {
                g_GameManager.nextExtend = 0xff;
            }
            break;
        case 10:
            goto stepSprites;
        case 12:
            g_App.FadeOutMusic(4.0f);
            break;
        case 11:
            g_GameManager.guiScore = g_GameManager.score;
            if (g_GameManager.isInPractice)
            {
                g_GameManager.guiScore = g_GameManager.score;
                g_App.scene = 7;
                goto stepSprites;
            }
            if (g_GameManager.stage < 5 || (g_GameManager.difficulty != 0 && g_GameManager.stage == 5))
            {
                g_App.scene = 3;
            }
            else if (g_GameManager.isInReplay == 0)
            {
                if (g_GameManager.difficulty == 4)
                {
                    g_GameManager.isGameCleared = 1;
                    g_GameManager.guiScore = g_GameManager.score;
                    g_App.scene = 7;
                    goto stepSprites;
                }
                g_App.scene = 10;
            }
            else
            {
                g_App.scene = 8;
            }
            goto stepSprites;
        case 13:
            this->msg.skippable = this->msg.instr->b;
            break;
        }
        this->msg.instr = (MsgInstr *)((BYTE *)this->msg.instr + 4 + this->msg.instr->argsSize);
    }
    this->msg.timer.NextTick();
stepSprites:
    g_SpriteEngine->Step(&this->msg.portraits[0]);
    g_SpriteEngine->Step(&this->msg.portraits[1]);
    g_SpriteEngine->Step(&this->msg.text[0]);
    g_SpriteEngine->Step(&this->msg.text[1]);
    g_SpriteEngine->Step(&this->msg.unk_45c[0]);
    g_SpriteEngine->Step(&this->msg.unk_45c[1]);
    if (this->msg.timer < 60 && this->msg.skippable && (g_Buttons & 0x100))
    {
        this->msg.timer.Set(60);
    }
    return 0;
}

int GuiImpl::DrawMsg()
{
    float height;

    if (this->msg.scriptIndex < 0)
    {
        return -1;
    }
    if (g_GameManager.stage == 6 && (this->msg.scriptIndex == 1 || this->msg.scriptIndex == 11))
    {
        return 0;
    }
    if (this->msg.timer < 60)
    {
        height = this->msg.timer.AsFloat() * 48.0f / 60.0f;
    }
    else
    {
        height = 48.0f;
    }
    VertexDiffuseXyzrwh vertices[4];
    vertices[0].pos = D3DXVECTOR3((g_GameManager.arena[2] - 256.0f) / 2.0f + g_GameManager.arena[0] - 16.0f, 384.0f, 0.0f);
    vertices[1].pos = D3DXVECTOR3((g_GameManager.arena[2] - 256.0f) / 2.0f + g_GameManager.arena[0] + 256.0f + 16.0f, 384.0f, 0.0f);
    vertices[2].pos = D3DXVECTOR3((g_GameManager.arena[2] - 256.0f) / 2.0f + g_GameManager.arena[0] - 16.0f, 384.0f + height, 0.0f);
    vertices[3].pos = D3DXVECTOR3((g_GameManager.arena[2] - 256.0f) / 2.0f + g_GameManager.arena[0] + 256.0f + 16.0f, 384.0f + height, 0.0f);
    vertices[0].diffuse = vertices[1].diffuse = 0xd0000000;
    vertices[2].diffuse = vertices[3].diffuse = 0x90000000;
    vertices[0].w = vertices[1].w = vertices[2].w = vertices[3].w = 1.0f;
    g_SpriteEngine->Draw(&this->msg.portraits[0]);
    g_SpriteEngine->Draw(&this->msg.portraits[1]);
    if (!g_App.cfg.flags.noColorBlending)
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    }
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    if (!g_App.cfg.flags.noDepthTest)
    {
        g_App.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }
    g_App.d3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_App.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(VertexDiffuseXyzrwh));
    g_SpriteEngine->ClearVertexFormat();
    g_SpriteEngine->ClearColorOp();
    g_SpriteEngine->ClearBlendMode();
    g_SpriteEngine->ClearZWrite();
    if (!g_App.cfg.flags.noColorBlending)
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_SpriteEngine->Draw(&this->msg.text[0]);
    g_SpriteEngine->Draw(&this->msg.text[1]);
    g_SpriteEngine->Draw(&this->msg.unk_45c[0]);
    g_SpriteEngine->Draw(&this->msg.unk_45c[1]);
    return 0;
}

BOOL Gui::Func419572()
{
    if (this->impl->msg.unk_6a0 > 0)
    {
        return FALSE;
    }
    return this->impl->msg.scriptIndex >= 0;
}

BOOL Gui::Func4195a2()
{
    return this->impl->msg.scriptIndex >= 0;
}

void Gui::UpdateInterface()
{
    int i;
    int bonus;

    for (i = 0; i < 0x1a; i++)
    {
        if (i == 0x13 && this->impl->msg.scriptIndex < 0)
        {
            if (this->bossPresent)
            {
                if (this->impl->bossBarState == 0)
                {
                    g_SpriteEngine->StartScript(&this->impl->unk_0000[i], 0x613);
                    this->impl->bossBarState = 1;
                    this->bossBarAlpha = 0;
                }
                else
                {
                    if (g_SpriteEngine->Step(&this->impl->unk_0000[i]) != 0)
                    {
                        this->impl->bossBarState = 2;
                    }
                    if (this->bossBarAlpha < 0xfc)
                    {
                        this->bossBarAlpha += 4;
                    }
                    else
                    {
                        this->bossBarAlpha = 0xff;
                    }
                }
            }
            else if (this->impl->bossBarState != 0)
            {
                if (this->impl->bossBarState <= 2)
                {
                    g_SpriteEngine->StartScript(&this->impl->unk_0000[i], 0x614);
                    this->impl->bossBarState = 3;
                }
                if (this->bossBarAlpha > 0)
                {
                    this->bossBarAlpha -= 4;
                }
                else
                {
                    this->bossBarAlpha = 0;
                }
                if (g_SpriteEngine->Step(&this->impl->unk_0000[i]) != 0)
                {
                    this->impl->bossBarState = 0;
                    this->bossHealthShown = 0.0f;
                    this->bossBarAlpha = 0;
                }
            }
            if (this->impl->bossBarState >= 2)
            {
                if (this->bossHealth > this->bossHealthShown)
                {
                    this->bossHealthShown += 0.01f;
                    if (this->bossHealth < this->bossHealthShown)
                    {
                        this->bossHealthShown = this->bossHealth;
                    }
                }
                else if (this->bossHealth < this->bossHealthShown)
                {
                    this->bossHealthShown -= 0.02f;
                    if (this->bossHealth > this->bossHealthShown)
                    {
                        this->bossHealthShown = this->bossHealth;
                    }
                }
            }
        }
        else
        {
            g_SpriteEngine->Step(&this->impl->unk_0000[i]);
        }
    }
    g_SpriteEngine->Step(&this->impl->unk_1ba4);
    g_SpriteEngine->Step(&this->impl->unk_1cb4);
    g_SpriteEngine->Step(&this->impl->unk_1dc4);
    g_SpriteEngine->Step(&this->impl->unk_1fe4);
    g_SpriteEngine->Step(&this->impl->unk_1ed4);
    g_SpriteEngine->Step(&this->impl->unk_20f4);
    if (this->impl->unk_2424.spriteIndex >= 0 && g_SpriteEngine->Step(&this->impl->unk_2424) != 0)
    {
        this->impl->unk_2424.spriteIndex = -1;
    }
    if (this->impl->bonusPopup.shown)
    {
        if (this->impl->bonusPopup.timer < 30)
        {
            this->impl->bonusPopup.pos.x = this->impl->bonusPopup.timer.AsFloat() * -312.0f / 30.0f + 416.0f;
        }
        else
        {
            this->impl->bonusPopup.pos.x = 104.0f;
        }
        if (this->impl->bonusPopup.timer >= 250)
        {
            this->impl->bonusPopup.shown = 0;
        }
        ++this->impl->bonusPopup.timer;
    }
    if (this->impl->fullPowerPopup.shown)
    {
        if (this->impl->fullPowerPopup.timer < 30)
        {
            this->impl->fullPowerPopup.pos.x = this->impl->fullPowerPopup.timer.AsFloat() * -312.0f / 30.0f + 416.0f;
        }
        else
        {
            this->impl->fullPowerPopup.pos.x = 104.0f;
        }
        if (this->impl->fullPowerPopup.timer >= 180)
        {
            this->impl->fullPowerPopup.shown = 0;
        }
        ++this->impl->fullPowerPopup.timer;
    }
    if (this->impl->unk_2c24.shown)
    {
        if (this->impl->unk_2c24.timer >= 280)
        {
            this->impl->unk_2c24.shown = 0;
        }
        ++this->impl->unk_2c24.timer;
    }
    if (this->impl->stageClearShown == 1)
    {
        bonus = 0;
        bonus += g_GameManager.stage * 1000;
        bonus += g_GameManager.graze * 10;
        bonus += g_GameManager.power * 100;
        bonus *= g_GameManager.pointItems;
        if (g_GameManager.stage >= 6)
        {
            bonus += g_GameManager.lives * 3000000;
            bonus += g_GameManager.bombs * 1000000;
        }
        switch (g_GameManager.difficulty)
        {
        case 0:
            bonus /= 2;
            bonus -= bonus % 10;
            break;
        case 2:
            bonus = bonus * 12 / 10;
            bonus -= bonus % 10;
            break;
        case 3:
            bonus = bonus * 15 / 10;
            bonus -= bonus % 10;
            break;
        case 4:
            bonus *= 2;
            bonus -= bonus % 10;
            break;
        }
        switch (g_App.startCfg.lives)
        {
        case 3:
            bonus = bonus * 5 / 10;
            bonus -= bonus % 10;
            break;
        case 4:
            bonus = bonus * 2 / 10;
            bonus -= bonus % 10;
            break;
        }
        this->impl->stageClearTotal = bonus;
        g_GameManager.score += bonus;
        this->impl->stageClearShown++;
    }
}

void Gui::DrawHud()
{
    float y;
    float xx;
    int idx;
    Sprite *vm;
    int clamped;
    D3DCOLOR c;
    D3DXVECTOR3 pos;

    if (this->impl->msg.scriptIndex < 0 && this->bossPresent + this->impl->bossBarState > 0)
    {
        vm = &this->impl->unk_0000[0x13];
        g_SpriteEngine->Draw(vm);
        vm = &this->impl->unk_0000[0x15];
        vm->flags.anchor = 3;
        vm->scaleX = this->bossHealthShown * 288.0f / 14.0f;
        vm->pos.x = 96.0f;
        vm->pos.y = 24.0f;
        vm->pos.z = 0.0f;
        g_SpriteEngine->Draw(vm);
        pos.x = 80.0f;
        pos.y = 16.0f;
        pos.z = 0.0f;
        g_AsciiText.SetColor(this->bossBarAlpha << 24 | 0xffff80);
        g_AsciiText.AddFormat(&pos, "%d", this->unk_14);
        pos = D3DXVECTOR3(384.0f, 16.0f, 0.0f);
        if (this->bossTimer >= 20)
        {
            c = g_BossTimerColors[0];
        }
        else if (this->bossTimer >= 10)
        {
            c = g_BossTimerColors[1];
        }
        else if (this->bossTimer >= 5)
        {
            c = g_BossTimerColors[2];
        }
        else
        {
            c = g_BossTimerColors[3];
        }
        g_AsciiText.SetColor(this->bossBarAlpha << 24 | c);
        clamped = this->bossTimer > 99 ? 99 : this->bossTimer;
        if (clamped < 10 && this->unk_1c != this->bossTimer)
        {
            g_SoundPlayer.QueueSound(0x1d, 0);
        }
        g_AsciiText.AddFormat(&pos, "%.2d", clamped);
        g_AsciiText.color = 0xffffffff;
        this->unk_1c = this->bossTimer;
    }
    g_App.viewport.X = 0;
    g_App.viewport.Y = 0;
    g_App.viewport.Width = 640;
    g_App.viewport.Height = 480;
    g_App.d3dDevice->SetViewport(&g_App.viewport);
    vm = &this->impl->unk_0000[6];
    if (!g_App.cfg.flags.noFrameItems &&
        (vm->scriptPos != NULL || g_App.redrawFrames != 0 || g_App.ClearsBackBuffer()))
    {
        for (y = 0.0f; y < 464.0f; y += 32.0f)
        {
            vm->pos = D3DXVECTOR3(0.0f, y, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        for (xx = 416.0f; xx < 624.0f; xx += 32.0f)
        {
            for (y = 0.0f; y < 464.0f; y += 32.0f)
            {
                vm->pos = D3DXVECTOR3(xx, y, 0.49f);
                g_SpriteEngine->Draw(vm);
            }
        }
        vm = &this->impl->unk_0000[7];
        for (xx = 32.0f; xx < 416.0f; xx += 32.0f)
        {
            vm->pos = D3DXVECTOR3(xx, 0.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        vm = &this->impl->unk_0000[8];
        for (xx = 32.0f; xx < 416.0f; xx += 32.0f)
        {
            vm->pos = D3DXVECTOR3(xx, 464.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        g_SpriteEngine->DrawRotated(&this->impl->unk_0000[5]);
        g_SpriteEngine->DrawRotated(&this->impl->unk_0000[0]);
        g_SpriteEngine->DrawRotated(&this->impl->unk_0000[1]);
        g_SpriteEngine->DrawRotated(&this->impl->unk_0000[3]);
        g_SpriteEngine->DrawRotated(&this->impl->unk_0000[4]);
        g_SpriteEngine->DrawRotated(&this->impl->unk_0000[2]);
        g_SpriteEngine->Draw(&this->impl->unk_0000[9]);
        g_SpriteEngine->Draw(&this->impl->unk_0000[10]);
        g_SpriteEngine->Draw(&this->impl->unk_0000[11]);
        g_SpriteEngine->Draw(&this->impl->unk_0000[12]);
        g_SpriteEngine->Draw(&this->impl->unk_0000[13]);
        g_SpriteEngine->Draw(&this->impl->unk_0000[14]);
        g_SpriteEngine->Draw(&this->impl->unk_0000[15]);
        this->flags.livesChanged = 2;
        this->flags.bombsChanged = 2;
        this->flags.grazeChanged = 2;
        this->flags.pointsChanged = 2;
        this->flags.powerChanged = 2;
    }
    if (!g_App.cfg.flags.noFrameItems)
    {
        vm = &this->impl->unk_0000[0x16];
        xx = 496.0f;
        vm->pos = D3DXVECTOR3(xx, 58.0f, 0.49f);
        g_SpriteEngine->Draw(vm);
        vm->pos = D3DXVECTOR3(xx, 82.0f, 0.49f);
        g_SpriteEngine->Draw(vm);
        if (this->flags.livesChanged)
        {
            vm->pos = D3DXVECTOR3(xx, 122.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        if (this->flags.bombsChanged)
        {
            vm->pos = D3DXVECTOR3(xx, 146.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        if (this->flags.powerChanged)
        {
            vm->pos = D3DXVECTOR3(xx, 186.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        if (this->flags.grazeChanged)
        {
            vm->pos = D3DXVECTOR3(xx, 206.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        if (this->flags.pointsChanged)
        {
            vm->pos = D3DXVECTOR3(xx, 226.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
        vm->pos = D3DXVECTOR3(488.0f, 464.0f, 0.49f);
        g_SpriteEngine->Draw(vm);
        vm->pos = D3DXVECTOR3(0.0f, 464.0f, 0.49f);
        g_SpriteEngine->Draw(vm);
    }
    if (this->flags.livesChanged || g_App.cfg.flags.noFrameItems)
    {
        vm = &this->impl->unk_0000[0x10];
        for (idx = 0, xx = 496.0f; idx < g_GameManager.lives; idx++, xx += 16.0f)
        {
            vm->pos = D3DXVECTOR3(xx, 122.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
    }
    if (this->flags.bombsChanged || g_App.cfg.flags.noFrameItems)
    {
        vm = &this->impl->unk_0000[0x11];
        for (idx = 0, xx = 496.0f; idx < g_GameManager.bombs; idx++, xx += 16.0f)
        {
            vm->pos = D3DXVECTOR3(xx, 146.0f, 0.49f);
            g_SpriteEngine->Draw(vm);
        }
    }
    if (this->flags.powerChanged || g_App.cfg.flags.noFrameItems)
    {
        VertexDiffuseXyzrwh vertices[4];

        if (g_GameManager.power > 0)
        {
            vertices[0].pos = D3DXVECTOR3(496.0f, 186.0f, 0.1f);
            vertices[1].pos = D3DXVECTOR3(g_GameManager.power + 496 + 0.0f, 186.0f, 0.1f);
            vertices[2].pos = D3DXVECTOR3(496.0f, 202.0f, 0.1f);
            vertices[3].pos = D3DXVECTOR3(g_GameManager.power + 496 + 0.0f, 202.0f, 0.1f);
            vertices[0].diffuse = vertices[2].diffuse = 0xe0e0e0ff;
            vertices[1].diffuse = vertices[3].diffuse = 0x80e0e0ff;
            vertices[0].w = vertices[1].w = vertices[2].w = vertices[3].w = 1.0f;
            if (!g_App.cfg.flags.noColorBlending)
            {
                g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            }
            g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
            g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
            if (!g_App.cfg.flags.noDepthTest)
            {
                g_App.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
                g_App.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
            }
            g_App.d3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            g_App.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(VertexDiffuseXyzrwh));
            g_SpriteEngine->ClearVertexFormat();
            g_SpriteEngine->ClearColorOp();
            g_SpriteEngine->ClearBlendMode();
            g_SpriteEngine->ClearZWrite();
            if (!g_App.cfg.flags.noColorBlending)
            {
                g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            }
            g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            if (g_GameManager.power >= 128)
            {
                vm = &this->impl->unk_0000[0x12];
                vm->pos = D3DXVECTOR3(496.0f, 186.0f, 0.0f);
                g_SpriteEngine->Draw(vm);
            }
        }
        if (g_GameManager.power < 128)
        {
            g_AsciiText.AddFormat(&D3DXVECTOR3(496.0f, 186.0f, 0.0f), "%d", g_GameManager.power);
        }
    }
    {
        // A block of its own: the original keeps scorePos after the
        // power bar's vertices.
        D3DXVECTOR3 scorePos(496.0f, 82.0f, 0.0f);

        g_AsciiText.AddFormat(&scorePos, "%.9d", g_GameManager.guiScore);
        scorePos = D3DXVECTOR3(496.0f, 58.0f, 0.0f);
        g_AsciiText.AddFormat(&scorePos, "%.9d", g_GameManager.highScore);
        if (this->flags.grazeChanged || g_App.cfg.flags.noFrameItems)
        {
            scorePos = D3DXVECTOR3(496.0f, 206.0f, 0.0f);
            g_AsciiText.AddFormat(&scorePos, "%d", g_GameManager.graze);
        }
        if (this->flags.pointsChanged || g_App.cfg.flags.noFrameItems)
        {
            scorePos = D3DXVECTOR3(496.0f, 226.0f, 0.0f);
            g_AsciiText.AddFormat(&scorePos, "%d", g_GameManager.pointItems);
        }
    }
    if (this->flags.livesChanged)
    {
        this->flags.livesChanged--;
    }
    if (this->flags.powerChanged)
    {
        this->flags.powerChanged--;
    }
    if (this->flags.bombsChanged)
    {
        this->flags.bombsChanged--;
    }
    if (this->flags.grazeChanged)
    {
        this->flags.grazeChanged--;
    }
    if (this->flags.pointsChanged)
    {
        this->flags.pointsChanged--;
    }
}

void Gui::DrawBanners()
{
    D3DXVECTOR3 pos;

    if (this->impl->unk_1ba4.flags.unk0)
    {
        pos.x = 168.0f;
        pos.y = 198.0f;
        pos.z = 0.0f;
        if (!g_GameManager.isInDemo)
        {
            g_SpriteEngine->Draw3DRounded(&this->impl->unk_1ba4);
            g_AsciiText.SetColor((this->impl->unk_1ba4.color & 0xff000000) | 0xffff40);
            if (g_GameManager.stage < 6)
            {
                pos.x = 168.0f;
                g_AsciiText.AddFormat(&pos, "STAGE %d", g_GameManager.stage);
            }
            else if (g_GameManager.stage == 6)
            {
                pos.x = 136.0f;
                g_AsciiText.AddFormat(&pos, "FINAL STAGE");
            }
            else
            {
                pos.x = 136.0f;
                g_AsciiText.AddFormat(&pos, "EXTRA STAGE");
            }
        }
        else
        {
            g_AsciiText.SetColor((this->impl->unk_1ba4.color & 0xff000000) | 0xffff40);
            pos.x = 136.0f;
            g_AsciiText.AddFormat(&pos, " DEMO PLAY");
        }
        g_AsciiText.color = 0xffffffff;
    }
    if (this->impl->unk_1cb4.flags.unk0 && !g_GameManager.isInDemo)
    {
        g_SpriteEngine->Draw3DRounded(&this->impl->unk_1cb4);
    }
    if (this->impl->unk_1dc4.flags.unk0)
    {
        g_SpriteEngine->Draw(&this->impl->unk_1dc4);
    }
    if (this->impl->unk_1ed4.flags.unk0)
    {
        g_SpriteEngine->Draw(&this->impl->unk_1ed4);
    }
    if (this->impl->unk_1fe4.flags.unk0)
    {
        this->impl->unk_2204.pos = this->impl->unk_1fe4.pos;
        this->impl->unk_2204.pos.x += this->unk_08 * 16.0f / 15.0f / 2.0f + -128.0f - 16.0f;
        this->impl->unk_2204.scaleX = this->unk_08 / 14.0f;
        g_SpriteEngine->Draw(&this->impl->unk_2204);
        g_SpriteEngine->Draw(&this->impl->unk_1fe4);
    }
    if (this->impl->unk_20f4.flags.unk0)
    {
        this->impl->unk_2314.pos = this->impl->unk_20f4.pos;
        this->impl->unk_2314.pos.x += 128.0f - this->unk_0c * 16.0f / 15.0f / 2.0f;
        this->impl->unk_2314.scaleX = this->unk_0c / 14.0f;
        g_SpriteEngine->Draw(&this->impl->unk_2314);
        g_SpriteEngine->Draw(&this->impl->unk_20f4);
    }
    if (this->impl->unk_2424.spriteIndex >= 0)
    {
        g_App.viewport.X = g_GameManager.arena[0];
        g_App.viewport.Y = g_GameManager.arena[1];
        g_App.viewport.Width = g_GameManager.arena[2];
        g_App.viewport.Height = g_GameManager.arena[3];
        g_App.d3dDevice->SetViewport(&g_App.viewport);
        g_SpriteEngine->Draw(&this->impl->unk_2424);
    }
}

int Gui::DeletedCallback(Gui *gui)
{
    g_SpriteEngine->ReleaseAnm(0x12);
    g_SpriteEngine->ReleaseAnm(0x13);
    g_SpriteEngine->ReleaseAnm(0x14);
    gui->FreeMsgFile();
    if ((g_App.scene != 3) != 0)
    {
        g_SpriteEngine->ReleaseAnm(0xd);
        g_SpriteEngine->ReleaseAnm(0xe);
        g_SpriteEngine->ReleaseAnm(0xf);
        g_SpriteEngine->ReleaseAnm(0x10);
        g_SpriteEngine->ReleaseAnm(0x11);
        delete gui->impl;
        gui->impl = NULL;
    }
    return 0;
}

int Gui::RegisterChain()
{
    Gui *gui = &g_Gui;

    if ((g_App.scene != 3) != 0)
    {
        memset(gui, 0, sizeof(Gui));
        gui->impl = new GuiImpl;
    }
    g_GuiCalcChain.SetCallback((ChainCallback)Gui::OnUpdate);
    g_GuiCalcChain.addedCallback = (ChainCallback)Gui::AddedCallback;
    g_GuiCalcChain.deletedCallback = (ChainCallback)Gui::DeletedCallback;
    g_GuiCalcChain.arg = gui;
    if (g_Chain.AddToCalcChain(&g_GuiCalcChain, 12) != 0)
    {
        return -1;
    }
    g_GuiDrawChain.SetCallback((ChainCallback)Gui::OnDraw);
    g_GuiDrawChain.arg = gui;
    g_Chain.AddToDrawChain(&g_GuiDrawChain, 11);
    return 0;
}

int Timer::NextTick()
{
    this->previous = this->current;
    g_App.TickTimer(&this->current, &this->subFrame);
    return this->current;
}
