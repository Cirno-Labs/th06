// The file with BulletManager.

#include "ascii.h"
#include "bullet.h"
#include "gamemanager.h"
#include "item.h"
#include "player.h"
#include "unsorted.h"

BulletManager::BulletManager()
{
    this->Reset();
}

void BulletManager::Reset()
{
    memset(this, 0, sizeof(BulletManager));
}

int BulletManager::DeletedCallback(BulletManager *mgr)
{
    if ((g_Unknown6c6d18.scene != 3) != 0)
    {
        g_SpriteEngine->ReleaseAnm(6);
        g_SpriteEngine->ReleaseAnm(7);
    }
    return 0;
}

void BulletManager::RemoveChain()
{
    g_Chain.Remove(&g_BulletCalcChain);
    g_Chain.Remove(&g_BulletDrawChain);
}

int BulletManager::FirePattern(BulletShooter *shooter)
{
    int col;
    float playerAngle;
    int row;

    playerAngle = g_Player.AngleFrom(&shooter->pos);
    for (row = 0; row < shooter->count2; row++)
    {
        for (col = 0; col < shooter->count1; col++)
        {
            if (this->SpawnBullet(shooter, col, row, playerAngle))
            {
                goto done;
            }
        }
    }
done:
    if (shooter->flags & 0x200)
    {
        g_Unknown6d3f50.QueueSound(shooter->sound, 0);
    }
    return 0;
}

Laser *BulletManager::SpawnLaser(BulletShooter *shooter)
{
    int i;
    Laser *laser;

    laser = this->lasers;
    for (i = 0; i < 0x40; i++, laser++)
    {
        if (laser->active != 0)
        {
            continue;
        }
        g_SpriteEngine->StartScript(&laser->sprite0, shooter->sprite + 0x209);
        g_SpriteEngine->SetSprite(&laser->sprite0, laser->sprite0.spriteIndex + shooter->color);
        g_SpriteEngine->InitSprite(&laser->sprite1, g_BulletColorOffsets16[shooter->color] + 0x28c);
        laser->sprite1.flags.blendMode = 1;
        laser->pos = shooter->pos;
        laser->color = shooter->color;
        laser->active = 1;
        laser->angle = shooter->angle1;
        if (shooter->aimMode == 0)
        {
            laser->angle += g_Player.AngleFrom(&shooter->pos);
        }
        laser->flags = shooter->flags;
        laser->timer.Set(0);
        laser->startOffset = shooter->exFloats[0];
        laser->endOffset = shooter->exFloats[1];
        laser->unk_238 = shooter->exFloats[2];
        laser->width = shooter->exFloats[3];
        laser->speed = shooter->speed1;
        laser->unk_244 = shooter->exInts[0];
        laser->unk_24c = shooter->exInts[1];
        laser->unk_250 = shooter->exInts[2];
        laser->unk_248 = shooter->exInts[3];
        laser->unk_254 = shooter->unk_40;
        if (laser->unk_244 == 0)
        {
            laser->state = 1;
        }
        else
        {
            laser->state = 0;
        }
        break;
    }
    return laser;
}

int BulletManager::RegisterChain(int unk)
{
    BulletManager *mgr = &g_BulletManager;

    if (!g_Unknown6c6d18.cfg.flags.unk0)
    {
        g_Unknown476438 = g_Unknown476358;
    }
    else
    {
        g_Unknown476438 = g_Unknown4763c8;
    }
    mgr->Reset();
    mgr->unk_f5c14 = unk;
    g_BulletCalcChain.SetCallback((ChainCallback)BulletManager::OnUpdate);
    g_BulletCalcChain.addedCallback = (ChainCallback)BulletManager::AddedCallback;
    g_BulletCalcChain.deletedCallback = (ChainCallback)BulletManager::DeletedCallback;
    g_BulletCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_BulletCalcChain, 0xb) != 0)
    {
        return -1;
    }
    g_BulletDrawChain.SetCallback((ChainCallback)BulletManager::OnDraw);
    g_BulletDrawChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_BulletDrawChain, 9);
    return 0;
}

// Picks `spr`'s sprite for `cur`'s color by the bullet's size.
#define SET_BULLET_SPRITE(spr)                                                                   \
    if (cur->sprites.sprite0.spriteData->height <= 16.0f)                                     \
    {                                                                                            \
        g_SpriteEngine->SetSprite(&(spr), (spr).spriteIndex + g_BulletColorOffsets16[shooter->color]); \
    }                                                                                            \
    else if (cur->sprites.sprite0.spriteData->height <= 32.0f)                                \
    {                                                                                            \
        if (cur->sprites.sprite0.scriptIndex != 0x207)                                        \
        {                                                                                        \
            g_SpriteEngine->SetSprite(&(spr), (spr).spriteIndex + g_BulletColorOffsets32[shooter->color]); \
        }                                                                                        \
        else                                                                                     \
        {                                                                                        \
            g_SpriteEngine->SetSprite(&(spr), (spr).spriteIndex + 1);                            \
        }                                                                                        \
    }                                                                                            \
    else                                                                                         \
    {                                                                                            \
        g_SpriteEngine->SetSprite(&(spr), (spr).spriteIndex + shooter->color);                   \
    }

int BulletManager::SpawnBullet(BulletShooter *shooter, int col, int row, float aimAngle)
{
    float v;
    int i = 0;
    Bullet *cur;
    float angle;

    cur = &this->bullets[this->nextBullet];
    for (i = 0; i < 0x280; i++)
    {
        this->nextBullet++;
        if (this->nextBullet >= 0x280)
        {
            this->nextBullet = 0;
        }
        if (cur->state != 0)
        {
            cur++;
            if (this->nextBullet == 0)
            {
                cur = this->bullets;
            }
            continue;
        }
        break;
    }
    if (i >= 0x280)
    {
        return 1;
    }
    angle = 0.0f;
    v = shooter->speed1 - (shooter->speed1 - shooter->speed2) * row / shooter->count2;
    switch (shooter->aimMode)
    {
    case 0:
    case 1:
        if (shooter->count1 & 1)
        {
            angle += (col + 1) / 2 * shooter->angle2;
        }
        else
        {
            angle += col / 2 * shooter->angle2 + shooter->angle2 * 0.5f;
        }
        if (col & 1)
        {
            angle *= -1.0f;
        }
        if (shooter->aimMode == 0)
        {
            angle += aimAngle;
        }
        angle += shooter->angle1;
        break;
    case 2:
        angle += aimAngle;
    case 3:
        angle += col * 6.2831855f / shooter->count1;
        angle += row * shooter->angle2 + shooter->angle1;
        break;
    case 4:
        angle += aimAngle;
    case 5:
        angle += 3.1415927f / shooter->count1;
        angle += col * 6.2831855f / shooter->count1;
        angle += shooter->angle1;
        break;
    case 6:
    {
        float angleDiff = shooter->angle1 - shooter->angle2;
        angle = g_Rng.GetRandomF32ZeroToOne() * angleDiff + shooter->angle2;
        break;
    }
    case 7:
    {
        float speedDiff = shooter->speed1 - shooter->speed2;
        v = g_Rng.GetRandomF32ZeroToOne() * speedDiff + shooter->speed2;
        angle += col * 6.2831855f / shooter->count1;
        angle += row * shooter->angle2 + shooter->angle1;
        break;
    }
    case 8:
    {
        float angleDiff = shooter->angle1 - shooter->angle2;
        angle = g_Rng.GetRandomF32ZeroToOne() * angleDiff + shooter->angle2;
        float speedDiff = shooter->speed1 - shooter->speed2;
        v = g_Rng.GetRandomF32ZeroToOne() * speedDiff + shooter->speed2;
        break;
    }
    }
    cur->state = 1;
    cur->unk_5c2 = 1;
    cur->speed = v;
    cur->angle = AddNormalizeAngle(angle, 0.0f);
    cur->pos = shooter->pos;
    cur->pos.z = 0.1f;
    SinCosScale(&cur->velocity, cur->angle, v);
    cur->flags = shooter->flags;
    cur->color = shooter->color;
    cur->sprites.sprite0 = this->typeSprites[shooter->sprite].sprite0;
    cur->sprites.sprite4 = this->typeSprites[shooter->sprite].sprite4;
    cur->sprites.hitboxSize = this->typeSprites[shooter->sprite].hitboxSize;
    cur->sprites.unk_55c = this->typeSprites[shooter->sprite].unk_55c;
    cur->sprites.height = this->typeSprites[shooter->sprite].height;
    if (cur->flags & 2)
    {
        cur->sprites.sprite1 = this->typeSprites[shooter->sprite].sprite1;
        SET_BULLET_SPRITE(cur->sprites.sprite1);
        cur->state = 2;
    }
    else if (cur->flags & 4)
    {
        cur->sprites.sprite2 = this->typeSprites[shooter->sprite].sprite2;
        SET_BULLET_SPRITE(cur->sprites.sprite2);
        cur->state = 3;
    }
    else if (cur->flags & 8)
    {
        cur->sprites.sprite3 = this->typeSprites[shooter->sprite].sprite3;
        SET_BULLET_SPRITE(cur->sprites.sprite3);
        cur->state = 4;
    }
    g_SpriteEngine->SetSprite(&cur->sprites.sprite0, cur->sprites.sprite0.spriteIndex + shooter->color);
    SET_BULLET_SPRITE(cur->sprites.sprite4);
    if (cur->flags & 0x10)
    {
        if (shooter->exFloats[1] <= -999.0f)
        {
            SinCosScale(&cur->accel, angle, shooter->exFloats[0]);
        }
        else
        {
            SinCosScale(&cur->accel, shooter->exFloats[1], shooter->exFloats[0]);
        }
        if (shooter->exInts[0] > 0)
        {
            cur->unk_5a8 = shooter->exInts[0];
        }
        else
        {
            cur->unk_5a8 = 99999;
        }
        cur->accel.z = 0.0f;
    }
    else if (cur->flags & 0x20)
    {
        cur->unk_588 = shooter->exFloats[0];
        cur->unk_594 = shooter->exFloats[1];
        cur->unk_5a8 = shooter->exInts[0];
    }
    if (cur->flags & 0x1c0)
    {
        cur->unk_598 = shooter->exFloats[0];
        if (shooter->exFloats[1] >= 0.0f)
        {
            cur->unk_58c = shooter->exFloats[1];
        }
        else
        {
            cur->unk_58c = v;
        }
        cur->unk_5ac = shooter->exInts[0];
        cur->unk_5b4 = shooter->exInts[1];
        cur->unk_5b0 = 0;
    }
    if (cur->flags & 0xc00)
    {
        if (shooter->exFloats[0] >= 0.0f)
        {
            cur->unk_58c = shooter->exFloats[0];
        }
        else
        {
            cur->unk_58c = v;
        }
        cur->unk_5b4 = shooter->exInts[0];
        cur->unk_5b0 = 0;
    }
    return 0;
}

void BulletManager::ClearAll(int toItems)
{
    D3DXVECTOR3 p;
    int i;
    float s;
    Bullet *curBullet;
    Laser *beam;
    float c;
    float t;

    curBullet = g_BulletManager.bullets;
    for (i = 0; i < 0x280; i++, curBullet++)
    {
        if (curBullet->state == 0 || curBullet->state == 5)
        {
            continue;
        }
        if (toItems)
        {
            g_ItemManager.SpawnItem(&curBullet->pos, 6, 1);
            memset(curBullet, 0, sizeof(Bullet));
        }
        else
        {
            curBullet->state = 5;
        }
    }
    beam = this->lasers;
    for (i = 0; i < 0x40; i++, beam++)
    {
        if (!beam->active)
        {
            continue;
        }
        if (beam->state < 2)
        {
            beam->state = 2;
            beam->timer.Set(0);
            if (toItems)
            {
                t = beam->startOffset;
                SinCos(&s, &c, beam->angle);
                while (beam->endOffset > t)
                {
                    p.x = c * t + beam->pos.x;
                    p.y = s * t + beam->pos.y;
                    p.z = 0.0f;
                    g_ItemManager.SpawnItem(&p, 6, 1);
                    t += 32.0f;
                }
            }
        }
        beam->unk_254 = 0;
    }
}

void BulletManager::ClearAllToItems()
{
    this->ClearAll(1);
}

int BulletManager::ClearAllWithBonus(int maxBonus, int toItems)
{
    int score;
    int totalBonus;
    int numCleared;
    int i;
    float s;
    Bullet *curBullet;
    D3DXVECTOR3 spot;
    Laser *beam;
    float c;
    float t;

    totalBonus = 0;
    score = 2000;
    numCleared = 0;
    curBullet = g_BulletManager.bullets;
    for (i = 0; i < 0x280; i++, curBullet++)
    {
        if (curBullet->state == 0)
        {
            continue;
        }
        if (toItems)
        {
            g_ItemManager.SpawnItem(&curBullet->pos, 6, 1);
        }
        g_AsciiText.AddPopup(&curBullet->pos, score, score >= maxBonus ? 0xffffff00 : 0xffffffff);
        totalBonus += score;
        numCleared++;
        score += 10;
        if (score > maxBonus)
        {
            score = maxBonus;
        }
        curBullet->state = 5;
    }
    beam = this->lasers;
    for (i = 0; i < 0x40; i++, beam++)
    {
        if (!beam->active)
        {
            continue;
        }
        if (beam->state < 2)
        {
            beam->state = 2;
            beam->timer.Set(0);
            if (toItems)
            {
                g_ItemManager.SpawnItem(&beam->pos, 6, 1);
                t = beam->startOffset;
                SinCos(&s, &c, beam->angle);
                while (beam->endOffset > t)
                {
                    spot.x = c * t + beam->pos.x;
                    spot.y = s * t + beam->pos.y;
                    spot.z = 0.0f;
                    g_ItemManager.SpawnItem(&spot, 6, 1);
                    t += 32.0f;
                }
            }
        }
        beam->unk_254 = 0;
    }
    g_GameManager.score += totalBonus;
    if (totalBonus != 0)
    {
        g_Gui.ShowBonusPopup(totalBonus);
    }
    return totalBonus;
}

int BulletManager::OnUpdate(BulletManager *mgr)
{
    int fadeTime6;
    D3DXVECTOR3 center2;
    Laser *las;
    int alpha4;
    Bullet *bullet = mgr->bullets;
    D3DXVECTOR3 extent5;
    float scale3;
    float speed7;
    int k7;
    int result;

    if (g_GameManager.unk_002c != 0)
    {
        return 1;
    }
    g_ItemManager.UpdateItems();
    mgr->bulletCount = 0;
    for (k7 = 0; k7 < 0x280; k7++, bullet++)
    {
        if (bullet->state == 0)
        {
            continue;
        }
        mgr->bulletCount++;
        switch (bullet->state)
        {
        case 2:
            bullet->pos += bullet->velocity / 2.0f * g_Unknown6c6d18.frameAdvance;
            if (g_SpriteEngine->Step(&bullet->sprites.sprite1) == 0)
            {
                break;
            }
            goto spawned;
        case 3:
            bullet->pos += bullet->velocity / 2.5f * g_Unknown6c6d18.frameAdvance;
            if (g_SpriteEngine->Step(&bullet->sprites.sprite2) == 0)
            {
                break;
            }
            goto spawned;
        case 4:
            bullet->pos += bullet->velocity / 3.0f * g_Unknown6c6d18.frameAdvance;
            if (g_SpriteEngine->Step(&bullet->sprites.sprite3) == 0)
            {
                break;
            }
        spawned:
            bullet->state = 1;
            bullet->timer.Set(0);
        case 1:
            if (bullet->flags != 0)
            {
                if (bullet->flags & 1)
                {
                    if (bullet->timer <= 16)
                    {
                        speed7 = 5.0f - bullet->timer.AsFloat() * 5.0f / 16.0f;
                        SinCosScale(&bullet->velocity, bullet->angle, speed7 + bullet->speed);
                    }
                    else
                    {
                        bullet->flags ^= 1;
                    }
                }
                else if (bullet->flags & 0x10)
                {
                    if (bullet->timer >= bullet->unk_5a8)
                    {
                        bullet->flags &= ~0x10;
                    }
                    else
                    {
                        bullet->velocity += bullet->accel * g_Unknown6c6d18.frameAdvance;
                        bullet->angle = atan2f(bullet->velocity.y, bullet->velocity.x);
                    }
                }
                else if (bullet->flags & 0x20)
                {
                    if (bullet->timer >= bullet->unk_5a8)
                    {
                        bullet->flags &= ~0x20;
                    }
                    else
                    {
                        bullet->angle = AddNormalizeAngle(bullet->angle, g_Unknown6c6d18.frameAdvance * bullet->unk_594);
                        bullet->speed = g_Unknown6c6d18.frameAdvance * bullet->unk_588 + bullet->speed;
                        SinCosScale(&bullet->velocity, bullet->angle, bullet->speed);
                    }
                }
                if (bullet->flags & 0x40)
                {
                    if (bullet->timer >= bullet->unk_5ac * (bullet->unk_5b0 + 1))
                    {
                        bullet->unk_5b0++;
                        if (bullet->unk_5b0 >= bullet->unk_5b4)
                        {
                            bullet->flags &= ~0x40;
                        }
                        bullet->angle += bullet->unk_598;
                        bullet->speed = bullet->unk_58c;
                        speed7 = bullet->speed;
                    }
                    else
                    {
                        speed7 = bullet->speed - (bullet->timer.AsFloat() - bullet->unk_5ac * bullet->unk_5b0) * bullet->speed / bullet->unk_5ac;
                    }
                    SinCosScale(&bullet->velocity, bullet->angle, speed7);
                }
                else if (bullet->flags & 0x100)
                {
                    if (bullet->timer >= bullet->unk_5ac * (bullet->unk_5b0 + 1))
                    {
                        bullet->unk_5b0++;
                        if (bullet->unk_5b0 >= bullet->unk_5b4)
                        {
                            bullet->flags &= ~0x100;
                        }
                        bullet->angle = bullet->unk_598;
                        bullet->speed = bullet->unk_58c;
                        speed7 = bullet->speed;
                    }
                    else
                    {
                        speed7 = bullet->speed - (bullet->timer.AsFloat() - bullet->unk_5ac * bullet->unk_5b0) * bullet->speed / bullet->unk_5ac;
                    }
                    SinCosScale(&bullet->velocity, bullet->angle, speed7);
                }
                else if (bullet->flags & 0x80)
                {
                    if (bullet->timer >= bullet->unk_5ac * (bullet->unk_5b0 + 1))
                    {
                        bullet->unk_5b0++;
                        if (bullet->unk_5b0 >= bullet->unk_5b4)
                        {
                            bullet->flags &= ~0x80;
                        }
                        bullet->angle = g_Player.AngleFrom(&bullet->pos) + bullet->unk_598;
                        bullet->speed = bullet->unk_58c;
                        speed7 = bullet->speed;
                    }
                    else
                    {
                        speed7 = bullet->speed - (bullet->timer.AsFloat() - bullet->unk_5ac * bullet->unk_5b0) * bullet->speed / bullet->unk_5ac;
                    }
                    SinCosScale(&bullet->velocity, bullet->angle, speed7);
                }
                else if (bullet->flags & 0x400)
                {
                    if (!g_GameManager.Func41b5e1(bullet->pos.x, bullet->pos.y, bullet->sprites.sprite0.spriteData->width, bullet->sprites.sprite0.spriteData->height))
                    {
                        if (bullet->pos.x < 0.0f || bullet->pos.x >= 384.0f)
                        {
                            bullet->angle = -bullet->angle - 3.1415927f;
                            bullet->angle = AddNormalizeAngle(bullet->angle, 0.0f);
                        }
                        if (bullet->pos.y < 0.0f || bullet->pos.y >= 448.0f)
                        {
                            bullet->angle = -bullet->angle;
                        }
                        bullet->speed = bullet->unk_58c;
                        speed7 = bullet->speed;
                        SinCosScale(&bullet->velocity, bullet->angle, speed7);
                        bullet->unk_5b0++;
                        if (bullet->unk_5b0 >= bullet->unk_5b4)
                        {
                            bullet->flags &= ~0x400;
                        }
                    }
                }
                else if (bullet->flags & 0x800)
                {
                    if (!g_GameManager.Func41b5e1(bullet->pos.x, bullet->pos.y, bullet->sprites.sprite0.spriteData->width, bullet->sprites.sprite0.spriteData->height))
                    {
                        if (bullet->pos.x < 0.0f || bullet->pos.x >= 384.0f)
                        {
                            bullet->angle = -bullet->angle - 3.1415927f;
                            bullet->angle = AddNormalizeAngle(bullet->angle, 0.0f);
                        }
                        if (bullet->pos.y < 0.0f)
                        {
                            bullet->angle = -bullet->angle;
                        }
                        bullet->speed = bullet->unk_58c;
                        speed7 = bullet->speed;
                        SinCosScale(&bullet->velocity, bullet->angle, speed7);
                        bullet->unk_5b0++;
                        if (bullet->unk_5b0 >= bullet->unk_5b4)
                        {
                            bullet->flags &= ~0x800;
                        }
                    }
                }
            }
            bullet->pos += bullet->velocity * g_Unknown6c6d18.frameAdvance;
            if (!g_GameManager.Func41b5e1(bullet->pos.x, bullet->pos.y, bullet->sprites.sprite0.spriteData->width, bullet->sprites.sprite0.spriteData->height))
            {
                if (!(bullet->flags & 0x40) && !(bullet->flags & 0x100) && !(bullet->flags & 0x80) && !(bullet->flags & 0x400) && !(bullet->flags & 0x800) && bullet->outsideFrames == 0)
                {
                    memset(bullet, 0, sizeof(Bullet));
                    continue;
                }
                bullet->outsideFrames++;
                if (bullet->outsideFrames >= 0x100)
                {
                    memset(bullet, 0, sizeof(Bullet));
                    continue;
                }
            }
            else
            {
                bullet->outsideFrames = 0;
            }
            if (bullet->grazed == 0)
            {
                result = g_Player.CheckGraze(&bullet->pos, &bullet->sprites.hitboxSize);
                if (result == 1)
                {
                    bullet->grazed = 1;
                    goto checkHit;
                }
                else if (result == 2)
                {
                    bullet->state = 5;
                    g_ItemManager.SpawnItem(&bullet->pos, 6, 1);
                }
            }
            else if (bullet->grazed == 1)
            {
            checkHit:
                result = g_Player.CheckHit(&bullet->pos, &bullet->sprites.hitboxSize);
                if (result != 0)
                {
                    bullet->state = 5;
                    if (result == 2)
                    {
                        g_ItemManager.SpawnItem(&bullet->pos, 6, 1);
                    }
                }
            }
            g_SpriteEngine->Step(&bullet->sprites.sprite0);
            break;
        case 5:
            bullet->pos += bullet->velocity / 2.0f * g_Unknown6c6d18.frameAdvance;
            if (g_SpriteEngine->Step(&bullet->sprites.sprite4) != 0)
            {
                memset(bullet, 0, sizeof(Bullet));
                continue;
            }
            break;
        }
        bullet->timer.Tick();
    }
    las = mgr->lasers;
    for (k7 = 0; k7 < 0x40; k7++, las++)
    {
        if (!las->active)
        {
            continue;
        }
        las->endOffset = g_Unknown6c6d18.frameAdvance * las->speed + las->endOffset;
        if (las->endOffset - las->startOffset > las->unk_238)
        {
            las->startOffset = las->endOffset - las->unk_238;
        }
        if (las->startOffset < 0.0f)
        {
            las->startOffset = 0.0f;
        }
        extent5.y = las->width / 2.0f;
        extent5.x = las->endOffset - las->startOffset;
        center2.x = (las->endOffset - las->startOffset) / 2.0f + las->startOffset + las->pos.x;
        center2.y = las->pos.y;
        las->sprite0.scaleX = las->width / las->sprite0.spriteData->width;
        scale3 = las->endOffset - las->startOffset;
        las->sprite0.scaleY = scale3 / las->sprite0.spriteData->height;
        las->sprite0.rotationZ = 1.5707964f - las->angle;
        switch (las->state)
        {
        case 0:
            if (las->flags & 1)
            {
                alpha4 = las->timer.AsFloat() * 255.0f / las->unk_244;
                if (alpha4 > 0xff)
                {
                    alpha4 = 0xff;
                }
                las->sprite0.color = alpha4 << 24;
            }
            else
            {
                fadeTime6 = las->unk_244 > 30 ? 30 : las->unk_244;
                if (las->unk_244 - fadeTime6 < las->timer.AsFrames())
                {
                    scale3 = las->timer.AsFloat() * las->width / las->unk_244;
                }
                else
                {
                    scale3 = 1.2f;
                }
                las->sprite0.scaleX = scale3 / 16.0f;
                extent5.x = scale3 / 2.0f;
            }
            if (las->timer >= las->unk_248)
            {
                g_Player.CheckLaserHit(&center2, &extent5, &las->pos, las->angle, las->timer.AsFrames() % 12 == 0);
            }
            if (las->timer < las->unk_244)
            {
                break;
            }
            las->timer.Set(0);
            las->state++;
        case 1:
            g_Player.CheckLaserHit(&center2, &extent5, &las->pos, las->angle, las->timer.AsFrames() % 12 == 0);
            if (las->timer < las->unk_24c)
            {
                break;
            }
            las->timer.Set(0);
            las->state++;
            if (las->unk_250 == 0)
            {
                las->active = 0;
                continue;
            }
        case 2:
            if (las->flags & 1)
            {
                alpha4 = las->timer.AsFloat() * 255.0f / las->unk_244;
                if (alpha4 > 0xff)
                {
                    alpha4 = 0xff;
                }
                las->sprite0.color = alpha4 << 24;
            }
            else if (las->unk_250 > 0)
            {
                scale3 = las->width - las->timer.AsFloat() * las->width / las->unk_250;
                las->sprite0.scaleX = scale3 / 16.0f;
                extent5.x = scale3 / 2.0f;
            }
            if (las->timer < las->unk_254)
            {
                g_Player.CheckLaserHit(&center2, &extent5, &las->pos, las->angle, las->timer.AsFrames() % 12 == 0);
            }
            if (las->timer < las->unk_250)
            {
                break;
            }
            las->active = 0;
            continue;
        }
        if (las->startOffset >= 640.0f)
        {
            las->active = 0;
        }
        las->timer.Tick();
        g_SpriteEngine->Step(&las->sprite0);
    }
    mgr->unk_f5c08.Tick();
    return 1;
}

int BulletManager::OnDraw(BulletManager *mgr)
{
    int i;
    float s;
    Laser *laser;
    float len;
    float ca;
    Bullet *blt;
    Bullet *bullet2;

    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    laser = mgr->lasers;
    for (i = 0; i < 0x40; i++, laser++)
    {
        if (!laser->active)
        {
            continue;
        }
        SinCos(&s, &ca, laser->angle);
        len = (laser->endOffset - laser->startOffset) / 2.0f + laser->startOffset;
        laser->sprite0.pos.x = ca * len + laser->pos.x;
        laser->sprite0.pos.y = s * len + laser->pos.y;
        laser->sprite0.pos.z = 0.0f;
        laser->color = laser->color & 0xff000000 | 0xffffff;
        g_SpriteEngine->Draw3D(&laser->sprite0);
        if (laser->startOffset < 16.0f || laser->speed == 0.0f)
        {
            laser->sprite1.pos.x = ca * laser->startOffset + laser->pos.x;
            laser->sprite1.pos.y = s * laser->startOffset + laser->pos.y;
            laser->sprite1.pos.z = 0.0f;
            laser->sprite1.color = laser->sprite0.color;
            laser->sprite1.flags.colorOp = 1;
            laser->sprite1.color = laser->sprite1.color & 0xffffff | 0xff000000;
            laser->sprite1.scaleX = laser->width / 10.0f * ((16.0f - laser->startOffset) / 16.0f);
            laser->sprite1.scaleY = laser->sprite1.scaleX;
            if (laser->sprite1.scaleY < 0.0f)
            {
                laser->sprite1.scaleX = laser->width / 10.0f;
                laser->sprite1.scaleY = laser->sprite1.scaleX;
            }
            g_SpriteEngine->Draw3D(&laser->sprite1);
        }
    }
    g_ItemManager.DrawItems();
    if (g_Unknown6c6d18.hardwareTnL)
    {
        blt = mgr->bullets;
        for (i = 0; i < 0x280; i++, blt++)
        {
            if (blt->state == 0)
            {
                continue;
            }
            if (blt->sprites.height > 16)
            {
                DrawBullet(blt);
            }
        }
        blt = mgr->bullets;
        for (i = 0; i < 0x280; i++, blt++)
        {
            if (blt->state == 0)
            {
                continue;
            }
            if (blt->sprites.height == 16 && (blt->sprites.sprite0.scriptIndex == 0x201 || blt->sprites.sprite0.scriptIndex == 0x203))
            {
                DrawBullet(blt);
            }
        }
        blt = mgr->bullets;
        for (i = 0; i < 0x280; i++, blt++)
        {
            if (blt->state == 0)
            {
                continue;
            }
            if (blt->sprites.height == 16 && blt->sprites.sprite0.scriptIndex != 0x201 && blt->sprites.sprite0.scriptIndex != 0x203)
            {
                DrawBullet(blt);
            }
        }
        blt = mgr->bullets;
        for (i = 0; i < 0x280; i++, blt++)
        {
            if (blt->state == 0)
            {
                continue;
            }
            if (blt->sprites.height == 8)
            {
                DrawBullet(blt);
            }
        }
    }
    else
    {
        bullet2 = mgr->bullets;
        for (i = 0; i < 0x280; i++, bullet2++)
        {
            if (bullet2->state == 0)
            {
                continue;
            }
            if (bullet2->sprites.height > 16)
            {
                Func416c50(bullet2);
            }
        }
        bullet2 = mgr->bullets;
        for (i = 0; i < 0x280; i++, bullet2++)
        {
            if (bullet2->state == 0)
            {
                continue;
            }
            if (bullet2->sprites.height == 16 && (bullet2->sprites.sprite0.scriptIndex == 0x201 || bullet2->sprites.sprite0.scriptIndex == 0x203))
            {
                Func416c50(bullet2);
            }
        }
        bullet2 = mgr->bullets;
        for (i = 0; i < 0x280; i++, bullet2++)
        {
            if (bullet2->state == 0)
            {
                continue;
            }
            if (bullet2->sprites.height == 16 && bullet2->sprites.sprite0.scriptIndex != 0x201 && bullet2->sprites.sprite0.scriptIndex != 0x203)
            {
                Func416c50(bullet2);
            }
        }
        bullet2 = mgr->bullets;
        for (i = 0; i < 0x280; i++, bullet2++)
        {
            if (bullet2->state == 0)
            {
                continue;
            }
            if (bullet2->sprites.height == 8)
            {
                Func416c50(bullet2);
            }
        }
    }
    g_Unknown6c6d18.d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    return 1;
}

void DrawBullet(Bullet *bullet)
{
    Sprite *sprite;

    switch (bullet->state)
    {
    case 2:
        sprite = &bullet->sprites.sprite1;
        break;
    case 3:
        sprite = &bullet->sprites.sprite2;
        break;
    case 4:
        sprite = &bullet->sprites.sprite3;
        break;
    case 5:
        sprite = &bullet->sprites.sprite4;
        break;
    default:
        sprite = &bullet->sprites.sprite0;
    }
    sprite->pos.x = bullet->pos.x;
    sprite->pos.y = bullet->pos.y;
    sprite->pos.z = 0.0f;
    sprite->color = sprite->color & 0xff000000 | 0xffffff;
    if (sprite->autoRotate != 0)
    {
        sprite->rotationZ = 1.5707964f - bullet->angle;
    }
    g_SpriteEngine->Draw3DRounded(sprite);
}

void Func416c50(Bullet *bullet)
{
    Sprite *sprite;

    switch (bullet->state)
    {
    case 2:
        sprite = &bullet->sprites.sprite1;
        break;
    case 3:
        sprite = &bullet->sprites.sprite2;
        break;
    case 4:
        sprite = &bullet->sprites.sprite3;
        break;
    case 5:
        sprite = &bullet->sprites.sprite4;
        break;
    default:
        sprite = &bullet->sprites.sprite0;
    }
    sprite->pos.x = g_GameManager.arena[0] + bullet->pos.x;
    sprite->pos.y = g_GameManager.arena[1] + bullet->pos.y;
    sprite->pos.z = 0.0f;
    sprite->color = sprite->color & 0xff000000 | 0xffffff;
    if (sprite->autoRotate != 0)
    {
        sprite->rotationZ = 1.5707964f - bullet->angle;
    }
    g_SpriteEngine->DrawRotated(sprite);
}

int BulletManager::AddedCallback(BulletManager *mgr)
{
    int i;

    if ((g_Unknown6c6d18.scene != 3) != 0)
    {
        if (g_SpriteEngine->LoadAnm(6, "data/etama3.anm", 0x200) != 0)
        {
            return -1;
        }
        if (g_SpriteEngine->LoadAnm(7, "data/etama4.anm", 0x29a) != 0)
        {
            return -1;
        }
    }
    for (i = 0; i < ARRAY_SIZE(g_BulletTypeScripts); i++)
    {
        g_SpriteEngine->StartScript(&mgr->typeSprites[i].sprite0, g_BulletTypeScripts[i][0]);
        g_SpriteEngine->StartScript(&mgr->typeSprites[i].sprite1, g_BulletTypeScripts[i][1]);
        g_SpriteEngine->StartScript(&mgr->typeSprites[i].sprite2, g_BulletTypeScripts[i][2]);
        g_SpriteEngine->StartScript(&mgr->typeSprites[i].sprite3, g_BulletTypeScripts[i][3]);
        g_SpriteEngine->StartScript(&mgr->typeSprites[i].sprite4, g_BulletTypeScripts[i][4]);
        mgr->typeSprites[i].sprite0.unk_0b2 = mgr->typeSprites[i].sprite0.spriteIndex;
        mgr->typeSprites[i].height = mgr->typeSprites[i].sprite0.spriteData->height;
        if (mgr->typeSprites[i].sprite0.spriteData->height <= 8.0f)
        {
            mgr->typeSprites[i].hitboxSize.x = 4.0f;
            mgr->typeSprites[i].hitboxSize.y = 4.0f;
        }
        else if (mgr->typeSprites[i].sprite0.spriteData->height <= 16.0f)
        {
            switch (g_BulletTypeScripts[i][0])
            {
            case 0x202:
                mgr->typeSprites[i].hitboxSize.x = 4.0f;
                mgr->typeSprites[i].hitboxSize.y = 4.0f;
                break;
            case 0x204:
                mgr->typeSprites[i].hitboxSize.x = 5.0f;
                mgr->typeSprites[i].hitboxSize.y = 5.0f;
                break;
            case 0x205:
                mgr->typeSprites[i].hitboxSize.x = 4.0f;
                mgr->typeSprites[i].hitboxSize.y = 4.0f;
                break;
            default:
                mgr->typeSprites[i].hitboxSize.x = 6.0f;
                mgr->typeSprites[i].hitboxSize.y = 6.0f;
            }
        }
        else if (mgr->typeSprites[i].sprite0.spriteData->height <= 32.0f)
        {
            switch (g_BulletTypeScripts[i][0])
            {
            case 0x207:
                mgr->typeSprites[i].hitboxSize.x = 11.0f;
                mgr->typeSprites[i].hitboxSize.y = 11.0f;
                break;
            case 0x208:
                mgr->typeSprites[i].hitboxSize.x = 9.0f;
                mgr->typeSprites[i].hitboxSize.y = 9.0f;
                break;
            default:
                mgr->typeSprites[i].hitboxSize.x = 16.0f;
                mgr->typeSprites[i].hitboxSize.y = 16.0f;
            }
        }
        else
        {
            mgr->typeSprites[i].hitboxSize.x = 32.0f;
            mgr->typeSprites[i].hitboxSize.y = 32.0f;
        }
    }
    memset(&g_ItemManager, 0, sizeof(ItemManager));
    return 0;
}
