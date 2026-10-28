// The file with ScreenEffect, from 0x42f6d0.

#include "gamemanager.h"
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

void ClearViewport(D3DCOLOR color)
{
    g_Unknown6c6d18.viewport.X = 0;
    g_Unknown6c6d18.viewport.Y = 0;
    g_Unknown6c6d18.viewport.Width = 640;
    g_Unknown6c6d18.viewport.Height = 480;
    g_Unknown6c6d18.viewport.MinZ = 0.0f;
    g_Unknown6c6d18.viewport.MaxZ = 1.0f;
    g_Unknown6c6d18.d3dDevice->SetViewport(&g_Unknown6c6d18.viewport);
    ClearScreen(color);
}

int ScreenEffect::OnUpdateFadeIn(ScreenEffect *effect)
{
    if (effect->duration != 0)
    {
        effect->alpha = 255.0f - effect->timer.AsFloat() * 255.0f / effect->duration;
        if (effect->alpha < 0)
        {
            effect->alpha = 0;
        }
    }
    if (effect->timer >= effect->duration)
    {
        return 0;
    }
    effect->timer.Tick();
    return 1;
}

int ScreenEffect::OnUpdateFadeOut(ScreenEffect *effect)
{
    if (effect->duration != 0)
    {
        effect->alpha = effect->timer.AsFloat() * 255.0f / effect->duration;
        if (effect->alpha < 0)
        {
            effect->alpha = 0;
        }
    }
    if (effect->timer >= effect->duration)
    {
        return 0;
    }
    effect->timer.Tick();
    return 1;
}

int ScreenEffect::OnDrawScreen(ScreenEffect *effect)
{
    float area[4];

    area[0] = 0.0f;
    area[1] = 0.0f;
    area[2] = 640.0f;
    area[3] = 480.0f;
    g_Unknown6c6d18.viewport.X = 0;
    g_Unknown6c6d18.viewport.Y = 0;
    g_Unknown6c6d18.viewport.Width = 640;
    g_Unknown6c6d18.viewport.Height = 480;
    g_Unknown6c6d18.d3dDevice->SetViewport(&g_Unknown6c6d18.viewport);
    Func42f8b0(area, effect->alpha << 24 | effect->color);
    return 1;
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

int ScreenEffect::OnUpdateShake(ScreenEffect *effect)
{
    float shake;

    if (g_GameManager.unk_002c)
    {
        g_GameManager.arena[0] = 32.0f;
        g_GameManager.arena[1] = 16.0f;
        g_GameManager.arena[2] = 384.0f;
        g_GameManager.arena[3] = 448.0f;
        return 1;
    }
    effect->timer.Tick();
    if (effect->timer >= effect->duration)
    {
        g_GameManager.arena[0] = 32.0f;
        g_GameManager.arena[1] = 16.0f;
        g_GameManager.arena[2] = 384.0f;
        g_GameManager.arena[3] = 448.0f;
        return 0;
    }
    shake = effect->timer.AsFloat() * (effect->shakeTo - effect->shakeFrom) / effect->duration + effect->shakeFrom;
    switch (g_Rng.GetRandomU32InRange(3))
    {
    case 0:
        g_GameManager.arena[0] = 32.0f;
        g_GameManager.arena[2] = 384.0f;
        break;
    case 1:
        g_GameManager.arena[0] = 32.0f + shake;
        g_GameManager.arena[2] = 384.0f - shake;
        break;
    case 2:
        g_GameManager.arena[0] = 32.0f;
        g_GameManager.arena[2] = 384.0f - shake;
        break;
    }
    switch (g_Rng.GetRandomU32InRange(3))
    {
    case 0:
        g_GameManager.arena[1] = 16.0f;
        g_GameManager.arena[3] = 448.0f;
        break;
    case 1:
        g_GameManager.arena[1] = 16.0f + shake;
        g_GameManager.arena[3] = 448.0f - shake;
        break;
    case 2:
        g_GameManager.arena[1] = 16.0f;
        g_GameManager.arena[3] = 448.0f - shake;
        break;
    }
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
