// cflags: /Os
// The /Os file with Gui, from 0x4172d3.

#include "gamemanager.h"
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
