// cflags: /Os
// The /Os file with Gui, from 0x4172d3.

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
    this->impl->unk_2bdc = 0;
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
            this->unk_2bdc = 1;
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

int Timer::NextTick()
{
    this->previous = this->current;
    g_Unknown6c6d18.TickTimer(&this->current, &this->subFrame);
    return this->current;
}
