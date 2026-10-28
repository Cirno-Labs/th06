// cflags: /Os -/Oi
// The /Os file from 0x42b0d9 to 0x42f6c9, with the result screen: the
// score file and its sorted lists.

#include <string.h>

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
