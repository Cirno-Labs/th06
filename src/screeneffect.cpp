// The file with ScreenEffect, from 0x42f6d0.

#include "screeneffect.h"
#include "unsorted.h"

void ClearScreen(D3DCOLOR color)
{
    g_Unknown6c6d18.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
    if (FAILED(g_Unknown6c6d18.d3dDevice->Present(NULL, NULL, NULL, NULL)))
    {
        g_Unknown6c6d18.d3dDevice->Reset(&g_Unknown6c6d18.presentParams);
    }
    g_Unknown6c6d18.d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
    if (FAILED(g_Unknown6c6d18.d3dDevice->Present(NULL, NULL, NULL, NULL)))
    {
        g_Unknown6c6d18.d3dDevice->Reset(&g_Unknown6c6d18.presentParams);
    }
}

int ScreenEffect::OnDrawPlayArea(ScreenEffect *effect)
{
    float area[4];

    area[0] = 32.0f;
    area[1] = 16.0f;
    area[2] = 416.0f;
    area[3] = 464.0f;
    Func42f8b0(area, effect->alpha << 24 | effect->color);
    return 1;
}

int ScreenEffect::AddedCallback(ScreenEffect *effect)
{
    effect->timer.Set(0);
    return 0;
}

int ScreenEffect::DeletedCallback(ScreenEffect *effect)
{
    effect->calcElem->deletedCallback = NULL;
    g_Chain.Remove(effect->drawElem);
    effect->drawElem = NULL;
    delete effect;
    effect = NULL;
    return 0;
}
