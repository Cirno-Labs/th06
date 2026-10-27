// cflags: /Os
// Part of the /Os file starting at 0x42333b.

#include "musicscreen.h"

int MusicScreen::RegisterChain()
{
    static MusicScreen g_MusicScreen;
    MusicScreen *screen = &g_MusicScreen;

    memset(screen, 0, sizeof(MusicScreen));
    screen->calcChain = g_Chain.CreateElem((ChainCallback)MusicScreen::OnUpdate);
    screen->calcChain->arg = screen;
    screen->calcChain->addedCallback = (ChainCallback)MusicScreen::AddedCallback;
    screen->calcChain->deletedCallback = (ChainCallback)MusicScreen::DeletedCallback;
    if (g_Chain.AddToCalcChain(screen->calcChain, 2) != 0)
    {
        return -1;
    }
    screen->drawChain = g_Chain.CreateElem((ChainCallback)MusicScreen::OnDraw);
    screen->drawChain->arg = screen;
    g_Chain.AddToDrawChain(screen->drawChain, 0);
    return 0;
}

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
                this->comment[i].flags.visible = 1;
                g_SpriteEngine->RenderText(&this->comment[i], 0xffe0c0, 0x300000, line);
            }
            else
            {
                this->comment[i].flags.visible = 0;
            }
            this->comment[i].pos.x = (i % 2) * 248.0f + 96.0f;
            this->comment[i].pos.y = 320.0f + (i / 2) * 16;
            this->comment[i].pos.z = 0.0f;
            this->comment[i].flags.unk8 = 3;
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
    g_SpriteEngine->Step(&screen->unk_0024[0]);
    return 1;
}

int MusicScreen::OnDraw(MusicScreen *screen)
{
    char arrow[2] = {0x7f, 0};
    D3DXVECTOR3 pos;
    int i;

    g_SpriteEngine->ClearTexture();
    g_SpriteEngine->Func435300(0, 0, 0, 0, 0);
    g_SpriteEngine->Draw(&screen->unk_0024[0]);
    for (i = screen->listTop; i < screen->listTop + 10; i++)
    {
        if (screen->cursor != i)
        {
            screen->titles[i].color = 0xe0808080;
            g_AsciiText.color = 0xe0808080;
        }
        else
        {
            screen->titles[i].color = 0xffffffff;
            g_AsciiText.color = 0xffffffff;
        }
        screen->titles[i].pos.x = 93.0f;
        screen->titles[i].pos.y = 104.0f + (i + 1 - screen->listTop) * 18 - 20.0f;
        screen->titles[i].pos.z = 0.0f;
        g_SpriteEngine->Draw(&screen->titles[i]);
        // The number left of the title, and the arrow left of that.
        pos = screen->titles[i].pos;
        pos.x -= 60.0f;
        if (screen->cursor == i)
        {
            g_AsciiText.AddString(&pos, arrow);
        }
        pos.x += 15.0f;
        g_AsciiText.AddFormat(&pos, "%2d.", i + 1);
    }
    // Has no effect, but the original increments here.
    i++;
    for (i = 0; i < 0x10; i++)
    {
        g_SpriteEngine->Draw(&screen->comment[i]);
    }
    g_AsciiText.color = 0xffffffff;
    return 1;
}

int MusicScreen::DeletedCallback(MusicScreen *screen)
{
    delete screen->tunes;
    screen->tunes = NULL;
    g_SpriteEngine->ReleaseSurface(0);
    g_SpriteEngine->ReleaseAnm(0x29);
    g_SpriteEngine->ReleaseAnm(0x2a);
    g_SpriteEngine->ReleaseAnm(0x2b);
    g_Chain.Remove(screen->drawChain);
    screen->drawChain = NULL;
    return 0;
}

// musiccmt.txt lists the tunes. Each starts with a line "@path", then
// its title on the next line and up to 8 lines of comment.
int MusicScreen::AddedCallback(MusicScreen *screen)
{
    int i;
    int row;
    char *rp;
    int k;
    char *data;
    char str[0x40];

    if (g_SpriteEngine->LoadSurface(0, "data/result/music.jpg") != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x29, "data/music00.anm", 0x100) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x2a, "data/music01.anm", 0x101) != 0)
    {
        return -1;
    }
    if (g_SpriteEngine->LoadAnm(0x2b, "data/music02.anm", 0x111) != 0)
    {
        return -1;
    }
    g_SpriteEngine->StartScript(&screen->unk_0024[0], 0x100);
    screen->stateFrames = 0;
    data = rp = (char *)Func41e290("data/musiccmt.txt", 0);
    if (rp == NULL)
    {
        return -1;
    }
    screen->tunes = new MusicTune[0x20];
    i = -1;
    while (rp - data < g_FileSize)
    {
        if (*rp == '@')
        {
            rp++;
            i++;
            k = 0;
            while (*rp != '\n' && *rp != '\r')
            {
                screen->tunes[i].path[k] = *rp;
                rp++;
                k++;
                if (rp - data >= g_FileSize)
                {
                    goto done;
                }
            }
            while (*rp == '\n' || *rp == '\r')
            {
                rp++;
                if (rp - data >= g_FileSize)
                {
                    goto done;
                }
            }
            k = 0;
            while (*rp != '\n' && *rp != '\r')
            {
                screen->tunes[i].title[k] = *rp;
                rp++;
                k++;
                if (rp - data >= g_FileSize)
                {
                    goto done;
                }
            }
            // Never true, so the row break after the title is skipped
            // as an empty comment row.
            while (*rp == '\n' && *rp == '\r')
            {
                rp++;
                if (rp - data >= g_FileSize)
                {
                    goto done;
                }
            }
            for (row = 0; row < 8; row++)
            {
                if (*rp == '@')
                {
                    break;
                }
                memset(screen->tunes[i].comment[row], 0, sizeof(screen->tunes[i].comment[row]));
                k = 0;
                while (*rp != '\n' && *rp != '\r')
                {
                    screen->tunes[i].comment[row][k] = *rp;
                    rp++;
                    k++;
                    if (rp - data >= g_FileSize)
                    {
                        goto done;
                    }
                }
                while (*rp == '\n' || *rp == '\r')
                {
                    rp++;
                    if (rp - data >= g_FileSize)
                    {
                        goto done;
                    }
                }
            }
        }
        else
        {
            rp++;
        }
    }

done:
    screen->tuneCount = i + 1;
    for (i = 0; i < screen->tuneCount; i++)
    {
        g_SpriteEngine->InitSprite(&screen->titles[i], i + 0x101);
        g_SpriteEngine->RenderText(&screen->titles[i], 0xc0e0ff, 0x302080, screen->tunes[i].title);
        screen->titles[i].pos.x = 93.0f;
        screen->titles[i].pos.y = 104.0f + (i + 1) * 18 - 20.0f;
        screen->titles[i].pos.z = 0.0f;
        screen->titles[i].flags.unk8 = 3;
    }
    // The comment of the first i, as UpdateList shows it.
    for (i = 0; i < 0x10; i++)
    {
        g_SpriteEngine->InitSprite(&screen->comment[i], i + 0x708);
        memset(str, 0, sizeof(str));
        if (i % 2 == 0 || strlen(screen->tunes[screen->playing].comment[i / 2]) > 0x20)
        {
            memcpy(str, &screen->tunes->comment[i / 2][i % 2 * 0x20], 0x20);
        }
        if (str[0] != '\0')
        {
            screen->comment[i].flags.visible = 1;
            g_SpriteEngine->RenderText(&screen->comment[i], 0xffe0c0, 0x300000, str);
        }
        else
        {
            screen->comment[i].flags.visible = 0;
        }
        screen->comment[i].pos.x = (i % 2) * 248.0f + 96.0f;
        screen->comment[i].pos.y = 320.0f + (i / 2) * 16;
        screen->comment[i].pos.z = 0.0f;
        screen->comment[i].flags.unk8 = 3;
    }
    free(data);
    return 0;
}
