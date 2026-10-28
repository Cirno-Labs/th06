// The file from 0x420330: start-up, the window and the display.

#include <stdio.h>

#include "unsorted.h"

int CreateD3d()
{
    g_Unknown6c6d18.d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (g_Unknown6c6d18.d3d == NULL)
    {
        Func41e620(&g_Unknown69d998, "Direct3D オブジェクトは何故か作成出来なかった\n");
        return 1;
    }
    return 0;
}

int CheckSingleInstance()
{
    g_AppMutex = CreateMutexA(NULL, TRUE, "Touhou Koumakyou App");
    if (g_AppMutex == NULL)
    {
        return -1;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        Func41e620(&g_Unknown69d998, "二つは起動できません\n");
        return -1;
    }
    return 0;
}

void Unknown69d998::Flush()
{
    FILE *file;

    if (this->cursor != this->buffer)
    {
        Func41e4d0(this, "---------------------------------------------------------- \n");
        if (this->hasError)
        {
            MessageBoxA(NULL, this->buffer, "log", MB_ICONERROR);
        }
        file = fopen("./log.txt", "wt");
        fprintf(file, this->buffer);
        fclose(file);
    }
}

void SpriteEngine::ReleaseSurfaces()
{
    int i;

    for (i = 0; i < 0x20; i++)
    {
        if (this->surfaces[i] != NULL)
        {
            this->surfaces[i]->Release();
            this->surfaces[i] = NULL;
        }
    }
}
