// The file with Stage, from 0x403790.

#include "stage.h"
#include "unsorted.h"

Stage::Stage()
{
}

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
    stage->scriptTime.Set(0);
    stage->scriptIndex = 0;
    stage->cameraPos.x = 0.0f;
    stage->cameraPos.y = 0.0f;
    stage->cameraPos.z = 0.0f;
    stage->unk_080 = 0;
    stage->fogFadeFrames = 0;
    if (stage->Load(g_StageFiles[g_GameManager.stage].anmPath, g_StageFiles[g_GameManager.stage].dataPath) != 0)
    {
        return -1;
    }
    stage->fog.color = 0xff000000;
    stage->fog.nearPlane = 200.0f;
    stage->fog.farPlane = 500.0f;
    stage->offsetTo = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    stage->offsetFrom = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    stage->offsetFadeFrames = 1;
    stage->offsetFadeTime.Set(0);
    stage->unk_2a8 = 0;
    g_App.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, stage->fog.color);
    g_App.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&stage->fog.nearPlane);
    g_App.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&stage->fog.farPlane);
    return 0;
}

int Stage::OnUpdate(Stage *stage)
{
    float keyT;
    StageInstr *cmd;
    D3DXVECTOR3 tmpVec;
    float camRatio;
    float fogT;
    int j;

    if (stage->data == NULL)
    {
        return 1;
    }
    if (g_GameManager.isFrozen)
    {
        ((BYTE *)&stage->unk_088.color)[3] = 0x60;
        ((BYTE *)&stage->unk_088.color)[2] = 0x80;
        ((BYTE *)&stage->unk_088.color)[1] = 0x30;
        ((BYTE *)&stage->unk_088.color)[0] = 0x30;
        return 1;
    }
    for (;;)
    {
        cmd = &stage->script[stage->scriptIndex];
        switch (cmd->opcode)
        {
        case 0:
            if (cmd->frame == -1)
            {
                stage->keyPos = cmd->args;
                stage->cameraPos.x = stage->keyPos.x;
                stage->cameraPos.y = stage->keyPos.y;
                stage->cameraPos.z = stage->keyPos.z;
            }
            else if (stage->scriptTime >= cmd->frame)
            {
                tmpVec = cmd->args;
                stage->cameraPos.x = tmpVec.x;
                stage->cameraPos.y = tmpVec.y;
                stage->cameraPos.z = tmpVec.z;
                stage->keyPos = tmpVec;
                stage->keyFrame = cmd->frame;
                stage->scriptIndex++;
                cmd++;
                while (cmd->opcode != 0)
                {
                    cmd++;
                }
                stage->nextKeyFrame = cmd->frame;
                stage->nextKeyPos = cmd->args;
            }
            break;
        case 1:
            if (stage->scriptTime >= cmd->frame)
            {
                stage->fog.color = *(D3DCOLOR *)&cmd->args.x;
                stage->fog.nearPlane = cmd->args.y;
                stage->fog.farPlane = cmd->args.z;
                if (stage->fogFadeFrames == 0)
                {
                    g_App.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, stage->fog.color);
                    g_App.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&stage->fog.nearPlane);
                    g_App.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&stage->fog.farPlane);
                }
                stage->scriptIndex++;
                stage->fogTarget = stage->fog;
                continue;
            }
            break;
        case 4:
            if (stage->scriptTime >= cmd->frame)
            {
                stage->fogStart = stage->fog;
                stage->fogFadeFrames = *(int *)&cmd->args.x;
                stage->fogFadeTime.Set(0);
                stage->scriptIndex++;
                continue;
            }
            break;
        case 2:
            if (stage->scriptTime >= cmd->frame)
            {
                stage->offsetFrom = stage->offsetTo;
                stage->offsetTo = cmd->args;
                stage->scriptIndex++;
                continue;
            }
            break;
        case 3:
            if (stage->scriptTime >= cmd->frame)
            {
                stage->offsetFadeFrames = *(int *)&cmd->args.x;
                stage->offsetFadeTime.Set(0);
                stage->scriptIndex++;
                continue;
            }
            break;
        case 5:
            if (stage->unk_2a8)
            {
                stage->scriptIndex++;
                stage->unk_2a8 = 0;
                continue;
            }
            break;
        }
        break;
    }
    if (cmd->frame != -1)
    {
        keyT = (stage->scriptTime.AsFloat() - stage->keyFrame) / (stage->nextKeyFrame - stage->keyFrame);
        tmpVec = stage->nextKeyPos;
        stage->cameraPos.x = (tmpVec.x - stage->keyPos.x) * keyT + stage->keyPos.x;
        stage->cameraPos.y = (tmpVec.y - stage->keyPos.y) * keyT + stage->keyPos.y;
        stage->cameraPos.z = (tmpVec.z - stage->keyPos.z) * keyT + stage->keyPos.z;
    }
    if (stage->offsetFadeFrames != 0)
    {
        if (stage->offsetFadeTime < stage->offsetFadeFrames)
        {
            stage->offsetFadeTime.Tick();
        }
        else
        {
            stage->offsetFadeTime.Set(stage->offsetFadeFrames);
        }
        tmpVec = stage->offsetTo - stage->offsetFrom;
        camRatio = stage->offsetFadeTime.AsFloat() / stage->offsetFadeFrames;
        g_Camera.offsetX = tmpVec.x * camRatio + stage->offsetFrom.x;
        g_Camera.offsetY = tmpVec.y * camRatio + stage->offsetFrom.y;
        g_Camera.zoom = tmpVec.z * camRatio + stage->offsetFrom.z;
    }
    if (stage->fogFadeFrames != 0)
    {
        stage->fogFadeTime.Tick();
        fogT = stage->fogFadeTime.AsFloat() / stage->fogFadeFrames;
        if (fogT >= 1.0f)
        {
            fogT = 1.0f;
        }
        for (j = 0; j < 4; j++)
        {
            ((BYTE *)&stage->fog.color)[j] =
                ((float)((BYTE *)&stage->fogTarget.color)[j] - (float)((BYTE *)&stage->fogStart.color)[j]) * fogT +
                (float)((BYTE *)&stage->fogStart.color)[j];
        }
        stage->fog.nearPlane = (stage->fogTarget.nearPlane - stage->fogStart.nearPlane) * fogT + stage->fogStart.nearPlane;
        stage->fog.farPlane = (stage->fogTarget.farPlane - stage->fogStart.farPlane) * fogT + stage->fogStart.farPlane;
        g_App.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, stage->fog.color);
        g_App.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&stage->fog.nearPlane);
        g_App.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&stage->fog.farPlane);
        if (stage->fogFadeTime >= stage->fogFadeFrames)
        {
            stage->fogFadeFrames = 0;
        }
    }
    if (cmd->opcode != 5)
    {
        stage->scriptTime.Tick();
    }
    stage->StepObjects();
    if (stage->unk_080 >= 1)
    {
        if (stage->unk_084 == 60)
        {
            stage->unk_080++;
        }
        stage->unk_084++;
        g_SpriteEngine->Step(&stage->unk_088);
    }
    return 1;
}

int Stage::OnDraw(Stage *stage)
{
    if (stage->fogColorChanged)
    {
        stage->fogColorChanged = 0;
        g_App.d3dDevice->SetRenderState(D3DRS_FOGCOLOR, stage->fog.color);
    }
    g_App.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&stage->fog.nearPlane);
    g_App.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&stage->fog.farPlane);
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
            DrawColorRect(area, opacity << 24);
        }
    }
    if (stage->unk_080 >= 1)
    {
        if (stage->unk_084 <= g_App.cfg.frameSkip)
        {
            g_SpriteEngine->StartScript(&stage->unk_088, 0x2b3);
        }
        g_SpriteEngine->DrawRotated(&stage->unk_088);
    }
    g_App.viewport.MinZ = 0.0f;
    g_App.viewport.MaxZ = 0.5f;
    SetupFixedCamera(0.0f);
    g_App.d3dDevice->SetViewport(&g_App.viewport);
    fog = 1000.0f;
    g_App.d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&fog);
    fog = 2000.0f;
    g_App.d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&fog);
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
    this->data = (StageData *)LoadFileData(dataPath, 0);
    if (this->data == NULL)
    {
        LogPrintf(&g_Log, "ステージデータが見つかりません。データが壊れています\n");
        return -1;
    }
    this->objectCount = this->data->objectCount;
    this->quadCount = this->data->quadCount;
    this->instances = (StageInstance *)(this->data->unk_04 + (int)this->data);
    this->script = (StageInstr *)(this->data->unk_08 + (int)this->data);
    this->objects = (StageObject **)((char *)this->data + 0x490);
    for (i = 0; i < this->objectCount; i++)
    {
        this->objects[i] = (StageObject *)((int)this->objects[i] + (int)this->data);
    }
    this->sprites = (Sprite *)MemAlloc(this->quadCount * sizeof(Sprite));
    for (i = 0, count = 0; i < this->objectCount; i++)
    {
        ob = this->objects[i];
        ob->flags = 1;
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

int Stage::StepObjects()
{
    StageQuad *altQuad;
    int busy;
    int i;
    Sprite *sprite;
    StageObject *obj;
    StageQuad *piece;

    for (i = 0; i < this->objectCount; i++)
    {
        obj = this->objects[i];
        if (obj->flags & 1)
        {
            busy = 0;
            piece = obj->quads;
            while (piece->type >= 0)
            {
                sprite = &this->sprites[piece->spriteIndex];
                switch (piece->type)
                {
                case 0:
                    g_SpriteEngine->Step(sprite);
                    break;
                case 1:
                    altQuad = piece;
                    g_SpriteEngine->Step(sprite);
                    break;
                }
                if (sprite->scriptPos != NULL)
                {
                    busy++;
                }
                piece = (StageQuad *)((char *)piece + piece->size);
            }
            if (busy == 0)
            {
                obj->flags &= ~1;
            }
        }
    }
    return 0;
}

// Whether the projected point shows on screen vertically.
#define IN_VIEWPORT_Y(v)                                                                                    \
    ((v).y >= (float)g_App.viewport.Y &&                                                          \
     (v).y <= (float)(g_App.viewport.Y + g_App.viewport.Height))

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
            mat._41 = objData->pos.x + placement->pos.x - this->cameraPos.x;
            mat._42 = -(objData->pos.y + placement->pos.y - this->cameraPos.y);
            mat._43 = objData->pos.z + placement->pos.z - this->cameraPos.z + objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 -= objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._43 -= objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 += objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._41 = objData->pos.x + placement->pos.x - this->cameraPos.x + objData->size.x;
            mat._42 = -(objData->pos.y + placement->pos.y - this->cameraPos.y);
            mat._43 = objData->pos.z + placement->pos.z - this->cameraPos.z + objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 -= objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._43 -= objData->size.z;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
            if (IN_VIEWPORT_Y(scrPos))
            {
                goto render;
            }
            mat._42 += objData->size.y;
            D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport, &g_App.projectionMatrix,
                            &g_App.viewMatrix, &mat);
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
                    spr->pos.x = quadData->pos.x + placement->pos.x - this->cameraPos.x;
                    spr->pos.y = quadData->pos.y + placement->pos.y - this->cameraPos.y;
                    spr->pos.z = quadData->pos.z + placement->pos.z - this->cameraPos.z;
                    if (quadData->width != 0.0f)
                    {
                        spr->scaleX = quadData->width / spr->spriteData->width;
                    }
                    if (quadData->height != 0.0f)
                    {
                        spr->scaleY = quadData->height / spr->spriteData->height;
                    }
                    if (spr->autoRotate == 2)
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
                        D3DXVec3Project(&scrPos, &objOrigin, &g_App.viewport,
                                        &g_App.projectionMatrix, &g_App.viewMatrix, &mat);
                        mat._41 += width * spr->scaleX;
                        D3DXVec3Project(&screenRight, &objOrigin, &g_App.viewport,
                                        &g_App.projectionMatrix, &g_App.viewMatrix, &mat);
                        spr->scaleX = (screenRight.x - scrPos.x) / width;
                        spr->scaleY = spr->scaleX;
                        spr->pos = scrPos;
                        g_SpriteEngine->DrawNoRound(spr);
                    }
                    else
                    {
                        g_SpriteEngine->Draw3D(spr);
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
