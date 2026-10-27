// The file with ReplayManager, around 0x42a140.

#include "replay.h"
#include "gamemanager.h"
#include "unsorted.h"

int ValidateReplay(void *data, int size)
{
    int i;
    ReplayHeader *file = (ReplayHeader *)data;
    BYTE b;
    BYTE *bytes;
    int sum;
    BYTE *ptr2;

    if (file == NULL)
    {
        return -1;
    }
    if (*(DWORD *)file->magic != *(DWORD *)"T6RP")
    {
        return -1;
    }
    bytes = (BYTE *)file + 0xf;
    b = file->key;
    for (i = 0; i < size - 0xf; i++, bytes++)
    {
        *bytes -= b;
        b += 7;
    }
    ptr2 = &file->key;
    sum = 0x3f000318;
    for (i = 0; i < size - 0xe; i++, ptr2++)
    {
        sum += *ptr2;
    }
    if (sum != file->checksum)
    {
        return -1;
    }
    if (file->version != 0x102)
    {
        return -1;
    }
    return 0;
}

int ReplayManager::RecordOnUpdate(ReplayManager *replay)
{
    WORD input;

    if (!g_GameManager.unk_1821)
    {
        return 1;
    }
    input = g_Buttons & 0x1f7;
    if (input != replay->cursors[0]->input)
    {
        replay->cursors[0]++;
        replay->cursors[g_GameManager.stage] = replay->cursors[0] + 1;
        replay->cursors[0]->frame = replay->frame;
        replay->cursors[0]->input = input;
    }
    replay->frame++;
    return 1;
}

int ReplayManager::PlayOnUpdate(ReplayManager *replay)
{
    if (!g_GameManager.unk_1821)
    {
        return 1;
    }
    while (replay->frame >= (replay->cursors[0] + 1)->frame)
    {
        replay->cursors[0]++;
    }
    g_Buttons = (g_Buttons & ~0x1f7) | replay->cursors[0]->input;
    g_ButtonRepeat = 0;
    if (g_PrevButtons == g_Buttons)
    {
        if (g_ButtonFrames >= 30)
        {
            if (g_ButtonFrames % 8 == 0)
            {
                g_ButtonRepeat = 1;
            }
            if (g_ButtonFrames >= 38)
            {
                g_ButtonFrames = 30;
            }
        }
        g_ButtonFrames++;
    }
    else
    {
        g_ButtonFrames = 0;
    }
    replay->frame++;
    return 1;
}

int ReplayManager::PlayOnUpdate2(ReplayManager *replay)
{
    if (!g_GameManager.unk_1821)
    {
        return 1;
    }
    if (g_Gui.Func4195a2() && g_Gui.Func417344() && replay->frame % 3 != 2)
    {
        return 6;
    }
    return 1;
}

int ReplayManager::OnDraw(ReplayManager *replay)
{
    return 1;
}

int ReplayManager::RegisterChain(int mode, char *path)
{
    ReplayManager *replay;

    if (g_Unknown6c6d18.gameSpeed < 0.99f && mode == 0)
    {
        return 0;
    }
    g_Unknown6c6d18.gameSpeed = 1.0f;
    if (g_ReplayManager == NULL)
    {
        replay = new ReplayManager;
        g_ReplayManager = replay;
        replay->data = NULL;
        replay->mode = mode;
        replay->path = path;
        switch (mode)
        {
        case 0:
            replay->calcChain = g_Chain.CreateElem((ChainCallback)ReplayManager::RecordOnUpdate);
            replay->calcChain->addedCallback = (ChainCallback)ReplayManager::RecordAddedCallback;
            replay->calcChain->deletedCallback = (ChainCallback)ReplayManager::DeletedCallback;
            replay->drawChain = g_Chain.CreateElem((ChainCallback)ReplayManager::OnDraw);
            replay->calcChain->arg = replay;
            if (g_Chain.AddToCalcChain(replay->calcChain, 0xf) != 0)
            {
                return -1;
            }
            replay->calcChain2 = NULL;
            break;
        case 1:
            replay->calcChain = g_Chain.CreateElem((ChainCallback)ReplayManager::PlayOnUpdate);
            replay->calcChain->addedCallback = (ChainCallback)ReplayManager::PlayAddedCallback;
            replay->calcChain->deletedCallback = (ChainCallback)ReplayManager::DeletedCallback;
            replay->drawChain = g_Chain.CreateElem((ChainCallback)ReplayManager::OnDraw);
            replay->calcChain->arg = replay;
            if (g_Chain.AddToCalcChain(replay->calcChain, 5) != 0)
            {
                return -1;
            }
            replay->calcChain2 = g_Chain.CreateElem((ChainCallback)ReplayManager::PlayOnUpdate2);
            replay->calcChain2->arg = replay;
            g_Chain.AddToCalcChain(replay->calcChain2, 0x10);
            break;
        }
        replay->drawChain->arg = replay;
        g_Chain.AddToDrawChain(replay->drawChain, 0xd);
    }
    else
    {
        switch (mode)
        {
        case 0:
            ReplayManager::RecordAddedCallback(g_ReplayManager);
            break;
        case 1:
            return ReplayManager::PlayAddedCallback(g_ReplayManager);
        }
    }
    return 0;
}

void ReplayManager::EndStage()
{
    ReplayManager *replay = g_ReplayManager;

    if (replay != NULL)
    {
        replay->cursors[0]++;
        replay->cursors[0]->frame = replay->frame;
        replay->cursors[0]->input = 0;
        replay->cursors[0]++;
        replay->cursors[0]->frame = 9999999;
        replay->cursors[0]->input = 0;
        replay->cursors[g_GameManager.stage] = replay->cursors[0] + 1;
    }
}

int ReplayManager::DeletedCallback(ReplayManager *replay)
{
    g_Chain.Remove(replay->drawChain);
    replay->drawChain = NULL;
    if (replay->calcChain2 != NULL)
    {
        g_Chain.Remove(replay->calcChain2);
        replay->calcChain2 = NULL;
    }
    MemFree(g_ReplayManager->data);
    delete g_ReplayManager;
    g_ReplayManager = NULL;
    g_ReplayManager = NULL;
    return 0;
}

int ReplayManager::RecordAddedCallback(ReplayManager *replay)
{
    ReplayStage *cur;
    int idx;
    ReplayStage *earlier;

    replay->frame = 0;
    if (replay->data == NULL)
    {
        replay->data = new ReplayHeader;
        *(DWORD *)replay->data->magic = *(DWORD *)"T6RP";
        replay->data->shotType = g_GameManager.character * 2 + g_GameManager.shotType;
        replay->data->version = 0x102;
        replay->data->difficulty = g_GameManager.difficulty;
        *(DWORD *)replay->data->name = *(DWORD *)"NO NAME";
        for (idx = 0; idx < 7; idx++)
        {
            replay->data->stages[idx + 1] = NULL;
        }
    }
    else
    {
        earlier = replay->data->stages[g_GameManager.stage - 1];
        if (earlier == NULL)
        {
            return -1;
        }
        earlier->unk_00 = g_GameManager.unk_0004;
    }
    if (replay->data->stages[g_GameManager.stage] != NULL)
    {
        DebugPrint("error : replay.cpp");
    }
    replay->data->stages[g_GameManager.stage] = (ReplayStage *)MemAlloc(0x69780);
    cur = replay->data->stages[g_GameManager.stage];
    cur->bombs = g_GameManager.bombs;
    cur->lives = g_GameManager.lives;
    cur->power = g_GameManager.power;
    cur->rank = g_GameManager.rank;
    cur->unk_06 = g_GameManager.unk_1816;
    cur->unk_04 = g_GameManager.unk_1a2c;
    cur->unk_0c = g_GameManager.unk_1819;
    replay->cursors[0] = cur->frames;
    replay->cursors[0]->frame = 0;
    replay->cursors[0]->input = 0;
    replay->unk_44 = 0;
    return 0;
}

int ReplayManager::PlayAddedCallback(ReplayManager *replay)
{
    ReplayStage *cur;
    int idx;

    replay->frame = 0;
    if (replay->data == NULL)
    {
        replay->data = (ReplayHeader *)Func41e290(replay->path, !g_GameManager.unk_1824);
        if (ValidateReplay(replay->data, g_FileSize) != 0)
        {
            return -1;
        }
        for (idx = 0; idx < 7; idx++)
        {
            if (replay->data->stages[idx + 1] != NULL)
            {
                replay->data->stages[idx + 1] = (ReplayStage *)((int)replay->data->stages[idx + 1] + (int)replay->data);
            }
        }
    }
    if (replay->data->stages[g_GameManager.stage] == NULL)
    {
        return -1;
    }
    cur = replay->data->stages[g_GameManager.stage];
    g_GameManager.character = replay->data->shotType / 2;
    g_GameManager.shotType = replay->data->shotType % 2;
    g_GameManager.difficulty = replay->data->difficulty;
    g_GameManager.unk_1816 = cur->unk_06;
    g_Rng.SetSeed(cur->unk_04);
    g_GameManager.rank = cur->rank;
    g_GameManager.lives = cur->lives;
    g_GameManager.bombs = cur->bombs;
    g_GameManager.power = cur->power;
    replay->cursors[0] = cur->frames;
    g_GameManager.unk_1819 = cur->unk_0c;
    if (g_GameManager.stage >= 2 && replay->data->stages[g_GameManager.stage - 1] != NULL)
    {
        g_GameManager.unk_0000 = g_GameManager.unk_0004 = replay->data->stages[g_GameManager.stage - 1]->unk_00;
    }
    return 0;
}
