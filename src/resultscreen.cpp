// cflags: /Os -/Oi
// The /Os file from 0x42b0d9 to 0x42f6c9, with the result screen: the
// score file and its sorted lists.

#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ascii.h"
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

    dat = (ScoreDat *)Func41e290(path, 1);
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
        clrds[i].unk_06 = sizeof(Clrd);
        clrds[i].size = sizeof(Clrd);
        clrds[i].version = 0x10;
        clrds[i].shot = i;
        for (k = 0; k < 5; k++)
        {
            clrds[i].unk_0c[k] = 1;
            clrds[i].unk_11[k] = 1;
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
                p->unk_06 = sizeof(Pscr);
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
    screen->th6k.unk_06 = sizeof(Th6k);
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
                    node->data->unk_06 = 0x1c;
                    node->data->size = 0x1c;
                    node->data->version = 0x10;
                    node->data->unk_09[0] = 0;
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
        clrd26->unk_06 = sizeof(Clrd);
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
            catk1->unk_06 = sizeof(Catk);
            catk1->size = sizeof(Catk);
            catk1->version = 0x10;
            memcpy(buf + size0, catk1, sizeof(Catk));
            size0 += sizeof(Catk);
        }
    }
    pscr0 = g_GameManager.pscrs;
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
    header->unk_01 = g_Rng.GetRandomU16InRange(256);
    header->unk_06 = g_Rng.GetRandomU16InRange(256);
    header->unk_04 = 0x10;
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
    Func41e460("score.dat", buf, size0);
    free(buf);
}

void ReleaseScoreDat(ScoreDat *dat)
{
    FreeListNodes(dat->scores);
    MemFree(dat->scores);
    free(dat);
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
        if (g_GameManager.unk_1822 == 0)
        {
            this->entry.stage = g_GameManager.stage;
        }
        else
        {
            this->entry.stage = 99;
        }
        this->entry.unk_09[0] = 1;
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
        g_Unknown6d3f50.QueueSound(0xc, 0);
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
        g_Unknown6d3f50.QueueSound(0xc, 0);
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
        g_Unknown6d3f50.QueueSound(0xc, 0);
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
        g_Unknown6d3f50.QueueSound(0xc, 0);
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
        g_Unknown6d3f50.QueueSound(10, 0);
    }
    if (WAS_PRESSED_OR_HELD(0xa))
    {
        int pos2 = this->cursor >= 8 ? 7 : this->cursor;

        if (this->cursor > 0)
        {
            this->cursor--;
            this->entry.name[pos2] = ' ';
        }
        g_Unknown6d3f50.QueueSound(0xb, 0);
    }
    if (WAS_PRESSED(8))
    {
    end:
        g_Unknown6d3f50.QueueSound(0xb, 0);
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
            else if (g_Unknown6c6d18.gameSpeed < 0.99f)
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
                g_Unknown6d3f50.QueueSound(10, 0);
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
            g_Unknown6d3f50.QueueSound(0xb, 0);
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
            g_Unknown6d3f50.QueueSound(0xb, 0);
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
                replay = (ReplayHeader *)Func41e290(path, 1);
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
            g_Unknown6d3f50.QueueSound(10, 0);
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
            g_Unknown6d3f50.QueueSound(0xb, 0);
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
            g_Unknown6d3f50.QueueSound(0xc, 0);
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
            g_Unknown6d3f50.QueueSound(0xc, 0);
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
            g_Unknown6d3f50.QueueSound(0xc, 0);
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
            g_Unknown6d3f50.QueueSound(0xc, 0);
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
            g_Unknown6d3f50.QueueSound(10, 0);
        }
        if (WAS_PRESSED_OR_HELD(0xa))
        {
            int pos2 = this->cursor >= 8 ? 7 : this->cursor;

            if (this->cursor > 0)
            {
                this->cursor--;
                this->name[pos2] = ' ';
            }
            g_Unknown6d3f50.QueueSound(0xb, 0);
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
        g_AsciiText.AddFormat(&cursorPos, "%9d", g_GameManager.unk_0000);
        if (g_GameManager.unk_0000 < 2000000)
        {
            rating -= 20.0f;
        }
        else if (g_GameManager.unk_0000 < 200000000)
        {
            rating += (g_GameManager.unk_0000 - 2000000) / 198000000.0f * 60.0f - 20.0f;
        }
        else
        {
            rating += 40.0f;
        }
        cursorPos.y += 22.0f;
        g_AsciiText.AddString(&cursorPos, g_DifficultyNames[g_GameManager.difficulty]);
        rating += g_DifficultyRatings[g_GameManager.difficulty];
        cursorPos.y += 22.0f;
        if (g_GameManager.difficulty == 0 || !g_GameManager.unk_1822)
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
        slowdown = (g_Unknown6c6d18.unk_1b4 / g_Unknown6c6d18.unk_1b8 - 0.5f) * 2.0f;
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

// Parked (row unnamed): the original reserves 0x44 bytes for the inlined
// constructor's slots, this 0x54. Without the Sprite at 0x39a0 the frames
// agree, but the constructor needs it; wrapping it in a struct makes
// both frames bigger.
int ResultScreen::RegisterChain(int unk)
{
    ResultScreen *screen = new ResultScreen;

    DebugPrint2("counat = %d\n", g_GameManager.progressFrames);
    screen->calcChain = g_Chain.CreateElem((ChainCallback)ResultScreen::OnUpdate);
    screen->calcChain->addedCallback = (ChainCallback)ResultScreen::AddedCallback;
    screen->calcChain->deletedCallback = (ChainCallback)ResultScreen::DeletedCallback;
    screen->calcChain->arg = screen;
    if (unk != 0)
    {
        if (!g_GameManager.unk_1823)
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

void MoveCursorVertical(ResultScreen *screen, int count)
{
    if (WAS_PRESSED_OR_HELD(0x10))
    {
        screen->cursor--;
        if (screen->cursor < 0)
        {
            screen->cursor += count;
        }
        g_Unknown6d3f50.QueueSound(0xc, 0);
    }
    if (WAS_PRESSED_OR_HELD(0x20))
    {
        screen->cursor++;
        if (screen->cursor >= count)
        {
            screen->cursor -= count;
        }
        g_Unknown6d3f50.QueueSound(0xc, 0);
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
        g_Unknown6d3f50.QueueSound(0xc, 0);
        return 1;
    }
    if (WAS_PRESSED_OR_HELD(0x80))
    {
        screen->cursor++;
        if (screen->cursor >= count)
        {
            screen->cursor -= count;
        }
        g_Unknown6d3f50.QueueSound(0xc, 0);
        return 1;
    }
    return 0;
}
