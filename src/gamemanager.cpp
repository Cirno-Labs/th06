// cflags: /Os
// The /Os file with GameManager, from 0x41b1a4 at least.

#include "gamemanager.h"

int GameManager::OnDraw(GameManager *mgr)
{
    if (mgr->unk_181f)
    {
        mgr->unk_181f = 2;
    }
    return 1;
}
