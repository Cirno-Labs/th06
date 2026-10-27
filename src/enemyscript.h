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

// The enemy script file, the global at 0x487e50: a header, then a table
// of subroutines. Load turns the file's offsets into pointers.
class EnemyScripts
{
public:
    // Frees the file.
    void Release();
    // Starts subroutine `sub` on `state`. Returns 0.
    int StartSub(EnemyScriptState *state, short sub);

    // The file, as loaded.
    void *file;
    // Points at the subroutine table in `file`.
    void **subs;
};
extern EnemyScripts g_EnemyScripts;
