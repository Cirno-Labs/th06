// The file with ReplayManager, around 0x42a140.

#include "replay.h"
#include "gamemanager.h"
#include "unsorted.h"

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

int ReplayManager::PlayOnDraw(ReplayManager *replay)
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
    g_Chain.Remove(replay->calcChain);
    replay->calcChain = NULL;
    if (replay->drawChain != NULL)
    {
        g_Chain.Remove(replay->drawChain);
        replay->drawChain = NULL;
    }
    MemFree(g_ReplayManager->data);
    delete g_ReplayManager;
    g_ReplayManager = NULL;
    g_ReplayManager = NULL;
    return 0;
}
