// cflags: /Os
// The file starting at 0x42333b, up to 0x425e03. Built with /Os: its
// functions are laid out without alignment.

#include <stdio.h>
#include <string.h>

#include "chain.h"
#include "ending.h"
#include "features.h"
#include "musicscreen.h"
#include "resultscreen.h"
#include "timer.h"
#include "unsorted.h"

int App::RegisterChain()
{
    App *game = &g_App;
    ChainElem *elem;

    game->curScene = 0;
    game->scene = -1;
    game->frames = 0;
    elem = g_Chain.CreateElem((ChainCallback)App::OnUpdate);
    elem->arg = game;
    elem->addedCallback = (ChainCallback)App::AddedCallback;
    elem->deletedCallback = (ChainCallback)App::DeletedCallback;
    if (g_Chain.AddToCalcChain(elem, 0) != 0)
    {
        return -1;
    }
    elem = g_Chain.CreateElem((ChainCallback)App::OnDraw);
    elem->arg = game;
    g_Chain.AddToDrawChain(elem, 0xe);
    return 0;
}

// Updates the buttons, then sets up the scene `scene` asks for, tearing
// down the old one, before the chain runs the scene's own callbacks.
// Returns 4 when a scene fails to set up (or the game is to quit), 5
// for scene 5, otherwise 1.
int App::OnUpdate(App *arg)
{
    if (g_SoundPlayer.bgm != NULL)
    {
        g_SoundPlayer.bgm->UpdateFade();
    }
    g_PrevButtons = g_Buttons;
    g_Buttons = ReadButtons();
#if FEAT_FRAME_CAP
    g_FrameCapButtons = g_Buttons;
#endif
    g_ButtonRepeat = 0;
    if (g_PrevButtons == g_Buttons)
    {
        if (g_ButtonFrames >= 30)
        {
            if (g_ButtonFrames % 8 == 0)
            {
                g_ButtonRepeat = 1;
            }
            if (g_ButtonFrames >= 38)
            {
                g_ButtonFrames = 30;
            }
        }
        g_ButtonFrames++;
    }
    else
    {
        g_ButtonFrames = 0;
    }
    if (arg->curScene != arg->scene)
    {
        arg->prevScene = arg->curScene;
        switch (arg->curScene)
        {
        case 0:
        title:
            arg->scene = 1;
            g_App.d3dDevice->ResourceManagerDiscardBytes(0);
            if (SetUpTitleScene(0) != 0)
            {
                return 4;
            }
            break;
        case 1:
            switch (arg->scene)
            {
            case 4:
                return 4;
            case 2:
                if (GameManager::RegisterChain() != 0)
                {
                    return 4;
                }
                break;
            case 5:
                return 5;
            case 6:
                if (ResultScreen::RegisterChain(0) != 0)
                {
                    return 4;
                }
                break;
            case 9:
                if (MusicScreen::RegisterChain() != 0)
                {
                    return 4;
                }
                break;
            case 10:
                GameManager::RemoveChain();
                if (Ending::RegisterChain() != 0)
                {
                    return 4;
                }
                break;
            }
            break;
        case 6:
            switch (arg->scene)
            {
            case 4:
                return 4;
            case 1:
                arg->scene = 0;
                goto title;
            }
            break;
        case 2:
            switch (arg->scene)
            {
            case 4:
                return 4;
            case 1:
            toTitle:
                GameManager::RemoveChain();
                arg->scene = 0;
                SaveReplay(0, NULL);
                goto title;
            case 7:
                GameManager::RemoveChain();
                if (ResultScreen::RegisterChain(1) != 0)
                {
                    return 4;
                }
                break;
            case 3:
                GameManager::RemoveChain();
                if (GameManager::RegisterChain() != 0)
                {
                    return 4;
                }
                if (arg->scene == 1)
                {
                    goto toTitle;
                }
                arg->scene = 2;
                break;
            case 8:
                GameManager::RemoveChain();
                arg->scene = 0;
                SaveReplay(0, NULL);
                arg->scene = 1;
                g_App.d3dDevice->ResourceManagerDiscardBytes(0);
                if (SetUpTitleScene(1) != 0)
                {
                    return 4;
                }
                break;
            case 10:
                GameManager::RemoveChain();
                if (Ending::RegisterChain() != 0)
                {
                    return 4;
                }
                break;
            }
            break;
        case 7:
            switch (arg->scene)
            {
            case 4:
                SaveReplay(0, NULL);
                return 4;
            case 1:
                arg->scene = 0;
                SaveReplay(0, NULL);
                goto title;
            }
            break;
        case 9:
            switch (arg->scene)
            {
            case 4:
                return 4;
            case 1:
                arg->scene = 0;
                goto title;
            }
            break;
        case 10:
            switch (arg->scene)
            {
            case 4:
                return 4;
            case 1:
                arg->scene = 0;
                goto title;
            case 7:
                if (ResultScreen::RegisterChain(1) != 0)
                {
                    return 4;
                }
            }
        }
        g_Buttons = g_PrevButtons = g_ButtonRepeat = 0;
    }
    arg->curScene = arg->scene;
    arg->frames++;
    return 1;
}

int App::OnDraw(App *arg)
{
    g_SpriteEngine->ClearVertexFormat();
    g_SpriteEngine->ClearSpriteData();
    g_SpriteEngine->ClearTexture();
    g_SpriteEngine->ClearColorOp();
    g_SpriteEngine->ClearBlendMode();
    g_SpriteEngine->ClearZWrite();
    DrawFps();
    return 1;
}

int App::AddedCallback(App *arg)
{
    int i;

    for (i = 0; i < 0x10; i++)
    {
        arg->archives[i] = NULL;
    }
    g_Archives = arg->archives;
#if FEAT_ENGLISH
    if (arg->LoadArchive(0, "th06e_IN.dat") != 0)
#else
    if (arg->LoadArchive(0, "紅魔郷IN.dat") != 0)
#endif
    {
        return -1;
    }
#if FEAT_ENGLISH
    g_EnglishSpellCards = (char *)LoadFileData("spellcards.eng", 0);
#endif
    // Shows the logo for two frames.
    g_SpriteEngine->LoadSurface(0, "data/title/th06logo.jpg");
    g_SpriteEngine->CopySurfaceToBackBuffer(0, 0, 0, 0, 0);
    if (FAILED(g_App.d3dDevice->Present(NULL, NULL, NULL, NULL)))
    {
        g_App.d3dDevice->Reset(&g_App.presentParams);
    }
    g_SpriteEngine->CopySurfaceToBackBuffer(0, 0, 0, 0, 0);
    if (FAILED(g_App.d3dDevice->Present(NULL, NULL, NULL, NULL)))
    {
        g_App.d3dDevice->Reset(&g_App.presentParams);
    }
    g_SpriteEngine->ReleaseSurface(0);
    arg->startTime = timeGetTime();
    InitDirectInput(arg);
    arg->midi = new MidiPlayer();
    g_Rng.SetSeed(timeGetTime());
    g_SoundPlayer.LoadSounds();
    if (g_SpriteEngine->LoadAnm(0, "data/text.anm", 0x700) != 0)
    {
        return -1;
    }
    if (AsciiText::RegisterChain() != 0)
    {
        LogPrintf(&g_Log, "error : 文字の初期化に失敗しました\n");
        return -1;
    }
    arg->redrawFrames = 0;
    g_SpriteEngine->CreateQuad();
    CreateTextSurface();
    arg->ReleaseArchive(0);
#if FEAT_ENGLISH
    if (g_App.LoadArchive(1, "th06e_MD.dat") != 0)
#else
    if (g_App.LoadArchive(1, "紅魔郷MD.dat") != 0)
#endif
    {
        return -1;
    }
    return 0;
}

int InitDirectInput(App *game)
{
    HINSTANCE hInst = (HINSTANCE)GetWindowLongA(game->hwnd, GWL_HINSTANCE);

    if (game->cfg.flags.noDirectInput)
    {
        return -1;
    }
    if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID **)&game->dinput, NULL)))
    {
        game->dinput = NULL;
        LogPrintf(&g_Log, "DirectInput が使用できません\n");
        return -1;
    }
    if (FAILED(game->dinput->CreateDevice(GUID_SysKeyboard, &game->keyboard, NULL)))
    {
        if (game->dinput)
        {
            game->dinput->Release();
            game->dinput = NULL;
        }
        LogPrintf(&g_Log, "DirectInput が使用できません\n");
        return -1;
    }
    if (FAILED(game->keyboard->SetDataFormat(&c_dfDIKeyboard)))
    {
        if (game->keyboard)
        {
            game->keyboard->Release();
            game->keyboard = NULL;
        }
        if (game->dinput)
        {
            game->dinput->Release();
            game->dinput = NULL;
        }
        LogPrintf(&g_Log, "DirectInput SetDataFormat が使用できません\n");
        return -1;
    }
#if FEAT_FRAME_CAP
    // vpatch reads the keyboard and the pad in the background too.
    if (FAILED(game->keyboard->SetCooperativeLevel(game->hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
#else
    if (FAILED(game->keyboard->SetCooperativeLevel(game->hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY)))
#endif
    {
        if (game->keyboard)
        {
            game->keyboard->Release();
            game->keyboard = NULL;
        }
        if (game->dinput)
        {
            game->dinput->Release();
            game->dinput = NULL;
        }
        LogPrintf(&g_Log, "DirectInput SetCooperativeLevel が使用できません\n");
        return -1;
    }
    game->keyboard->Acquire();
    LogPrintf(&g_Log, "DirectInput は正常に初期化されました\n");
    game->dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
    if (game->joystick)
    {
        game->joystick->SetDataFormat(&c_dfDIJoystick2);
#if FEAT_FRAME_CAP
        game->joystick->SetCooperativeLevel(game->hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
#else
        game->joystick->SetCooperativeLevel(game->hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
#endif
        g_App.joystickCaps.dwSize = sizeof(DIDEVCAPS);
        game->joystick->GetCapabilities(&g_App.joystickCaps);
        game->joystick->EnumObjects(EnumAxesCallback, NULL, DIDFT_ALL);
        LogPrintf(&g_Log, "有効なパッドを発見しました\n");
    }
    return 0;
}

BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE *pdidoi, VOID *pContext)
{
    HWND hDlg = (HWND)pContext;
    DIPROPRANGE diprg;

    if (pdidoi->dwType & DIDFT_AXIS)
    {
        diprg.diph.dwSize = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow = DIPH_BYID;
        diprg.diph.dwObj = pdidoi->dwType;
        diprg.lMin = -1000;
        diprg.lMax = +1000;

        if (FAILED(g_App.joystick->SetProperty(DIPROP_RANGE, &diprg.diph)))
        {
            return DIENUM_STOP;
        }
    }
    return DIENUM_CONTINUE;
}

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
    HRESULT hr;

    if (g_App.joystick == NULL)
    {
        hr = g_App.dinput->CreateDevice(pdidInstance->guidInstance, &g_App.joystick, NULL);
        if (FAILED(hr))
        {
            return DIENUM_CONTINUE;
        }
    }
    return DIENUM_STOP;
}

// Ends with the DirectX 8 SDK Joystick sample's FreeDirectInput, for
// both devices.
int App::DeletedCallback(App *arg)
{
    int i;

    g_SpriteEngine->ReleaseVertexBuffer();
    for (i = 0; i < 0x10; i++)
    {
        arg->ReleaseArchive(i);
    }
    g_SpriteEngine->ReleaseAnm(0);
    AsciiText::RemoveChain();
    g_SoundPlayer.StopBgm();
    if (arg->midi != NULL)
    {
        arg->midi->Stop();
        delete arg->midi;
        arg->midi = NULL;
    }
    SaveReplay(0, NULL);
    ReleaseTextSurface();
    if (arg->keyboard)
    {
        arg->keyboard->Unacquire();
    }
    if (arg->keyboard)
    {
        arg->keyboard->Release();
        arg->keyboard = NULL;
    }
    if (arg->joystick)
    {
        arg->joystick->Unacquire();
    }
    if (arg->joystick)
    {
        arg->joystick->Release();
        arg->joystick = NULL;
    }
    if (arg->dinput)
    {
        arg->dinput->Release();
        arg->dinput = NULL;
    }
    return 0;
}

void DrawFps()
{
    static DWORD lastTime = timeGetTime();
    static DWORD frames;
    static char fpsText[0x100];
    DWORD now;
    float target;
    float fps;
    float duration;
    D3DXVECTOR3 pos;

#if FEAT_FRAME_CAP
    // vpatch shows the rate it measures at each present instead.
    if (FrameCapFps(&fps))
    {
#else
    now = timeGetTime();
    frames += 1 + g_App.cfg.frameSkip;
    if (now - lastTime >= 500)
    {
        duration = (now - lastTime) / 1000.0f;
        fps = frames / duration;
        lastTime = now;
        frames = 0;
#endif
        sprintf(fpsText, "%.02ffps", fps);
        if (g_GameManager.isPlaying)
        {
            target = 60.0f / g_App.gameSpeed;
            g_App.expectedFrames += target;
            if (target * 0.9f < fps)
            {
                g_App.achievedFrames += target;
            }
            else if (target * 0.7f < fps)
            {
                g_App.achievedFrames += target * 0.8f;
            }
            else if (target * 0.5f < fps)
            {
                g_App.achievedFrames += target * 0.6f;
            }
            else
            {
                g_App.achievedFrames += target * 0.5f;
            }
        }
    }
    if (g_App.hideFps == 0)
    {
        pos.x = 512.0f;
        pos.y = 464.0f;
        pos.z = 0.0f;
        g_AsciiText.AddString(&pos, fpsText);
    }
}

void SpriteEngine::ReleaseVertexBuffer()
{
    if (this->vertexBuffer != NULL)
    {
        this->vertexBuffer->Release();
        this->vertexBuffer = NULL;
    }
}

void Timer::Initialize()
{
    this->current = 0;
    this->previous = -1;
    this->subFrame = 0.0f;
}

void Timer::Add(int frames)
{
    if (g_App.gameSpeed > 0.99f)
    {
        this->current += frames;
    }
    else if (frames < 0)
    {
        this->Sub(-frames);
    }
    else
    {
        this->previous = this->current;
        this->subFrame += g_App.frameAdvance * frames;
        while (this->subFrame >= 1.0f)
        {
            this->current++;
            this->subFrame -= 1.0f;
        }
    }
}

void Timer::Sub(int frames)
{
    if (g_App.gameSpeed > 0.99f)
    {
        this->current -= frames;
    }
    else if (frames < 0)
    {
        this->Add(-frames);
    }
    else
    {
        this->previous = this->current;
        this->subFrame -= g_App.frameAdvance * frames;
        while (this->subFrame < 0.0f)
        {
            this->current--;
            this->subFrame += 1.0f;
        }
    }
}

void App::TickTimer(int *frames, float *subFrame)
{
    if (this->gameSpeed <= 0.99f)
    {
        *subFrame += this->frameAdvance;
        if (*subFrame >= 1.0f)
        {
            (*frames)++;
            *subFrame -= 1.0f;
        }
    }
    else
    {
        (*frames)++;
    }
}

void App::ReleaseArchive(int index)
{
    if (this->archives[index] == NULL)
    {
        return;
    }
    this->archives[index]->Release();
    delete this->archives[index];
    this->archives[index] = NULL;
}

int App::LoadArchive(int index, char *path)
{
    int idx;
    char name[0x80];

    if (this->archives[index] == NULL || strcmp(path, this->archivePaths[index]) != 0)
    {
        this->ReleaseArchive(index);
        this->archives[index] = new DatArchive();
        DebugPrint2("%s open ...\n", path);
        if (this->archives[index]->Open(path))
        {
            strcpy(this->archivePaths[index], path);
            sprintf(name, "ver%.4x.dat", 0x102);
            idx = this->archives[index]->FindEntry(name);
            if (idx < 0)
            {
                LogError(&g_Log, "error : データのバージョンが違います\n");
                return 1;
            }
        }
        else
        {
            delete this->archives[index];
            this->archives[index] = NULL;
            this->archives[index] = NULL;
        }
    }
    return 0;
}

// Sets the configuration's defaults; the option flags are left alone.
#define SET_DEFAULT_CONFIG() \
{ \
    FILE *wav; \
 \
    g_App.cfg.lives = 2; \
    g_App.cfg.bombs = 3; \
    g_App.cfg.colorMode16Bits = 0xff; \
    g_App.cfg.version = 0x102; \
    g_App.cfg.padThresholdX = 600; \
    g_App.cfg.padThresholdY = 600; \
    wav = fopen("bgm/th06_01.wav", "rb"); \
    if (wav != NULL) \
    { \
        g_App.cfg.musicMode = 1; \
        fclose(wav); \
    } \
    else \
    { \
        g_App.cfg.musicMode = 2; \
        DebugPrint2("wave データが無いので、midi にします\n"); \
    } \
    g_App.cfg.playSounds = 1; \
    g_App.cfg.defaultDifficulty = 1; \
    g_App.cfg.windowed = 0; \
    g_App.cfg.frameSkip = 0; \
    g_App.cfg.controllerMapping = g_ControllerMapping; \
}

int App::LoadConfig(char *path)
{
    GameConfig *file;

    memset(&g_App.cfg, 0, sizeof(GameConfig));
    g_App.cfg.flags.emulateColorAdd = 1;
    file = (GameConfig *)LoadFileData(path, 1);
    if (file == NULL)
    {
        SET_DEFAULT_CONFIG();
        LogPrintf(&g_Log, "コンフィグデータが見つからないので初期化しました\n");
    }
    else
    {
        g_App.cfg = *file;
        if (g_App.cfg.lives >= 5 || g_App.cfg.bombs >= 4 ||
            g_App.cfg.colorMode16Bits >= 2 || g_App.cfg.musicMode >= 3 ||
            g_App.cfg.defaultDifficulty >= 5 || g_App.cfg.playSounds >= 2 ||
            g_App.cfg.windowed >= 2 || g_App.cfg.frameSkip >= 3 ||
            g_App.cfg.version != 0x102 || g_FileSize != sizeof(GameConfig))
        {
            SET_DEFAULT_CONFIG();
            memset(&g_App.cfg.flags, 0, sizeof(GameConfigFlags));
            g_App.cfg.flags.emulateColorAdd = 1;
            LogPrintf(&g_Log, "コンフィグデータが破壊されていたので再初期化しました\n");
        }
        g_ControllerMapping = g_App.cfg.controllerMapping;
        free(file);
    }
    if (this->cfg.flags.noVertexBuffer)
    {
        LogPrintf(&g_Log, "頂点バッファの使用を抑制します\n");
    }
    if (this->cfg.flags.noFog)
    {
        LogPrintf(&g_Log, "フォグの使用を抑制します\n");
    }
    if (this->cfg.flags.force16BitTextures)
    {
        LogPrintf(&g_Log, "16Bit のテクスチャの使用を強制します\n");
    }
    if (this->ClearsBackBuffer())
    {
        LogPrintf(&g_Log, "バックバッファの消去を強制します\n");
    }
    if (this->cfg.flags.noFrameItems)
    {
        LogPrintf(&g_Log, "ゲーム周りのアイテムの描画を抑制します\n");
    }
    if (this->cfg.flags.noGouraudShading)
    {
        LogPrintf(&g_Log, "グーローシェーディングを抑制します\n");
    }
    if (this->cfg.flags.noDepthTest)
    {
        LogPrintf(&g_Log, "デプステストを抑制します\n");
    }
    if (this->cfg.flags.force60Fps)
    {
        LogPrintf(&g_Log, "６０フレーム強制モードにします\n");
        this->presentImmediate = 0;
    }
    if (this->cfg.flags.noColorBlending)
    {
        // Sic: the message ends in "n", not a line break.
        LogPrintf(&g_Log, "テクスチャの色合成を抑制しますn");
    }
    if (this->cfg.flags.noColorBlending)
    {
        LogPrintf(&g_Log, "ウィンドウモードで起動します\n");
    }
    if (this->cfg.flags.referenceRasterizer)
    {
        LogPrintf(&g_Log, "リファレンスラスタライザを強制します\n");
    }
    if (this->cfg.flags.noDirectInput)
    {
        LogPrintf(&g_Log, "パッド、キーボードの入力に DirectInput を使用しません\n");
    }
    if (WriteToFile(path, &g_App.cfg, sizeof(GameConfig)) != 0)
    {
        LogError(&g_Log, "ファイルが書き出せません %s\n", path);
        LogError(&g_Log, "フォルダが書込み禁止属性になっているか、ディスクがいっぱいいっぱいになってませんか？\n");
        return -1;
    }
    return 0;
}

int App::LoadMidi(int slot, char *path)
{
    if (g_App.cfg.musicMode == 2)
    {
        if (g_App.midi != NULL)
        {
            g_App.midi->LoadFile(slot, path);
        }
        return 0;
    }
    return 1;
}

int App::PlayMidi(int slot)
{
    if (g_App.cfg.musicMode == 2)
    {
        if (g_App.midi != NULL)
        {
            MidiPlayer *midi = g_App.midi;
            midi->Stop();
            midi->ParseFile(slot);
            midi->Play();
        }
        return 0;
    }
    return 1;
}

int App::LoadMusic(char *path)
{
    if (g_App.cfg.musicMode == 2)
    {
    }
    else if (g_App.cfg.musicMode == 1)
    {
    }
    else
    {
        return -1;
    }
    return 0;
}

int App::PlayMusic(char *path)
{
    char *dot;
    char posFile[256];
    char wavFile[256];

    if (g_App.cfg.musicMode == 2)
    {
        if (g_App.midi != NULL)
        {
            g_App.midi->Restart(path);
        }
    }
    else if (g_App.cfg.musicMode == 1)
    {
        strcpy(wavFile, path);
        strcpy(posFile, path);
        dot = strrchr(wavFile, '.');
        dot[1] = 'w';
        dot[2] = 'a';
        dot[3] = 'v';
        dot = strrchr(posFile, '.');
        dot[1] = 'p';
        dot[2] = 'o';
        dot[3] = 's';
        g_SoundPlayer.LoadBgm(wavFile);
        if (g_SoundPlayer.LoadBgmLoop(posFile) < 0)
        {
            g_SoundPlayer.PlayBgm(0);
        }
        else
        {
            g_SoundPlayer.PlayBgm(1);
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

int App::StopMusic()
{
    if (g_App.cfg.musicMode == 2)
    {
        if (g_App.midi != NULL)
        {
            g_App.midi->Stop();
        }
    }
    else if (g_App.cfg.musicMode == 1)
    {
        g_SoundPlayer.StopBgm();
    }
    else
    {
        return -1;
    }
    return 0;
}

int App::FadeOutMusic(float seconds)
{
    // Never used, but the original's frame has room for three dwords
    // besides `this`.
    int unused1;
    int unused2;
    int unused3;

    if (g_App.cfg.musicMode == 2)
    {
        if (g_App.midi != NULL)
        {
            g_App.midi->FadeOut(1000.0f * seconds);
        }
    }
    else if (g_App.cfg.musicMode == 1)
    {
        // The wave fade counts frames, so it is stretched while the
        // game runs slow.
        if (this->frameAdvance == 0.0f)
        {
            g_SoundPlayer.FadeOutBgm(seconds);
        }
        else if (this->frameAdvance > 1.0f)
        {
            g_SoundPlayer.FadeOutBgm(seconds);
        }
        else
        {
            g_SoundPlayer.FadeOutBgm(seconds / this->frameAdvance);
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

void SoundPlayer::FadeOutBgm(float seconds)
{
    if (this->bgm != NULL)
    {
        CStreamingSound *bgm = this->bgm;
        bgm->fading = 1;
        bgm->fadeFrames = seconds * 60.0f;
        bgm->fadeTotal = bgm->fadeFrames;
    }
}
