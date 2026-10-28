// The file with SpriteEngine.

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "sprite.h"
#include "unsorted.h"

SpriteEngine::SpriteEngine()
{
    int i;

    this->spriteCount = 0;
    memset(this, 0, sizeof(*this));
    for (i = 0; i < 0x800; i++)
    {
        this->sprites[i].sourceFileIndex = -1;
    }
    g_Quad[0].w = g_Quad[1].w = g_Quad[2].w = g_Quad[3].w = 1.0f;
    g_Quad[0].uv.x = 0.0f;
    g_Quad[0].uv.y = 0.0f;
    g_Quad[1].uv.x = 1.0f;
    g_Quad[1].uv.y = 0.0f;
    g_Quad[2].uv.x = 0.0f;
    g_Quad[2].uv.y = 1.0f;
    g_Quad[3].uv.x = 1.0f;
    g_Quad[3].uv.y = 1.0f;
    g_DiffuseQuad[0].w = g_DiffuseQuad[1].w = g_DiffuseQuad[2].w = g_DiffuseQuad[3].w = 1.0f;
    g_DiffuseQuad[0].uv.x = 0.0f;
    g_DiffuseQuad[0].uv.y = 0.0f;
    g_DiffuseQuad[1].uv.x = 1.0f;
    g_DiffuseQuad[1].uv.y = 0.0f;
    g_DiffuseQuad[2].uv.x = 0.0f;
    g_DiffuseQuad[2].uv.y = 1.0f;
    g_DiffuseQuad[3].uv.x = 1.0f;
    g_DiffuseQuad[3].uv.y = 1.0f;
    this->vertexBuffer = NULL;
    this->currentTexture = NULL;
    this->blendMode = 0;
    this->colorOp = 0;
    this->currentColor = 1;
    this->vertexFormat = 0;
    this->zWriteDisabled = 0;
    this->pendingCopyTexture = -1;
}

void SpriteEngine::CreateQuad()
{
    VertexTex1Xyz *data;

    this->quad[0].pos.x = this->quad[2].pos.x = -128.0f;
    this->quad[1].pos.x = this->quad[3].pos.x = 128.0f;
    this->quad[0].pos.y = this->quad[1].pos.y = -128.0f;
    this->quad[2].pos.y = this->quad[3].pos.y = 128.0f;
    this->quad[0].pos.z = this->quad[1].pos.z = this->quad[2].pos.z = this->quad[3].pos.z = 0.0f;
    this->quad[0].uv.x = this->quad[2].uv.x = 0.0f;
    this->quad[1].uv.x = this->quad[3].uv.x = 1.0f;
    this->quad[0].uv.y = this->quad[1].uv.y = 0.0f;
    this->quad[2].uv.y = this->quad[3].uv.y = 1.0f;
    g_DiffuseQuadXyz[0].pos = this->quad[0].pos;
    g_DiffuseQuadXyz[1].pos = this->quad[1].pos;
    g_DiffuseQuadXyz[2].pos = this->quad[2].pos;
    g_DiffuseQuadXyz[3].pos = this->quad[3].pos;
    g_DiffuseQuadXyz[0].uv.x = this->quad[0].uv.x;
    g_DiffuseQuadXyz[0].uv.y = this->quad[0].uv.y;
    g_DiffuseQuadXyz[1].uv.x = this->quad[1].uv.x;
    g_DiffuseQuadXyz[1].uv.y = this->quad[1].uv.y;
    g_DiffuseQuadXyz[2].uv.x = this->quad[2].uv.x;
    g_DiffuseQuadXyz[2].uv.y = this->quad[2].uv.y;
    g_DiffuseQuadXyz[3].uv.x = this->quad[3].uv.x;
    g_DiffuseQuadXyz[3].uv.y = this->quad[3].uv.y;
    if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
    {
        g_Unknown6c6d18.d3dDevice->CreateVertexBuffer(sizeof(this->quad), 0, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_MANAGED,
                                                      &this->vertexBuffer);
        this->vertexBuffer->Lock(0, 0, (BYTE **)&data, 0);
        memcpy(data, this->quad, sizeof(this->quad));
        this->vertexBuffer->Unlock();
        g_Unknown6c6d18.d3dDevice->SetStreamSource(0, g_SpriteEngine->vertexBuffer, sizeof(VertexTex1Xyz));
    }
}

int SpriteEngine::LoadTexture(int index, char *path, int format, D3DCOLOR colorKey)
{
    this->ReleaseTexture(index);
    this->textureFiles[index] = LoadFileData(path, 0);
    if (this->textureFiles[index] == NULL)
    {
        return -1;
    }
    if (g_Unknown6c6d18.cfg.flags.force16BitTextures)
    {
        if (g_TextureFormats[format] == D3DFMT_A8R8G8B8 || g_TextureFormats[format] == D3DFMT_UNKNOWN)
        {
            format = 5;
        }
        else if (g_TextureFormats[format] == D3DFMT_R8G8B8)
        {
            format = 3;
        }
    }
    if (D3DXCreateTextureFromFileInMemoryEx(g_Unknown6c6d18.d3dDevice, this->textureFiles[index], g_FileSize, 0, 0, 0, 0,
                                            g_TextureFormats[format], D3DPOOL_MANAGED, D3DX_FILTER_LINEAR,
                                            D3DX_DEFAULT, colorKey, NULL, NULL, &this->textures[index]) != D3D_OK)
    {
        return -1;
    }
    return 0;
}

// Pixels in the texture formats LoadTextureAlpha handles.
struct PixelA8R8G8B8
{
    BYTE b;
    BYTE g;
    BYTE r;
    BYTE a;
};
struct PixelA1R5G5B5
{
    WORD b : 5;
    WORD g : 5;
    WORD r : 5;
    WORD a : 1;
};
struct PixelA4R4G4B4
{
    WORD b : 4;
    WORD g : 4;
    WORD r : 4;
    WORD a : 4;
};

int SpriteEngine::LoadTextureAlpha(int index, char *path, int format, D3DCOLOR colorKey)
{
    D3DSURFACE_DESC desc;
    void *fileData;
    D3DLOCKED_RECT dstRect;
    D3DLOCKED_RECT srcRect;
    IDirect3DTexture8 *srcTexture = NULL;

    fileData = LoadFileData(path, 0);
    if (fileData == NULL)
    {
        return -1;
    }
    this->textures[index]->GetLevelDesc(0, &desc);
    if (desc.Format != D3DFMT_A8R8G8B8 && desc.Format != D3DFMT_A4R4G4B4 && desc.Format != D3DFMT_A1R5G5B5)
    {
        LogError(&g_Unknown69d998, "error : イメージがαを持っていません\n");
        goto fail;
    }
    if (D3DXCreateTextureFromFileInMemoryEx(g_Unknown6c6d18.d3dDevice, fileData, g_FileSize, 0, 0, 0, 0, desc.Format,
                                            D3DPOOL_SYSTEMMEM, D3DX_FILTER_LINEAR, D3DX_DEFAULT, colorKey, NULL, NULL,
                                            &srcTexture) != D3D_OK)
    {
        goto fail;
    }
    if (this->textures[index]->LockRect(0, &dstRect, NULL, 0) != D3D_OK)
    {
        goto fail;
    }
    if (srcTexture->LockRect(0, &srcRect, NULL, D3DLOCK_NO_DIRTY_UPDATE) != D3D_OK)
    {
        goto fail;
    }
    switch (desc.Format)
    {
    case D3DFMT_A8R8G8B8:
    {
        PixelA8R8G8B8 *dstPx;
        PixelA8R8G8B8 *srcPx;
        UINT py;
        UINT tx;

        for (py = 0; py < desc.Height; py++)
        {
            dstPx = (PixelA8R8G8B8 *)((BYTE *)dstRect.pBits + py * dstRect.Pitch);
            srcPx = (PixelA8R8G8B8 *)((BYTE *)srcRect.pBits + py * srcRect.Pitch);
            for (tx = 0; tx < desc.Width; tx++, srcPx++, dstPx++)
            {
                dstPx->a = srcPx->b;
            }
        }
        break;
    }
    case D3DFMT_A1R5G5B5:
    {
        PixelA1R5G5B5 *dstPx;
        PixelA1R5G5B5 *srcPx;
        UINT py;
        UINT tx;

        for (py = 0; py < desc.Height; py++)
        {
            dstPx = (PixelA1R5G5B5 *)((BYTE *)dstRect.pBits + py * dstRect.Pitch);
            srcPx = (PixelA1R5G5B5 *)((BYTE *)srcRect.pBits + py * srcRect.Pitch);
            for (tx = 0; tx < desc.Width; tx++, srcPx++, dstPx++)
            {
                dstPx->a = srcPx->b >> 4;
            }
        }
        break;
    }
    case D3DFMT_A4R4G4B4:
    {
        PixelA4R4G4B4 *dstPx;
        PixelA4R4G4B4 *srcPx;
        UINT py;
        UINT tx;

        for (py = 0; py < desc.Height; py++)
        {
            dstPx = (PixelA4R4G4B4 *)((BYTE *)dstRect.pBits + py * dstRect.Pitch);
            srcPx = (PixelA4R4G4B4 *)((BYTE *)srcRect.pBits + py * srcRect.Pitch);
            for (tx = 0; tx < desc.Width; tx++, srcPx++, dstPx++)
            {
                dstPx->a = srcPx->b;
            }
        }
        break;
    }
    }
    srcTexture->UnlockRect(0);
    this->textures[index]->UnlockRect(0);
    if (srcTexture != NULL)
    {
        srcTexture->Release();
        srcTexture = NULL;
    }
    free(fileData);
    return 0;

fail:
    if (srcTexture != NULL)
    {
        srcTexture->Release();
        srcTexture = NULL;
    }
    free(fileData);
    return -1;
}

int SpriteEngine::CreateTexture(int index, int width, int height, int format)
{
    D3DXCreateTexture(g_Unknown6c6d18.d3dDevice, width, height, 1, 0, g_TextureFormats[format], D3DPOOL_MANAGED,
                      &this->textures[index]);
    return 0;
}

int SpriteEngine::LoadAnm(int slot, char *path, int firstSprite)
{
    AnmFile *anmHeader;
    char *name;
    AnmSprite *rawSprite;
    int k;
    SpriteData data;
    int *entries;

    this->ReleaseAnm(slot);
    this->anmFiles[slot] = (AnmFile *)LoadFileData(path, 0);
    anmHeader = this->anmFiles[slot];
    if (anmHeader == NULL)
    {
        LogError(&g_Unknown69d998, "スプライトアニメ %s が読み込めません。データが失われてるか壊れています\n", path);
        return -1;
    }
    anmHeader->textureIndex = slot;
    name = (char *)anmHeader + anmHeader->nameOffset;
    if (*name == '@')
    {
        this->CreateTexture(anmHeader->textureIndex, anmHeader->width, anmHeader->height, anmHeader->format);
    }
    else if (this->LoadTexture(anmHeader->textureIndex, name, anmHeader->format, anmHeader->colorKey) != 0)
    {
        LogError(&g_Unknown69d998, "テクスチャ %s が読み込めません。データが失われてるか壊れています\n", name);
        return -1;
    }
    if (anmHeader->alphaNameOffset != 0)
    {
        name = (char *)anmHeader + anmHeader->alphaNameOffset;
        if (this->LoadTextureAlpha(anmHeader->textureIndex, name, anmHeader->format, anmHeader->colorKey) != 0)
        {
            LogError(&g_Unknown69d998, "テクスチャ %s が読み込めません。データが失われてるか壊れています\n", name);
            return -1;
        }
    }
    anmHeader->spriteOffset = firstSprite;
    entries = anmHeader->spriteOffsets;
    for (k = 0; k < this->anmFiles[slot]->numSprites; k++, entries++)
    {
        rawSprite = (AnmSprite *)((char *)anmHeader + *entries);
        data.sourceFileIndex = this->anmFiles[slot]->textureIndex;
        data.startX = rawSprite->x;
        data.startY = rawSprite->y;
        data.endX = rawSprite->x + rawSprite->width;
        data.endY = rawSprite->y + rawSprite->height;
        data.textureWidth = anmHeader->width;
        data.textureHeight = anmHeader->height;
        this->LoadSprite(rawSprite->id + firstSprite, &data);
    }
    for (k = 0; k < anmHeader->numScripts; k++, entries += 2)
    {
        this->scripts[entries[0] + firstSprite] = (char *)anmHeader + entries[1];
        this->scriptSpriteOffsets[entries[0] + firstSprite] = firstSprite;
    }
    this->anmFileSpriteOffsets[slot] = firstSprite;
    return 0;
}

void SpriteEngine::ReleaseAnm(int slot)
{
    AnmFile *anmHeader;
    AnmSprite *rawSprite;
    int k;
    int startSprite;
    int *entries;

    if (this->anmFiles[slot] != NULL)
    {
        startSprite = this->anmFileSpriteOffsets[slot];
        entries = this->anmFiles[slot]->spriteOffsets;
        for (k = 0; k < this->anmFiles[slot]->numSprites; k++, entries++)
        {
            rawSprite = (AnmSprite *)((char *)this->anmFiles[slot] + *entries);
            memset(&this->sprites[rawSprite->id + startSprite], 0, sizeof(SpriteData));
            this->sprites[rawSprite->id + startSprite].sourceFileIndex = -1;
        }
        for (k = 0; k < this->anmFiles[slot]->numScripts; k++, entries += 2)
        {
            this->scripts[entries[0] + startSprite] = NULL;
            this->scriptSpriteOffsets[entries[0] + startSprite] = 0;
        }
        this->anmFileSpriteOffsets[slot] = 0;
        anmHeader = this->anmFiles[slot];
        this->ReleaseTexture(anmHeader->textureIndex);
        MemFree(this->anmFiles[slot]);
        this->anmFiles[slot] = NULL;
        this->ClearBlendMode();
        this->ClearColorOp();
        this->ClearVertexFormat();
        this->ClearTexture();
    }
}

void SpriteEngine::ReleaseTexture(int index)
{
    if (this->textures[index] != NULL)
    {
        this->textures[index]->Release();
        this->textures[index] = NULL;
    }
    MemFree(this->textureFiles[index]);
    this->textureFiles[index] = NULL;
}

int SpriteEngine::SetSprite(Sprite *vm, int sprite)
{
    if (this->sprites[sprite].sourceFileIndex < 0)
    {
        return -1;
    }
    vm->spriteIndex = sprite;
    vm->spriteData = &this->sprites[sprite];
    D3DXMatrixIdentity(&vm->matrix);
    vm->matrix.m[0][0] = vm->spriteData->width / vm->spriteData->textureWidth;
    vm->matrix.m[1][1] = vm->spriteData->height / vm->spriteData->textureHeight;
    return 0;
}

void SpriteEngine::SetScript(Sprite *vm, void *script)
{
    vm->flags.flip = 0;
    vm->Initialize();
    vm->scriptStart = (AnmInstr *)script;
    vm->scriptPos = vm->scriptStart;
    vm->scriptTime.Set(0);
    vm->flags.unk0 = 0;
    if (script != NULL)
    {
        this->Step(vm);
    }
}

void SpriteEngine::SetRenderStateForSprite(Sprite *vm)
{
    if (this->blendMode != vm->flags.blendMode)
    {
        this->blendMode = vm->flags.blendMode;
        if (this->blendMode == 0)
        {
            g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        }
        else
        {
            g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        }
    }
    if (!g_Unknown6c6d18.cfg.flags.unk0 && !g_Unknown6c6d18.cfg.flags.noColorBlending &&
        this->colorOp != vm->flags.colorOp)
    {
        this->colorOp = vm->flags.colorOp;
        if (this->colorOp == 0)
        {
            g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        }
        else
        {
            g_Unknown6c6d18.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
        }
    }
    if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
    {
        if (this->currentColor != vm->color)
        {
            this->currentColor = vm->color;
            g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, this->currentColor);
        }
    }
    else
    {
        g_DiffuseQuad[0].diffuse = vm->color;
        g_DiffuseQuad[1].diffuse = vm->color;
        g_DiffuseQuad[2].diffuse = vm->color;
        g_DiffuseQuad[3].diffuse = vm->color;
        g_DiffuseQuadXyz[0].diffuse = vm->color;
        g_DiffuseQuadXyz[1].diffuse = vm->color;
        g_DiffuseQuadXyz[2].diffuse = vm->color;
        g_DiffuseQuadXyz[3].diffuse = vm->color;
    }
    if (!g_Unknown6c6d18.cfg.flags.noDepthTest && this->zWriteDisabled != vm->flags.zWriteDisabled)
    {
        this->zWriteDisabled = vm->flags.zWriteDisabled;
        if (this->zWriteDisabled == 0)
        {
            g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        }
        else
        {
            g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        }
    }
}

int SpriteEngine::DrawQuad(Sprite *vm, int roundToPixel)
{
    if (roundToPixel)
    {
        __asm
        {
            fld dword ptr [g_Quad + 0x00]
            frndint
            fsub g_Half
            fld dword ptr [g_Quad + 0x18]
            frndint
            fsub g_Half
            fld dword ptr [g_Quad + 0x04]
            frndint
            fsub g_Half
            fld dword ptr [g_Quad + 0x34]
            frndint
            fsub g_Half
            fst dword ptr [g_Quad + 0x34]
            fstp dword ptr [g_Quad + 0x4c]
            fst dword ptr [g_Quad + 0x04]
            fstp dword ptr [g_Quad + 0x1c]
            fst dword ptr [g_Quad + 0x18]
            fstp dword ptr [g_Quad + 0x48]
            fst dword ptr [g_Quad + 0x00]
            fstp dword ptr [g_Quad + 0x30]
        }
    }
    g_Quad[0].pos.z = g_Quad[1].pos.z = g_Quad[2].pos.z = g_Quad[3].pos.z = vm->pos.z;
    if (this->currentSpriteData != vm->spriteData)
    {
        this->currentSpriteData = vm->spriteData;
        g_Quad[0].uv.x = g_Quad[2].uv.x = vm->spriteData->uvStartX + vm->uvScrollX;
        g_Quad[1].uv.x = g_Quad[3].uv.x = vm->spriteData->uvEndX + vm->uvScrollX;
        g_Quad[0].uv.y = g_Quad[1].uv.y = vm->spriteData->uvStartY + vm->uvScrollY;
        g_Quad[2].uv.y = g_Quad[3].uv.y = vm->spriteData->uvEndY + vm->uvScrollY;
        if (this->currentTexture != this->textures[vm->spriteData->sourceFileIndex])
        {
            this->currentTexture = this->textures[vm->spriteData->sourceFileIndex];
            g_Unknown6c6d18.d3dDevice->SetTexture(0, this->currentTexture);
        }
    }
    if (this->vertexFormat != 2)
    {
        if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
        {
            g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
        }
        else
        {
            g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        }
        this->vertexFormat = 2;
    }
    this->SetRenderStateForSprite(vm);
    if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
    {
        g_Unknown6c6d18.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(g_Quad[0]));
    }
    else
    {
        g_DiffuseQuad[0].pos = g_Quad[0].pos;
        g_DiffuseQuad[1].pos = g_Quad[1].pos;
        g_DiffuseQuad[2].pos = g_Quad[2].pos;
        g_DiffuseQuad[3].pos = g_Quad[3].pos;
        g_DiffuseQuad[0].uv.x = g_DiffuseQuad[2].uv.x = vm->spriteData->uvStartX + vm->uvScrollX;
        g_DiffuseQuad[1].uv.x = g_DiffuseQuad[3].uv.x = vm->spriteData->uvEndX + vm->uvScrollX;
        g_DiffuseQuad[0].uv.y = g_DiffuseQuad[1].uv.y = vm->spriteData->uvStartY + vm->uvScrollY;
        g_DiffuseQuad[2].uv.y = g_DiffuseQuad[3].uv.y = vm->spriteData->uvEndY + vm->uvScrollY;
        g_Unknown6c6d18.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_DiffuseQuad, sizeof(g_DiffuseQuad[0]));
    }
    return 0;
}

int SpriteEngine::Draw(Sprite *vm)
{
    float w;
    float h;

    if (!vm->flags.unk0)
    {
        return -1;
    }
    if (!vm->flags.visible)
    {
        return -1;
    }
    if (vm->color == 0)
    {
        return -1;
    }
    w = vm->spriteData->width * vm->scaleX / 2.0f;
    h = vm->spriteData->height * vm->scaleY / 2.0f;
    if ((vm->flags.anchor & 1) == 0)
    {
        g_Quad[0].pos.x = g_Quad[2].pos.x = vm->pos.x - w;
        g_Quad[1].pos.x = g_Quad[3].pos.x = w + vm->pos.x;
    }
    else
    {
        g_Quad[0].pos.x = g_Quad[2].pos.x = vm->pos.x;
        g_Quad[1].pos.x = g_Quad[3].pos.x = w + vm->pos.x + w;
    }
    if ((vm->flags.anchor & 2) == 0)
    {
        g_Quad[0].pos.y = g_Quad[1].pos.y = vm->pos.y - h;
        g_Quad[2].pos.y = g_Quad[3].pos.y = h + vm->pos.y;
    }
    else
    {
        g_Quad[0].pos.y = g_Quad[1].pos.y = vm->pos.y;
        g_Quad[2].pos.y = g_Quad[3].pos.y = h + vm->pos.y + h;
    }
    return this->DrawQuad(vm, 1);
}

void SpriteEngine::RotatePoint(float *out, float x, float y, float sine, float cosine, float cx, float cy)
{
    out[0] = x * cosine + y * sine + cx;
    out[1] = -x * sine + y * cosine + cy;
}

int SpriteEngine::DrawRotated(Sprite *vm)
{
    float w;
    float h;
    float y;
    float cx;
    float s;
    float angle;
    float c;

    if (vm->rotationZ == 0.0f)
    {
        return this->Draw(vm);
    }
    if (!vm->flags.unk0)
    {
        return -1;
    }
    if (!vm->flags.visible)
    {
        return -1;
    }
    if (vm->color == 0)
    {
        return -1;
    }
    angle = vm->rotationZ;
    __asm
    {
        fld angle
        fsincos
        fstp c
        fstp s
    }
    cx = Rint(vm->pos.x);
    y = Rint(vm->pos.y);
    w = Rint(vm->spriteData->width * vm->scaleX / 2.0f);
    h = Rint(vm->spriteData->height * vm->scaleY / 2.0f);
    this->RotatePoint((float *)&g_Quad[0].pos, -w - 0.5f, -h - 0.5f, s, c, cx, y);
    this->RotatePoint((float *)&g_Quad[1].pos, w - 0.5f, -h - 0.5f, s, c, cx, y);
    this->RotatePoint((float *)&g_Quad[2].pos, -w - 0.5f, h - 0.5f, s, c, cx, y);
    this->RotatePoint((float *)&g_Quad[3].pos, w - 0.5f, h - 0.5f, s, c, cx, y);
    g_Quad[0].pos.z = g_Quad[1].pos.z = g_Quad[2].pos.z = g_Quad[3].pos.z = vm->pos.z;
    if (vm->flags.anchor & 1)
    {
        g_Quad[0].pos.x += w;
        g_Quad[1].pos.x += w;
        g_Quad[2].pos.x += w;
        g_Quad[3].pos.x += w;
    }
    if (vm->flags.anchor & 2)
    {
        g_Quad[0].pos.y += h;
        g_Quad[1].pos.y += h;
        g_Quad[2].pos.y += h;
        g_Quad[3].pos.y += h;
    }
    return this->DrawQuad(vm, 0);
}

int SpriteEngine::DrawNoRound(Sprite *vm)
{
    float w;
    float h;

    if (!vm->flags.unk0)
    {
        return -1;
    }
    if (!vm->flags.visible)
    {
        return -1;
    }
    if (vm->color == 0)
    {
        return -1;
    }
    w = vm->spriteData->width * vm->scaleX / 2.0f;
    h = vm->spriteData->height * vm->scaleY / 2.0f;
    if ((vm->flags.anchor & 1) == 0)
    {
        g_Quad[0].pos.x = g_Quad[2].pos.x = vm->pos.x - w;
        g_Quad[1].pos.x = g_Quad[3].pos.x = w + vm->pos.x;
    }
    else
    {
        g_Quad[0].pos.x = g_Quad[2].pos.x = vm->pos.x;
        g_Quad[1].pos.x = g_Quad[3].pos.x = w + vm->pos.x + w;
    }
    if ((vm->flags.anchor & 2) == 0)
    {
        g_Quad[0].pos.y = g_Quad[1].pos.y = vm->pos.y - h;
        g_Quad[2].pos.y = g_Quad[3].pos.y = h + vm->pos.y;
    }
    else
    {
        g_Quad[0].pos.y = g_Quad[1].pos.y = vm->pos.y;
        g_Quad[2].pos.y = g_Quad[3].pos.y = h + vm->pos.y + h;
    }
    return this->DrawQuad(vm, 0);
}

int SpriteEngine::Draw3D(Sprite *vm)
{
    D3DXMATRIX textureMatrix;
    D3DXMATRIX rotationMatrix;
    D3DXMATRIX worldMatrix;

    if (!vm->flags.unk0)
    {
        return -1;
    }
    if (!vm->flags.visible)
    {
        return -1;
    }
    if (vm->color == 0)
    {
        return -1;
    }
    worldMatrix = vm->matrix;
    worldMatrix._11 *= vm->scaleX;
    worldMatrix._22 = -vm->scaleY * worldMatrix._22;
    if (vm->rotationX != 0.0)
    {
        D3DXMatrixRotationX(&rotationMatrix, vm->rotationX);
        D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotationMatrix);
    }
    if (vm->rotationY != 0.0)
    {
        D3DXMatrixRotationY(&rotationMatrix, vm->rotationY);
        D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotationMatrix);
    }
    if (vm->rotationZ != 0.0)
    {
        D3DXMatrixRotationZ(&rotationMatrix, vm->rotationZ);
        D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &rotationMatrix);
    }
    if ((vm->flags.anchor & 1) == 0)
    {
        worldMatrix._41 = vm->pos.x;
    }
    else
    {
        worldMatrix._41 = fabsf(vm->spriteData->width * vm->scaleX / 2.0f) + vm->pos.x;
    }
    if ((vm->flags.anchor & 2) == 0)
    {
        worldMatrix._42 = -vm->pos.y;
    }
    else
    {
        worldMatrix._42 = -vm->pos.y - fabsf(vm->spriteData->height * vm->scaleY / 2.0f);
    }
    worldMatrix._43 = vm->pos.z;
    g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix);
    if (this->currentSpriteData != vm->spriteData)
    {
        this->currentSpriteData = vm->spriteData;
        textureMatrix = vm->matrix;
        textureMatrix._31 = vm->spriteData->uvStartX + vm->uvScrollX;
        textureMatrix._32 = vm->spriteData->uvStartY + vm->uvScrollY;
        g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_TEXTURE0, &textureMatrix);
        if (this->currentTexture != this->textures[vm->spriteData->sourceFileIndex])
        {
            this->currentTexture = this->textures[vm->spriteData->sourceFileIndex];
            g_Unknown6c6d18.d3dDevice->SetTexture(0, this->currentTexture);
        }
    }
    if (this->vertexFormat != 3)
    {
        if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
        {
            g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX1);
            g_Unknown6c6d18.d3dDevice->SetStreamSource(0, this->vertexBuffer, sizeof(VertexTex1Xyz));
        }
        else
        {
            g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        }
        this->vertexFormat = 3;
    }
    this->SetRenderStateForSprite(vm);
    if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
    {
        g_Unknown6c6d18.d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }
    else
    {
        g_Unknown6c6d18.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_DiffuseQuadXyz, sizeof(g_DiffuseQuadXyz[0]));
    }
    return 0;
}

int SpriteEngine::Draw3DRounded(Sprite *vm)
{
    D3DXMATRIX textureMatrix;
    D3DXMATRIX rotationMatrix;
    D3DXMATRIX worldMatrix;

    if (!vm->flags.unk0)
    {
        return -1;
    }
    if (!vm->flags.visible)
    {
        return -1;
    }
    if (vm->rotationX != 0.0f || vm->rotationY != 0.0f || vm->rotationZ != 0.0f)
    {
        return this->Draw3D(vm);
    }
    if (vm->color == 0)
    {
        return -1;
    }
    worldMatrix = vm->matrix;
    worldMatrix._41 = Rint(vm->pos.x) - 0.5f;
    worldMatrix._42 = -Rint(vm->pos.y) + 0.5f;
    if (vm->flags.anchor & 1)
    {
        worldMatrix._41 += vm->spriteData->width * vm->scaleX / 2.0f;
    }
    if (vm->flags.anchor & 2)
    {
        worldMatrix._42 -= vm->spriteData->height * vm->scaleY / 2.0f;
    }
    worldMatrix._43 = vm->pos.z;
    worldMatrix._11 *= vm->scaleX;
    worldMatrix._22 = -vm->scaleY * worldMatrix._22;
    g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix);
    if (this->currentSpriteData != vm->spriteData)
    {
        this->currentSpriteData = vm->spriteData;
        textureMatrix = vm->matrix;
        textureMatrix._31 = vm->spriteData->uvStartX + vm->uvScrollX;
        textureMatrix._32 = vm->spriteData->uvStartY + vm->uvScrollY;
        g_Unknown6c6d18.d3dDevice->SetTransform(D3DTS_TEXTURE0, &textureMatrix);
        if (this->currentTexture != this->textures[vm->spriteData->sourceFileIndex])
        {
            this->currentTexture = this->textures[vm->spriteData->sourceFileIndex];
            g_Unknown6c6d18.d3dDevice->SetTexture(0, this->currentTexture);
        }
        if (this->vertexFormat != 3)
        {
            if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
            {
                g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX1);
                g_Unknown6c6d18.d3dDevice->SetStreamSource(0, this->vertexBuffer, sizeof(VertexTex1Xyz));
            }
            else
            {
                g_Unknown6c6d18.d3dDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
            }
            this->vertexFormat = 3;
        }
    }
    this->SetRenderStateForSprite(vm);
    if (!g_Unknown6c6d18.cfg.flags.noVertexBuffer)
    {
        g_Unknown6c6d18.d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }
    else
    {
        g_Unknown6c6d18.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_DiffuseQuadXyz, sizeof(g_DiffuseQuadXyz[0]));
    }
    return 0;
}

int SpriteEngine::Step(Sprite *vm)
{
    AnmInstr *curInstr;

    if (vm->scriptPos == NULL)
    {
        return 1;
    }
    if (vm->pendingInterrupt != 0)
    {
        goto interrupt;
    }
    while (curInstr = vm->scriptPos, curInstr->time <= vm->scriptTime.AsFrames())
    {
        switch (curInstr->opcode)
        {
        case 0:
            vm->flags.unk0 = 0;
        case 15:
            vm->scriptPos = NULL;
            return 1;
        case 1:
            vm->flags.unk0 = 1;
            this->SetSprite(vm, curInstr->args[0] + this->scriptSpriteOffsets[vm->scriptIndex]);
            vm->spriteTime = vm->scriptTime.AsFrames();
            break;
        case 16:
        {
            int *spriteArgs;

            vm->flags.unk0 = 1;
            spriteArgs = curInstr->args;
            this->SetSprite(vm, spriteArgs[0] + g_Rng.GetRandomU16InRange(*(WORD *)&spriteArgs[1]) +
                                    this->scriptSpriteOffsets[vm->scriptIndex]);
            vm->spriteTime = vm->scriptTime.AsFrames();
            break;
        }
        case 2:
            vm->scaleX = *(float *)&curInstr->args[0];
            vm->scaleY = *(float *)&curInstr->args[1];
            break;
        case 3:
            ((BYTE *)&vm->color)[3] = curInstr->args[0] & 0xff;
            break;
        case 4:
            vm->color = (vm->color & 0xff000000) | (curInstr->args[0] & 0xffffff);
            break;
        case 5:
            vm->scriptPos = (AnmInstr *)((BYTE *)vm->scriptStart + curInstr->args[0]);
            vm->scriptTime.current = vm->scriptPos->time;
            continue;
        case 7:
            vm->flags.flip ^= 1;
            vm->scaleX *= -1.0f;
            break;
        case 25:
            vm->flags.usePosOffset = curInstr->args[0];
            break;
        case 8:
            vm->flags.flip ^= 2;
            vm->scaleY *= -1.0f;
            break;
        case 9:
        {
            float *rotation;

            rotation = (float *)curInstr->args;
            vm->rotationX = *rotation;
            rotation++;
            vm->rotationY = *rotation;
            rotation++;
            vm->rotationZ = *rotation;
            break;
        }
        case 10:
        {
            float *rotationSpeed;

            rotationSpeed = (float *)curInstr->args;
            vm->rotationSpeedX = *rotationSpeed;
            rotationSpeed++;
            vm->rotationSpeedY = *rotationSpeed;
            rotationSpeed++;
            vm->rotationSpeedZ = *rotationSpeed;
            break;
        }
        case 11:
        {
            float *scaleSpeed;

            scaleSpeed = (float *)curInstr->args;
            vm->scaleTargetX = *scaleSpeed;
            scaleSpeed++;
            vm->scaleTargetY = *scaleSpeed;
            vm->scaleInterpTime = 0;
            break;
        }
        case 30:
        {
            float *scaleArgs;

            scaleArgs = (float *)curInstr->args;
            vm->scaleTargetX = *scaleArgs;
            scaleArgs++;
            vm->scaleTargetY = *scaleArgs;
            scaleArgs++;
            vm->scaleInterpTime = *(short *)scaleArgs;
            vm->scaleInterpTimer.Set(0);
            vm->scaleInitialX = vm->scaleX;
            vm->scaleInitialY = vm->scaleY;
            break;
        }
        case 12:
        {
            int *alphaArgs;

            alphaArgs = curInstr->args;
            vm->alphaInitial = vm->color;
            vm->alphaFinal = (vm->color & 0xffffff) | ((alphaArgs[0] & 0xff) << 24);
            vm->alphaInterpTime = *(short *)&alphaArgs[1];
            vm->alphaInterpTimer.Set(0);
            break;
        }
        case 13:
            vm->flags.blendMode = 1;
            break;
        case 14:
            vm->flags.blendMode = 0;
            break;
        case 17:
            if (!vm->flags.usePosOffset)
            {
                vm->pos = D3DXVECTOR3(*(float *)&curInstr->args[0], *(float *)&curInstr->args[1],
                                      *(float *)&curInstr->args[2]);
            }
            else
            {
                vm->posOffset = D3DXVECTOR3(*(float *)&curInstr->args[0], *(float *)&curInstr->args[1],
                                            *(float *)&curInstr->args[2]);
            }
            break;
        case 20:
            vm->flags.posInterpMode = 2;
            goto posInterp;
        case 19:
            vm->flags.posInterpMode = 1;
            goto posInterp;
        case 18:
            vm->flags.posInterpMode = 0;
        posInterp:
            if (!vm->flags.usePosOffset)
            {
                vm->posInitial = vm->pos;
            }
            else
            {
                vm->posInitial = vm->posOffset;
            }
            vm->posFinal = D3DXVECTOR3(*(float *)&curInstr->args[0], *(float *)&curInstr->args[1],
                                       *(float *)&curInstr->args[2]);
            vm->posInterpTime = *(short *)&curInstr->args[3];
            vm->posInterpTimer.Set(0);
            break;
        case 24:
            vm->flags.unk0 = 0;
        case 21:
        {
            AnmInstr *fallback;

            if (vm->pendingInterrupt == 0)
            {
                vm->flags.stopped = 1;
                vm->scriptTime.Sub(1);
                goto done;
            }
        interrupt:
            fallback = NULL;
            curInstr = vm->scriptStart;
            while ((curInstr->opcode != 22 || vm->pendingInterrupt != curInstr->args[0]) && curInstr->opcode != 0 &&
                   curInstr->opcode != 15)
            {
                if (curInstr->opcode == 22 && curInstr->args[0] == -1)
                {
                    fallback = curInstr;
                }
                curInstr = (AnmInstr *)((BYTE *)curInstr + 4 + curInstr->argsSize);
            }
            vm->pendingInterrupt = 0;
            vm->flags.stopped = 0;
            if (curInstr->opcode != 22)
            {
                if (fallback == NULL)
                {
                    vm->scriptTime.Sub(1);
                    goto done;
                }
                curInstr = fallback;
            }
            curInstr = (AnmInstr *)((BYTE *)curInstr + 4 + curInstr->argsSize);
            vm->scriptPos = curInstr;
            vm->scriptTime.Set(vm->scriptPos->time);
            vm->flags.unk0 = 1;
            continue;
        }
        case 29:
            vm->flags.unk0 = curInstr->args[0];
            break;
        case 23:
            vm->flags.anchor = 3;
            break;
        case 26:
            vm->autoRotate = *(short *)&curInstr->args[0];
            break;
        case 27:
            vm->uvScrollX += *(float *)&curInstr->args[0];
            if (vm->uvScrollX >= 1.0f)
            {
                vm->uvScrollX -= 1.0f;
            }
            else if (vm->uvScrollX < 0.0f)
            {
                vm->uvScrollX += 1.0f;
            }
            break;
        case 28:
            vm->uvScrollY += *(float *)&curInstr->args[0];
            if (vm->uvScrollY >= 1.0f)
            {
                vm->uvScrollY -= 1.0f;
            }
            else if (vm->uvScrollY < 0.0f)
            {
                vm->uvScrollY += 1.0f;
            }
            break;
        case 31:
            vm->flags.zWriteDisabled = curInstr->args[0];
            break;
        }
        vm->scriptPos = (AnmInstr *)((BYTE *)curInstr + 4 + curInstr->argsSize);
    }

done:
    if (vm->rotationSpeedX != 0.0f)
    {
        vm->rotationX = AddNormalizeAngle(vm->rotationX, g_Unknown6c6d18.frameAdvance * vm->rotationSpeedX);
    }
    if (vm->rotationSpeedY != 0.0f)
    {
        vm->rotationY = AddNormalizeAngle(vm->rotationY, g_Unknown6c6d18.frameAdvance * vm->rotationSpeedY);
    }
    if (vm->rotationSpeedZ != 0.0f)
    {
        vm->rotationZ = AddNormalizeAngle(vm->rotationZ, g_Unknown6c6d18.frameAdvance * vm->rotationSpeedZ);
    }
    if (vm->scaleInterpTime > 0)
    {
        vm->scaleInterpTimer.Tick();
        if (vm->scaleInterpTimer.AsFrames() >= vm->scaleInterpTime)
        {
            vm->scaleY = vm->scaleTargetY;
            vm->scaleX = vm->scaleTargetX;
            vm->scaleInterpTime = 0;
            vm->scaleTargetY = 0.0f;
            vm->scaleTargetX = 0.0f;
        }
        else
        {
            vm->scaleX = (vm->scaleTargetX - vm->scaleInitialX) * vm->scaleInterpTimer.AsFloat() /
                             (float)vm->scaleInterpTime +
                         vm->scaleInitialX;
            vm->scaleY = (vm->scaleTargetY - vm->scaleInitialY) * vm->scaleInterpTimer.AsFloat() /
                             (float)vm->scaleInterpTime +
                         vm->scaleInitialY;
        }
        if (vm->flags.flip & 1)
        {
            vm->scaleX *= -1.0f;
        }
        if (vm->flags.flip & 2)
        {
            vm->scaleY *= -1.0f;
        }
    }
    else
    {
        vm->scaleY += g_Unknown6c6d18.frameAdvance * vm->scaleTargetY;
        vm->scaleX += g_Unknown6c6d18.frameAdvance * vm->scaleTargetX;
    }
    if (vm->alphaInterpTime > 0)
    {
        D3DCOLOR col;
        D3DCOLOR goal;
        float alpha;
        int x;
        int i;

        vm->alphaInterpTimer.Tick();
        col = vm->alphaInitial;
        goal = vm->alphaFinal;
        alpha = vm->alphaInterpTimer.AsFloat() / (float)vm->alphaInterpTime;
        if (alpha >= 1.0f)
        {
            alpha = 1.0f;
        }
        for (i = 0; i < 4; i++)
        {
            x = (int)(((float)((BYTE *)&goal)[i] - (float)((BYTE *)&col)[i]) * alpha + (float)((BYTE *)&col)[i]);
            if (x < 0)
            {
                x = 0;
            }
            ((BYTE *)&col)[i] = x >= 0x100 ? 0xff : x;
        }
        vm->color = col;
        if (vm->alphaInterpTimer.AsFrames() >= vm->alphaInterpTime)
        {
            vm->alphaInterpTime = 0;
        }
    }
    if (vm->posInterpTime != 0)
    {
        float posT;

        posT = vm->posInterpTimer.AsFloat() / (float)vm->posInterpTime;
        if (posT >= 1.0f)
        {
            posT = 1.0f;
        }
        switch (vm->flags.posInterpMode)
        {
        case 1:
            posT = 1.0f - posT;
            posT = posT * posT;
            posT = 1.0f - posT;
            break;
        case 2:
            posT = 1.0f - posT;
            posT = posT * posT * posT * posT;
            posT = 1.0f - posT;
            break;
        }
        if (!vm->flags.usePosOffset)
        {
            vm->pos.x = posT * vm->posFinal.x + (1.0f - posT) * vm->posInitial.x;
            vm->pos.y = posT * vm->posFinal.y + (1.0f - posT) * vm->posInitial.y;
            vm->pos.z = posT * vm->posFinal.z + (1.0f - posT) * vm->posInitial.z;
        }
        else
        {
            vm->posOffset.x = posT * vm->posFinal.x + (1.0f - posT) * vm->posInitial.x;
            vm->posOffset.y = posT * vm->posFinal.y + (1.0f - posT) * vm->posInitial.y;
            vm->posOffset.z = posT * vm->posFinal.z + (1.0f - posT) * vm->posInitial.z;
        }
        if (vm->posInterpTimer.AsFrames() >= vm->posInterpTime)
        {
            vm->posInterpTime = 0;
        }
        vm->posInterpTimer.Tick();
    }
    vm->scriptTime.Tick();
    return 0;
}

void SpriteEngine::RenderTextToTextureIndex(int textureIndex, int x, int y, int width, int unk4, int fontHeight,
                                            int unk6, D3DCOLOR textColor, D3DCOLOR shadowColor, char *text)
{
    if (fontHeight <= 0)
    {
        fontHeight = 15;
    }
    if (unk6 <= 0)
    {
        unk6 = 15;
    }
    RenderTextToTexture(x, y, width, unk4, fontHeight, unk6, textColor, shadowColor, text, this->textures[textureIndex]);
}

void SpriteEngine::RenderText(Sprite *sprite, D3DCOLOR textColor, D3DCOLOR shadowColor, char *fmt, ...)
{
    va_list args;
    char buf[64];
    int fontW;

    fontW = sprite->unk_10c;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    this->RenderTextToTextureIndex(sprite->spriteData->sourceFileIndex, sprite->spriteData->startX,
                                   sprite->spriteData->startY, sprite->spriteData->textureWidth,
                                   sprite->spriteData->textureHeight, fontW, sprite->unk_10d, textColor, shadowColor,
                                   buf);
    sprite->flags.unk0 = 1;
}

void SpriteEngine::RenderTextRightAligned(Sprite *vm, D3DCOLOR color, D3DCOLOR shadowColor, char *fmt, ...)
{
    va_list args;
    int xPos;
    char buf[64];
    int fontW;

    fontW = vm->unk_10c <= 0 ? 15 : vm->unk_10c;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    this->RenderTextToTextureIndex(vm->spriteData->sourceFileIndex, vm->spriteData->startX, vm->spriteData->startY,
                                   vm->spriteData->textureWidth, vm->spriteData->textureHeight, fontW, vm->unk_10d,
                                   color, shadowColor, " ");
    xPos = vm->spriteData->startX + vm->spriteData->textureWidth -
           (float)strlen(buf) * (float)(fontW + 1) / 2.0f;
    this->RenderTextToTextureIndex(vm->spriteData->sourceFileIndex, xPos, vm->spriteData->startY,
                                   vm->spriteData->textureWidth, vm->spriteData->textureHeight, fontW, vm->unk_10d,
                                   color, shadowColor, buf);
    vm->flags.unk0 = 1;
}

void SpriteEngine::RenderTextCentered(Sprite *vm, D3DCOLOR color, D3DCOLOR shadowColor, char *fmt, ...)
{
    va_list args;
    int xPos;
    char buf[64];
    int fontW;

    fontW = vm->unk_10c <= 0 ? 15 : vm->unk_10c;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    this->RenderTextToTextureIndex(vm->spriteData->sourceFileIndex, vm->spriteData->startX, vm->spriteData->startY,
                                   vm->spriteData->textureWidth, vm->spriteData->textureHeight, fontW, vm->unk_10d,
                                   color, shadowColor, " ");
    xPos = vm->spriteData->startX + vm->spriteData->textureWidth / 2.0f -
           (float)strlen(buf) * (float)(fontW + 1) / 4.0f;
    this->RenderTextToTextureIndex(vm->spriteData->sourceFileIndex, xPos, vm->spriteData->startY,
                                   vm->spriteData->textureWidth, vm->spriteData->textureHeight, fontW, vm->unk_10d,
                                   color, shadowColor, buf);
    vm->flags.unk0 = 1;
}

int SpriteEngine::LoadSurface(int surface, char *path)
{
    IDirect3DSurface8 *srcSurface;
    void *fileData;

    if (this->surfaces[surface] != NULL)
    {
        this->ReleaseSurface(surface);
    }
    fileData = LoadFileData(path, 0);
    if (fileData == NULL)
    {
        LogError(&g_Unknown69d998, "%sが読み込めないです。\n", path);
        return -1;
    }
    if (g_Unknown6c6d18.d3dDevice->CreateImageSurface(640, 1024, g_Unknown6c6d18.presentParams.BackBufferFormat,
                                                     &srcSurface) != D3D_OK)
    {
        return -1;
    }
    if (D3DXLoadSurfaceFromFileInMemory(srcSurface, NULL, NULL, fileData, g_FileSize, NULL, D3DX_FILTER_NONE, 0,
                                        &this->surfaceInfos[surface]) != D3D_OK)
    {
        goto fail;
    }
    if (g_Unknown6c6d18.d3dDevice->CreateRenderTarget(
            this->surfaceInfos[surface].Width, this->surfaceInfos[surface].Height,
            g_Unknown6c6d18.presentParams.BackBufferFormat, D3DMULTISAMPLE_NONE, TRUE, &this->surfaces[surface]) != D3D_OK &&
        g_Unknown6c6d18.d3dDevice->CreateImageSurface(this->surfaceInfos[surface].Width,
                                                     this->surfaceInfos[surface].Height,
                                                     g_Unknown6c6d18.presentParams.BackBufferFormat,
                                                     &this->surfaces[surface]) != D3D_OK)
    {
        goto fail;
    }
    if (g_Unknown6c6d18.d3dDevice->CreateImageSurface(this->surfaceInfos[surface].Width,
                                                     this->surfaceInfos[surface].Height,
                                                     g_Unknown6c6d18.presentParams.BackBufferFormat,
                                                     &this->surfacesBackup[surface]) != D3D_OK)
    {
        goto fail;
    }
    if (D3DXLoadSurfaceFromSurface(this->surfaces[surface], NULL, NULL, srcSurface, NULL, NULL, D3DX_FILTER_NONE, 0) !=
        D3D_OK)
    {
        goto fail;
    }
    if (D3DXLoadSurfaceFromSurface(this->surfacesBackup[surface], NULL, NULL, srcSurface, NULL, NULL, D3DX_FILTER_NONE,
                                   0) != D3D_OK)
    {
        goto fail;
    }
    if (srcSurface != NULL)
    {
        srcSurface->Release();
        srcSurface = NULL;
    }
    free(fileData);
    return 0;

fail:
    if (srcSurface != NULL)
    {
        srcSurface->Release();
        srcSurface = NULL;
    }
    free(fileData);
    return -1;
}

void SpriteEngine::ReleaseSurface(int surface)
{
    if (this->surfaces[surface] != NULL)
    {
        this->surfaces[surface]->Release();
        this->surfaces[surface] = NULL;
    }
    if (this->surfacesBackup[surface] != NULL)
    {
        this->surfacesBackup[surface]->Release();
        this->surfacesBackup[surface] = NULL;
    }
}

void SpriteEngine::CopySurfaceToBackBuffer(int surface, int left, int top, int x, int y)
{
    IDirect3DSurface8 *backBuffer;
    RECT rect;
    POINT point;

    if (this->surfacesBackup[surface] == NULL)
    {
        return;
    }
    if (g_Unknown6c6d18.d3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer) != D3D_OK)
    {
        return;
    }
    if (this->surfaces[surface] == NULL)
    {
        if (g_Unknown6c6d18.d3dDevice->CreateRenderTarget(
                this->surfaceInfos[surface].Width, this->surfaceInfos[surface].Height,
                g_Unknown6c6d18.presentParams.BackBufferFormat, D3DMULTISAMPLE_NONE, TRUE,
                &this->surfaces[surface]) != D3D_OK &&
            g_Unknown6c6d18.d3dDevice->CreateImageSurface(this->surfaceInfos[surface].Width,
                                                         this->surfaceInfos[surface].Height,
                                                         g_Unknown6c6d18.presentParams.BackBufferFormat,
                                                         &this->surfaces[surface]) != D3D_OK)
        {
            backBuffer->Release();
            return;
        }
        if (D3DXLoadSurfaceFromSurface(this->surfaces[surface], NULL, NULL, this->surfacesBackup[surface], NULL, NULL,
                                       D3DX_FILTER_NONE, 0) != D3D_OK)
        {
            backBuffer->Release();
            return;
        }
    }
    rect.left = left;
    rect.top = top;
    rect.right = this->surfaceInfos[surface].Width;
    rect.bottom = this->surfaceInfos[surface].Height;
    point.x = x;
    point.y = y;
    g_Unknown6c6d18.d3dDevice->CopyRects(this->surfaces[surface], &rect, 1, backBuffer, &point);
    backBuffer->Release();
}

void SpriteEngine::CopySurfaceRectToBackBuffer(int surface, int x, int y, int left, int top, int width, int height)
{
    IDirect3DSurface8 *backBuffer;
    RECT rect;
    POINT point;

    if (this->surfacesBackup[surface] == NULL)
    {
        return;
    }
    if (g_Unknown6c6d18.d3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer) != D3D_OK)
    {
        return;
    }
    if (this->surfaces[surface] == NULL)
    {
        if (g_Unknown6c6d18.d3dDevice->CreateRenderTarget(
                this->surfaceInfos[surface].Width, this->surfaceInfos[surface].Height,
                g_Unknown6c6d18.presentParams.BackBufferFormat, D3DMULTISAMPLE_NONE, TRUE,
                &this->surfaces[surface]) != D3D_OK &&
            g_Unknown6c6d18.d3dDevice->CreateImageSurface(this->surfaceInfos[surface].Width,
                                                         this->surfaceInfos[surface].Height,
                                                         g_Unknown6c6d18.presentParams.BackBufferFormat,
                                                         &this->surfaces[surface]) != D3D_OK)
        {
            backBuffer->Release();
            return;
        }
        if (D3DXLoadSurfaceFromSurface(this->surfaces[surface], NULL, NULL, this->surfacesBackup[surface], NULL, NULL,
                                       D3DX_FILTER_NONE, 0) != D3D_OK)
        {
            backBuffer->Release();
            return;
        }
    }
    rect.left = left;
    rect.top = top;
    rect.right = left + width;
    rect.bottom = top + height;
    point.x = x;
    point.y = y;
    g_Unknown6c6d18.d3dDevice->CopyRects(this->surfaces[surface], &rect, 1, backBuffer, &point);
    backBuffer->Release();
}

void SpriteEngine::CopyBackBufferToTexture(int texture, int left, int top, int width, int height)
{
    IDirect3DSurface8 *backBuffer;
    IDirect3DSurface8 *dest;
    RECT rect;

    if (this->textures[texture] == NULL)
    {
        return;
    }
    if (g_Unknown6c6d18.d3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer) != D3D_OK)
    {
        return;
    }
    if (this->textures[texture]->GetSurfaceLevel(0, &dest) != D3D_OK)
    {
        backBuffer->Release();
        return;
    }
    rect.left = left;
    rect.top = top;
    rect.right = left + width;
    rect.bottom = top + height;
    if (D3DXLoadSurfaceFromSurface(dest, NULL, NULL, backBuffer, NULL, &rect, D3DX_DEFAULT, 0) != D3D_OK)
    {
        dest->Release();
        backBuffer->Release();
        return;
    }
    dest->Release();
    backBuffer->Release();
}
