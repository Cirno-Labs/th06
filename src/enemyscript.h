// Enemy scripts, loaded from the stage's enemy data file.

#pragma once

#include <windows.h>

#include "common.h"
#include "timer.h"

// Where one enemy is in its script.
struct EnemyScriptState
{
    // The next instruction.
    void *code;
    Timer timer;
    DWORD unk_10[0xe];
    // The subroutine running.
    short sub;
    short unk_4a;
};

// The start of an enemy script file.
struct EnemyScriptHeader
{
    short subCount;
    short unk_02;
    // An offset from the start of the file; Load makes it a pointer.
    int unk_04;
    DWORD unk_08[0x2];
    // Then the subroutines' offsets.
};

// The enemy script file, the global at 0x487e50: a header, then a table
// of subroutines. Load turns the file's offsets into pointers.
class EnemyScripts
{
public:
    // Loads the file at `path` and turns its offsets into pointers.
    // Returns 0, or -1 after logging "敵データの読み込みに失敗しました".
    int Load(char *path);
    // Frees the file.
    void Release();
    // Starts subroutine `sub` on `state`. Returns 0.
    int StartSub(EnemyScriptState *state, short sub);

    // The file, as loaded.
    EnemyScriptHeader *file;
    // Points at the subroutine table in `file`.
    void **subs;
    // The header's unk_04, as a pointer.
    void *unk_08;
};
extern EnemyScripts g_EnemyScripts;
