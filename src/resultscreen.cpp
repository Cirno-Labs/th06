// cflags: /Os -/Oi
// The /Os file from 0x42b0d9 to 0x42f6c9, with the result screen: the
// score file and its sorted lists.

#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ascii.h"
#include "features.h"
#include "gamemanager.h"
#include "resultscreen.h"
#include "unsorted.h"

ScoreDat *LoadScoreDat(char *path)
{
    ScoreDat *dat;
    BYTE *p1;
    int remaining;
    WORD checksumVal;
    BYTE key;
    int n;
    ScoreChunk *chunk0;
    int rest0;

    dat = (ScoreDat *)LoadFileData(path, 1);
    if (dat == NULL)
    {
    fail:
        dat = (ScoreDat *)MemAlloc(sizeof(ScoreDat));
        dat->dataOffset = sizeof(ScoreDat);
        dat->fileSize = sizeof(ScoreDat);
        goto done;
    }
    if (g_FileSize < sizeof(ScoreDat))
    {
        free(dat);
        goto fail;
    }
    remaining = g_FileSize - 2;
    checksumVal = 0;
    key = 0;
    n = 0;
    p1 = (BYTE *)dat + 1;
    while (remaining > 0)
    {
        key += *p1;
        key = ((key & 0xe0) >> 5) | ((key & 0x1f) << 3);
        p1[1] ^= key;
        if (n >= 2)
        {
            checksumVal += p1[1];
        }
        p1++;
        remaining--;
        n++;
    }
    if (dat->checksum != checksumVal)
    {
        free(dat);
        goto fail;
    }
    rest0 = dat->fileSize;
    chunk0 = (ScoreChunk *)((BYTE *)dat + dat->dataOffset);
    rest0 -= dat->dataOffset;
    while (rest0 > 0)
    {
        if (chunk0->magic == 0x4b364854)
        {
            break;
        }
        chunk0 = (ScoreChunk *)((BYTE *)chunk0 + chunk0->size);
        rest0 -= chunk0->size;
    }
    if (rest0 <= 0)
    {
        free(dat);
        goto fail;
    }
done:
    dat->scores = (ListNode *)MemAlloc(sizeof(ListNode));
    dat->scores->next = NULL;
    dat->scores->data = NULL;
    dat->scores->prev = NULL;
    return dat;
}

DWORD LoadHighScores(void *scoreDat, ListNode *list, int shot, int difficulty)
{
    Hscr *chunk;
    int left;
    ScoreDat *dat1 = (ScoreDat *)scoreDat;

    if (list == NULL)
    {
        FreeListNodes(dat1->scores);
        dat1->scores->next = NULL;
        dat1->scores->data = NULL;
        dat1->scores->prev = NULL;
    }
    left = dat1->fileSize;
    chunk = (Hscr *)((BYTE *)dat1 + dat1->dataOffset);
    left -= dat1->dataOffset;
    while (left > 0)
    {
        if (chunk->magic == 0x52435348 && chunk->version == 0x10 && chunk->shot == shot &&
            chunk->difficulty == difficulty)
        {
            if (list != NULL)
            {
                InsertScore(list, chunk);
            }
            else
            {
                InsertScore(dat1->scores, chunk);
            }
        }
        left -= chunk->size;
        chunk = (Hscr *)((BYTE *)chunk + chunk->size);
    }
    return dat1->scores->next != NULL
               ? (dat1->scores->next->data->score > 1000000 ? dat1->scores->next->data->score : 1000000)
               : 1000000;
}

int InsertScore(ListNode *head, Hscr *entry)
{
    int index = 0;
    ListNode *next;

    while (head->next != NULL)
    {
        if (head->next->data != NULL && head->next->data->score <= entry->score)
        {
            break;
        }
        head = head->next;
        index++;
    }
    next = head->next;
    head->next = (ListNode *)MemAlloc(sizeof(ListNode));
    head->next->prev = head;
    head = head->next;
    head->data = entry;
    head->next = next;
    return index;
}

void FreeListNodes(ListNode *head)
{
    ListNode *next;

    head = head->next;
    while (head != NULL)
    {
        next = head->next;
        free(head);
        head = next;
    }
}

int LoadSpellCards(void *scoreDat, Catk *catks)
{
    Catk *chunk;
    int left;
    ScoreDat *dat1 = (ScoreDat *)scoreDat;

    if (catks == NULL)
    {
        return -1;
    }
    chunk = (Catk *)((BYTE *)dat1 + dat1->dataOffset);
    left = dat1->fileSize - dat1->dataOffset;
    while (left > 0)
    {
        if (chunk->magic == 0x4b544143 && chunk->version == 0x10)
        {
            if (chunk->index >= 0x40)
            {
                break;
            }
            catks[chunk->index] = *chunk;
        }
        left -= chunk->size;
        chunk = (Catk *)((BYTE *)chunk + chunk->size);
    }
    return 0;
}

int LoadClears(void *scoreDat, Clrd *clrds)
{
    Clrd *p;
    int i;
    int left;
    int k;
    ScoreDat *dat1 = (ScoreDat *)scoreDat;

    if (clrds == NULL)
    {
        return -1;
    }
    for (i = 0; i < 4; i++)
    {
        memset(&clrds[i], 0, sizeof(Clrd));
        clrds[i].magic = 0x44524c43;
        clrds[i].size2 = sizeof(Clrd);
        clrds[i].size = sizeof(Clrd);
        clrds[i].version = 0x10;
        clrds[i].shot = i;
        for (k = 0; k < 5; k++)
        {
            clrds[i].stagesClearedNoContinue[k] = 1;
            clrds[i].stagesCleared[k] = 1;
        }
    }
    p = (Clrd *)((BYTE *)dat1 + dat1->dataOffset);
    left = dat1->fileSize - dat1->dataOffset;
    while (left > 0)
    {
        if (p->magic == 0x44524c43 && p->version == 0x10)
        {
            if (p->shot >= 4)
            {
                break;
            }
            clrds[p->shot] = *p;
        }
        left -= p->size;
        p = (Clrd *)((BYTE *)p + p->size);
    }
    return 0;
}

int LoadPracticeScores(void *scoreDat, Pscr *pscrs)
{
    Pscr *p;
    Pscr *chunk;
    int i3;
    int j0;
    int rest3;
    int k1;
    ScoreDat *dat10 = (ScoreDat *)scoreDat;

    if (pscrs == NULL)
    {
        return -1;
    }
    p = pscrs;
    for (i3 = 0; i3 < 4; i3++)
    {
        for (j0 = 0; j0 < 6; j0++)
        {
            for (k1 = 0; k1 < 4; k1++, p++)
            {
                memset(p, 0, sizeof(Pscr));
                p->magic = 0x52435350;
                p->size2 = sizeof(Pscr);
                p->size = sizeof(Pscr);
                p->version = 0x10;
                p->shot = i3;
                p->difficulty = k1;
                p->stage = j0;
            }
        }
    }
    chunk = (Pscr *)((BYTE *)dat10 + dat10->dataOffset);
    rest3 = dat10->fileSize - dat10->dataOffset;
    while (rest3 > 0)
    {
        if (chunk->magic == 0x52435350 && chunk->version == 0x10)
        {
            p = chunk;
            if (p->shot >= 4 || p->difficulty >= 5 || p->stage >= 7)
            {
                break;
            }
            pscrs[p->shot * 6 * 4 + p->stage * 4 + p->difficulty] = *p;
        }
        rest3 -= chunk->size;
        chunk = (Pscr *)((BYTE *)chunk + chunk->size);
    }
    return 0;
}

void ReleaseScoreDat(ScoreDat *dat)
{
    FreeListNodes(dat->scores);
    MemFree(dat->scores);
    free(dat);
}

void SaveScoreDat(ResultScreen *screen)
{
    ScoreDat *header;
    BYTE *p24;
    BYTE key26;
    int m;
    BYTE prev3;
    int l2;
    int j;
    Pscr *pscr0;
    Catk *catk1;
    Clrd *clrd26;
    int s14;
    ListNode *node;
    int size0 = 0;
    BYTE *buf;
    int n;
    int i;

    buf = (BYTE *)MemAlloc(0xa0000);
    memcpy(buf + size0, screen->scoreDat, sizeof(ScoreDat));
    size0 += sizeof(ScoreDat);
    screen->th6k.magic = 0x4b364854;
    screen->th6k.size2 = sizeof(Th6k);
    screen->th6k.size = sizeof(Th6k);
    screen->th6k.version = 0x10;
    memcpy(buf + size0, &screen->th6k, sizeof(Th6k));
    size0 += sizeof(Th6k);
    for (i = 0; i < 5; i++)
    {
        for (s14 = 0; s14 < 4; s14++)
        {
            node = screen->scores[i][s14].next;
            n = 0;
            while (node != NULL)
            {
                if (node->data->magic == 0x52435348)
                {
                    node->data->shot = s14;
                    node->data->difficulty = i;
                    node->data->size2 = 0x1c;
                    node->data->size = 0x1c;
                    node->data->version = 0x10;
                    node->data->isNew = 0;
                    memcpy(buf + size0, node->data, 0x1c);
                    size0 += 0x1c;
                }
                node = node->next;
                n++;
                if (n >= 10)
                {
                    break;
                }
            }
        }
    }
    clrd26 = g_GameManager.clrds;
    for (i = 0; i < 4; i++, clrd26++)
    {
        clrd26->magic = 0x44524c43;
        clrd26->size2 = sizeof(Clrd);
        clrd26->size = sizeof(Clrd);
        clrd26->version = 0x10;
        memcpy(buf + size0, clrd26, sizeof(Clrd));
        size0 += sizeof(Clrd);
    }
    catk1 = g_GameManager.catks;
    for (i = 0; i < 0x40; i++, catk1++)
    {
        if (catk1->magic == 0x4b544143)
        {
            catk1->index = i;
            catk1->size2 = sizeof(Catk);
            catk1->size = sizeof(Catk);
            catk1->version = 0x10;
            memcpy(buf + size0, catk1, sizeof(Catk));
            size0 += sizeof(Catk);
        }
    }
    pscr0 = &g_GameManager.pscrs[0][0][0];
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 6; j++)
        {
            for (l2 = 0; l2 < 4; l2++, pscr0++)
            {
                if (pscr0->score != 0)
                {
                    memcpy(buf + size0, pscr0, sizeof(Pscr));
                    size0 += sizeof(Pscr);
                }
            }
        }
    }
    header = (ScoreDat *)buf;
    header->dataOffset = sizeof(ScoreDat);
    header->fileSize = size0;
    header->checksum = 0;
    header->key = g_Rng.GetRandomU16InRange(256);
    header->unk_06 = g_Rng.GetRandomU16InRange(256);
    header->version = 0x10;
    for (m = 4; m < size0; m++)
    {
        header->checksum += buf[m];
    }
    key26 = 0;
    prev3 = 0;
    p24 = (BYTE *)header + 1;
    m = size0;
    m -= 2;
    key26 = *p24;
    while (m > 0)
    {
        prev3 = p24[1];
        key26 = ((key26 & 0xe0) >> 5) | ((key26 & 0x1f) << 3);
        p24[1] ^= key26;
        key26 += prev3;
        p24++;
        m--;
    }
    WriteToFile("score.dat", buf, size0);
    free(buf);
}

int ResultScreen::AddScore(Hscr *entry, int difficulty, int shot)
{
    return InsertScore(&this->scores[difficulty][shot], entry);
}

void ResultScreen::ReleaseScores(int difficulty, int shot)
{
    FreeListNodes(&this->scores[difficulty][shot]);
}

int ResultScreen::HandleNameEntry()
{
    Sprite *sprite;
    int i;

    if (this->frame == 0)
    {
        this->character = g_GameManager.character;
        this->difficulty = g_GameManager.difficulty;
        sprite = this->sprites;
        for (i = 0; i < 0x26; i++, sprite++)
        {
            sprite->pendingInterrupt = this->difficulty + 3;
        }
        g_SpriteEngine->RenderTextCentered(&this->textSprites[0], 0xffffff, 0, g_ShotNames[this->character * 2]);
        if (g_GameManager.shotType != 0)
        {
            this->textSprites[0].color = 0x80ffffff;
        }
        g_SpriteEngine->RenderTextCentered(&this->textSprites[1], 0xffffff, 0, g_ShotNames[this->character * 2 + 1]);
        if (g_GameManager.shotType != 1)
        {
            this->textSprites[1].color = 0x80ffffff;
        }
        this->entry.shot = this->character * 2 + g_GameManager.shotType;
        this->entry.difficulty = this->difficulty;
        this->entry.score = g_GameManager.score;
        this->entry.version = 0x10;
        this->entry.magic = *(DWORD *)"HSCR";
        if (g_GameManager.isGameCleared == 0)
        {
            this->entry.stage = g_GameManager.stage;
        }
        else
        {
            this->entry.stage = 99;
        }
        this->entry.isNew = 1;
        strcpy(this->entry.name, "        ");
        if (this->AddScore(&this->entry, this->difficulty, this->character * 2 + g_GameManager.shotType) >= 10)
        {
            goto done;
        }
        this->cursor = 0;
        strcpy(this->name, "");
    }
    if (this->frame < 30)
    {
        return 0;
    }
    if (WAS_PRESSED_OR_HELD(0x10))
    {
        for (;;)
        {
            this->charIndex -= 16;
            if (this->charIndex < 0)
            {
                this->charIndex += 0x60;
            }
            if (g_NameAlphabet[this->charIndex] == ' ')
            {
                continue;
            }
            break;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
    }
    if (WAS_PRESSED_OR_HELD(0x20))
    {
        for (;;)
        {
            this->charIndex += 16;
            if (this->charIndex >= 0x60)
            {
                this->charIndex -= 0x60;
            }
            if (g_NameAlphabet[this->charIndex] == ' ')
            {
                continue;
            }
            break;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
    }
    if (WAS_PRESSED_OR_HELD(0x40))
    {
        for (;;)
        {
            this->charIndex--;
            if (this->charIndex % 16 == 15)
            {
                this->charIndex += 16;
            }
            if (this->charIndex < 0)
            {
                this->charIndex = 15;
            }
            if (g_NameAlphabet[this->charIndex] == ' ')
            {
                continue;
            }
            break;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
    }
    if (WAS_PRESSED_OR_HELD(0x80))
    {
        for (;;)
        {
            this->charIndex++;
            if (this->charIndex % 16 == 0)
            {
                this->charIndex -= 16;
            }
            if (g_NameAlphabet[this->charIndex] == ' ')
            {
                continue;
            }
            break;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
    }
    if (WAS_PRESSED_OR_HELD(0x1001))
    {
        int pos = this->cursor >= 8 ? 7 : this->cursor;

        if (this->charIndex < 0x5e)
        {
            this->entry.name[pos] = g_NameAlphabet[this->charIndex];
        }
        else if (this->charIndex == 0x5e)
        {
            this->entry.name[pos] = ' ';
        }
        else
        {
            goto end;
        }
        if (this->cursor < 8)
        {
            this->cursor++;
            if (this->cursor == 8)
            {
                this->charIndex = 0x5f;
            }
        }
        g_SoundPlayer.QueueSound(10, 0);
    }
    if (WAS_PRESSED_OR_HELD(0xa))
    {
        int pos2 = this->cursor >= 8 ? 7 : this->cursor;

        if (this->cursor > 0)
        {
            this->cursor--;
            this->entry.name[pos2] = ' ';
        }
        g_SoundPlayer.QueueSound(0xb, 0);
    }
    if (WAS_PRESSED(8))
    {
    end:
        g_SoundPlayer.QueueSound(0xb, 0);
    done:
        this->state = 0xf;
        this->frame = 0;
        sprite = this->sprites;
        for (i = 0; i < 0x26; i++, sprite++)
        {
            sprite->pendingInterrupt = 2;
        }
        strcpy(this->name, this->entry.name);
    }
    return 0;
}

int ResultScreen::HandleReplaySaving()
{
    Sprite *spr;
    int interrupt;
    int k;
    ReplayHeader *replay;
    char path[0x40];

    switch (this->state)
    {
    case 10:
        if (this->frame == 60)
        {
            if (g_GameManager.continuesUsed)
            {
                interrupt = 0xc;
            }
            else if (g_App.gameSpeed < 0.99f)
            {
                interrupt = 0xd;
            }
            else
            {
                interrupt = 9;
            }
            spr = &this->sprites[1];
            for (k = 0; k < 0x26; k++, spr++)
            {
                spr->pendingInterrupt = interrupt;
            }
            if (interrupt != 9)
            {
                this->state = 0xb;
            }
            this->cursor = 0;
        }
        spr = &this->sprites[0x10];
        if (this->cursor == 0)
        {
            spr->color = spr->color & 0xff000000 | 0xff6060;
            spr[1].color = spr[1].color & 0xff000000 | 0x606060;
        }
        else
        {
            spr->color = spr->color & 0xff000000 | 0x606060;
            spr[1].color = spr[1].color & 0xff000000 | 0xff6060;
        }
        if (this->frame < 80)
        {
            return 0;
        }
        MoveCursorHorizontal(this, 2);
        if (WAS_PRESSED(0xa) || WAS_PRESSED(8))
        {
            goto cancel;
        }
        if (WAS_PRESSED(0x1001))
        {
            if (this->cursor == 0)
            {
            chooseSlot:
                g_SoundPlayer.QueueSound(10, 0);
                this->state = 0xc;
                spr = this->sprites;
                for (k = 0; k < 0x26; k++, spr++)
                {
                    spr->pendingInterrupt = 10;
                }
                this->frame = 0;
                goto listSlots;
            }
        cancel:
            this->frame = 0;
            g_SoundPlayer.QueueSound(0xb, 0);
            this->state = 2;
            spr = this->sprites;
            for (k = 0; k < 0x26; k++, spr++)
            {
                spr->pendingInterrupt = 2;
            }
        }
        break;
    case 0xb:
        if (this->frame < 20)
        {
            return 0;
        }
        if (WAS_PRESSED(0x1001) || WAS_PRESSED(0xa))
        {
            this->frame = 0;
            g_SoundPlayer.QueueSound(0xb, 0);
            this->state = 2;
            spr = this->sprites;
            for (k = 0; k < 0x26; k++, spr++)
            {
                spr->pendingInterrupt = 2;
            }
        }
        break;
    case 0xc:
    listSlots:
        if (this->frame == 0)
        {
            _mkdir("replay");
            for (k = 0; k < 0xf; k++)
            {
                sprintf(path, "./replay/th6_%.2d.rpy", k + 1);
                replay = (ReplayHeader *)LoadFileData(path, 1);
                if (replay == NULL)
                {
                    continue;
                }
                if (ValidateReplay(replay, g_FileSize) == 0)
                {
                    this->replays[k] = *replay;
                }
                free(replay);
            }
        }
        if (this->frame < 20)
        {
            return 0;
        }
        MoveCursorVertical(this, 0xf);
        this->replaySlot = this->cursor;
        if (WAS_PRESSED(0x1001))
        {
            g_SoundPlayer.QueueSound(10, 0);
            this->replaySlot = this->cursor;
            this->frame = 0;
            _strdate(this->replay.date);
            this->replay.score = g_GameManager.score;
            if (*(DWORD *)this->replays[this->cursor].magic != *(DWORD *)"T6RP" ||
                this->replays[this->cursor].version != 0x102)
            {
                spr = this->sprites;
                for (k = 0; k < 0x26; k++, spr++)
                {
                    spr->pendingInterrupt = 0xf;
                }
                spr = &this->sprites[this->replaySlot + 0x16];
                spr->pendingInterrupt = 0xe;
                this->state = 0xd;
            }
            else
            {
                spr = this->sprites;
                for (k = 0; k < 0x26; k++, spr++)
                {
                    spr->pendingInterrupt = 0xb;
                }
                spr = &this->sprites[this->replaySlot + 0x16];
                spr->pendingInterrupt = 0xe;
                this->state = 0xe;
            }
            this->cursor = 0;
            this->charIndex = 0;
        }
        if (WAS_PRESSED(0xa))
        {
            g_SoundPlayer.QueueSound(0xb, 0);
            this->state = 10;
            spr = this->sprites;
            for (k = 0; k < 0x26; k++, spr++)
            {
                spr->pendingInterrupt = 2;
            }
            this->frame = 0;
        }
        break;
    case 0xd:
        if (this->frame < 30)
        {
            return 0;
        }
        if (WAS_PRESSED_OR_HELD(0x10))
        {
            for (;;)
            {
                this->charIndex -= 16;
                if (this->charIndex < 0)
                {
                    this->charIndex += 0x60;
                }
                if (g_NameAlphabet[this->charIndex] == ' ')
                {
                    continue;
                }
                break;
            }
            g_SoundPlayer.QueueSound(0xc, 0);
        }
        if (WAS_PRESSED_OR_HELD(0x20))
        {
            for (;;)
            {
                this->charIndex += 16;
                if (this->charIndex >= 0x60)
                {
                    this->charIndex -= 0x60;
                }
                if (g_NameAlphabet[this->charIndex] == ' ')
                {
                    continue;
                }
                break;
            }
            g_SoundPlayer.QueueSound(0xc, 0);
        }
        if (WAS_PRESSED_OR_HELD(0x40))
        {
            for (;;)
            {
                this->charIndex--;
                if (this->charIndex % 16 == 15)
                {
                    this->charIndex += 16;
                }
                if (this->charIndex < 0)
                {
                    this->charIndex = 15;
                }
                if (g_NameAlphabet[this->charIndex] == ' ')
                {
                    continue;
                }
                break;
            }
            g_SoundPlayer.QueueSound(0xc, 0);
        }
        if (WAS_PRESSED_OR_HELD(0x80))
        {
            for (;;)
            {
                this->charIndex++;
                if (this->charIndex % 16 == 0)
                {
                    this->charIndex -= 16;
                }
                if (g_NameAlphabet[this->charIndex] == ' ')
                {
                    continue;
                }
                break;
            }
            g_SoundPlayer.QueueSound(0xc, 0);
        }
        if (WAS_PRESSED_OR_HELD(0x1001))
        {
            int pos = this->cursor >= 8 ? 7 : this->cursor;

            if (this->charIndex < 0x5e)
            {
                this->name[pos] = g_NameAlphabet[this->charIndex];
            }
            else if (this->charIndex == 0x5e)
            {
                this->name[pos] = ' ';
            }
            else
            {
                char replayPath[0x40];

                sprintf(replayPath, "./replay/th6_%.2d.rpy", this->replaySlot + 1);
                SaveReplay(replayPath, this->name);
                this->frame = 0;
                this->state = 2;
                spr = this->sprites;
                for (k = 0; k < 0x26; k++, spr++)
                {
                    spr->pendingInterrupt = 2;
                }
            }
            if (this->cursor < 8)
            {
                this->cursor++;
                if (this->cursor == 8)
                {
                    this->charIndex = 0x5f;
                }
            }
            g_SoundPlayer.QueueSound(10, 0);
        }
        if (WAS_PRESSED_OR_HELD(0xa))
        {
            int pos2 = this->cursor >= 8 ? 7 : this->cursor;

            if (this->cursor > 0)
            {
                this->cursor--;
                this->name[pos2] = ' ';
            }
            g_SoundPlayer.QueueSound(0xb, 0);
        }
        if (WAS_PRESSED(8))
        {
            goto chooseSlot;
        }
        break;
    case 0xe:
        spr = &this->sprites[0x10];
        if (this->cursor == 0)
        {
            spr->color = spr->color & 0xff000000 | 0xff6060;
            spr[1].color = spr[1].color & 0xff000000 | 0x606060;
        }
        else
        {
            spr->color = spr->color & 0xff000000 | 0x606060;
            spr[1].color = spr[1].color & 0xff000000 | 0xff6060;
        }
        if (this->frame < 20)
        {
            return 0;
        }
        MoveCursorHorizontal(this, 2);
        if (WAS_PRESSED(0xa) || WAS_PRESSED(8))
        {
            goto chooseSlot;
        }
        if (WAS_PRESSED(0x1001))
        {
            this->frame = 0;
            if (this->cursor == 0)
            {
                spr = this->sprites;
                for (k = 0; k < 0x26; k++, spr++)
                {
                    spr->pendingInterrupt = 0xf;
                }
                spr = &this->sprites[this->replaySlot + 0x16];
                spr->pendingInterrupt = 0xe;
                this->state = 0xd;
            }
            else
            {
                goto chooseSlot;
            }
        }
        break;
    }
    return 0;
}

void MoveCursorVertical(ResultScreen *screen, int count)
{
    if (WAS_PRESSED_OR_HELD(0x10))
    {
        screen->cursor--;
        if (screen->cursor < 0)
        {
            screen->cursor += count;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
    }
    if (WAS_PRESSED_OR_HELD(0x20))
    {
        screen->cursor++;
        if (screen->cursor >= count)
        {
            screen->cursor -= count;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
    }
}

int MoveCursorHorizontal(ResultScreen *screen, int count)
{
    if (WAS_PRESSED_OR_HELD(0x40))
    {
        screen->cursor--;
        if (screen->cursor < 0)
        {
            screen->cursor += count;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
        return 1;
    }
    if (WAS_PRESSED_OR_HELD(0x80))
    {
        screen->cursor++;
        if (screen->cursor >= count)
        {
            screen->cursor -= count;
        }
        g_SoundPlayer.QueueSound(0xc, 0);
        return 1;
    }
    return 0;
}

int ResultScreen::WaitAfterNameEntry()
{
    Sprite *sprite;

    switch (this->state)
    {
    case 0xf:
        if (this->frame <= 30)
        {
            sprite = &this->sprites[0x25];
            sprite->pendingInterrupt = 0x10;
        }
        if (this->frame >= 90 && WAS_PRESSED(0x1001))
        {
            sprite = &this->sprites[0x25];
            sprite->pendingInterrupt = 2;
            this->frame = 0;
            this->state = 0x10;
        }
        break;
    case 0x10:
        if (this->frame >= 30)
        {
            this->frame = 59;
            this->state = 10;
        }
        break;
    }
    return 0;
}

int ResultScreen::DrawStatistics()
{
#ifdef TH06_EXE
    static const float g_DifficultyRatings[5] = {
        -30.0f, -10.0f, 20.0f, 30.0f, 30.0f,
    };
#endif // TH06_EXE
    Sprite *spr;
    D3DXVECTOR3 cursorPos;
    float progress;
    float rating;
    float slowdown;

    switch (this->state)
    {
    case 0xf:
    case 0x10:
        spr = &this->sprites[0x25];
        {
            D3DCOLOR color = spr->color;

            g_AsciiText.color = color;
        }
        rating = 0.0f;
        progress = g_GameManager.difficulty < 4 ? g_GameManager.progressFrames / 89500.0f
                                                : g_GameManager.progressFrames / 39600.0f;
        cursorPos = spr->pos;
        cursorPos.x += 224.0f;
        cursorPos.y += 32.0f;
        g_AsciiText.AddFormat(&cursorPos, "%9d", g_GameManager.guiScore);
        if (g_GameManager.guiScore < 2000000)
        {
            rating -= 20.0f;
        }
        else if (g_GameManager.guiScore < 200000000)
        {
            rating += (g_GameManager.guiScore - 2000000) / 198000000.0f * 60.0f - 20.0f;
        }
        else
        {
            rating += 40.0f;
        }
        cursorPos.y += 22.0f;
        g_AsciiText.AddString(&cursorPos, g_DifficultyNames[g_GameManager.difficulty]);
        rating += g_DifficultyRatings[g_GameManager.difficulty];
        cursorPos.y += 22.0f;
        if (g_GameManager.difficulty == 0 || !g_GameManager.isGameCleared)
        {
            g_AsciiText.AddFormat(&cursorPos, "    %3.2f%%", progress * 100.0f);
            rating += progress * 70.0f;
        }
        else
        {
            g_AsciiText.AddFormat(&cursorPos, "      100%%");
            rating += 70.0f;
        }
        cursorPos.y += 22.0f;
        g_AsciiText.AddFormat(&cursorPos, "%9d", g_GameManager.continuesUsed);
        rating -= g_GameManager.continuesUsed * 10.0f;
        cursorPos.y += 22.0f;
        g_AsciiText.AddFormat(&cursorPos, "%9d", g_GameManager.deaths);
        rating -= g_GameManager.deaths * 5.0f - 10.0f;
        cursorPos.y += 22.0f;
        g_AsciiText.AddFormat(&cursorPos, "%9d", g_GameManager.bombsUsed);
        rating -= g_GameManager.bombsUsed * 2.0f - 10.0f;
        cursorPos.y += 22.0f;
        g_AsciiText.AddFormat(&cursorPos, "%9d", g_GameManager.spellCardsCaptured);
        rating += g_GameManager.spellCardsCaptured * g_SpellCardRatings[g_GameManager.difficulty];
        slowdown = (g_App.achievedFrames / g_App.expectedFrames - 0.5f) * 2.0f;
        if (slowdown < 0.0f)
        {
            slowdown = 0.0f;
        }
        else if (slowdown >= 1.0f)
        {
            slowdown = 1.0f;
        }
        slowdown = (1.0f - slowdown) * 100.0f;
        cursorPos.y += 22.0f;
        g_AsciiText.AddFormat(&cursorPos, "    %3.2f%%", slowdown);
        if (slowdown < 50.0f)
        {
            rating -= 70.0f * slowdown / 100.0f;
        }
        else
        {
            rating = -999.0f;
        }
        if (g_GameManager.pointItemsTotal < 800)
        {
            rating += 0.01f * g_GameManager.pointItemsTotal;
        }
        else
        {
            rating += 8.0f;
        }
        if (g_GameManager.grazeTotal < 5000)
        {
            rating += 0.0025f * g_GameManager.grazeTotal;
        }
        else
        {
            rating += 12.5f;
        }
        g_AsciiText.color = 0xffffffff;
        break;
    }
    return 0;
}

int ResultScreen::RegisterChain(int unk)
{
    ResultScreen *screen = (ResultScreen *)MemTrack(new ResultScreen);

    DebugPrint2("counat = %d\n", g_GameManager.progressFrames);
    screen->calcChain = g_Chain.CreateElem((ChainCallback)ResultScreen::OnUpdate);
    screen->calcChain->addedCallback = (ChainCallback)ResultScreen::AddedCallback;
    screen->calcChain->deletedCallback = (ChainCallback)ResultScreen::DeletedCallback;
    screen->calcChain->arg = screen;
    if (unk != 0)
    {
        if (!g_GameManager.isInPractice)
        {
            screen->state = 9;
        }
        else
        {
            screen->state = 0x11;
        }
    }
    if (g_Chain.AddToCalcChain(screen->calcChain, 0xd) != 0)
    {
        return -1;
    }
    screen->drawChain = g_Chain.CreateElem((ChainCallback)ResultScreen::OnDraw);
    screen->drawChain->arg = screen;
    g_Chain.AddToDrawChain(screen->drawChain, 0xc);
    return 0;
}

int ResultScreen::OnUpdate(ResultScreen *screen)
{
    int i;
    Sprite *sprite;
    int shot;
    int k;

    switch (screen->state)
    {
    case 0x11:
        g_App.scene = 1;
        return 0;
    case 0:
        if (screen->frame == 0)
        {
            sprite = screen->sprites;
            for (i = 0; i < 0x26; i++, sprite++)
            {
                sprite->pendingInterrupt = 1;
                sprite->flags.colorOp = 1;
                if (!g_App.cfg.flags.emulateColorAdd)
                {
                    sprite->color = sprite->color & 0xff000000;
                }
                else
                {
                    sprite->color = sprite->color;
                }
            }
            sprite = &screen->sprites[1];
            for (i = 0; i <= 6; i++, sprite++)
            {
                if (i == screen->cursor)
                {
                    if (!g_App.cfg.flags.emulateColorAdd)
                    {
                        sprite->color = 0xff505050;
                    }
                    else
                    {
                        sprite->color = 0xffffffff;
                    }
                    sprite->posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
                }
                else
                {
                    if (!g_App.cfg.flags.emulateColorAdd)
                    {
                        sprite->color = 0xb0000000;
                    }
                    else
                    {
                        sprite->color = 0xb0ffffff;
                    }
                    sprite->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
                }
            }
        }
        if (screen->frame < 20)
        {
            break;
        }
        screen->state++;
        screen->frame = 0;
    case 1:
        MoveCursorVertical(screen, 7);
        sprite = &screen->sprites[1];
        for (i = 0; i <= 6; i++, sprite++)
        {
            if (i == screen->cursor)
            {
                if (!g_App.cfg.flags.emulateColorAdd)
                {
                    sprite->color = 0xff505050;
                }
                else
                {
                    sprite->color = 0xffffffff;
                }
                sprite->posOffset = D3DXVECTOR3(-4.0f, -4.0f, 0.0f);
            }
            else
            {
                if (!g_App.cfg.flags.emulateColorAdd)
                {
                    sprite->color = 0xb0000000;
                }
                else
                {
                    sprite->color = 0xb0ffffff;
                }
                sprite->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            }
        }
        if (WAS_PRESSED(0x1001))
        {
            sprite = screen->sprites;
            switch (screen->cursor)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                for (i = 0; i < 0x26; i++, sprite++)
                {
                    sprite->pendingInterrupt = screen->cursor + 3;
                }
                screen->difficulty = screen->cursor;
                screen->state = screen->cursor + 3;
                screen->listState = screen->state;
                screen->frame = 0;
                screen->cursor = screen->savedCursor;
                screen->character = -1;
                screen->spellPage = -1;
                break;
            case 5:
                for (i = 0; i < 0x26; i++, sprite++)
                {
                    sprite->pendingInterrupt = screen->cursor + 3;
                }
                screen->difficulty = screen->cursor;
                screen->state = 8;
                screen->listState = screen->state;
                screen->frame = 0;
                screen->character = -1;
                screen->cursor = screen->savedSpellPage;
                screen->spellPage = -1;
                break;
            case 6:
                for (i = 0; i < 0x26; i++, sprite++)
                {
                    sprite->pendingInterrupt = 2;
                }
                screen->state = 2;
                g_SoundPlayer.QueueSound(0xb, 0);
                break;
            }
        }
        if (WAS_PRESSED(0xa))
        {
            screen->cursor = 6;
            g_SoundPlayer.QueueSound(0xb, 0);
        }
        break;
    case 2:
        if (screen->frame < 60)
        {
            break;
        }
        g_App.scene = 1;
        return 0;
    case 7:
        if ((g_Buttons & 4) || (g_Buttons & 0x100))
        {
            if (screen->cheatProgress < 5)
            {
                if (WAS_PRESSED(0x800))
                {
                    screen->cheatProgress++;
                }
                else if (WAS_PRESSED(0x160b))
                {
                    screen->cheatProgress = 0;
                }
            }
            else if (screen->cheatProgress < 7)
            {
                if (WAS_PRESSED(0x200))
                {
                    screen->cheatProgress++;
                }
                else if (WAS_PRESSED(0x160b))
                {
                    screen->cheatProgress = 0;
                }
            }
            else if (screen->cheatProgress < 10)
            {
                if (WAS_PRESSED(0x400))
                {
                    screen->cheatProgress++;
                }
                else if (WAS_PRESSED(0x160b))
                {
                    screen->cheatProgress = 0;
                }
            }
            else
            {
                for (shot = 0; shot < 4; shot++)
                {
                    for (k = 0; k < 5; k++)
                    {
                        g_GameManager.clrds[shot].stagesClearedNoContinue[k] = 99;
                        g_GameManager.clrds[shot].stagesCleared[k] = 99;
                    }
                }
                screen->cheatProgress = 0;
                g_SoundPlayer.QueueSound(0x1c, 0);
            }
        }
        else
        {
            screen->cheatProgress = 0;
        }
    case 3:
    case 4:
    case 5:
    case 6:
        if (screen->character != screen->cursor && screen->frame == 20)
        {
            screen->character = screen->cursor;
            g_SpriteEngine->RenderTextCentered(&screen->textSprites[0], 0xffffff, 0, g_ShotNames[screen->character * 2]);
            g_SpriteEngine->RenderTextCentered(&screen->textSprites[1], 0xffffff, 0,
                                               g_ShotNames[screen->character * 2 + 1]);
        }
        if (screen->frame < 30)
        {
            break;
        }
        if (MoveCursorHorizontal(screen, 2))
        {
            screen->frame = 0;
            sprite = screen->sprites;
            for (i = 0; i < 0x26; i++, sprite++)
            {
                sprite->pendingInterrupt = screen->difficulty + 3;
            }
        }
        if (WAS_PRESSED(0xa))
        {
            g_SoundPlayer.QueueSound(0xb, 0);
            screen->state = 0;
            screen->frame = 1;
            sprite = screen->sprites;
            for (i = 0; i < 0x26; i++, sprite++)
            {
                sprite->pendingInterrupt = 1;
            }
            screen->savedCursor = screen->cursor;
            screen->cursor = screen->difficulty;
        }
        break;
    case 8:
        if (screen->spellPage != screen->cursor && screen->frame == 20)
        {
            screen->spellPage = screen->cursor;
            for (i = screen->spellPage * 10; i < screen->spellPage * 10 + 10; i++)
            {
                if (i >= 0x40)
                {
                    break;
                }
                if (g_GameManager.catks[i].attempts == 0)
                {
#if FEAT_ENGLISH
                    g_SpriteEngine->RenderText(&screen->textSprites[i % 10], 0xffffff, 0, EN_UNKNOWN_NAME);
#else
                    g_SpriteEngine->RenderText(&screen->textSprites[i % 10], 0xffffff, 0, "？？？？？");
#endif
                }
                else
                {
#if FEAT_ENGLISH
                    g_SpriteEngine->RenderText(&screen->textSprites[i % 10], 0xffffff, 0, ENGLISH_SPELL_CARD(i));
#else
                    g_SpriteEngine->RenderText(&screen->textSprites[i % 10], 0xffffff, 0, g_GameManager.catks[i].name);
#endif
                }
            }
        }
        if (screen->frame < 30)
        {
            break;
        }
        if (MoveCursorHorizontal(screen, 7))
        {
            screen->frame = 0;
            sprite = screen->sprites;
            for (i = 0; i < 0x26; i++, sprite++)
            {
                sprite->pendingInterrupt = screen->difficulty + 3;
            }
        }
        if (WAS_PRESSED(0xa))
        {
            g_SoundPlayer.QueueSound(0xb, 0);
            screen->state = 0;
            screen->frame = 1;
            sprite = screen->sprites;
            for (i = 0; i < 0x26; i++, sprite++)
            {
                sprite->pendingInterrupt = 1;
            }
            screen->savedSpellPage = screen->cursor;
            screen->cursor = screen->difficulty;
        }
        break;
    case 9:
        screen->HandleNameEntry();
        break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        screen->HandleReplaySaving();
        break;
    case 15:
    case 16:
        screen->WaitAfterNameEntry();
        break;
    }
    sprite = screen->sprites;
    for (i = 0; i < 0x26; i++, sprite++)
    {
        g_SpriteEngine->Step(sprite);
    }
    screen->frame++;
    return 1;
}

int ResultScreen::OnDraw(ResultScreen *screen)
{
    char str[2];
    D3DXVECTOR3 unused_;
    float shift;
    float scale3;
    int card;
    D3DXVECTOR3 pos3;
    ListNode *node21;
    int col0;
    ListNode *node3;
    Sprite *sprite;
    char name[9];
    int i;
    D3DXVECTOR3 charPos;

    sprite = screen->sprites;
    g_App.viewport.X = 0;
    g_App.viewport.Y = 0;
    g_App.viewport.Width = 640;
    g_App.viewport.Height = 480;
    g_App.d3dDevice->SetViewport(&g_App.viewport);
    g_SpriteEngine->CopySurfaceToBackBuffer(0, 0, 0, 0, 0);
    for (i = 0; i < 0x26; i++, sprite++)
    {
        pos3 = sprite->pos;
        sprite->pos += sprite->posOffset;
        g_SpriteEngine->Draw(sprite);
        sprite->pos = pos3;
    }
    sprite = &screen->sprites[0xe];
    if (sprite->pos.x < 640.0f)
    {
        if (screen->listState != 8)
        {
            pos3 = sprite->pos;
            screen->textSprites[0].pos = pos3;
            g_SpriteEngine->Draw(&screen->textSprites[0]);
            pos3[0] += 320.0f;
            screen->textSprites[1].pos = pos3;
            g_SpriteEngine->Draw(&screen->textSprites[1]);
            pos3[0] -= 320.0f;
            pos3[1] += 18.0f;
            pos3[1] += 18.0f;
            node3 = screen->scores[screen->difficulty][screen->character * 2].next;
            node21 = screen->scores[screen->difficulty][screen->character * 2 + 1].next;
            for (i = 0; i < 10; i++)
            {
            if (screen->state == 9)
            {
                if (g_GameManager.shotType == 0)
                {
                    if (node3->data->isNew != 0)
                    {
                        g_AsciiText.color = 0xfff0f0ff;
                        *(DWORD *)&name[0] = *(DWORD *)"    ";
                        *(DWORD *)&name[4] = *(DWORD *)"    ";
                        name[8] = 0;
                        name[screen->cursor >= 8 ? 7 : screen->cursor] = '_';
                        g_AsciiText.AddFormat(&pos3, "   %8s", name);
                    }
                    else
                    {
                        g_AsciiText.color = 0x80ffffc0;
                    }
                }
                else
                {
                    g_AsciiText.color = 0x80ffc0c0;
                }
            }
            else
            {
                g_AsciiText.color = 0xffffc0c0;
            }
                g_AsciiText.AddFormat(&pos3, "%2d", i + 1);
                pos3.x += 36.0f;
            if (node3->data->stage <= 6)
            {
                g_AsciiText.AddFormat(&pos3, "%8s %9d(%d)", node3->data->name, node3->data->score, node3->data->stage);
            }
            else if (node3->data->stage == 7)
            {
                g_AsciiText.AddFormat(&pos3, "%8s %9d(1)", node3->data->name, node3->data->score);
            }
            else
            {
                g_AsciiText.AddFormat(&pos3, "%8s %9d(C)", node3->data->name, node3->data->score);
            }
                pos3[0] += 300.0f;
            if (screen->state == 9)
            {
                if (g_GameManager.shotType == 1)
                {
                    if (node21->data->isNew != 0)
                    {
                        g_AsciiText.color = 0xfffff0f0;
                        *(DWORD *)&name[0] = *(DWORD *)"    ";
                        *(DWORD *)&name[4] = *(DWORD *)"    ";
                        name[8] = 0;
                        name[screen->cursor >= 8 ? 7 : screen->cursor] = '_';
                        g_AsciiText.AddFormat(&pos3, "%8s", name);
                    }
                    else
                    {
                        g_AsciiText.color = 0xc0c0c0ff;
                    }
                }
                else
                {
                    g_AsciiText.color = 0x80c0c0ff;
                }
            }
            else
            {
                g_AsciiText.color = 0xffc0c0ff;
            }
            if (node21->data->stage <= 6)
            {
                g_AsciiText.AddFormat(&pos3, "%8s %9d(%d)", node21->data->name, node21->data->score, node21->data->stage);
            }
            else if (node21->data->stage == 7)
            {
                g_AsciiText.AddFormat(&pos3, "%8s %9d(1)", node21->data->name, node21->data->score);
            }
            else
            {
                g_AsciiText.AddFormat(&pos3, "%8s %9d(C)", node21->data->name, node21->data->score);
            }
                pos3[0] -= 336.0f;
                pos3[1] += 18.0f;
                node3 = node3->next;
                node21 = node21->next;
            }
        }
        else
        {
            pos3 = sprite->pos;
            pos3[1] += 16.0f;
            for (i = 0; i < 10; i++)
            {
                card = screen->spellPage * 10 + i;
                if (card >= 0x40)
                {
                    break;
                }
                screen->textSprites[i].pos = pos3;
                if (g_GameManager.catks[card].attempts == 0)
                {
                    g_AsciiText.color = 0x80c0c0ff;
                }
                else if (g_GameManager.catks[card].captures == 0)
                {
                    g_AsciiText.color = 0xffc0a0a0;
                }
                else
                {
                    g_AsciiText.color = 0xfff0f0ff - i * 0x80800;
                }
                g_AsciiText.AddFormat(&pos3, "No.%.2d", card + 1);
                screen->textSprites[i].pos[0] += 96.0f;
                g_SpriteEngine->Draw(&screen->textSprites[i]);
                pos3[0] += 368.0f;
                g_AsciiText.AddFormat(&pos3, "%3d/%3d", g_GameManager.catks[card].captures, g_GameManager.catks[card].attempts);
                pos3[0] -= 368.0f;
                pos3[1] += 30.0f;
            }
        }
    }
    if (screen->state == 9 || screen->state == 0xd)
    {
        pos3 = D3DXVECTOR3(160.0f, 356.0f, 0.0f);
        for (i = 0; i < 6; i++)
        {
            for (col0 = 0; col0 < 0x10; col0++)
            {
                scale3 = 0.0f;
                shift = 0.0f;
                if (screen->charIndex == i * 16 + col0)
                {
                    g_AsciiText.color = 0xffffffc0;
                    if (screen->frame % 64 < 32)
                    {
                        scale3 = 0.8f * (screen->frame % 32) / 32.0f + 1.2f;
                    }
                    else
                    {
                        scale3 = 2.0f - 0.8f * (screen->frame % 32) / 32.0f;
                    }
                    g_AsciiText.scaleX = scale3;
                    g_AsciiText.scaleY = scale3;
                    scale3 = -(scale3 - 1.0f) * 8.0f;
                    shift = scale3;
                }
                else
                {
                    g_AsciiText.color = 0x60c0c0c0;
                    g_AsciiText.scaleX = 1.0f;
                    g_AsciiText.scaleY = 1.0f;
                }
                charPos = pos3;
                charPos.x += scale3;
                charPos.y += shift;
                str[0] = g_NameAlphabet[i * 16 + col0];
                str[1] = 0;
                if (i == 5)
                {
                    if (col0 == 14)
                    {
                        str[0] = 0x80;
                    }
                    else if (col0 == 15)
                    {
                        str[0] = 0x81;
                    }
                }
                g_AsciiText.AddString(&charPos, str);
                pos3[0] += 20.0f;
            }
            pos3[0] -= col0 * 20;
            pos3[1] += 18.0f;
        }
    }
    g_AsciiText.scaleX = 1.0f;
    g_AsciiText.scaleY = 1.0f;
    if (screen->state >= 10 && screen->state <= 14)
    {
        sprite = &screen->sprites[0xf];
        for (i = 0; i < 6; i++, sprite++)
        {
            g_SpriteEngine->Draw(sprite);
        }
        sprite = &screen->sprites[0x15];
        pos3 = sprite->pos;
        sprite++;
        g_AsciiText.AddFormat(&pos3, "No.   Name     Date     Player Score");
        for (i = 0; i < 0xf; i++)
        {
            pos3 = sprite->pos;
            sprite++;
            if (i == screen->replaySlot)
            {
                g_AsciiText.color = 0xffff8080;
            }
            else
            {
                g_AsciiText.color = 0xff808080;
            }
            if (screen->state == 0xd)
            {
                g_AsciiText.AddFormat(&pos3, "No.%.2d %8s %8s %7s %9d", i + 1, screen->name, screen->replay.date,
                                      g_ShotTypeNames[g_GameManager.character * 2 + g_GameManager.shotType],
                                      screen->replay.score);
                g_AsciiText.color = 0xfff0f0ff;
                *(DWORD *)&name[0] = *(DWORD *)"    ";
                *(DWORD *)&name[4] = *(DWORD *)"    ";
                name[8] = 0;
                name[screen->cursor >= 8 ? 7 : screen->cursor] = '_';
                g_AsciiText.AddFormat(&pos3, "      %8s", name);
            }
            else if (*(DWORD *)screen->replays[i].magic != *(DWORD *)"T6RP" || screen->replays[i].version != 0x102)
            {
                g_AsciiText.AddFormat(&pos3, "No.%.2d -------- --/--/-- -------         0", i + 1);
            }
            else
            {
                g_AsciiText.AddFormat(&pos3, "No.%.2d %8s %8s %7s %9d", i + 1, screen->replays[i].name,
                                      screen->replays[i].date, g_ShotTypeNames[screen->replays[i].shotType],
                                      screen->replays[i].score);
            }
        }
    }
    g_AsciiText.color = 0xffffffff;
    screen->DrawStatistics();
    return 1;
}

int ResultScreen::AddedCallback(ResultScreen *screen)
{
    int i;
    Sprite *sprite;
    int shot;
    int k;

    if (screen->state != 0x11)
    {
        if (g_SpriteEngine->LoadSurface(0, "data/result/result.jpg") != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x25, "data/result00.anm", 0x100) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x26, "data/result01.anm", 0x108) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x27, "data/result02.anm", 0x10f) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(0x28, "data/result03.anm", 0x125) != 0)
        {
            return -1;
        }
        sprite = screen->sprites;
        for (i = 0; i < 0x26; i++, sprite++)
        {
            sprite->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            sprite->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            g_SpriteEngine->StartScript(sprite, i + 0x100);
        }
        sprite = screen->textSprites;
        for (i = 0; i < 0x10; i++, sprite++)
        {
            g_SpriteEngine->InitSprite(sprite, i + 0x708);
            sprite->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            sprite->flags.anchor = 3;
            sprite->fontWidth = 15;
            sprite->fontHeight = 15;
        }
    }
    for (i = 0; i < 5; i++)
    {
        for (shot = 0; shot < 4; shot++)
        {
            for (k = 0; k < 10; k++)
            {
                screen->defaultScores[i][shot][k].score = 1000000 - k * 100000;
                screen->defaultScores[i][shot][k].magic = *(DWORD *)"DMYS";
                screen->defaultScores[i][shot][k].difficulty = i;
                screen->defaultScores[i][shot][k].version = 0x10;
                screen->defaultScores[i][shot][k].size2 = 0x1c;
                screen->defaultScores[i][shot][k].size = 0x1c;
                screen->defaultScores[i][shot][k].stage = 1;
                screen->defaultScores[i][shot][k].isNew = 0;
                screen->AddScore(&screen->defaultScores[i][shot][k], i, shot);
                strcpy(screen->defaultScores[i][shot][k].name, "Nanashi ");
            }
        }
    }
    screen->savedCursor = 0;
    screen->scoreDat = LoadScoreDat("score.dat");
    for (i = 0; i < 5; i++)
    {
        for (shot = 0; shot < 4; shot++)
        {
            LoadHighScores(screen->scoreDat, &screen->scores[i][shot], shot, i);
        }
    }
    if (screen->state != 9 && screen->state != 0x11)
    {
        LoadSpellCards(screen->scoreDat, g_GameManager.catks);
        LoadClears(screen->scoreDat, g_GameManager.clrds);
        LoadPracticeScores(screen->scoreDat, &g_GameManager.pscrs[0][0][0]);
    }
    if (screen->state == 0x11 &&
        g_GameManager.pscrs[g_GameManager.character * 2 + g_GameManager.shotType][g_GameManager.stage - 1]
                           [g_GameManager.difficulty]
                               .score < g_GameManager.score)
    {
        g_GameManager.pscrs[g_GameManager.character * 2 + g_GameManager.shotType][g_GameManager.stage - 1]
                           [g_GameManager.difficulty]
                               .score = g_GameManager.score;
    }
    screen->unk_39a0.spriteIndex = -1;
    return 0;
}

int ResultScreen::DeletedCallback(ResultScreen *screen)
{
    int i;
    int j;

    if (screen->scoreDat != NULL)
    {
        SaveScoreDat(screen);
        ReleaseScoreDat(screen->scoreDat);
    }
    screen->scoreDat = NULL;
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 4; j++)
        {
            screen->ReleaseScores(i, j);
        }
    }
    g_SpriteEngine->ReleaseAnm(0x25);
    g_SpriteEngine->ReleaseAnm(0x26);
    g_SpriteEngine->ReleaseAnm(0x27);
    g_SpriteEngine->ReleaseAnm(0x28);
    g_SpriteEngine->ReleaseSurface(0);
    g_Chain.Remove(screen->drawChain);
    screen->drawChain = NULL;
    delete screen;
    screen = NULL;
    return 0;
}

void DebugPrint2(const char *fmt, ...)
{
}
