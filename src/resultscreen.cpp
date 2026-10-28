// cflags: /Os -/Oi
// The /Os file from 0x42b0d9 to 0x42f6c9, with the result screen: the
// score file and its sorted lists.

#include <string.h>

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

DWORD LoadHighScores(void *scoreDat, ListNode *list, int difficulty, int shot)
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
        if (chunk->magic == 0x52435348 && chunk->version == 0x10 && chunk->difficulty == difficulty &&
            chunk->shot == shot)
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

void ReleaseScoreDat(ScoreDat *dat)
{
    FreeListNodes(dat->scores);
    MemFree(dat->scores);
    free(dat);
}

void ResultScreen::AddScore(Hscr *entry, int difficulty, int shot)
{
    InsertScore(&this->scores[difficulty][shot], entry);
}

void ResultScreen::ReleaseScores(int difficulty, int shot)
{
    FreeListNodes(&this->scores[difficulty][shot]);
}
