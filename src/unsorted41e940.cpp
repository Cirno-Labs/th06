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
