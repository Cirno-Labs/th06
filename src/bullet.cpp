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
        g_Unknown6d3f50.Func4311e0(shooter->sound, 0);
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
        g_Gui.Func41735a(totalBonus);
    }
    return totalBonus;
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
    if (sprite->unk_088 != 0)
    {
        sprite->rotation = 1.5707964f - bullet->angle;
    }
    g_SpriteEngine->Func433590(sprite);
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
    if (sprite->unk_088 != 0)
    {
        sprite->rotation = 1.5707964f - bullet->angle;
    }
    g_SpriteEngine->Func432cc0(sprite);
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
