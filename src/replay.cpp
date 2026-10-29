// The file with ReplayManager, around 0x42a140.

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "replay.h"
#include "gamemanager.h"
#include "unsorted.h"

// The game's data, as the original defines it here. The DLL build uses
// the original's copies instead.
#ifdef TH06_EXE
ReplayManager *g_ReplayManager;
#endif // TH06_EXE

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

int ReplayManager::RegisterChain(int mode, char *path)
{
    ReplayManager *replay;

    if (g_App.gameSpeed < 0.99f && mode == 0)
    {
        return 0;
    }
    g_App.gameSpeed = 1.0f;
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

int ReplayManager::RecordOnUpdate(ReplayManager *replay)
{
    WORD input;

    if (!g_GameManager.isPlaying)
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

int ReplayManager::PlayOnUpdate2(ReplayManager *replay)
{
    if (!g_GameManager.isPlaying)
    {
        return 1;
    }
    if (g_Gui.IsMsgRunning() && g_Gui.IsMsgSkippable() && replay->frame % 3 != 2)
    {
        return 6;
    }
    return 1;
}

int ReplayManager::PlayOnUpdate(ReplayManager *replay)
{
    if (!g_GameManager.isPlaying)
    {
        return 1;
    }
    while (replay->frame >= (replay->cursors[0] + 1)->frame)
    {
        replay->cursors[0]++;
    }
    g_Buttons = (g_Buttons & ~0x1f7) | replay->cursors[0]->input;
    g_RepeatButtons = 0;
    if (g_LastButtons == g_Buttons)
    {
        if (g_ButtonHeldFrames >= 30)
        {
            if (g_ButtonHeldFrames % 8 == 0)
            {
                g_RepeatButtons = 1;
            }
            if (g_ButtonHeldFrames >= 38)
            {
                g_ButtonHeldFrames = 30;
            }
        }
        g_ButtonHeldFrames++;
    }
    else
    {
        g_ButtonHeldFrames = 0;
    }
    replay->frame++;
    return 1;
}

int ReplayManager::OnDraw(ReplayManager *replay)
{
    return 1;
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
        earlier->score = g_GameManager.score;
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
    cur->pointItemsTotal = g_GameManager.pointItemsTotal;
    cur->rngSeed = g_GameManager.rngSeed;
    cur->powerItemCombo = g_GameManager.powerItemCombo;
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
        replay->data = (ReplayHeader *)LoadFileData(replay->path, !g_GameManager.isInDemo);
        if (ValidateReplay(replay->data, g_LastFileSize) != 0)
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
    g_GameManager.pointItemsTotal = cur->pointItemsTotal;
    g_Random.SetSeed(cur->rngSeed);
    g_GameManager.rank = cur->rank;
    g_GameManager.lives = cur->lives;
    g_GameManager.bombs = cur->bombs;
    g_GameManager.power = cur->power;
    replay->cursors[0] = cur->frames;
    g_GameManager.powerItemCombo = cur->powerItemCombo;
    if (g_GameManager.stage >= 2 && replay->data->stages[g_GameManager.stage - 1] != NULL)
    {
        g_GameManager.guiScore = g_GameManager.score = replay->data->stages[g_GameManager.stage - 1]->score;
    }
    return 0;
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

void SaveReplay(char *path, char *name)
{
    int i;
    ReplayManager *replayTmp;
    float slowdown2;
    ReplayHeader hX;
    int offset;
    FILE *f2;
    int j1;
    int checksum;
    BYTE *bpCur;
    BYTE kbVal;
    int k1;
    BYTE *dp2;

    if (g_ReplayManager != NULL)
    {
        replayTmp = g_ReplayManager;
        if (replayTmp->GetMode() == 0)
        {
            if (path != NULL)
            {
                hX = *replayTmp->data;
                ReplayManager::EndStage();
                offset = sizeof(ReplayHeader);
                for (i = 0; i < 7; i++)
                {
                    if (replayTmp->data->stages[i + 1] != NULL)
                    {
                        hX.stages[i + 1] = (ReplayStage *)offset;
                        offset += (int)replayTmp->cursors[i + 1] - (int)replayTmp->data->stages[i + 1];
                    }
                }
                DebugPrint("%s write ...\n", path);
                hX.score = g_GameManager.guiScore;
                slowdown2 = (g_App.achievedFrames / g_App.expectedFrames - 0.5f) * 2.0f;
                if (slowdown2 < 0.0f)
                {
                    slowdown2 = 0.0f;
                }
                else if (slowdown2 >= 1.0f)
                {
                    slowdown2 = 1.0f;
                }
                hX.slowdown = (1.0f - slowdown2) * 100.0f;
                hX.slowdown2 = hX.slowdown + 1.12f;
                *(float *)&hX.stages[0] = hX.slowdown + 2.34f;
                replayTmp->data->stages[g_GameManager.stage]->score = g_GameManager.score;
                strcpy(hX.name, name);
                _strdate(hX.date);
                hX.key = g_Random.GetRandomU16InRange(128) + 64;
                hX.unk_0f = g_Random.GetRandomU16InRange(256);
                hX.unk_0c[0] = g_Random.GetRandomU16InRange(256);
                hX.unk_0c[1] = g_Random.GetRandomU16InRange(256);
                bpCur = &hX.key;
                checksum = 0x3f000318;
                for (i = 0; i < sizeof(ReplayHeader) - 0xe; i++, bpCur++)
                {
                    checksum += *bpCur;
                }
                for (i = 0; i < 7; i++)
                {
                    if (replayTmp->data->stages[i + 1] != NULL)
                    {
                        bpCur = (BYTE *)replayTmp->data->stages[i + 1];
                        for (j1 = 0; j1 < (int)replayTmp->cursors[i + 1] - (int)replayTmp->data->stages[i + 1]; j1++, bpCur++)
                        {
                            checksum += *bpCur;
                        }
                    }
                }
                hX.checksum = checksum;
                dp2 = &hX.unk_0f;
                kbVal = hX.key;
                for (i = 0; i < sizeof(ReplayHeader) - 0xf; i++, dp2++)
                {
                    *dp2 += kbVal;
                    kbVal += 7;
                }
                for (i = 0; i < 7; i++)
                {
                    if (replayTmp->data->stages[i + 1] != NULL)
                    {
                        dp2 = (BYTE *)replayTmp->data->stages[i + 1];
                        for (k1 = 0; k1 < (int)replayTmp->cursors[i + 1] - (int)replayTmp->data->stages[i + 1]; k1++, dp2++)
                        {
                            *dp2 += kbVal;
                            kbVal += 7;
                        }
                    }
                }
                f2 = fopen(path, "wb");
                fwrite(&hX, sizeof(ReplayHeader), 1, f2);
                for (i = 0; i < 7; i++)
                {
                    if (replayTmp->data->stages[i + 1] != NULL)
                    {
                        fwrite(replayTmp->data->stages[i + 1], 1,
                               (int)replayTmp->cursors[i + 1] - (int)replayTmp->data->stages[i + 1], f2);
                    }
                }
                fclose(f2);
            }
            for (i = 0; i < 7; i++)
            {
                if (g_ReplayManager->data->stages[i + 1] != NULL)
                {
                    DebugPrint("Replay Size %d\n", (int)replayTmp->cursors[i + 1] - (int)replayTmp->data->stages[i + 1]);
                    MemFree(g_ReplayManager->data->stages[i + 1]);
                }
            }
        }
        g_Chain.Remove(g_ReplayManager->calcChain);
    }
}
