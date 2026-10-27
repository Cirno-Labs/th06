// The file with EnemyScripts, from 0x4071b0 at least.

#include "enemyscript.h"

void EnemyScripts::Release()
{
    if (this->file != NULL)
    {
        MemFree(this->file);
    }
    this->file = NULL;
}
