#include <math.h>

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
