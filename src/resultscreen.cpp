// cflags: /Os -/Oi
// The /Os file from 0x42b0d9 to 0x42f6c9, with the result screen: the
// score file and its sorted lists.

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
