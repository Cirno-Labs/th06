// The file with ReplayManager, around 0x42a140.

#include "replay.h"
#include "gamemanager.h"

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
