// cflags: /Os
// The file starting at 0x42333b, up to 0x425e03. Built with /Os: its
// functions are laid out without alignment.

#include <stdio.h>

#include "chain.h"
#include "musicscreen.h"
#include "timer.h"
#include "unsorted.h"

int Unknown6c6d18::RegisterChain()
{
    Unknown6c6d18 *game = &g_Unknown6c6d18;
    ChainElem *elem;

    game->curScene = 0;
    game->scene = -1;
    game->frames = 0;
    elem = g_Chain.CreateElem((ChainCallback)Unknown6c6d18::OnUpdate);
    elem->arg = game;
    elem->addedCallback = (ChainCallback)Unknown6c6d18::AddedCallback;
    elem->deletedCallback = (ChainCallback)Unknown6c6d18::DeletedCallback;
    if (g_Chain.AddToCalcChain(elem, 0) != 0)
    {
        return -1;
    }
    elem = g_Chain.CreateElem((ChainCallback)Unknown6c6d18::OnDraw);
    elem->arg = game;
    g_Chain.AddToDrawChain(elem, 0xe);
    return 0;
}

// Updates the buttons, then sets up the scene `scene` asks for, tearing
// down the old one, before the chain runs the scene's own callbacks.
// Returns 4 when a scene fails to set up (or the game is to quit), 5
// for scene 5, otherwise 1.
int Unknown6c6d18::OnUpdate(Unknown6c6d18 *arg)
{
    if (g_Unknown6d457c != NULL)
    {
        g_Unknown6d457c->Func43b4d0();
    }
    g_PrevButtons = g_Buttons;
    g_Buttons = ReadButtons();
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
            g_Unknown6c6d18.d3dDevice->ResourceManagerDiscardBytes(0);
            if (Func43a338(0) != 0)
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
                if (Func42d773(0) != 0)
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
                if (Func4107b0() != 0)
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
                Func42ab30(0, NULL);
                goto title;
            case 7:
                GameManager::RemoveChain();
                if (Func42d773(1) != 0)
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
                Func42ab30(0, NULL);
                arg->scene = 1;
                g_Unknown6c6d18.d3dDevice->ResourceManagerDiscardBytes(0);
                if (Func43a338(1) != 0)
                {
                    return 4;
                }
                break;
            case 10:
                GameManager::RemoveChain();
                if (Func4107b0() != 0)
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
                Func42ab30(0, NULL);
                return 4;
            case 1:
                arg->scene = 0;
                Func42ab30(0, NULL);
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
                if (Func42d773(1) != 0)
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

int Unknown6c6d18::OnDraw(Unknown6c6d18 *arg)
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

int Unknown6c6d18::AddedCallback(Unknown6c6d18 *arg)
{
    int i;

    for (i = 0; i < 0x10; i++)
    {
        arg->archives[i] = NULL;
    }
    g_Archives = arg->archives;
    if (arg->LoadArchive(0, "紅魔郷IN.dat") != 0)
    {
        return -1;
    }
    // Shows the logo for two frames.
    g_SpriteEngine->LoadSurface(0, "data/title/th06logo.jpg");
    g_SpriteEngine->Func435300(0, 0, 0, 0, 0);
    if (FAILED(g_Unknown6c6d18.d3dDevice->Present(NULL, NULL, NULL, NULL)))
    {
        g_Unknown6c6d18.d3dDevice->Reset(&g_Unknown6c6d18.presentParams);
    }
    g_SpriteEngine->Func435300(0, 0, 0, 0, 0);
    if (FAILED(g_Unknown6c6d18.d3dDevice->Present(NULL, NULL, NULL, NULL)))
    {
        g_Unknown6c6d18.d3dDevice->Reset(&g_Unknown6c6d18.presentParams);
    }
    g_SpriteEngine->ReleaseSurface(0);
    arg->unk_400 = timeGetTime();
    InitDirectInput(arg);
    arg->midi = new MidiPlayer();
    g_Rng.SetSeed(timeGetTime());
    g_Unknown6d3f50.LoadSounds();
    if (g_SpriteEngine->LoadAnm(0, "data/text.anm", 0x700) != 0)
    {
        return -1;
    }
    if (AsciiText::RegisterChain() != 0)
    {
        Func41e4d0(&g_Unknown69d998, "error : 文字の初期化に失敗しました\n");
        return -1;
    }
    arg->unk_198 = 0;
    g_SpriteEngine->Func431660();
    Func41f008();
    arg->ReleaseArchive(0);
    if (g_Unknown6c6d18.LoadArchive(1, "紅魔郷MD.dat") != 0)
    {
        return -1;
    }
    return 0;
}

int InitDirectInput(Unknown6c6d18 *game)
{
    HINSTANCE hInst = (HINSTANCE)GetWindowLongA(game->hwnd, GWL_HINSTANCE);

    if (game->cfg.flags.noDirectInput)
    {
        return -1;
    }
    if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID **)&game->dinput, NULL)))
    {
        game->dinput = NULL;
        Func41e4d0(&g_Unknown69d998, "DirectInput が使用できません\n");
        return -1;
    }
    if (FAILED(game->dinput->CreateDevice(GUID_SysKeyboard, &game->keyboard, NULL)))
    {
        if (game->dinput)
        {
            game->dinput->Release();
            game->dinput = NULL;
        }
        Func41e4d0(&g_Unknown69d998, "DirectInput が使用できません\n");
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
        Func41e4d0(&g_Unknown69d998, "DirectInput SetDataFormat が使用できません\n");
        return -1;
    }
    if (FAILED(game->keyboard->SetCooperativeLevel(game->hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY)))
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
        Func41e4d0(&g_Unknown69d998, "DirectInput SetCooperativeLevel が使用できません\n");
        return -1;
    }
    game->keyboard->Acquire();
    Func41e4d0(&g_Unknown69d998, "DirectInput は正常に初期化されました\n");
    game->dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
    if (game->joystick)
    {
        game->joystick->SetDataFormat(&c_dfDIJoystick2);
        game->joystick->SetCooperativeLevel(game->hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
        g_Unknown6c6d18.joystickCaps.dwSize = sizeof(DIDEVCAPS);
        game->joystick->GetCapabilities(&g_Unknown6c6d18.joystickCaps);
        game->joystick->EnumObjects(EnumAxesCallback, NULL, DIDFT_ALL);
        Func41e4d0(&g_Unknown69d998, "有効なパッドを発見しました\n");
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

        if (FAILED(g_Unknown6c6d18.joystick->SetProperty(DIPROP_RANGE, &diprg.diph)))
        {
            return DIENUM_STOP;
        }
    }
    return DIENUM_CONTINUE;
}

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
    HRESULT hr;

    if (g_Unknown6c6d18.joystick == NULL)
    {
        hr = g_Unknown6c6d18.dinput->CreateDevice(pdidInstance->guidInstance, &g_Unknown6c6d18.joystick, NULL);
        if (FAILED(hr))
        {
            return DIENUM_CONTINUE;
        }
    }
    return DIENUM_STOP;
}

// Ends with the DirectX 8 SDK Joystick sample's FreeDirectInput, for
// both devices.
int Unknown6c6d18::DeletedCallback(Unknown6c6d18 *arg)
{
    int i;

    g_SpriteEngine->ReleaseVertexBuffer();
    for (i = 0; i < 0x10; i++)
    {
        arg->ReleaseArchive(i);
    }
    g_SpriteEngine->ReleaseAnm(0);
    AsciiText::RemoveChain();
    g_Unknown6d3f50.StopBgm();
    if (arg->midi != NULL)
    {
        arg->midi->Stop();
        delete arg->midi;
        arg->midi = NULL;
    }
    Func42ab30(0, NULL);
    Func41f02b();
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

    now = timeGetTime();
    frames += 1 + g_Unknown6c6d18.cfg.frameSkip;
    if (now - lastTime >= 500)
    {
        duration = (now - lastTime) / 1000.0f;
        fps = frames / duration;
        lastTime = now;
        frames = 0;
        sprintf(fpsText, "%.02ffps", fps);
        if (g_GameManager.unk_1821)
        {
            target = 60.0f / g_Unknown6c6d18.gameSpeed;
            g_Unknown6c6d18.unk_1b8 += target;
            if (target * 0.9f < fps)
            {
                g_Unknown6c6d18.unk_1b4 += target;
            }
            else if (target * 0.7f < fps)
            {
                g_Unknown6c6d18.unk_1b4 += target * 0.8f;
            }
            else if (target * 0.5f < fps)
            {
                g_Unknown6c6d18.unk_1b4 += target * 0.6f;
            }
            else
            {
                g_Unknown6c6d18.unk_1b4 += target * 0.5f;
            }
        }
    }
    if (g_Unknown6c6d18.unk_19c == 0)
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
    if (g_Unknown6c6d18.gameSpeed > 0.99f)
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
        this->subFrame += g_Unknown6c6d18.frameAdvance * frames;
        while (this->subFrame >= 1.0f)
        {
            this->current++;
            this->subFrame -= 1.0f;
        }
    }
}

void Timer::Sub(int frames)
{
    if (g_Unknown6c6d18.gameSpeed > 0.99f)
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
        this->subFrame -= g_Unknown6c6d18.frameAdvance * frames;
        while (this->subFrame < 0.0f)
        {
            this->current--;
            this->subFrame += 1.0f;
        }
    }
}

void Unknown6c6d18::TickTimer(int *frames, float *subFrame)
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

void Unknown6c6d18::ReleaseArchive(int index)
{
    if (this->archives[index] == NULL)
    {
        return;
    }
    this->archives[index]->Release();
    delete this->archives[index];
    this->archives[index] = NULL;
}

int Unknown6c6d18::LoadArchive(int index, char *path)
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
                Func41e620(&g_Unknown69d998, "error : データのバージョンが違います\n");
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
    g_Unknown6c6d18.cfg.unk_18 = 2; \
    g_Unknown6c6d18.cfg.unk_19 = 3; \
    g_Unknown6c6d18.cfg.unk_1a = 0xff; \
    g_Unknown6c6d18.cfg.version = 0x102; \
    g_Unknown6c6d18.cfg.unk_20 = 600; \
    g_Unknown6c6d18.cfg.unk_22 = 600; \
    wav = fopen("bgm/th06_01.wav", "rb"); \
    if (wav != NULL) \
    { \
        g_Unknown6c6d18.cfg.musicMode = 1; \
        fclose(wav); \
    } \
    else \
    { \
        g_Unknown6c6d18.cfg.musicMode = 2; \
        DebugPrint2("wave データが無いので、midi にします\n"); \
    } \
    g_Unknown6c6d18.cfg.unk_1c = 1; \
    g_Unknown6c6d18.cfg.unk_1d = 1; \
    g_Unknown6c6d18.cfg.unk_1e = 0; \
    g_Unknown6c6d18.cfg.frameSkip = 0; \
    g_Unknown6c6d18.cfg.controllerMapping = g_ControllerMapping; \
}

int Unknown6c6d18::LoadConfig(char *path)
{
    GameConfig *file;

    memset(&g_Unknown6c6d18.cfg, 0, sizeof(GameConfig));
    g_Unknown6c6d18.cfg.flags.unk0 = 1;
    file = (GameConfig *)Func41e290(path, 1);
    if (file == NULL)
    {
        SET_DEFAULT_CONFIG();
        Func41e4d0(&g_Unknown69d998, "コンフィグデータが見つからないので初期化しました\n");
    }
    else
    {
        g_Unknown6c6d18.cfg = *file;
        if (g_Unknown6c6d18.cfg.unk_18 >= 5 || g_Unknown6c6d18.cfg.unk_19 >= 4 ||
            g_Unknown6c6d18.cfg.unk_1a >= 2 || g_Unknown6c6d18.cfg.musicMode >= 3 ||
            g_Unknown6c6d18.cfg.unk_1d >= 5 || g_Unknown6c6d18.cfg.unk_1c >= 2 ||
            g_Unknown6c6d18.cfg.unk_1e >= 2 || g_Unknown6c6d18.cfg.frameSkip >= 3 ||
            g_Unknown6c6d18.cfg.version != 0x102 || g_FileSize != sizeof(GameConfig))
        {
            SET_DEFAULT_CONFIG();
            memset(&g_Unknown6c6d18.cfg.flags, 0, sizeof(GameConfigFlags));
            g_Unknown6c6d18.cfg.flags.unk0 = 1;
            Func41e4d0(&g_Unknown69d998, "コンフィグデータが破壊されていたので再初期化しました\n");
        }
        g_ControllerMapping = g_Unknown6c6d18.cfg.controllerMapping;
        free(file);
    }
    if (this->cfg.flags.noVertexBuffer)
    {
        Func41e4d0(&g_Unknown69d998, "頂点バッファの使用を抑制します\n");
    }
    if (this->cfg.flags.noFog)
    {
        Func41e4d0(&g_Unknown69d998, "フォグの使用を抑制します\n");
    }
    if (this->cfg.flags.force16BitTextures)
    {
        Func41e4d0(&g_Unknown69d998, "16Bit のテクスチャの使用を強制します\n");
    }
    if (this->ClearsBackBuffer())
    {
        Func41e4d0(&g_Unknown69d998, "バックバッファの消去を強制します\n");
    }
    if (this->cfg.flags.noFrameItems)
    {
        Func41e4d0(&g_Unknown69d998, "ゲーム周りのアイテムの描画を抑制します\n");
    }
    if (this->cfg.flags.noGouraudShading)
    {
        Func41e4d0(&g_Unknown69d998, "グーローシェーディングを抑制します\n");
    }
    if (this->cfg.flags.noDepthTest)
    {
        Func41e4d0(&g_Unknown69d998, "デプステストを抑制します\n");
    }
    if (this->cfg.flags.force60Fps)
    {
        Func41e4d0(&g_Unknown69d998, "６０フレーム強制モードにします\n");
        this->unk_1a0 = 0;
    }
    if (this->cfg.flags.noColorBlending)
    {
        // Sic: the message ends in "n", not a line break.
        Func41e4d0(&g_Unknown69d998, "テクスチャの色合成を抑制しますn");
    }
    if (this->cfg.flags.noColorBlending)
    {
        Func41e4d0(&g_Unknown69d998, "ウィンドウモードで起動します\n");
    }
    if (this->cfg.flags.referenceRasterizer)
    {
        Func41e4d0(&g_Unknown69d998, "リファレンスラスタライザを強制します\n");
    }
    if (this->cfg.flags.noDirectInput)
    {
        Func41e4d0(&g_Unknown69d998, "パッド、キーボードの入力に DirectInput を使用しません\n");
    }
    if (Func41e460(path, &g_Unknown6c6d18.cfg, sizeof(GameConfig)) != 0)
    {
        Func41e620(&g_Unknown69d998, "ファイルが書き出せません %s\n", path);
        Func41e620(&g_Unknown69d998, "フォルダが書込み禁止属性になっているか、ディスクがいっぱいいっぱいになってませんか？\n");
        return -1;
    }
    return 0;
}

int Unknown6c6d18::LoadMidi(int slot, char *path)
{
    if (g_Unknown6c6d18.cfg.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            g_Unknown6c6d18.midi->LoadFile(slot, path);
        }
        return 0;
    }
    return 1;
}

int Unknown6c6d18::PlayMidi(int slot)
{
    if (g_Unknown6c6d18.cfg.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            MidiPlayer *midi = g_Unknown6c6d18.midi;
            midi->Stop();
            midi->ParseFile(slot);
            midi->Play();
        }
        return 0;
    }
    return 1;
}

int Unknown6c6d18::LoadMusic(char *path)
{
    if (g_Unknown6c6d18.cfg.musicMode == 2)
    {
    }
    else if (g_Unknown6c6d18.cfg.musicMode == 1)
    {
    }
    else
    {
        return -1;
    }
    return 0;
}

int Unknown6c6d18::StopMusic()
{
    if (g_Unknown6c6d18.cfg.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            g_Unknown6c6d18.midi->Stop();
        }
    }
    else if (g_Unknown6c6d18.cfg.musicMode == 1)
    {
        g_Unknown6d3f50.StopBgm();
    }
    else
    {
        return -1;
    }
    return 0;
}

int Unknown6c6d18::FadeOutMusic(float seconds)
{
    // Never used, but the original's frame has room for three dwords
    // besides `this`.
    int unused1;
    int unused2;
    int unused3;

    if (g_Unknown6c6d18.cfg.musicMode == 2)
    {
        if (g_Unknown6c6d18.midi != NULL)
        {
            g_Unknown6c6d18.midi->FadeOut(1000.0f * seconds);
        }
    }
    else if (g_Unknown6c6d18.cfg.musicMode == 1)
    {
        // The wave fade counts frames, so it is stretched while the
        // game runs slow.
        if (this->frameAdvance == 0.0f)
        {
            g_Unknown6d3f50.FadeOutBgm(seconds);
        }
        else if (this->frameAdvance > 1.0f)
        {
            g_Unknown6d3f50.FadeOutBgm(seconds);
        }
        else
        {
            g_Unknown6d3f50.FadeOutBgm(seconds / this->frameAdvance);
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

void Unknown6d3f50::FadeOutBgm(float seconds)
{
    if (this->bgm != NULL)
    {
        CStreamingSound *bgm = this->bgm;
        bgm->fading = 1;
        bgm->fadeFrames = seconds * 60.0f;
        bgm->fadeTotal = bgm->fadeFrames;
    }
}
