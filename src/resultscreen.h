// The result screen ("data/result/result.jpg", "data/result00.anm"): the
// score file ("score.dat") and its high score lists.

#pragma once

#include <windows.h>

#include "common.h"

// A high score chunk of the score file ("HSCR").
struct Hscr
{
    DWORD magic;
    WORD size;
    WORD unk_06;
    BYTE version;
    BYTE unk_09[0x3];
    DWORD score;
    // The difficulty (0 to 4) and shot (0 to 3) of its list.
    BYTE difficulty;
    BYTE shot;
};

// A spell card chunk of the score file ("CATK"), one per card.
struct Catk
{
    DWORD magic;
    WORD size;
    WORD unk_06;
    BYTE version;
    BYTE unk_09[0x7];
    // The card's number, below 0x40.
    WORD index;
    BYTE unk_12[0x2e];
};
ASSERT_SIZE(Catk, 0x40);

// A chunk of the score file ("CLRD") with what a shot type cleared.
struct Clrd
{
    DWORD magic;
    WORD size;
    WORD unk_06;
    BYTE version;
    BYTE unk_09[0x3];
    // By difficulty.
    BYTE unk_0c[5];
    BYTE unk_11[5];
    BYTE shot;
    BYTE unk_17;
};
ASSERT_SIZE(Clrd, 0x18);

// A node of the score file's lists. Each list starts with a head node
// whose `data` is NULL; the entries after it are sorted.
struct ListNode
{
    ListNode *prev;
    ListNode *next;
    Hscr *data;
};

// Frees every node after `head`.
void FreeListNodes(ListNode *head);
// Links `entry` in after `head`, before the first entry whose score is no
// higher. Returns its rank, from 0.
int InsertScore(ListNode *head, Hscr *entry);

// The start of a chunk of the score file.
struct ScoreChunk
{
    DWORD magic;
    // The chunk's size, header included.
    WORD size;
    WORD unk_06;
};

// "score.dat" as loaded: a 0x14-byte header, then chunks ("TH6K", ...).
// Every byte from the third on is stored xored with a key that runs over
// the bytes before it.
struct ScoreDat
{
    BYTE unk_00;
    BYTE unk_01;
    // The sum of the decrypted bytes from the fifth on.
    WORD checksum;
    DWORD unk_04;
    // Where the chunks start.
    int dataOffset;
    // The list of the score entries read from the chunks; set up when
    // the file is loaded.
    ListNode *scores;
    int fileSize;
};
ASSERT_SIZE(ScoreDat, 0x14);

// Loads and decrypts the score file at `path`, or makes an empty one if
// it is missing, too short, fails its checksum or has no "TH6K" chunk;
// sets up its list.
ScoreDat *LoadScoreDat(char *path);
// Inserts the score file's high scores for `difficulty` and `shot` into
// `list`, or, if it is NULL, into the file's own list after emptying it.
// Returns the best score in the file's list, at least 1000000.
DWORD LoadHighScores(void *scoreDat, ListNode *list, int difficulty, int shot);
// Copies the score file's spell card chunks into `catks`, by number,
// stopping at a number of 0x40 or more. Returns 0, or -1 if `catks` is
// NULL.
int LoadSpellCards(void *scoreDat, Catk *catks);
// Fills `clrds` with a fresh chunk for each of the four shots, then
// copies the score file's "CLRD" chunks over them, by shot, stopping at
// a shot of 4 or more. Returns 0, or -1 if `clrds` is NULL.
int LoadClears(void *scoreDat, Clrd *clrds);
// Frees `dat` and its list.
void ReleaseScoreDat(ScoreDat *dat);

// The result screen's scene object (0x56b0 bytes, set up by Func42d773).
class ResultScreen
{
public:
    // Frees the entries of the high score list for `difficulty` and
    // `shot` (0 to 4 and 0 to 3).
    void ReleaseScores(int difficulty, int shot);
    // Inserts `entry` into the high score list for `difficulty` and
    // `shot`.
    void AddScore(Hscr *entry, int difficulty, int shot);

    DWORD unk_0000[0xeac];
    ListNode scores[5][4];
    DWORD unk_3ba0[0x6c4];
};
ASSERT_SIZE(ResultScreen, 0x56b0);
