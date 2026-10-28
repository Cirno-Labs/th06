// The file from 0x420330: start-up, the window and the display.

#include <math.h>
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

int InitD3dDevice()
{
    float fovAngle;
    float aspectXY;
    float halfWidth;
    float halfHeight4;
    float dist;
    D3DPRESENT_PARAMETERS params3;
    D3DDISPLAYMODE displayMode2;
    bool checkTextureFormat2 = true;

    memset(&params3, 0, sizeof(params3));
    g_Unknown6c6d18.d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode2);
    if (g_Unknown6c6d18.cfg.windowed == 0)
    {
        if (g_Unknown6c6d18.cfg.flags.force16BitTextures == 1)
        {
            params3.BackBufferFormat = D3DFMT_R5G6B5;
            g_Unknown6c6d18.cfg.colorMode16Bits = 1;
        }
        else if (g_Unknown6c6d18.cfg.colorMode16Bits == 0xff)
        {
            if (displayMode2.Format == D3DFMT_X8R8G8B8 || displayMode2.Format == D3DFMT_A8R8G8B8)
            {
                params3.BackBufferFormat = D3DFMT_X8R8G8B8;
                g_Unknown6c6d18.cfg.colorMode16Bits = 0;
                Func41e4d0(&g_Unknown69d998, "初回起動、画面を 32Bits で初期化しました\n");
            }
            else
            {
                params3.BackBufferFormat = D3DFMT_R5G6B5;
                g_Unknown6c6d18.cfg.colorMode16Bits = 1;
                Func41e4d0(&g_Unknown69d998, "初回起動、画面を 16Bits で初期化しました\n");
            }
        }
        else if (g_Unknown6c6d18.cfg.colorMode16Bits == 0)
        {
            params3.BackBufferFormat = D3DFMT_X8R8G8B8;
        }
        else
        {
            params3.BackBufferFormat = D3DFMT_R5G6B5;
        }
        if (g_Unknown6c6d18.cfg.flags.force60Fps == 0)
        {
            params3.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        }
        else
        {
            params3.FullScreen_RefreshRateInHz = 60;
            params3.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            Func41e4d0(&g_Unknown69d998, "リフレッシュレートを60Hzに変更します\n");
        }
        if (g_Unknown6c6d18.cfg.frameSkip == 0)
        {
            params3.SwapEffect = D3DSWAPEFFECT_FLIP;
        }
        else
        {
            params3.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
        }
    }
    else
    {
        params3.BackBufferFormat = displayMode2.Format;
        params3.SwapEffect = D3DSWAPEFFECT_COPY;
        params3.Windowed = TRUE;
    }
    params3.BackBufferWidth = 640;
    params3.BackBufferHeight = 480;
    params3.EnableAutoDepthStencil = TRUE;
    params3.AutoDepthStencilFormat = D3DFMT_D16;
    params3.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    g_Unknown6c6d18.lockableBackBuffer = 1;
    memcpy(&g_Unknown6c6d18.presentParams, &params3, sizeof(params3));
retry:
    if (g_Unknown6c6d18.cfg.flags.referenceRasterizer)
    {
        goto reference;
    }
    if (g_Unknown6c6d18.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_GameWindow.hwnd,
                                          D3DCREATE_HARDWARE_VERTEXPROCESSING, &params3,
                                          &g_Unknown6c6d18.d3dDevice) < 0)
    {
        Func41e4d0(&g_Unknown69d998, "T&L HAL は使用できないようです\n");
        if (g_Unknown6c6d18.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_GameWindow.hwnd,
                                              D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params3,
                                              &g_Unknown6c6d18.d3dDevice) < 0)
        {
            Func41e4d0(&g_Unknown69d998, "HAL も使用できないようです\n");
        reference:
            if (g_Unknown6c6d18.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_GameWindow.hwnd,
                                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params3,
                                                  &g_Unknown6c6d18.d3dDevice) < 0)
            {
                if (g_Unknown6c6d18.cfg.flags.force60Fps && g_Unknown6c6d18.presentImmediate == 0)
                {
                    Func41e4d0(&g_Unknown69d998, "リフレッシュレートが変更できません、vsync 非同期に変更します\n");
                    params3.FullScreen_RefreshRateInHz = 0;
                    g_Unknown6c6d18.presentImmediate = 1;
                    params3.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                    goto retry;
                }
                if (params3.Flags == D3DPRESENTFLAG_LOCKABLE_BACKBUFFER)
                {
                    Func41e4d0(&g_Unknown69d998, "バックバッファをロック不可能にしてみます\n");
                    params3.Flags = 0;
                    g_Unknown6c6d18.lockableBackBuffer = 0;
                    goto retry;
                }
                Func41e620(&g_Unknown69d998, "Direct3D の初期化に失敗、これではゲームは出来ません\n");
                if (g_Unknown6c6d18.d3d != NULL)
                {
                    g_Unknown6c6d18.d3d->Release();
                    g_Unknown6c6d18.d3d = NULL;
                }
                return 1;
            }
            else
            {
                Func41e4d0(&g_Unknown69d998, "REF で動作しますが、重すぎて恐らくゲームになりません...\n");
                g_Unknown6c6d18.hardwareTnL = 0;
                checkTextureFormat2 = false;
            }
        }
        else
        {
            Func41e4d0(&g_Unknown69d998, "HAL で動作します\n");
            g_Unknown6c6d18.hardwareTnL = 0;
        }
    }
    else
    {
        Func41e4d0(&g_Unknown69d998, "T&L HAL で動作しま～す\n");
        g_Unknown6c6d18.hardwareTnL = 1;
    }
    halfWidth = 320.0f;
    halfHeight4 = 240.0f;
    aspectXY = 640.0f / 480.0f;
    fovAngle = 0.5235988f;
    dist = halfHeight4 / tanf(fovAngle / 2.0f);
    {
        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
        D3DXVECTOR3 lookAt(halfWidth, -halfHeight4, 0.0f);
        D3DXVECTOR3 from(halfWidth, -halfHeight4, -dist);
        D3DXMatrixLookAtLH(&g_Unknown6c6d18.viewMatrix, &from, &lookAt, &up);
        D3DXMatrixPerspectiveFovLH(&g_Unknown6c6d18.projectionMatrix, fovAngle, aspectXY, 100.0f, 10000.0f);
    }
    g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_VIEW, &g_Unknown6c6d18.viewMatrix);
    g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_PROJECTION, &g_Unknown6c6d18.projectionMatrix);
    g_Unknown6c6d18.d3dDevice->GetViewport(&g_Unknown6c6d18.viewport);
    g_Unknown6c6d18.d3dDevice->GetDeviceCaps(&g_Unknown6c6d18.caps);
    if (g_Unknown6c6d18.cfg.flags.unk0 == 0 && (g_Unknown6c6d18.caps.TextureOpCaps & D3DTEXOPCAPS_ADD) == 0)
    {
        Func41e4d0(&g_Unknown69d998, "D3DTEXOPCAPS_ADD をサポートしていません、色加算エミュレートモードで動作します\n");
        g_Unknown6c6d18.cfg.flags.unk0 = 1;
    }
    if (g_Unknown6c6d18.UsesPresentImmediate() &&
        (g_Unknown6c6d18.caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) == 0)
    {
        Func41e4d0(&g_Unknown69d998, "ビデオカードが非同期フリップをサポートしていません、Force60Frameで動作できません\n");
        g_Unknown6c6d18.cfg.flags.force60Fps = 0;
    }
    if (g_Unknown6c6d18.cfg.flags.force16BitTextures == 0 && checkTextureFormat2)
    {
        if (g_Unknown6c6d18.d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, params3.BackBufferFormat,
                                                   0, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8) == D3D_OK)
        {
            g_Unknown6c6d18.hasA8R8G8B8Textures = 1;
        }
        else
        {
            g_Unknown6c6d18.hasA8R8G8B8Textures = 0;
            g_Unknown6c6d18.cfg.flags.force16BitTextures = 1;
            Func41e4d0(&g_Unknown69d998, "D3DFMT_A8R8G8B8 をサポートしていません、減色モードで動作します\n");
        }
    }
    Func421420();
    ClearViewport(0);
    g_GameWindow.isClosing = 0;
    g_Unknown6c6d18.unk_1a4 = 0;
    g_Unknown6c6d18.gameSpeed = 0.0f;
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
