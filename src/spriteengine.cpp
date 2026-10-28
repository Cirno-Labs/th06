// The file with SpriteEngine.

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
    this->unk_21118 = -1;
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
    this->textureFiles[index] = Func41e290(path, 0);
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

    fileData = Func41e290(path, 0);
    if (fileData == NULL)
    {
        return -1;
    }
    this->textures[index]->GetLevelDesc(0, &desc);
    if (desc.Format != D3DFMT_A8R8G8B8 && desc.Format != D3DFMT_A4R4G4B4 && desc.Format != D3DFMT_A1R5G5B5)
    {
        Func41e620(&g_Unknown69d998, "error : イメージがαを持っていません\n");
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
    this->anmFiles[slot] = (AnmFile *)Func41e290(path, 0);
    anmHeader = this->anmFiles[slot];
    if (anmHeader == NULL)
    {
        Func41e620(&g_Unknown69d998, "スプライトアニメ %s が読み込めません。データが失われてるか壊れています\n", path);
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
        Func41e620(&g_Unknown69d998, "テクスチャ %s が読み込めません。データが失われてるか壊れています\n", name);
        return -1;
    }
    if (anmHeader->alphaNameOffset != 0)
    {
        name = (char *)anmHeader + anmHeader->alphaNameOffset;
        if (this->LoadTextureAlpha(anmHeader->textureIndex, name, anmHeader->format, anmHeader->colorKey) != 0)
        {
            Func41e620(&g_Unknown69d998, "テクスチャ %s が読み込めません。データが失われてるか壊れています\n", name);
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

void SpriteEngine::SetScript(Sprite *vm, void *script)
{
    vm->flags.unk6 = 0;
    vm->Initialize();
    vm->scriptStart = script;
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
