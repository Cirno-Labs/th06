// The file from 0x420330: start-up, the window and the display.

#include <stdio.h>

#include "unsorted.h"

void PresentFrame()
{
    if (g_Unknown6c6d18.d3dDevice->Present(NULL, NULL, NULL, NULL) < 0)
    {
        g_SpriteEngine->ReleaseSurfaces();
        g_Unknown6c6d18.d3dDevice->Reset(&g_Unknown6c6d18.presentParams);
        Func421420();
        g_Unknown6c6d18.unk_198 = 2;
    }
    g_SpriteEngine->DoPendingCopy();
    if (g_Unknown6c6d18.unk_198 != 0)
    {
        g_Unknown6c6d18.unk_198--;
    }
}

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

void CreateGameWindow(HINSTANCE instance)
{
    WNDCLASSA wc;
    int width;
    int height;

    memset(&wc, 0, sizeof(wc));
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hInstance = instance;
    wc.lpfnWndProc = WindowProc;
    g_GameWindow.isAppActive = 0;
    g_GameWindow.isAppInactive = 0;
    wc.lpszClassName = "BASE";
    RegisterClassA(&wc);
    if (g_Unknown6c6d18.cfg.windowed == 0)
    {
        width = 640;
        height = 480;
        g_GameWindow.hwnd = CreateWindowExA(0, "BASE", "東方紅魔郷　～ the Embodiment of Scarlet Devil", WS_OVERLAPPEDWINDOW, 0,
                                 0, width, height, NULL, NULL, instance, NULL);
    }
    else
    {
        width = GetSystemMetrics(SM_CXDLGFRAME) * 2 + 640;
        height = GetSystemMetrics(SM_CYDLGFRAME) * 2 + 480 + GetSystemMetrics(SM_CYCAPTION);
        g_GameWindow.hwnd = CreateWindowExA(0, "BASE", "東方紅魔郷　～ the Embodiment of Scarlet Devil",
                                 WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                                 NULL, NULL, instance, NULL);
    }
    g_Unknown6c6d18.hwnd = g_GameWindow.hwnd;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case MM_MOM_DONE:
        if (g_Unknown6c6d18.midi != NULL)
        {
            g_Unknown6c6d18.midi->ReleaseHeader((LPMIDIHDR)lParam);
        }
        break;
    case WM_ACTIVATEAPP:
        g_GameWindow.isAppActive = wParam;
        if (g_GameWindow.isAppActive != 0)
        {
            g_GameWindow.isAppInactive = 0;
        }
        else
        {
            g_GameWindow.isAppInactive = 1;
        }
        break;
    case WM_SETCURSOR:
        if (g_Unknown6c6d18.cfg.windowed == 0)
        {
            if (g_GameWindow.isAppInactive != 0)
            {
                SetCursor(LoadCursorA(NULL, IDC_ARROW));
                ShowCursor(TRUE);
            }
            else
            {
                ShowCursor(FALSE);
                SetCursor(NULL);
            }
        }
        else
        {
            SetCursor(LoadCursorA(NULL, IDC_ARROW));
            ShowCursor(TRUE);
        }
        return 1;
    case WM_CLOSE:
        g_GameWindow.isClosing = 1;
        return 1;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
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

// The functions from here on are inline but defined after their callers,
// so they are called rather than expanded, and each call site still
// reserves their locals' slots.
inline void SpriteEngine::ReleaseSurfaces()
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

inline void SpriteEngine::DoPendingCopy()
{
    if (this->pendingCopyTexture >= 0)
    {
        this->CopyBackBufferToTexture(this->pendingCopyTexture, this->pendingCopyLeft, this->pendingCopyTop,
                                      this->pendingCopyWidth, this->pendingCopyHeight);
        this->pendingCopyTexture = -1;
    }
}
