// The file with Stage, from 0x403810.

#include "stage.h"
#include "unsorted.h"

void Stage::RemoveChain()
{
    g_Chain.Remove(&g_StageCalcChain);
    g_Chain.Remove(&g_StageDrawChain);
    g_Chain.Remove(&g_StageDrawChain2);
}

int Stage::RegisterChain(int stage)
{
    Stage *s = &g_Stage;

    memset(s, 0, sizeof(Stage));
    s->data = NULL;
    s->timer.Set(0);
    s->stage = stage;
    g_StageCalcChain.SetCallback((ChainCallback)Stage::OnUpdate);
    g_StageCalcChain.addedCallback = (ChainCallback)Stage::AddedCallback;
    g_StageCalcChain.deletedCallback = (ChainCallback)Stage::DeletedCallback;
    g_StageCalcChain.arg = s;
    if (g_Chain.AddToCalcChain(&g_StageCalcChain, 6) != 0)
    {
        return -1;
    }
    g_StageDrawChain.SetCallback((ChainCallback)Stage::OnDraw);
    g_StageDrawChain.arg = s;
    g_Chain.AddToDrawChain(&g_StageDrawChain, 3);
    g_StageDrawChain2.SetCallback((ChainCallback)Stage::OnDraw2);
    g_StageDrawChain2.arg = s;
    g_Chain.AddToDrawChain(&g_StageDrawChain2, 4);
    return 0;
}

int Stage::DeletedCallback(Stage *stage)
{
    g_SpriteEngine->ReleaseAnm(4);
    if (stage->sprites != NULL)
    {
        MemFree(stage->sprites);
        stage->sprites = NULL;
    }
    if (stage->data != NULL)
    {
        MemFree(stage->data);
        stage->data = NULL;
    }
    return 0;
}

int Stage::AddedCallback(Stage *stage)
{
    stage->unk_01c.Set(0);
    stage->unk_028 = 0;
    stage->unk_03c.x = 0.0f;
    stage->unk_03c.y = 0.0f;
    stage->unk_03c.z = 0.0f;
    stage->unk_080 = 0;
    stage->unk_06c = 0;
    if (stage->Load(g_StageFiles[g_GameManager.stage].anmPath, g_StageFiles[g_GameManager.stage].dataPath) != 0)
    {
        return -1;
    }
    stage->fogColor = 0xff000000;
    stage->fogNear = 200.0f;
    stage->fogFar = 500.0f;
    stage->unk_2b8 = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    stage->unk_2ac = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    stage->unk_2c4 = 1;
    stage->unk_2c8.Set(0);
    stage->unk_2a8 = 0;
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, stage->fogColor);
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&stage->fogNear);
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&stage->fogFar);
    return 0;
}

int Stage::OnDraw(Stage *stage)
{
    if (stage->fogColorChanged)
    {
        stage->fogColorChanged = 0;
        g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, stage->fogColor);
    }
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&stage->fogNear);
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&stage->fogFar);
    if (stage->unk_080 <= 1 && !g_Gui.Func4172d3())
    {
        stage->DrawLayer(0);
        stage->DrawLayer(1);
    }
    return 1;
}

int Stage::OnDraw2(Stage *stage)
{
    float fog;
    int opacity;
    float area[4];

    if (stage->unk_080 <= 1 && !g_Gui.Func4172d3())
    {
        stage->DrawLayer(2);
        stage->DrawLayer(3);
        if (stage->unk_080 == 1)
        {
            area[0] = 32.0f;
            area[1] = 16.0f;
            area[2] = 416.0f;
            area[3] = 464.0f;
            opacity = stage->unk_084 * 0xff / 60;
            Func42f8b0(area, opacity << 24);
        }
    }
    if (stage->unk_080 >= 1)
    {
        if (stage->unk_084 <= g_Unknown6c6d18.cfg.frameSkip)
        {
            g_SpriteEngine->StartScript(&stage->unk_088, 0x2b3);
        }
        g_SpriteEngine->Func432cc0(&stage->unk_088);
    }
    g_Unknown6c6d18.viewport.MinZ = 0.0f;
    g_Unknown6c6d18.viewport.MaxZ = 0.5f;
    Func41c28c(0);
    g_Unknown6c6d18.d3dDevice->SetViewport(&g_Unknown6c6d18.viewport);
    fog = 1000.0f;
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&fog);
    fog = 2000.0f;
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&fog);
    return 1;
}

int Stage::Load(char *anmPath, char *dataPath)
{
    int count;
    int i;
    StageObject *ob;
    StageQuad *quad;

    if (g_SpriteEngine->LoadAnm(4, anmPath, 0x300) != 0)
    {
        return -1;
    }
    this->data = (StageData *)Func41e290(dataPath, 0);
    if (this->data == NULL)
    {
        Func41e4d0(&g_Unknown69d998, "ステージデータが見つかりません。データが壊れています\n");
        return -1;
    }
    this->objectCount = this->data->objectCount;
    this->quadCount = this->data->quadCount;
    this->instances = (StageInstance *)(this->data->unk_04 + (int)this->data);
    this->unk_018 = (void *)(this->data->unk_08 + (int)this->data);
    this->objects = (StageObject **)((char *)this->data + 0x490);
    for (i = 0; i < this->objectCount; i++)
    {
        this->objects[i] = (StageObject *)((int)this->objects[i] + (int)this->data);
    }
    this->sprites = (Sprite *)MemAlloc(this->quadCount * sizeof(Sprite));
    for (i = 0, count = 0; i < this->objectCount; i++)
    {
        ob = this->objects[i];
        ob->unk_03 = 1;
        quad = ob->quads;
        while (quad->type >= 0)
        {
            g_SpriteEngine->Func4051b0(&this->sprites[count], quad->script + 0x300);
            quad->spriteIndex = count;
            count++;
            quad = (StageQuad *)((char *)quad + quad->size);
        }
    }
    return 0;
}

// Whether the projected point shows on screen vertically.
#define IN_VIEWPORT_Y(v)                                                                                    \
    ((v).y >= (float)g_Unknown6c6d18.viewport.Y &&                                                          \
     (v).y <= (float)(g_Unknown6c6d18.viewport.Y + g_Unknown6c6d18.viewport.Height))

int Stage::DrawLayer(int layer)
{
    StageInstance *placement;
    int objCount;
    int rendered;
    D3DXVECTOR3 objOrigin;
    D3DXMATRIX mat;
    D3DXVECTOR3 scrPos;
    D3DXVECTOR3 screenRight;
    StageObject *objData;
    StageQuad *quadData;
    Sprite *spr;
    // Set to 0 for each object, never read.
    int mode;

    placement = this->instances;
    objCount = 0;
    rendered = 0;
    objOrigin.x = 0.0f;
    objOrigin.y = 0.0f;
    objOrigin.z = 0.0f;
    D3DXMatrixIdentity(&mat);
    while (placement->id >= 0)
    {
        objData = this->objects[placement->id];
        if (objData->layer == layer)
        {
            quadData = objData->quads;
            mode = 0;
            mat._41 = objData->pos.x + placement->pos.x - this->unk_03c.x;
            mat._42 = -(objData->pos.y + placement->pos.y - this->unk_03c.y);
            mat._43 = objData->pos.z + placement->pos.z - this->unk_03c.z + objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 -= objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._43 -= objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 += objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._41 = objData->pos.x + placement->pos.x - this->unk_03c.x + objData->size.x;
            mat._42 = -(objData->pos.y + placement->pos.y - this->unk_03c.y);
            mat._43 = objData->pos.z + placement->pos.z - this->unk_03c.z + objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 -= objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._43 -= objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 += objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport, &g_Unknown6c6d18.projectionMatrix,
                            &g_Unknown6c6d18.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            goto next;

        render:
            rendered = 1;
            while (quadData->type >= 0)
            {
                spr = &this->sprites[quadData->spriteIndex];
                switch (quadData->type)
                {
                case 0:
                    spr->pos.x = quadData->pos.x + placement->pos.x - this->unk_03c.x;
                    spr->pos.y = quadData->pos.y + placement->pos.y - this->unk_03c.y;
                    spr->pos.z = quadData->pos.z + placement->pos.z - this->unk_03c.z;
                    if (quadData->width != 0.0f)
                    {
                        spr->scaleX = quadData->width / spr->spriteData->width;
                    }
                    if (quadData->height != 0.0f)
                    {
                        spr->scaleY = quadData->height / spr->spriteData->height;
                    }
                    if (spr->unk_088 == 2)
                    {
                        float width;

                        if (quadData->width != 0.0f)
                        {
                            width = quadData->width;
                        }
                        else
                        {
                            width = spr->spriteData->width;
                        }
                        mat._41 = spr->pos.x;
                        mat._42 = -spr->pos.y;
                        mat._43 = spr->pos.z;
                        D3DXVec3Project(&scrPos, &objOrigin, &g_Unknown6c6d18.viewport,
                                        &g_Unknown6c6d18.projectionMatrix, &g_Unknown6c6d18.viewMatrix, &mat);
                        mat._41 += width * spr->scaleX;
                        D3DXVec3Project(&screenRight, &objOrigin, &g_Unknown6c6d18.viewport,
                                        &g_Unknown6c6d18.projectionMatrix, &g_Unknown6c6d18.viewMatrix, &mat);
                        spr->scaleX = (screenRight.x - scrPos.x) / width;
                        spr->scaleY = spr->scaleX;
                        spr->pos = scrPos;
                        g_SpriteEngine->Func432fa0(spr);
                    }
                    else
                    {
                        g_SpriteEngine->Func433150(spr);
                    }
                    break;
                }
                quadData = (StageQuad *)((char *)quadData + quadData->size);
            }
            objCount++;
        }
    next:
        placement++;
    }
    return 0;
}
