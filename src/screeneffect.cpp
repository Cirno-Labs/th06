// The file with ScreenEffect, from 0x42f6d0.

#include <string.h>

#include "gamemanager.h"
#include "screeneffect.h"
#include "sprite.h"
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

void DrawColorRect(float *rect, D3DCOLOR color)
{
    VertexDiffuseXyzrwh vertices[4];

    vertices[0].pos = D3DXVECTOR3(rect[0], rect[1], 0.0f);
    vertices[1].pos = D3DXVECTOR3(rect[2], rect[1], 0.0f);
    vertices[2].pos = D3DXVECTOR3(rect[0], rect[3], 0.0f);
    vertices[3].pos = D3DXVECTOR3(rect[2], rect[3], 0.0f);
    vertices[0].w = vertices[1].w = vertices[2].w = vertices[3].w = 1.0f;
    vertices[0].diffuse = vertices[1].diffuse = vertices[2].diffuse = vertices[3].diffuse = color;
    if (!g_Unknown6c6d18.cfg.flags.noColorBlending)
    {
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    }
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    if (!g_Unknown6c6d18.cfg.flags.noDepthTest)
    {
        g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
        g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_Unknown6c6d18.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(VertexDiffuseXyzrwh));
    g_SpriteEngine->ClearVertexFormat();
    g_SpriteEngine->ClearSpriteData();
    g_SpriteEngine->ClearTexture();
    g_SpriteEngine->ClearColorOp();
    g_SpriteEngine->ClearBlendMode();
    g_SpriteEngine->ClearZWrite();
    if (!g_Unknown6c6d18.cfg.flags.noColorBlending)
    {
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
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

ScreenEffect *ScreenEffect::StartEffect(int type, int duration, int a, int b, int c)
{
    ChainElem *calc = NULL;
    ChainElem *drawElem = NULL;
    ScreenEffect *effect = new ScreenEffect;

    if (effect == NULL)
    {
        return NULL;
    }
    memset(effect, 0, sizeof(ScreenEffect));
    switch (type)
    {
    case 0:
        calc = g_Chain.CreateElem((ChainCallback)ScreenEffect::OnUpdateFadeIn);
        drawElem = g_Chain.CreateElem((ChainCallback)ScreenEffect::OnDrawScreen);
        break;
    case 1:
        calc = g_Chain.CreateElem((ChainCallback)ScreenEffect::OnUpdateShake);
        break;
    case 2:
        calc = g_Chain.CreateElem((ChainCallback)ScreenEffect::OnUpdateFadeOut);
        drawElem = g_Chain.CreateElem((ChainCallback)ScreenEffect::OnDrawPlayArea);
        break;
    }
    calc->addedCallback = (ChainCallback)ScreenEffect::AddedCallback;
    calc->deletedCallback = (ChainCallback)ScreenEffect::DeletedCallback;
    calc->arg = effect;
    effect->type = type;
    effect->duration = duration;
    effect->shakeFrom = a;
    effect->shakeTo = b;
    effect->unk_20 = c;
    if (g_Chain.AddToCalcChain(calc, 0xe) != 0)
    {
        return NULL;
    }
    if (drawElem != NULL)
    {
        drawElem->arg = effect;
        g_Chain.AddToDrawChain(drawElem, 0x10);
    }
    effect->calcElem = calc;
    effect->drawElem = drawElem;
    return effect;
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
    DrawColorRect(area, effect->alpha << 24 | effect->color);
    return 1;
}

int ScreenEffect::OnDrawPlayArea(ScreenEffect *effect)
{
    float area[4];

    area[0] = 32.0f;
    area[1] = 16.0f;
    area[2] = 416.0f;
    area[3] = 464.0f;
    DrawColorRect(area, effect->alpha << 24 | effect->color);
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
