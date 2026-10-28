// The result screen ("data/result/result.jpg", "data/result00.anm"): the
// score file ("score.dat") and its high score lists.

#pragma once

#include <windows.h>

#include "common.h"
#include "sprite.h"

// A high score chunk of the score file ("HSCR").
struct Hscr
{
    DWORD magic;
    WORD size;
    WORD unk_06;
    BYTE version;
    BYTE unk_09[0x3];
    DWORD score;
    // The shot (0 to 3) and difficulty (0 to 4) of its list.
    BYTE shot;
    BYTE difficulty;
    // The stage reached, 99 for a clear.
    BYTE stage;
    // Eight characters and a NUL.
    char name[9];
};
ASSERT_SIZE(Hscr, 0x1c);

// The score file's first chunk ("TH6K").
struct Th6k
{
    DWORD magic;
    WORD size;
    WORD unk_06;
    BYTE version;
    BYTE unk_09[0x3];
};
ASSERT_SIZE(Th6k, 0xc);

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

// A practice score chunk of the score file ("PSCR").
struct Pscr
{
    DWORD magic;
    WORD size;
    WORD unk_06;
    BYTE version;
    BYTE unk_09[0x3];
    DWORD score;
    BYTE shot;
    BYTE difficulty;
    BYTE stage;
    BYTE unk_13;
};
ASSERT_SIZE(Pscr, 0x14);

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
    // Where the key starts; random.
    BYTE unk_01;
    // The sum of the decrypted bytes from the fifth on.
    WORD checksum;
    WORD unk_04;
    BYTE unk_06;
    BYTE unk_07;
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
// Inserts the score file's high scores for `shot` and `difficulty` into
// `list`, or, if it is NULL, into the file's own list after emptying it.
// Returns the best score in the file's list, at least 1000000.
DWORD LoadHighScores(void *scoreDat, ListNode *list, int shot, int difficulty);
// Copies the score file's spell card chunks into `catks`, by number,
// stopping at a number of 0x40 or more. Returns 0, or -1 if `catks` is
// NULL.
int LoadSpellCards(void *scoreDat, Catk *catks);
// Fills `clrds` with a fresh chunk for each of the four shots, then
// copies the score file's "CLRD" chunks over them, by shot, stopping at
// a shot of 4 or more. Returns 0, or -1 if `clrds` is NULL.
int LoadClears(void *scoreDat, Clrd *clrds);
// Fills `pscrs` (by shot, stage and difficulty, 4 by 6 by 4) with fresh
// chunks, then copies the score file's "PSCR" chunks over them, stopping
// at one out of range. Returns 0, or -1 if `pscrs` is NULL.
int LoadPracticeScores(void *scoreDat, Pscr *pscrs);
// Writes "score.dat" from `screen`'s file header, a "TH6K" chunk, its
// high score lists (ten entries each at most) and g_GameManager's clear,
// spell card and practice chunks, encrypted.
void SaveScoreDat(class ResultScreen *screen);
// Frees `dat` and its list.
void ReleaseScoreDat(ScoreDat *dat);

// Moves `screen`'s cursor up or down through `count` items, wrapping.
void MoveCursorVertical(class ResultScreen *screen, int count);

// The characters a name can be made of, 16 to a row; the last two
// stand for a space and the end.
extern char *g_NameAlphabet;
// Each character's two shot types' names ("博麗 霊夢 (霊)", ...), by
// character * 2 + shotType.
extern char *g_ShotNames[8];

// The result screen's scene object (0x56b0 bytes, set up by Func42d773).
class ResultScreen
{
public:
    // Frees the entries of the high score list for `difficulty` and
    // `shot` (0 to 4 and 0 to 3).
    void ReleaseScores(int difficulty, int shot);
    // Inserts `entry` into the high score list for `difficulty` and
    // `shot`.
    int AddScore(Hscr *entry, int difficulty, int shot);

    // Lets the player enter a name for `entry`: up and down move
    // `charIndex` through g_NameAlphabet by rows of 16, left and right
    // along a row; the shot button writes the character at `cursor`
    // (the last two are a space and the end) and the bomb button erases.
    // On the end or the cancel button the name is copied to `name` and
    // the screen moves to state 0xf; also right away when the score
    // ranks below tenth. Returns 0.
    int HandleNameEntry();

    // The score file, as LoadScoreDat loaded it.
    ScoreDat *scoreDat;
    // Counts the frames in the current state.
    int frame;
    int state;
    DWORD unk_000c;
    // The menu item selected, or where the next character of a name
    // goes (0 to 8).
    int cursor;
    DWORD unk_0014[0x3];
    // The character of g_NameAlphabet selected.
    int charIndex;
    // g_GameManager's character and difficulty.
    int character;
    DWORD unk_0028;
    int difficulty;
    DWORD unk_0030;
    char name[0xc];
    Sprite sprites[0x26];
    // Text drawn with RenderTextCentered; the first two hold the two shot
    // types' names.
    Sprite textSprites[0x10];
    Sprite unk_39a0;
    ListNode scores[5][4];
    DWORD unk_3ba0[0x578];
    // The score being entered.
    Hscr entry;
    // The "TH6K" chunk SaveScoreDat writes.
    Th6k th6k;
    DWORD unk_51a8[0x142];
};
ASSERT_SIZE(ResultScreen, 0x56b0);
