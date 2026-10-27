// cflags: /Os
// Part of the /Os file starting at 0x42333b.

#include "musicscreen.h"

int MusicScreen::UpdateIntro()
{
    if (this->stateFrames >= 8)
    {
        this->state = 1;
    }
    return 0;
}

int MusicScreen::UpdateList()
{
    int prevTop = this->listTop;
    char line[0x40];
    int i;

    if (WAS_PRESSED(0x10))
    {
        this->cursor--;
        if (this->cursor < 0)
        {
            this->cursor = this->tuneCount - 1;
            this->listTop = this->tuneCount - 10;
        }
        else if (this->listTop > this->cursor)
        {
            this->listTop = this->cursor;
        }
    }
    if (WAS_PRESSED(0x20))
    {
        this->cursor++;
        if (this->cursor >= this->tuneCount)
        {
            this->cursor = 0;
            this->listTop = 0;
        }
        else if (this->listTop <= this->cursor - 10)
        {
            this->listTop = this->cursor - 9;
        }
    }
    if (WAS_PRESSED(0x1001))
    {
        this->playing = this->cursor;
        g_Unknown6c6d18.PlayMusic(this->tunes[this->playing].path);
        // Each comment line takes two sprites, one per half; the second
        // only when the line runs past the first half.
        for (i = 0; i < 0x10; i++)
        {
            memset(line, 0, sizeof(line));
            if (i % 2 == 0 || strlen(this->tunes[this->playing].comment[i / 2]) > 0x20)
            {
                memcpy(line, &this->tunes[this->playing].comment[i / 2][i % 2 * 0x20], 0x20);
            }
            if (line[0] != '\0')
            {
                this->comment[i].flags |= 2;
                g_SpriteEngine->RenderText(&this->comment[i], 0xffe0c0, 0x300000, line);
            }
            else
            {
                this->comment[i].flags &= ~2;
            }
            this->comment[i].pos.x = (i % 2) * 248.0f + 96.0f;
            this->comment[i].pos.y = 320.0f + (i / 2) * 16;
            this->comment[i].pos.z = 0.0f;
            this->comment[i].flags |= 0x300;
        }
    }
    if (WAS_PRESSED(0xa))
    {
        g_Unknown6c6d18.scene = 1;
        return 1;
    }
    return 0;
}

int MusicScreen::OnUpdate(MusicScreen *screen)
{
    int oldState = screen->state;

again:
    switch (screen->state)
    {
    case 0:
        if (screen->UpdateIntro() == 0)
        {
            break;
        }
        // The state changed: run the new one this frame.
        goto again;
    case 1:
        if (screen->UpdateList() != 0)
        {
            return 0;
        }
        break;
    }
    if (oldState != screen->state)
    {
        screen->stateFrames = 0;
    }
    else
    {
        screen->stateFrames++;
    }
    g_SpriteEngine->Step(&screen->unk_0024);
    return 1;
}
