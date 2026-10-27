// The result screen ("data/result/result.jpg", "data/result00.anm"): the
// score file ("score.dat") and its high score lists.

#pragma once

#include <windows.h>

#include "common.h"

// A node of the score file's lists. Each list starts with a head node
// whose `data` is NULL; the entries after it are sorted.
struct ListNode
{
    ListNode *prev;
    ListNode *next;
    void *data;
};

// Frees every node after `head`.
void FreeListNodes(ListNode *head);

// "score.dat" as loaded: a 0x14-byte header, then chunks ("TH6K", ...).
struct ScoreDat
{
    DWORD unk_00[0x3];
    // The list of the score entries read from the chunks; set up when
    // the file is loaded.
    ListNode *scores;
    DWORD unk_10;
};
ASSERT_SIZE(ScoreDat, 0x14);

// Frees `dat` and its list.
void ReleaseScoreDat(ScoreDat *dat);
