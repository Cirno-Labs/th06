#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "unsorted.h"

void DebugPrint(const char *fmt, ...)
{
}

void DebugPrint2(const char *fmt, ...)
{
}

SpriteEngine::~SpriteEngine()
{
}

int Gui::AddedCallback(Gui *gui)
{
    return gui->Func417d29();
}

void Gui::RemoveChain()
{
    g_Chain.Remove(&g_GuiCalcChain);
    g_Chain.Remove(&g_GuiDrawChain);
}

DWORD Rng::GetRandomU32()
{
    return (this->GetRandomU16() << 16) | this->GetRandomU16();
}

float Rng::GetRandomF32ZeroToOne()
{
    return this->GetRandomU32() / 4294967296.0f;
}

float AddNormalizeAngle(float a, float b)
{
    int i = 0;

    a += b;
    while (a > 3.1415927f)
    {
        a -= 6.2831855f;
        if (i++ > 16)
        {
            break;
        }
    }
    while (a < -3.1415927f)
    {
        a += 6.2831855f;
        if (i++ > 16)
        {
            break;
        }
    }
    return a;
}

void Rotate2D(D3DXVECTOR3 *out, D3DXVECTOR3 *in, float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);

    out->x = c * in->x + s * in->y;
    out->y = c * in->y - s * in->x;
}

WORD InitJoystick()
{
    JOYINFOEX joyInfo;

    joyInfo.dwSize = sizeof(joyInfo);
    joyInfo.dwFlags = JOY_RETURNALL;
    if (joyGetPosEx(JOYSTICKID1, &joyInfo) != JOYERR_NOERROR)
    {
        LogPrintf(&g_Unknown69d998, "使えるパッドが存在しないようです、残念\n");
        return 1;
    }
    joyGetDevCapsA(JOYSTICKID1, &g_JoyCaps, sizeof(g_JoyCaps));
    return 0;
}

WORD ReadJoystick(WORD buttons)
{
    JOYINFOEX joyInfo;
    DWORD margin;
    int shotPressed;
    int diFire;
    HRESULT hr;
    DIJOYSTATE2 state;

    if (g_Unknown6c6d18.joystick == NULL)
    {
        memset(&joyInfo, 0, sizeof(joyInfo));
        joyInfo.dwSize = sizeof(joyInfo);
        joyInfo.dwFlags = JOY_RETURNALL;
        if (joyGetPosEx(JOYSTICKID1, &joyInfo) != JOYERR_NOERROR)
        {
            return buttons;
        }
        shotPressed = SetButtonFromJoystick(&buttons, g_ControllerMapping.unk_00[0], 1, joyInfo.dwButtons);
        if (g_ControllerMapping.unk_00[0] != g_ControllerMapping.unk_00[2])
        {
            SetButtonFromJoystick(&buttons, g_ControllerMapping.unk_00[2], 4, joyInfo.dwButtons);
        }
        else if (shotPressed)
        {
            if (g_ShotHeldFrames < 16)
            {
                g_ShotHeldFrames++;
            }
            if (g_ShotHeldFrames >= 8)
            {
                buttons |= 4;
            }
        }
        else if (g_ShotHeldFrames > 8)
        {
            g_ShotHeldFrames -= 8;
        }
        else
        {
            g_ShotHeldFrames = 0;
        }
        SetButtonFromJoystick(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[1], 2, joyInfo.dwButtons);
        SetButtonFromJoystick(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[3], 8, joyInfo.dwButtons);
        SetButtonFromJoystick(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[4], 0x10, joyInfo.dwButtons);
        SetButtonFromJoystick(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[5], 0x20, joyInfo.dwButtons);
        SetButtonFromJoystick(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[6], 0x40, joyInfo.dwButtons);
        SetButtonFromJoystick(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[7], 0x80, joyInfo.dwButtons);
        SetButtonFromJoystick(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[8], 0x100, joyInfo.dwButtons);
        margin = (g_JoyCaps.wXmax - g_JoyCaps.wXmin) / 2 / 2;
        buttons |= ((g_JoyCaps.wXmin + g_JoyCaps.wXmax) / 2 + margin < joyInfo.dwXpos) ? 0x80 : 0;
        buttons |= (joyInfo.dwXpos < (g_JoyCaps.wXmin + g_JoyCaps.wXmax) / 2 - margin) ? 0x40 : 0;
        margin = (g_JoyCaps.wYmax - g_JoyCaps.wYmin) / 2 / 2;
        buttons |= ((g_JoyCaps.wYmin + g_JoyCaps.wYmax) / 2 + margin < joyInfo.dwYpos) ? 0x20 : 0;
        buttons |= (joyInfo.dwYpos < (g_JoyCaps.wYmin + g_JoyCaps.wYmax) / 2 - margin) ? 0x10 : 0;
        return buttons;
    }
    else
    {
        hr = g_Unknown6c6d18.joystick->Poll();
        if (FAILED(hr))
        {
            int retries = 0;

            DebugPrint("error : DIERR_INPUTLOST\n");
            hr = g_Unknown6c6d18.joystick->Acquire();
            while (hr == DIERR_INPUTLOST)
            {
                hr = g_Unknown6c6d18.joystick->Acquire();
                DebugPrint("error : DIERR_INPUTLOST %d\n", retries);
                retries++;
                if (retries >= 400)
                {
                    return buttons;
                }
            }
            return buttons;
        }
        memset(&state, 0, sizeof(state));
        hr = g_Unknown6c6d18.joystick->GetDeviceState(sizeof(DIJOYSTATE2), &state);
        if (FAILED(hr))
        {
            return buttons;
        }
        diFire = SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[0], 1, state.rgbButtons);
        if (g_ControllerMapping.unk_00[0] != g_ControllerMapping.unk_00[2])
        {
            SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[2], 4, state.rgbButtons);
        }
        else if (diFire)
        {
            if (g_ShotHeldFrames < 16)
            {
                g_ShotHeldFrames++;
            }
            if (g_ShotHeldFrames >= 8)
            {
                buttons |= 4;
            }
        }
        else if (g_ShotHeldFrames > 8)
        {
            g_ShotHeldFrames -= 8;
        }
        else
        {
            g_ShotHeldFrames = 0;
        }
        SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[1], 2, state.rgbButtons);
        SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[3], 8, state.rgbButtons);
        SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[4], 0x10, state.rgbButtons);
        SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[5], 0x20, state.rgbButtons);
        SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[6], 0x40, state.rgbButtons);
        SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[7], 0x80, state.rgbButtons);
        SetButtonFromDirectInput(&buttons, g_Unknown6c6d18.cfg.controllerMapping.unk_00[8], 0x100, state.rgbButtons);
        buttons |= (state.lX > g_Unknown6c6d18.cfg.padThresholdX) ? 0x80 : 0;
        buttons |= (state.lX < -g_Unknown6c6d18.cfg.padThresholdX) ? 0x40 : 0;
        buttons |= (state.lY > g_Unknown6c6d18.cfg.padThresholdY) ? 0x20 : 0;
        buttons |= (state.lY < -g_Unknown6c6d18.cfg.padThresholdY) ? 0x10 : 0;
        return buttons;
    }
}

int SetButtonFromDirectInput(WORD *buttons, short button, WORD bit, BYTE *rgbButtons)
{
    if (button < 0)
    {
        return 0;
    }
    *buttons |= (rgbButtons[button] & 0x80) ? bit : 0;
    return (rgbButtons[button] & 0x80) ? bit : 0;
}

int SetButtonFromJoystick(WORD *buttons, short button, WORD bit, DWORD joyButtons)
{
    if (button < 0)
    {
        return 0;
    }
    DWORD mask = 1 << button;
    *buttons |= (joyButtons & mask) ? bit : 0;
    return (joyButtons & mask) ? bit : 0;
}

BYTE *GetPadButtons()
{
    JOYINFOEX joyInfo;
    DWORD mask;
    DWORD button;
    HRESULT hr;
    DIJOYSTATE2 state;

    memset(g_PadButtons, 0, sizeof(g_PadButtons));
    if (g_Unknown6c6d18.joystick == NULL)
    {
        memset(&joyInfo, 0, sizeof(joyInfo));
        joyInfo.dwSize = sizeof(joyInfo);
        joyInfo.dwFlags = JOY_RETURNALL;
        if (joyGetPosEx(JOYSTICKID1, &joyInfo) != JOYERR_NOERROR)
        {
            return g_PadButtons;
        }
        mask = joyInfo.dwButtons;
        for (button = 0; button < 32; button++, mask >>= 1)
        {
            if (mask & 1)
            {
                g_PadButtons[button] = 0x80;
            }
        }
        return g_PadButtons;
    }
    else
    {
        hr = g_Unknown6c6d18.joystick->Poll();
        if (FAILED(hr))
        {
            int retries = 0;

            DebugPrint("error : DIERR_INPUTLOST\n");
            hr = g_Unknown6c6d18.joystick->Acquire();
            while (hr == DIERR_INPUTLOST)
            {
                hr = g_Unknown6c6d18.joystick->Acquire();
                DebugPrint("error : DIERR_INPUTLOST %d\n", retries);
                retries++;
                if (retries >= 400)
                {
                    return g_PadButtons;
                }
            }
            return g_PadButtons;
        }
        g_Unknown6c6d18.joystick->GetDeviceState(sizeof(DIJOYSTATE2), &state);
        if (FAILED(hr))
        {
            return g_PadButtons;
        }
        memcpy(g_PadButtons, state.rgbButtons, sizeof(g_PadButtons));
        return g_PadButtons;
    }
}

// Sets `bit` in `buttons` while key `key` of `keyState` is down.
#define KEY(key, bit) buttons |= ((keyState[key] & 0x80) ? (bit) : 0)

WORD ReadButtons()
{
    WORD buttons = 0;
    BYTE keyState[0x100];

    if (g_Unknown6c6d18.keyboard == NULL)
    {
        GetKeyboardState(keyState);
        KEY(VK_UP, 0x10);
        KEY(VK_DOWN, 0x20);
        KEY(VK_LEFT, 0x40);
        KEY(VK_RIGHT, 0x80);
        KEY(VK_NUMPAD8, 0x10);
        KEY(VK_NUMPAD2, 0x20);
        KEY(VK_NUMPAD4, 0x40);
        KEY(VK_NUMPAD6, 0x80);
        KEY(VK_NUMPAD7, 0x50);
        KEY(VK_NUMPAD9, 0x90);
        KEY(VK_NUMPAD1, 0x60);
        KEY(VK_NUMPAD3, 0xa0);
        KEY(VK_HOME, 0x800);
        KEY('Z', 0x1);
        KEY('X', 0x2);
        KEY(VK_SHIFT, 0x4);
        KEY(VK_ESCAPE, 0x8);
        KEY(VK_CONTROL, 0x100);
        KEY('Q', 0x200);
        KEY('S', 0x400);
        KEY(VK_RETURN, 0x1000);
    }
    else
    {
        HRESULT hr = g_Unknown6c6d18.keyboard->GetDeviceState(sizeof(keyState), keyState);
        buttons = 0;
        if (hr == DIERR_INPUTLOST)
        {
            g_Unknown6c6d18.keyboard->Acquire();
            return ReadJoystick(buttons);
        }
        KEY(DIK_UP, 0x10);
        KEY(DIK_DOWN, 0x20);
        KEY(DIK_LEFT, 0x40);
        KEY(DIK_RIGHT, 0x80);
        KEY(DIK_NUMPAD8, 0x10);
        KEY(DIK_NUMPAD2, 0x20);
        KEY(DIK_NUMPAD4, 0x40);
        KEY(DIK_NUMPAD6, 0x80);
        KEY(DIK_NUMPAD7, 0x50);
        KEY(DIK_NUMPAD9, 0x90);
        KEY(DIK_NUMPAD1, 0x60);
        KEY(DIK_NUMPAD3, 0xa0);
        KEY(DIK_HOME, 0x800);
        KEY(DIK_Z, 0x1);
        KEY(DIK_X, 0x2);
        KEY(DIK_LSHIFT, 0x4);
        KEY(DIK_RSHIFT, 0x4);
        KEY(DIK_ESCAPE, 0x8);
        KEY(DIK_LCONTROL, 0x100);
        KEY(DIK_RCONTROL, 0x100);
        KEY(DIK_Q, 0x200);
        KEY(DIK_S, 0x400);
        KEY(DIK_RETURN, 0x1000);
    }
    return ReadJoystick(buttons);
}

void ResetKeyboard()
{
    BYTE keyState[0x100];
    int i;

    GetKeyboardState(keyState);
    for (i = 0; i < 0x100; i++)
    {
        keyState[i] &= 0x7f;
    }
    SetKeyboardState(keyState);
}

void Unknown41e150::Create(IDirect3DDevice8 *device, int width, int height)
{
    HDC dc = NULL;
    HFONT font = NULL;
    HGDIOBJ oldFont = NULL;

    dc = CreateCompatibleDC(NULL);
    font = CreateFontA(height, width, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック");
    if (font == NULL)
    {
        return;
    }
    oldFont = SelectObject(dc, font);
    if (FAILED(D3DXCreateFont(device, font, &this->font)))
    {
        MessageBoxA(NULL, "D3DXCreateFontIndirect FALSE", "ok", MB_OK);
        return;
    }
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

int Unknown41e150::Draw(char *text, int x, int y, D3DCOLOR color)
{
    RECT rect;

    rect.left = x;
    rect.right = 640;
    rect.top = y;
    rect.bottom = 480;
    return this->font->DrawTextA(text, -1, &rect, DT_EXPANDTABS, color);
}

void Unknown41e150::Release()
{
    if (this->font != NULL)
    {
        this->font->Release();
        this->font = NULL;
    }
}

void *LoadFileData(char *path, int isExternal)
{
    FILE *handle;
    void *buffer;
    int fileSize3;
    int entry = -1;
    char *name;
    int i;

    if (isExternal == 0)
    {
        name = strrchr(path, '\\');
        if (name == NULL)
        {
            name = path;
        }
        else
        {
            name++;
        }
        name = strrchr(name, '/');
        if (name == NULL)
        {
            name = path;
        }
        else
        {
            name++;
        }
        if (g_Archives != NULL)
        {
            for (i = 0; i < 0x10; i++)
            {
                if (g_Archives[i] != NULL)
                {
                    entry = g_Archives[i]->FindEntry(name);
                    if (entry >= 0)
                    {
                        break;
                    }
                }
            }
        }
        if (entry < 0)
        {
            return NULL;
        }
    }
    if (entry >= 0)
    {
        DebugPrint("%s Decode ... \n", name);
        buffer = g_Archives[i]->Decode(entry, name);
        g_FileSize = g_Archives[i]->GetEntrySize(entry);
    }
    else
    {
        DebugPrint("%s Load ... \n", path);
        handle = fopen(path, "rb");
        if (handle == NULL)
        {
            DebugPrint("error : %s is not found.\n", path);
            return NULL;
        }
        fseek(handle, 0, SEEK_END);
        fileSize3 = ftell(handle);
        g_FileSize = fileSize3;
        fseek(handle, 0, SEEK_SET);
        buffer = malloc(fileSize3);
        fread(buffer, 1, fileSize3, handle);
        fclose(handle);
    }
    return buffer;
}

int WriteToFile(char *path, void *data, int size)
{
    FILE *file;

    file = fopen(path, "wb");
    if (file == NULL)
    {
        return -1;
    }
    if (fwrite(data, 1, size, file) != size)
    {
        fclose(file);
        return -2;
    }
    fclose(file);
    return 0;
}

int LogPrintf(Unknown69d998 *log, const char *fmt, ...)
{
    int len2;
    char buf[0x200];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    len2 = strlen(buf);
    if (log->cursor + len2 < log->buffer + sizeof(log->buffer) - 1)
    {
        strcpy(log->cursor, buf);
        log->cursor += len2;
        *log->cursor = '\0';
    }
    va_end(args);
    return (int)fmt;
}

int LogError(Unknown69d998 *log, const char *fmt, ...)
{
    int len2;
    char buf[0x200];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    len2 = strlen(buf);
    if (log->cursor + len2 < log->buffer + sizeof(log->buffer) - 1)
    {
        strcpy(log->cursor, buf);
        log->cursor += len2;
        *log->cursor = '\0';
    }
    va_end(args);
    log->hasError = 1;
    return (int)fmt;
}
