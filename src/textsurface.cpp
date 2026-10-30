// cflags: /Os -/Oi
// The text helpers, 0x41f008 to 0x41f22c: the text surface and rendering
// text into a texture (g_TextSurface is defined in unsorted41e940.cpp).
// Built with /Os.

#include "features.h"
#include "unsorted.h"

#ifdef TH06_EXE
// Unused: the original's object also referenced memmove and threw, from
// code /OPT:REF dropped. libcmt's memmove.obj then comes right after
// memcpy.obj, and throw.obj (whose code is dropped too, but not its
// four bytes of .data) pulls unhandld.obj in right after write.obj.
void UnusedMoveBits(void *dst, void *src, size_t size)
{
    memmove(dst, src, size);
    throw;
}

// Unused: the original's object also drew text with DrawTextA, setting
// the DC's background mode and then its text color before its first
// TextOutA, from code /OPT:REF dropped. That order of first mention
// decides GDI32's IAT order, and mentioning DrawTextA here (D3DX8
// mentions it only later) part of USER32's.
void UnusedDrawText(HDC hdc, char *text, RECT *rect, COLORREF color)
{
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    DrawTextA(hdc, text, -1, rect, DT_LEFT);
}
#endif // TH06_EXE

void CreateTextSurface()
{
    g_App.d3dDevice->CreateImageSurface(640, 64, D3DFMT_A1R5G5B5, &g_TextSurface);
}

void ReleaseTextSurface()
{
    if (g_TextSurface != NULL)
    {
        g_TextSurface->Release();
        g_TextSurface = NULL;
    }
}

void RenderTextToTexture(int x, int y, int width, int unk4, int fontHeight, int unk6, COLORREF textColor,
                         COLORREF shadowColor, char *text, IDirect3DTexture8 *texture)
{
    HDC hdcMem;
    HFONT hNewFont;
    D3DSURFACE_DESC sd;
    HGDIOBJ oldObject;
    RECT rcSrc;
    RECT rcDst;
    IDirect3DSurface8 *dstSurface;

#if FEAT_ENGLISH
    hNewFont = CreateFontA(fontHeight * 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial");
#else
    hNewFont = CreateFontA(fontHeight * 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN | FIXED_PITCH, "ＭＳ ゴシック");
#endif
    GdiSurface dib;
    g_TextSurface->GetDesc(&sd);
    dib.Create(sd.Width, sd.Height, sd.Format);
    hdcMem = dib.hdc;
    oldObject = SelectObject(hdcMem, hNewFont);
    dib.InvertAlpha(0, 0, width * 2, fontHeight * 2 + 6);
    SetBkMode(hdcMem, TRANSPARENT);
    if (shadowColor != (COLORREF)-1)
    {
        SetTextColor(hdcMem, shadowColor);
        TextOutA(hdcMem, x * 2 + 3, 2, text, strlen(text));
    }
    SetTextColor(hdcMem, textColor);
    TextOutA(hdcMem, x * 2, 0, text, strlen(text));
    SelectObject(hdcMem, oldObject);
    dib.InvertAlpha(0, 0, width * 2, fontHeight * 2 + 6);
    dib.CopyToSurface(g_TextSurface);
    SelectObject(hdcMem, oldObject);
    DeleteObject(hNewFont);
    rcDst.left = 0;
    rcDst.top = y;
    rcDst.right = width;
    rcDst.bottom = y + 16;
    rcSrc.left = 0;
    rcSrc.top = 0;
    rcSrc.right = width * 2 - 2;
    rcSrc.bottom = fontHeight * 2 - 2;
    texture->GetSurfaceLevel(0, &dstSurface);
    D3DXLoadSurfaceFromSurface(dstSurface, NULL, &rcDst, g_TextSurface, NULL, &rcSrc, D3DX_FILTER_TRIANGLE, 0);
    if (dstSurface != NULL)
    {
        dstSurface->Release();
        dstSurface = NULL;
    }
}
