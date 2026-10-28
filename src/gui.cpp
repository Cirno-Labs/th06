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
    return this->impl->unk_2bd8;
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
    gui->impl->Func4188c9();
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
    g_SpriteEngine->RenderTextRightAligned(&this->impl->unk_1cb4, 0xe0ffff, 0, "♪%s", g_Stage.data->songName);
    this->impl->unk_253c = -1;
    this->impl->unk_2bdc[0] = 0;
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
    this->impl->msgFile = (MsgFile *)LoadFileData(path, 0);
    if (this->impl->msgFile == NULL)
    {
        LogPrintf(&g_Unknown69d998, "error : メッセージファイル %s が読み込めませんでした\n", path);
        return -1;
    }
    this->impl->unk_253c = -1;
    this->impl->unk_2538 = 0;
    for (i = 0; i < this->impl->msgFile->count; i++)
    {
        this->impl->msgFile->scripts[i] = (char *)this->impl->msgFile->scripts[i] + (int)this->impl->msgFile;
    }
    return 0;
}

void Gui::FreeMsgFile()
{
    if (this->impl->msgFile != NULL)
    {
        MemFree(this->impl->msgFile);
        this->impl->msgFile = NULL;
    }
}

void Gui::StartMsg(int index)
{
    this->impl->Func41878b(index);
    g_Unknown6c6d18.unk_198 = 3;
}

BOOL Gui::Func419572()
{
    if (this->impl->unk_2bd4 > 0)
    {
        return FALSE;
    }
    return this->impl->unk_253c >= 0;
}

BOOL Gui::Func4195a2()
{
    return this->impl->unk_253c >= 0;
}

int Timer::NextTick()
{
    this->previous = this->current;
    g_Unknown6c6d18.TickTimer(&this->current, &this->subFrame);
    return this->current;
}
