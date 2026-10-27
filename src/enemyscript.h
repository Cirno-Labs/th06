// Enemy scripts, loaded from the stage's enemy data file.

#pragma once

#include <windows.h>

#include "common.h"

// The enemy script file, the global at 0x487e50: a header, then a table
// of subroutines. Load turns the file's offsets into pointers.
class EnemyScripts
{
public:
    // Frees the file.
    void Release();

    // The file, as loaded.
    void *file;
    // Points at the subroutine table in `file`.
    void **subs;
};
extern EnemyScripts g_EnemyScripts;
