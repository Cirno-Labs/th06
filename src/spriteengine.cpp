// The file with SpriteEngine.

#include "sprite.h"
#include "unsorted.h"

SpriteEngine::SpriteEngine()
{
    int i;

    this->unk_1c930 = 0;
    memset(this, 0, sizeof(*this));
    for (i = 0; i < 0x800; i++)
    {
        this->sprites[i].unk_00 = -1;
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
    this->unk_210b4 = 1;
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

int SpriteEngine::CreateTexture(int index, int width, int height, int format)
{
    D3DXCreateTexture(g_Unknown6c6d18.d3dDevice, width, height, 1, 0, g_TextureFormats[format], D3DPOOL_MANAGED,
                      &this->textures[index]);
    return 0;
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

void SpriteEngine::RotatePoint(float *out, float sine, float cosine, float x, float y, float cx, float cy)
{
    out[0] = sine * y + cosine * x + cx;
    out[1] = -sine * x + cosine * y + cy;
}
