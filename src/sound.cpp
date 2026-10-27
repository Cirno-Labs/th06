// The file with Unknown6d3f50, the sound effects and wave music, from
// 0x430270.

#include "unsorted.h"

// Finds chunk `name` among the RIFF chunks at `data`, `size` bytes of
// them. Returns its contents and sets *chunkSize, or returns NULL.
char *FindRiffChunk(char *data, char *name, int *chunkSize, unsigned int size)
{
    while (size > 0)
    {
        *chunkSize = *(int *)(data + 4);
        if (strncmp(data, name, 4) == 0)
        {
            return data + 8;
        }
        size -= *chunkSize + 8;
        data += *chunkSize + 8;
    }
    return NULL;
}

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

void Unknown6d3f50::PlayQueuedSounds()
{
    int i;
    int sound;

    if (!this->unk_610)
    {
        return;
    }
    if (!g_Unknown6c6d18.cfg.playSounds)
    {
        return;
    }
    for (i = 0; i < 3; i++)
    {
        if (this->queue[i] < 0)
        {
            break;
        }
        sound = this->queue[i];
        this->queue[i] = -1;
        if (this->sounds[sound] == NULL)
        {
            continue;
        }
        this->sounds[sound]->Stop();
        this->sounds[sound]->SetCurrentPosition(0);
        this->sounds[sound]->Play(0, 0, 0);
    }
}
