// cflags: /Os
// The file starting at 0x42333b, up to 0x425e03. Built with /Os: its
// functions are laid out without alignment.

#include "timer.h"
#include "unsorted.h"

void Timer::Initialize()
{
    this->current = 0;
    this->previous = -1;
    this->subFrame = 0.0f;
}

void Timer::Add(int frames)
{
    if (g_Unknown6c6d18.gameSpeed > 0.99f)
    {
        this->current += frames;
    }
    else if (frames < 0)
    {
        this->Sub(-frames);
    }
    else
    {
        this->previous = this->current;
        this->subFrame += g_Unknown6c6d18.frameAdvance * frames;
        while (this->subFrame >= 1.0f)
        {
            this->current++;
            this->subFrame -= 1.0f;
        }
    }
}

void Timer::Sub(int frames)
{
    if (g_Unknown6c6d18.gameSpeed > 0.99f)
    {
        this->current -= frames;
    }
    else if (frames < 0)
    {
        this->Add(-frames);
    }
    else
    {
        this->previous = this->current;
        this->subFrame -= g_Unknown6c6d18.frameAdvance * frames;
        while (this->subFrame < 0.0f)
        {
            this->current--;
            this->subFrame += 1.0f;
        }
    }
}

void Unknown6c6d18::TickTimer(int *frames, float *subFrame)
{
    if (this->gameSpeed <= 0.99f)
    {
        *subFrame += this->frameAdvance;
        if (*subFrame >= 1.0f)
        {
            (*frames)++;
            *subFrame -= 1.0f;
        }
    }
    else
    {
        (*frames)++;
    }
}

int Unknown6c6d18::LoadMidi(int slot, char *path)
{
    if (g_Unknown6c6d18.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            g_Unknown6c6d18.midi->LoadFile(slot, path);
        }
        return 0;
    }
    return 1;
}

int Unknown6c6d18::PlayMidi(int slot)
{
    if (g_Unknown6c6d18.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            MidiPlayer *midi = g_Unknown6c6d18.midi;
            midi->Stop();
            midi->ParseFile(slot);
            midi->Play();
        }
        return 0;
    }
    return 1;
}

int Unknown6c6d18::StopMusic()
{
    if (g_Unknown6c6d18.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            g_Unknown6c6d18.midi->Stop();
        }
    }
    else if (g_Unknown6c6d18.musicMode == 1)
    {
        g_Unknown6d3f50.StopBgm();
    }
    else
    {
        return -1;
    }
    return 0;
}

int Unknown6c6d18::FadeOutMusic(float seconds)
{
    // Never used, but the original's frame has room for three dwords
    // besides `this`.
    int unused1;
    int unused2;
    int unused3;

    if (g_Unknown6c6d18.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            g_Unknown6c6d18.midi->FadeOut(1000.0f * seconds);
        }
    }
    else if (g_Unknown6c6d18.musicMode == 1)
    {
        // The wave fade counts frames, so it is stretched while the
        // game runs slow.
        if (this->frameAdvance == 0.0f)
        {
            g_Unknown6d3f50.FadeOutBgm(seconds);
        }
        else if (this->frameAdvance > 1.0f)
        {
            g_Unknown6d3f50.FadeOutBgm(seconds);
        }
        else
        {
            g_Unknown6d3f50.FadeOutBgm(seconds / this->frameAdvance);
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

void Unknown6d3f50::FadeOutBgm(float seconds)
{
    if (this->bgm != NULL)
    {
        CStreamingSound *bgm = this->bgm;
        bgm->fading = 1;
        bgm->fadeFrames = seconds * 60.0f;
        bgm->fadeTotal = bgm->fadeFrames;
    }
}
