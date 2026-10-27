// The file with EnemyScripts, from 0x407340.

#include "enemyscript.h"
#include "unsorted.h"

int EnemyScripts::Load(char *path)
{
    int i;

    this->file = (EnemyScriptHeader *)Func41e290(path, 0);
    if (this->file == NULL)
    {
        Func41e4d0(&g_Unknown69d998, "敵データの読み込みに失敗しました、データが壊れてるか失われています\n");
        return -1;
    }
    this->file->unk_04 += (int)this->file;
    this->subs = (void **)((char *)this->file + sizeof(EnemyScriptHeader));
    for (i = 0; i < this->file->subCount; i++)
    {
        this->subs[i] = (char *)this->subs[i] + (int)this->file;
    }
    this->unk_08 = (void *)this->file->unk_04;
    return 0;
}

void EnemyScripts::Release()
{
    if (this->file != NULL)
    {
        MemFree(this->file);
    }
    this->file = NULL;
}

int EnemyScripts::StartSub(EnemyScriptState *state, short sub)
{
    state->code = this->subs[sub];
    state->timer.Set(0);
    state->sub = sub;
    return 0;
}
