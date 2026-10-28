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
    if (g_GameManager.unk_002c)
    {
        return 1;
    }
    gui->Func4195bf();
    gui->impl->RunMsg();
    return 1;
}

int Gui::OnDraw(Gui *gui)
{
    D3DXVECTOR3 cursor;
    char buf[32];

    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
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
        if (g_GameManager.difficulty < 4 && !g_GameManager.unk_1823)
        {
            switch (g_Unknown6c6d18.unk_14c.lives)
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
    gui->Func419c99();
    g_AsciiText.unk_6230 = 1;
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
    g_AsciiText.unk_6230 = 0;
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    return 1;
}

void Gui::Func417bfd(int sprite, char *text)
{
    g_SpriteEngine->StartScript(&this->impl->unk_1ed4, 0x4a3);
    g_SpriteEngine->SetSprite(&this->impl->unk_1ed4, sprite + 0x4a8);
    g_SpriteEngine->StartScript(&this->impl->unk_20f4, 0x707);
    g_SpriteEngine->RenderTextRightAligned(&this->impl->unk_20f4, 0xfff0f0, 0, text);
    this->unk_0c = strlen(text) * 15 / 2.0f + 16.0f;
    g_Unknown6d3f50.QueueSound(14, 0);
}

int Gui::LoadResources()
{
    int i;

    if ((g_Unknown6c6d18.scene != 3) != 0)
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
    if ((g_Unknown6c6d18.scene != 3) != 0)
    {
        for (i = 0; i < 0x1a; i++)
        {
            g_SpriteEngine->StartScript(&this->impl->unk_0000[i], i + 0x600);
        }
    }
    this->bossPresent = 0;
    this->impl->unk_1ba0 = 0;
    this->bossHealth = 0.0f;
    this->unk_28 = 0.0f;
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
        LogPrintf(&g_Unknown69d998, "error : メッセージファイル %s が読み込めませんでした\n", path);
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
    g_Unknown6c6d18.unk_198 = 3;
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
            if (g_Unknown6c6d18.PlayMidi(this->msg.instr->i) != 0)
            {
                g_Unknown6c6d18.PlayMusic(g_Stage.data->songPaths[this->msg.instr->i]);
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
                g_GameManager.unk_181c = 0xff;
            }
            break;
        case 10:
            goto stepSprites;
        case 12:
            g_Unknown6c6d18.FadeOutMusic(4.0f);
            break;
        case 11:
            g_GameManager.unk_0000 = g_GameManager.score;
            if (g_GameManager.unk_1823)
            {
                g_GameManager.unk_0000 = g_GameManager.score;
                g_Unknown6c6d18.scene = 7;
                goto stepSprites;
            }
            if (g_GameManager.stage < 5 || (g_GameManager.difficulty != 0 && g_GameManager.stage == 5))
            {
                g_Unknown6c6d18.scene = 3;
            }
            else if (g_GameManager.unk_001c == 0)
            {
                if (g_GameManager.difficulty == 4)
                {
                    g_GameManager.unk_1822 = 1;
                    g_GameManager.unk_0000 = g_GameManager.score;
                    g_Unknown6c6d18.scene = 7;
                    goto stepSprites;
                }
                g_Unknown6c6d18.scene = 10;
            }
            else
            {
                g_Unknown6c6d18.scene = 8;
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
    if (!g_Unknown6c6d18.cfg.flags.noColorBlending)
    {
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    }
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    if (!g_Unknown6c6d18.cfg.flags.noDepthTest)
    {
        g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }
    g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_Unknown6c6d18.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(VertexDiffuseXyzrwh));
    g_SpriteEngine->ClearVertexFormat();
    g_SpriteEngine->ClearColorOp();
    g_SpriteEngine->ClearBlendMode();
    g_SpriteEngine->ClearZWrite();
    if (!g_Unknown6c6d18.cfg.flags.noColorBlending)
    {
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
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

void Gui::DrawBanners()
{
    D3DXVECTOR3 pos;

    if (this->impl->unk_1ba4.flags.unk0)
    {
        pos.x = 168.0f;
        pos.y = 198.0f;
        pos.z = 0.0f;
        if (!g_GameManager.unk_1824)
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
    if (this->impl->unk_1cb4.flags.unk0 && !g_GameManager.unk_1824)
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
        g_Unknown6c6d18.viewport.X = g_GameManager.arena[0];
        g_Unknown6c6d18.viewport.Y = g_GameManager.arena[1];
        g_Unknown6c6d18.viewport.Width = g_GameManager.arena[2];
        g_Unknown6c6d18.viewport.Height = g_GameManager.arena[3];
        g_Unknown6c6d18.d3dDevice->SetViewport(&g_Unknown6c6d18.viewport);
        g_SpriteEngine->Draw(&this->impl->unk_2424);
    }
}

int Timer::NextTick()
{
    this->previous = this->current;
    g_Unknown6c6d18.TickTimer(&this->current, &this->subFrame);
    return this->current;
}
