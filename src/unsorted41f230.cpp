// The file from 0x420330: start-up, the window and the display.

#include <math.h>
#include <stdio.h>

#include "chain.h"
#include "features.h"
#include "stage.h"
#include "unsorted.h"

// The game's data, as the original defines it here. The DLL build uses
// the original's copies instead.
#ifdef TH06_EXE
GameWindow g_GameWindow;
#endif // TH06_EXE

#if FEAT_ENGLISH
#define CONFIG_FILE "th06e.cfg"
#define WINDOW_TITLE EN_WINDOW_TITLE
#else
#define CONFIG_FILE "東方紅魔郷.cfg"
#define WINDOW_TITLE "東方紅魔郷　～ the Embodiment of Scarlet Devil"
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HRESULT resetResult;
    MSG message;
    HRESULT testResult;
    int renderResult = 0;

#if FEAT_FRAME_CAP
    FrameCapInit();
#endif
    if (CheckSingleInstance() != 0)
    {
        g_Log.Flush();
        return 1;
    }
    g_App.hInstance = hInstance;
    if (g_App.LoadConfig(CONFIG_FILE) != 0)
    {
        g_Log.Flush();
        return -1;
    }
#if FEAT_FRAME_CAP
    // vpatch draws every frame.
    g_App.cfg.frameSkip = 0;
#endif
    if (CreateD3d() != 0)
    {
        g_Log.Flush();
        return 1;
    }
    SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0, &g_GameWindow.screenSaveActive, 0);
    SystemParametersInfoA(SPI_GETLOWPOWERACTIVE, 0, &g_GameWindow.lowPowerActive, 0);
    SystemParametersInfoA(SPI_GETPOWEROFFACTIVE, 0, &g_GameWindow.powerOffActive, 0);
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, 0, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, 0, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, 0, NULL, SPIF_SENDCHANGE);
restart:
    CreateGameWindow(hInstance);
    if (InitD3dDevice() != 0)
    {
        g_Log.Flush();
        return 1;
    }
    g_SoundPlayer.Initialize(g_GameWindow.hwnd);
    InitJoystick();
    ResetKeyboard();
    g_SpriteEngine = new SpriteEngine();
    if (App::RegisterChain() != 0)
    {
        goto stop;
    }
    if (g_App.cfg.windowed == 0)
    {
        ShowCursor(FALSE);
    }
    g_GameWindow.curFrame = 0;
    while (g_GameWindow.isClosing == 0)
    {
        if (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        else
        {
            testResult = g_App.d3dDevice->TestCooperativeLevel();
            if (testResult == D3D_OK)
            {
                renderResult = g_GameWindow.RunFrame();
                if (renderResult != 0)
                {
                    goto stop;
                }
            }
            else if (testResult == D3DERR_DEVICENOTRESET)
            {
                g_SpriteEngine->ReleaseSurfaces();
                resetResult = g_App.d3dDevice->Reset(&g_App.presentParams);
                if (resetResult != 0)
                {
                    goto stop;
                }
                SetDefaultRenderStates();
                g_App.redrawFrames = 3;
            }
        }
    }
stop:
    g_Chain.Release();
    g_SoundPlayer.Release();
    delete g_SpriteEngine;
    g_SpriteEngine = NULL;
    if (g_App.d3dDevice != NULL)
    {
        g_App.d3dDevice->Release();
        g_App.d3dDevice = NULL;
    }
    ShowWindow(g_GameWindow.hwnd, SW_HIDE);
    MoveWindow(g_GameWindow.hwnd, 0, 0, 0, 0, FALSE);
    DestroyWindow(g_GameWindow.hwnd);
    if (renderResult == 2)
    {
        g_Log.cursor = g_Log.buffer;
        *g_Log.cursor = '\0';
        LogPrintf(&g_Log, "再起動を要するオプションが変更されたので再起動します\n");
        if (g_App.cfg.windowed == 0)
        {
            ShowCursor(TRUE);
        }
        goto restart;
    }
    WriteToFile(CONFIG_FILE, &g_App.cfg, sizeof(GameConfig));
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, g_GameWindow.screenSaveActive, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, g_GameWindow.lowPowerActive, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, g_GameWindow.powerOffActive, NULL, SPIF_SENDCHANGE);
    if (g_App.d3d != NULL)
    {
        g_App.d3d->Release();
        g_App.d3d = NULL;
    }
    ShowCursor(TRUE);
    g_Log.Flush();
    return 0;
}

int GameWindow::RunFrame()
{
    DWORD time2;
    double ratio2;
    double delta;
    double now;
    D3DVIEWPORT8 viewport;
    int res;
    static int s_frames;
    static double s_lastFrameTime;

    if (this->isAppActive == 0)
    {
#if FEAT_FRAME_CAP
        Sleep(15);
#endif
        return 0;
    }
    if (this->curFrame == 0)
    {
    loop:
        if (g_App.cfg.frameSkip <= this->curFrame)
        {
            if (g_App.ClearsBackBuffer())
            {
                viewport.X = 0;
                viewport.Y = 0;
                viewport.Width = 640;
                viewport.Height = 480;
                viewport.MinZ = 0.0f;
                viewport.MaxZ = 1.0f;
                g_App.d3dDevice->SetViewport(&viewport);
                g_App.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, g_Stage.fog.color, 1.0f,
                                                 0);
                g_App.d3dDevice->SetViewport(&g_App.viewport);
            }
            g_App.d3dDevice->BeginScene();
            g_Chain.RunDrawChain();
            g_App.d3dDevice->EndScene();
            g_App.d3dDevice->SetTexture(0, NULL);
        }
        g_App.viewport.X = 0;
        g_App.viewport.Y = 0;
        g_App.viewport.Width = 640;
        g_App.viewport.Height = 480;
        g_App.d3dDevice->SetViewport(&g_App.viewport);
#if FEAT_FRAME_CAP
        FrameCapBeforeCalc();
#endif
        res = g_Chain.RunCalcChain();
        g_SoundPlayer.PlayQueuedSounds();
        if (res == 0)
        {
            return 1;
        }
        if (res == -1)
        {
            return 2;
        }
        this->curFrame++;
    }
#if FEAT_FRAME_CAP
    // The frame was presented in FrameCapBeforeCalc, on time: only the
    // bookkeeping after a present is left.
    if (this->curFrame == 0)
    {
        return 0;
    }
    g_App.gameSpeed = 1.0f;
    s_lastFrameTime = timeGetTime();
    if (g_App.cfg.frameSkip >= this->curFrame)
    {
        goto loop;
    }
    goto present;
#endif
    if ((g_App.cfg.windowed != 0 || g_App.UsesPresentImmediate()) && this->curFrame != 0)
    {
        g_App.gameSpeed = 1.0f;
        timeBeginPeriod(1);
        now = timeGetTime();
        if (now < s_lastFrameTime)
        {
            s_lastFrameTime = now;
        }
        delta = fabs(now - s_lastFrameTime);
        timeEndPeriod(1);
        if (delta >= 1000.0 / 60.0)
        {
            do
            {
                s_lastFrameTime += 1000.0 / 60.0;
                delta -= 1000.0 / 60.0;
            } while (delta >= 1000.0 / 60.0);
            if (g_App.cfg.frameSkip < this->curFrame)
            {
                goto present;
            }
            goto loop;
        }
    }
    if (g_App.cfg.windowed == 0 && !g_App.UsesPresentImmediate())
    {
        if (g_App.cfg.frameSkip >= this->curFrame)
        {
            PresentFrame();
            goto loop;
        }
    present:
        PresentFrame();
        if (g_App.gameSpeed == 0.0f)
        {
            if (s_frames >= 2)
            {
                timeBeginPeriod(1);
                time2 = timeGetTime();
                if (time2 < g_App.lastFrameTime)
                {
                    g_App.lastFrameTime = time2;
                }
                ratio2 = time2 - g_App.lastFrameTime;
                ratio2 = ratio2 * 60.0 / 2.0 / 1000.0;
                ratio2 /= g_App.cfg.frameSkip + 1;
                if (ratio2 >= 0.865)
                {
                    ratio2 = 1.0;
                }
                else if (ratio2 >= 0.6)
                {
                    ratio2 = 0.8;
                }
                else
                {
                    ratio2 = 0.5;
                }
                g_App.frameAdvance = ratio2;
                g_App.lastFrameTime = time2;
                timeEndPeriod(1);
                s_frames = 0;
            }
        }
        else
        {
            g_App.frameAdvance = g_App.gameSpeed;
        }
        this->curFrame = 0;
        s_frames++;
    }
    return 0;
}

void PresentFrame()
{
    // With FEAT_FRAME_CAP, FrameCapBeforeCalc presents instead.
#if !FEAT_FRAME_CAP
    if (g_App.d3dDevice->Present(NULL, NULL, NULL, NULL) < 0)
    {
        g_SpriteEngine->ReleaseSurfaces();
        g_App.d3dDevice->Reset(&g_App.presentParams);
        SetDefaultRenderStates();
        g_App.redrawFrames = 2;
    }
#endif
    g_SpriteEngine->DoPendingCopy();
    if (g_App.redrawFrames != 0)
    {
        g_App.redrawFrames--;
    }
}

int CreateD3d()
{
    g_App.d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (g_App.d3d == NULL)
    {
        LogError(&g_Log, "Direct3D オブジェクトは何故か作成出来なかった\n");
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
    if (g_App.cfg.windowed == 0)
    {
        width = 640;
        height = 480;
        g_GameWindow.hwnd = CreateWindowExA(0, "BASE", WINDOW_TITLE, WS_OVERLAPPEDWINDOW, 0,
                                 0, width, height, NULL, NULL, instance, NULL);
    }
    else
    {
        width = GetSystemMetrics(SM_CXDLGFRAME) * 2 + 640;
        height = GetSystemMetrics(SM_CYDLGFRAME) * 2 + 480 + GetSystemMetrics(SM_CYCAPTION);
        g_GameWindow.hwnd = CreateWindowExA(0, "BASE", WINDOW_TITLE,
                                 WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                                 NULL, NULL, instance, NULL);
    }
    g_App.hwnd = g_GameWindow.hwnd;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case MM_MOM_DONE:
        if (g_App.midi != NULL)
        {
            g_App.midi->ReleaseHeader((LPMIDIHDR)lParam);
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
        if (g_App.cfg.windowed == 0)
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
    g_App.d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode2);
    if (g_App.cfg.windowed == 0)
    {
        if (g_App.cfg.flags.force16BitTextures == 1)
        {
            params3.BackBufferFormat = D3DFMT_R5G6B5;
            g_App.cfg.colorMode16Bits = 1;
        }
        else if (g_App.cfg.colorMode16Bits == 0xff)
        {
            if (displayMode2.Format == D3DFMT_X8R8G8B8 || displayMode2.Format == D3DFMT_A8R8G8B8)
            {
                params3.BackBufferFormat = D3DFMT_X8R8G8B8;
                g_App.cfg.colorMode16Bits = 0;
                LogPrintf(&g_Log, "初回起動、画面を 32Bits で初期化しました\n");
            }
            else
            {
                params3.BackBufferFormat = D3DFMT_R5G6B5;
                g_App.cfg.colorMode16Bits = 1;
                LogPrintf(&g_Log, "初回起動、画面を 16Bits で初期化しました\n");
            }
        }
        else if (g_App.cfg.colorMode16Bits == 0)
        {
            params3.BackBufferFormat = D3DFMT_X8R8G8B8;
        }
        else
        {
            params3.BackBufferFormat = D3DFMT_R5G6B5;
        }
#if FEAT_FRAME_CAP
        // vpatch presents without waiting for vsync and keeps the
        // display's refresh rate.
        if (g_App.cfg.flags.force60Fps != 0)
        {
            LogPrintf(&g_Log, "リフレッシュレートを60Hzに変更します\n");
        }
        params3.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        params3.SwapEffect = D3DSWAPEFFECT_DISCARD;
#else
        if (g_App.cfg.flags.force60Fps == 0)
        {
            params3.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        }
        else
        {
            params3.FullScreen_RefreshRateInHz = 60;
            params3.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            LogPrintf(&g_Log, "リフレッシュレートを60Hzに変更します\n");
        }
        if (g_App.cfg.frameSkip == 0)
        {
            params3.SwapEffect = D3DSWAPEFFECT_FLIP;
        }
        else
        {
            params3.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
        }
#endif
    }
    else
    {
        params3.BackBufferFormat = displayMode2.Format;
#if FEAT_FRAME_CAP
        params3.SwapEffect = D3DSWAPEFFECT_DISCARD;
#else
        params3.SwapEffect = D3DSWAPEFFECT_COPY;
#endif
        params3.Windowed = TRUE;
    }
    params3.BackBufferWidth = 640;
    params3.BackBufferHeight = 480;
    params3.EnableAutoDepthStencil = TRUE;
    params3.AutoDepthStencilFormat = D3DFMT_D16;
    params3.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    g_App.lockableBackBuffer = 1;
    memcpy(&g_App.presentParams, &params3, sizeof(params3));
retry:
    if (g_App.cfg.flags.referenceRasterizer)
    {
        goto reference;
    }
    if (g_App.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_GameWindow.hwnd,
                                          D3DCREATE_HARDWARE_VERTEXPROCESSING, &params3,
                                          &g_App.d3dDevice) < 0)
    {
        LogPrintf(&g_Log, "T&L HAL は使用できないようです\n");
        if (g_App.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_GameWindow.hwnd,
                                              D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params3,
                                              &g_App.d3dDevice) < 0)
        {
            LogPrintf(&g_Log, "HAL も使用できないようです\n");
        reference:
            if (g_App.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, g_GameWindow.hwnd,
                                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params3,
                                                  &g_App.d3dDevice) < 0)
            {
                if (g_App.cfg.flags.force60Fps && g_App.presentImmediate == 0)
                {
                    LogPrintf(&g_Log, "リフレッシュレートが変更できません、vsync 非同期に変更します\n");
                    params3.FullScreen_RefreshRateInHz = 0;
                    g_App.presentImmediate = 1;
                    params3.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                    goto retry;
                }
                if (params3.Flags == D3DPRESENTFLAG_LOCKABLE_BACKBUFFER)
                {
                    LogPrintf(&g_Log, "バックバッファをロック不可能にしてみます\n");
                    params3.Flags = 0;
                    g_App.lockableBackBuffer = 0;
                    goto retry;
                }
                LogError(&g_Log, "Direct3D の初期化に失敗、これではゲームは出来ません\n");
                if (g_App.d3d != NULL)
                {
                    g_App.d3d->Release();
                    g_App.d3d = NULL;
                }
                return 1;
            }
            else
            {
                LogPrintf(&g_Log, "REF で動作しますが、重すぎて恐らくゲームになりません...\n");
                g_App.hardwareTnL = 0;
                checkTextureFormat2 = false;
            }
        }
        else
        {
            LogPrintf(&g_Log, "HAL で動作します\n");
            g_App.hardwareTnL = 0;
        }
    }
    else
    {
        LogPrintf(&g_Log, "T&L HAL で動作しま～す\n");
        g_App.hardwareTnL = 1;
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
        D3DXMatrixLookAtLH(&g_App.viewMatrix, &from, &lookAt, &up);
        D3DXMatrixPerspectiveFovLH(&g_App.projectionMatrix, fovAngle, aspectXY, 100.0f, 10000.0f);
    }
    g_App.d3dDevice->SetTransform(D3DTS_VIEW, &g_App.viewMatrix);
    g_App.d3dDevice->SetTransform(D3DTS_PROJECTION, &g_App.projectionMatrix);
    g_App.d3dDevice->GetViewport(&g_App.viewport);
    g_App.d3dDevice->GetDeviceCaps(&g_App.caps);
    if (g_App.cfg.flags.emulateColorAdd == 0 && (g_App.caps.TextureOpCaps & D3DTEXOPCAPS_ADD) == 0)
    {
        LogPrintf(&g_Log, "D3DTEXOPCAPS_ADD をサポートしていません、色加算エミュレートモードで動作します\n");
        g_App.cfg.flags.emulateColorAdd = 1;
    }
    if (g_App.UsesPresentImmediate() &&
        (g_App.caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) == 0)
    {
        LogPrintf(&g_Log, "ビデオカードが非同期フリップをサポートしていません、Force60Frameで動作できません\n");
        g_App.cfg.flags.force60Fps = 0;
    }
    if (g_App.cfg.flags.force16BitTextures == 0 && checkTextureFormat2)
    {
        if (g_App.d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, params3.BackBufferFormat,
                                                   0, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8) == D3D_OK)
        {
            g_App.hasA8R8G8B8Textures = 1;
        }
        else
        {
            g_App.hasA8R8G8B8Textures = 0;
            g_App.cfg.flags.force16BitTextures = 1;
            LogPrintf(&g_Log, "D3DFMT_A8R8G8B8 をサポートしていません、減色モードで動作します\n");
        }
    }
    SetDefaultRenderStates();
    ClearViewport(0);
    g_GameWindow.isClosing = 0;
    g_App.lastFrameTime = 0;
    g_App.gameSpeed = 0.0f;
    return 0;
}

void SetDefaultRenderStates()
{
    float fogDist;
    float fogDensity;

    if (g_App.cfg.flags.noDepthTest == 0)
    {
        g_App.d3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    }
    else
    {
        g_App.d3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    }
    g_App.d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_App.d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_App.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    if (g_App.cfg.flags.noGouraudShading == 0)
    {
        g_App.d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    }
    else
    {
        g_App.d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
    }
    g_App.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_App.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    if (g_App.cfg.flags.noDepthTest == 0)
    {
        g_App.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    }
    else
    {
        g_App.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    }
    g_App.d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_App.d3dDevice->SetRenderState(D3DRS_ALPHAREF, 4);
    g_App.d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    if (g_App.cfg.flags.noFog == 0)
    {
        g_App.d3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    }
    else
    {
        g_App.d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    }
    fogDensity = 1.0f;
    g_App.d3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)&fogDensity);
    g_App.d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    g_App.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0xffa0a0a0);
    fogDist = 1000.0f;
    g_App.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&fogDist);
    fogDist = 5000.0f;
    g_App.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&fogDist);
    if (g_App.cfg.flags.noColorBlending == 0)
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    }
    else
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    }
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    if (g_App.cfg.flags.noVertexBuffer == 0)
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    }
    else
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    }
    if (g_App.cfg.flags.noColorBlending == 0)
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }
    else
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    }
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    if (g_App.cfg.flags.noVertexBuffer == 0)
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    }
    else
    {
        g_App.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    }
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    g_App.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
    if (g_SpriteEngine != NULL)
    {
        g_SpriteEngine->ClearBlendMode();
        g_SpriteEngine->ClearColorOp();
        g_SpriteEngine->ClearVertexFormat();
        g_SpriteEngine->ClearTexture();
    }
    g_Stage.fogColorChanged = 1;
}

// The functions from here on are inline but defined after their callers,
// so they are called rather than expanded, and each call site still
// reserves their locals' slots.
inline int CheckSingleInstance()
{
#ifdef TH06_EXE
    // The original's is a static of this function.
    static HANDLE g_AppMutex;
#endif // TH06_EXE
    g_AppMutex = CreateMutexA(NULL, TRUE, "Touhou Koumakyou App");
    if (g_AppMutex == NULL)
    {
        return -1;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        LogError(&g_Log, "二つは起動できません\n");
        return -1;
    }
    return 0;
}

inline void Log::Flush()
{
    FILE *file;

    if (this->cursor != this->buffer)
    {
        LogPrintf(this, "---------------------------------------------------------- \n");
        if (this->hasError)
        {
            MessageBoxA(NULL, this->buffer, "log", MB_ICONERROR);
        }
        file = fopen("./log.txt", "wt");
        fprintf(file, this->buffer);
        fclose(file);
    }
}

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
