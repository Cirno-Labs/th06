// The file with EffectManager, up to 0x40f7c0 at least.

#include "effect.h"
#include "unsorted.h"

EffectManager::EffectManager()
{
    this->Reset();
}

void EffectManager::Reset()
{
    memset(this, 0, sizeof(EffectManager));
}

// An effect update: on its first frame, picks a random drift that slows
// to a stop over 19 frames; moves by it.
int EffectUpdateDrift(Effect *effect)
{
    if (effect->timer == 0 && effect->timer.HasTicked())
    {
        effect->velocity.x = (g_Rng.GetRandomF32ZeroToOne() * 256.0f - 128.0f) / 12.0f;
        effect->velocity.y = (g_Rng.GetRandomF32ZeroToOne() * 256.0f - 128.0f) / 12.0f;
        effect->velocity.z = 0.0f;
        effect->acceleration = -effect->velocity / 19.0f;
    }
    effect->pos += effect->velocity * g_App.frameAdvance;
    effect->velocity += effect->acceleration * g_App.frameAdvance;
    return 1;
}

// Like EffectUpdateDrift, faster, slowing to a stop over 20 frames.
int EffectUpdateDriftFast(Effect *effect)
{
    if (effect->timer == 0 && effect->timer.HasTicked())
    {
        effect->velocity.x = (g_Rng.GetRandomF32ZeroToOne() * 256.0f - 128.0f) * 4.0f / 33.0f;
        effect->velocity.y = (g_Rng.GetRandomF32ZeroToOne() * 256.0f - 128.0f) * 4.0f / 33.0f;
        effect->velocity.z = 0.0f;
        effect->acceleration = -effect->velocity / 20.0f;
    }
    effect->pos += effect->velocity * g_App.frameAdvance;
    effect->velocity += effect->acceleration * g_App.frameAdvance;
    return 1;
}

// An effect update: moves by velocity, which changes by acceleration.
int EffectUpdateMove(Effect *effect)
{
    effect->pos += effect->velocity * g_App.frameAdvance;
    effect->velocity += effect->acceleration * g_App.frameAdvance;
    return 1;
}

// An effect update: circles origin at `radius` about `axis`, at `angle`
// (flattened); once fadeOut is set, fades out and grows over 16 frames,
// then ends.
int EffectUpdateOrbit(Effect *effect)
{
    float a2;
    D3DXVECTOR3 n2;
    float c;
    D3DXMATRIX m2;
    float s;
    D3DXVECTOR3 v;

    D3DXVec3Normalize(&n2, &effect->axis);
    s = sinf(effect->angle);
    c = cosf(effect->angle);
    effect->rotation.x = n2.x * s;
    effect->rotation.y = n2.y * s;
    effect->rotation.z = n2.z * s;
    effect->rotation.w = c;
    D3DXMatrixRotationQuaternion(&m2, &effect->rotation);
    v.x = n2.y * 1.0f - n2.z * 0.0f;
    v.y = n2.z * 0.0f - n2.x * 1.0f;
    v.z = n2.x * 0.0f - n2.y * 0.0f;
    if (D3DXVec3LengthSq(&v) < 0.00001f)
    {
        // Sets the axis, not v: v stays (0, 0, 0).
        n2 = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    }
    else
    {
        D3DXVec3Normalize(&v, &v);
    }
    v *= effect->radius;
    D3DXVec3TransformCoord(&v, &v, &m2);
    v.z *= 6.0f;
    effect->pos = effect->origin + v;
    if (effect->fadeOut != 0)
    {
        effect->fadeFrames++;
        if (effect->fadeFrames >= 16)
        {
            return 0;
        }
        a2 = 1.0f - effect->fadeFrames / 16.0f;
        effect->sprite.color = (effect->sprite.color & 0xffffff) | ((int)(a2 * 255.0f) << 24);
        effect->sprite.scaleY = 2.0f - a2;
        effect->sprite.scaleX = effect->sprite.scaleY;
    }
    return 1;
}

// An effect update: on its first frame, picks a random direction; comes
// in from 256 along it to where it started over 60 frames.
int EffectUpdateConverge(Effect *effect)
{
    float f;

    if (effect->timer == 0 && effect->timer.HasTicked())
    {
        effect->origin = effect->pos;
        f = g_Rng.GetRandomF32InRange(6.2831855f) - 3.1415927f;
        effect->axis.x = cosf(f);
        effect->axis.y = sinf(f);
        effect->axis.z = 0.0f;
    }
    f = 256.0f - effect->timer.AsFloat() * 256.0f / 60.0f;
    effect->pos = f * effect->axis + effect->origin;
    return 1;
}

// Like EffectUpdateConverge, over 240 frames.
int EffectUpdateConvergeSlow(Effect *effect)
{
    float f;

    if (effect->timer == 0 && effect->timer.HasTicked())
    {
        effect->origin = effect->pos;
        f = g_Rng.GetRandomF32InRange(6.2831855f) - 3.1415927f;
        effect->axis.x = cosf(f);
        effect->axis.y = sinf(f);
        effect->axis.z = 0.0f;
    }
    f = 256.0f - effect->timer.AsFloat() * 256.0f / 240.0f;
    effect->pos = f * effect->axis + effect->origin;
    return 1;
}

Effect *EffectManager::SpawnEffect(int type, D3DXVECTOR3 *pos, int count, D3DCOLOR color)
{
    Effect *effect;
    int tries;

    effect = &this->effects[this->next];
    for (tries = 0; tries < 0x200; tries++)
    {
        this->next++;
        if (this->next >= 0x200)
        {
            this->next = 0;
        }
        if (effect->active)
        {
            if (this->next == 0)
            {
                effect = this->effects;
            }
            else
            {
                effect++;
            }
            continue;
        }
        effect->active = 1;
        effect->type = type;
        effect->pos = *pos;
        g_SpriteEngine->StartScript(&effect->sprite, g_EffectInfos[type].script);
        effect->sprite.color = color;
        effect->update = g_EffectInfos[type].update;
        effect->timer.Set(0);
        effect->fadeOut = 0;
        effect->fadeFrames = 0;
        count--;
        if (count == 0)
        {
            break;
        }
        if (this->next == 0)
        {
            effect = this->effects;
        }
        else
        {
            effect++;
        }
    }
    return tries >= 0x200 ? &this->effects[0x200] : effect;
}

int EffectManager::OnUpdate(EffectManager *mgr)
{
    Effect *eff = mgr->effects;
    int i;

    mgr->activeCount = 0;
    for (i = 0; i < 0x200; i++, eff++)
    {
        if (!eff->active)
        {
            continue;
        }
        mgr->activeCount++;
        if (eff->update != NULL && eff->update(eff) != 1)
        {
            eff->active = 0;
        }
        if (g_SpriteEngine->Step(&eff->sprite) != 0)
        {
            eff->active = 0;
        }
        eff->timer.Tick();
    }
    return 1;
}

int EffectManager::OnDraw(EffectManager *mgr)
{
    Effect *eff = mgr->effects;
    int i;

    for (i = 0; i < 0x200; i++, eff++)
    {
        if (!eff->active)
        {
            continue;
        }
        eff->sprite.pos = eff->pos;
        g_SpriteEngine->Draw3D(&eff->sprite);
    }
    return 1;
}

int EffectManager::AddedCallback(EffectManager *mgr)
{
    mgr->Reset();
    switch (g_GameManager.stage)
    {
    case 0:
    case 1:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff01.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 2:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff02.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 3:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff03.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 4:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff04.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 5:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff05.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 6:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff05.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    case 7:
        if (g_SpriteEngine->LoadAnm(0xb, "data/eff04.anm", 0x2b3) != 0)
        {
            return -1;
        }
        break;
    }
    return 0;
}

int EffectManager::DeletedCallback(EffectManager *mgr)
{
    g_SpriteEngine->ReleaseAnm(0xb);
    return 0;
}

int EffectManager::RegisterChain()
{
    EffectManager *mgr = &g_EffectManager;

    mgr->Reset();
    g_EffectCalcChain.SetCallback((ChainCallback)EffectManager::OnUpdate);
    g_EffectCalcChain.addedCallback = (ChainCallback)EffectManager::AddedCallback;
    g_EffectCalcChain.deletedCallback = (ChainCallback)EffectManager::DeletedCallback;
    g_EffectCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_EffectCalcChain, 10) != 0)
    {
        return -1;
    }
    g_EffectDrawChain.SetCallback((ChainCallback)EffectManager::OnDraw);
    g_EffectDrawChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_EffectDrawChain, 8);
    return 0;
}

void EffectManager::RemoveChain()
{
    g_Chain.Remove(&g_EffectCalcChain);
    g_Chain.Remove(&g_EffectDrawChain);
}
