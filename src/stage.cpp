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
