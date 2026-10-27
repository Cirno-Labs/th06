// cflags: /Os -/Oi
// The file starting at 0x41e940. It is built with /Os: functions end with
// `leave` instead of `mov esp, ebp; pop ebp`.

#include "unsorted.h"

GdiSurface::GdiSurface()
{
    this->format = -1;
    this->width = 0;
    this->height = 0;
    this->hdc = NULL;
    this->bitmap = NULL;
    this->oldObject = NULL;
    this->bits = NULL;
}

GdiSurface::~GdiSurface()
{
    this->Release();
}

void CreateTextSurface()
{
    g_Unknown6c6d18.d3dDevice->CreateImageSurface(640, 64, D3DFMT_A1R5G5B5, &g_TextSurface);
}

void ReleaseTextSurface()
{
    if (g_TextSurface != NULL)
    {
        g_TextSurface->Release();
        g_TextSurface = NULL;
    }
}

bool GdiSurface::Release()
{
    if (this->hdc != NULL)
    {
        SelectObject(this->hdc, this->oldObject);
        DeleteDC(this->hdc);
        DeleteObject(this->bitmap);
        this->format = -1;
        this->width = 0;
        this->height = 0;
        this->hdc = NULL;
        this->bitmap = NULL;
        this->oldObject = NULL;
        this->bits = NULL;
        return true;
    }
    return false;
}

bool GdiSurface::Create(int width, int height, int format)
{
    if (this->CreateInFormat(width, height, format))
    {
        return true;
    }
    if (format == D3DFMT_A1R5G5B5 || format == D3DFMT_A4R4G4B4)
    {
        return this->CreateInFormat(width, height, D3DFMT_A8R8G8B8);
    }
    if (format == D3DFMT_R5G6B5)
    {
        return this->CreateInFormat(width, height, D3DFMT_X8R8G8B8);
    }
    return false;
}

FormatInfo *GdiSurface::FindFormat(int format)
{
    int i;

    for (i = 0; g_FormatInfos[i].format != -1 && g_FormatInfos[i].format != format; i++)
    {
    }
    if (format == -1)
    {
        return NULL;
    }
    return &g_FormatInfos[i];
}
