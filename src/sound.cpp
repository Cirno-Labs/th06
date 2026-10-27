// The file with Unknown6d3f50, the sound effects and wave music, from
// 0x430270.

#include "unsorted.h"

void Unknown6d3f50::QueueSound(int sound, int unused)
{
    int i;
    int unk;

    unk = g_SoundInfos[sound].unk_6;
    for (i = 0; i < 3; i++)
    {
        if (this->queue[i] < 0)
        {
            break;
        }
        if (this->queue[i] == sound)
        {
            return;
        }
    }
    if (i >= 3)
    {
        return;
    }
    this->queue[i] = sound;
    this->unk_408[sound] = unk;
}
