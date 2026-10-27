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

// The result screen's scene object (0x56b0 bytes, set up by Func42d773).
class ResultScreen
{
public:
    // Frees the entries of the high score list for `difficulty` and
    // `shot` (0 to 4 and 0 to 3).
    void ReleaseScores(int difficulty, int shot);

    DWORD unk_0000[0xeac];
    ListNode scores[5][4];
    DWORD unk_3ba0[0x6c4];
};
ASSERT_SIZE(ResultScreen, 0x56b0);
