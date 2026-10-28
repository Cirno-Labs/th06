// The file with EnemyManager.

#include "enemy.h"
#include "gamemanager.h"
#include "item.h"
#include "player.h"
#include "sprite.h"
#include "unsorted.h"

int EnemyManager::DeletedCallback(EnemyManager *mgr)
{
    g_SpriteEngine->ReleaseAnm(9);
    g_SpriteEngine->ReleaseAnm(8);
    return 0;
}

void EnemyManager::RemoveChain()
{
    g_Chain.Remove(&g_EnemyCalcChain);
    g_Chain.Remove(&g_EnemyDrawChain);
}

EnemyManager::EnemyManager()
{
    this->Initialize();
}

Enemy *EnemyManager::Spawn(short sub, D3DXVECTOR3 *pos, short life, char unk_e3f, int score)
{
    Enemy *enemy;
    int i;

    enemy = this->enemies;
    for (i = 0; i < 0x100; i++, enemy++)
    {
        if (enemy->flags.active)
        {
            continue;
        }
        *enemy = this->enemyTemplate;
        if (life >= 0)
        {
            enemy->life = life;
        }
        enemy->pos = *pos;
        g_EnemyScripts.StartSub(&enemy->ctx, sub);
        g_EnemyScripts.Func4074a0(enemy);
        enemy->color = enemy->sprite.color;
        enemy->unk_e3f = unk_e3f;
        if (life >= 0)
        {
            enemy->life = life;
        }
        if (score >= 0)
        {
            enemy->score = score;
        }
        enemy->maxLife = enemy->life;
        break;
    }
    return enemy;
}

void ReleaseEnemyEffects(Enemy *enemy)
{
    int i;

    for (i = 0; i < enemy->effectCount; i++)
    {
        if (enemy->effects[i] == NULL)
        {
            continue;
        }
        enemy->effects[i]->unk_17a = 1;
        enemy->effects[i] = NULL;
    }
    enemy->effectCount = 0;
}

void Enemy::Func4121b0()
{
    if (this->flags.unk13 == 0)
    {
        this->flags.active = 0;
    }
    else
    {
        this->flags.unk8 = 0;
    }
    if (this->flags.unk11)
    {
        g_Gui.unk_20 = 0;
    }
    if (this->effectCount != 0)
    {
        ReleaseEnemyEffects(this);
    }
}

void Enemy::ClampPos()
{
    if (this->flags.clampPos)
    {
        if (this->pos.x < this->lowerBound[0])
        {
            this->pos.x = this->lowerBound[0];
        }
        else if (this->pos.x > this->upperBound[0])
        {
            this->pos.x = this->upperBound[0];
        }
        if (this->pos.y < this->lowerBound[1])
        {
            this->pos.y = this->lowerBound[1];
        }
        else if (this->pos.y > this->upperBound[1])
        {
            this->pos.y = this->upperBound[1];
        }
    }
}

void Enemy::Move()
{
    if (!this->flags.mirrored)
    {
        this->pos.x += g_Unknown6c6d18.frameAdvance * this->velocity.x;
    }
    else
    {
        this->pos.x -= g_Unknown6c6d18.frameAdvance * this->velocity.x;
    }
    this->pos.y += g_Unknown6c6d18.frameAdvance * this->velocity.y;
    this->pos.z += g_Unknown6c6d18.frameAdvance * this->velocity.z;
}

int EnemyManager::OnUpdate(EnemyManager *mgr)
{
    int bomb = 0;
    int hurt;
    int i;
    D3DXVECTOR3 size;
    int k;
    int lifeBefore;
    Enemy *em;

    mgr->Func411530();
    em = mgr->enemies;
    mgr->enemyCount = 0;
    for (i = 0; i < 0x100; i++, em++)
    {
        if (!em->flags.active)
        {
            continue;
        }
        mgr->enemyCount++;
        em->Move();
        em->ClampPos();
        if (em->flags.onScreen == 0 &&
            g_GameManager.Func41b5e1(em->pos.x, em->pos.y, em->sprite.spriteData->width,
                                     em->sprite.spriteData->height) != 0)
        {
            em->flags.onScreen = 1;
        }
        if (em->flags.onScreen == 1 &&
            g_GameManager.Func41b5e1(em->pos.x, em->pos.y, em->sprite.spriteData->width,
                                     em->sprite.spriteData->height) == 0)
        {
            em->flags.active = 0;
            em->Func4121b0();
            continue;
        }
        if (em->unk_ea8 >= 0)
        {
            em->Func411da0();
        }
        if (em->unk_eb0 >= 0)
        {
            em->Func411f40();
        }
        if (g_EnemyScripts.Func4074a0(em) == -1)
        {
            em->flags.active = 0;
            em->Func4121b0();
            continue;
        }
        em->ClampPos();
        em->sprite.color = em->color;
        g_SpriteEngine->Step(&em->sprite);
        em->color = em->sprite.color;
        for (k = 0; k < 8; k++)
        {
            if (em->sprites[k].scriptIndex >= 0 && g_SpriteEngine->Step(&em->sprites[k]) != 0)
            {
                em->sprites[k].scriptIndex = -1;
            }
        }
        bomb = 0;
        if (em->flags.onScreen && !em->flags.hidden)
        {
            lifeBefore = em->life;
            if (em->flags.unk9 && em->flags.unk8)
            {
                size = em->hitboxSize / 1.5f;
                if (g_Player.CheckHit(&em->pos, &size) == 1 && em->flags.unk8 && !em->flags.unk11)
                {
                    em->life -= 10;
                }
            }
            if (em->flags.unk8)
            {
                hurt = g_Player.CalcDamage(&em->pos, &em->hitboxSize, &bomb);
                if (hurt >= 70)
                {
                    hurt = 70;
                }
                g_GameManager.score = hurt / 5 * 10 + g_GameManager.score;
                if (mgr->unk_ee5c8 != 0)
                {
                    if (bomb == 0)
                    {
                        if (hurt > 7)
                        {
                            hurt /= 7;
                        }
                        else if (hurt != 0)
                        {
                            hurt = 1;
                        }
                    }
                    else if (mgr->unk_ee5d4 != 0)
                    {
                        if (hurt > 3)
                        {
                            hurt /= 3;
                        }
                        else if (hurt != 0)
                        {
                            hurt = 1;
                        }
                    }
                    else
                    {
                        hurt = 0;
                    }
                }
                if (em->flags.unk12)
                {
                    em->life -= hurt;
                }
                if (g_Player.unk_0a1c.y < em->pos.y)
                {
                    g_Player.unk_0a1c = em->pos;
                }
            }
            if (em->life <= 0 && em->flags.unk8)
            {
                em->unk_ea8 = -1;
                em->unk_eb0 = -1;
                switch (em->flags.unk13)
                {
                case 3:
                    em->life = 1;
                    em->flags.unk12 = 0;
                    em->flags.unk13 = 0;
                    g_Gui.unk_20 = 0;
                    g_EffectManager.SpawnEffect(em->deathEffect, &em->pos, 1, 0xffffffff);
                    g_EffectManager.SpawnEffect(em->deathEffect, &em->pos, 1, 0xffffffff);
                    g_EffectManager.SpawnEffect(em->deathEffect, &em->pos, 1, 0xffffffff);
                    break;
                case 1:
                    g_GameManager.AddScore(em->score);
                    em->flags.unk8 = 0;
                    goto dead;
                case 0:
                    g_GameManager.AddScore(em->score);
                    em->flags.active = 0;
                dead:
                    if (em->flags.unk11)
                    {
                        g_Gui.unk_20 = 0;
                        ReleaseEnemyEffects(em);
                    }
                case 2:
                    if (em->unk_e3f >= 0)
                    {
                        g_EffectManager.SpawnEffect(em->unk_e3d + 4, &em->pos, 3, 0xffffffff);
                        g_ItemManager.SpawnItem(&em->pos, em->unk_e3f, bomb);
                    }
                    else if (em->unk_e3f == -1)
                    {
                        if (mgr->unk_ee5b8 % 3 == 0)
                        {
                            g_EffectManager.SpawnEffect(em->unk_e3d + 4, &em->pos, 6, 0xffffffff);
                            g_ItemManager.SpawnItem(&em->pos, g_EnemyItemDrops[mgr->unk_ee5ba], bomb);
                            mgr->unk_ee5ba++;
                            if (mgr->unk_ee5ba >= 0x20)
                            {
                                mgr->unk_ee5ba = 0;
                            }
                        }
                        mgr->unk_ee5b8++;
                    }
                    if (em->flags.unk11 && g_EnemyManager.unk_ee5c8 == 0)
                    {
                        g_BulletManager.ClearAllWithBonus(0x3200, 0);
                    }
                    em->life = 0;
                    break;
                }
                g_Unknown6d3f50.QueueSound(i % 2 + 2, 0);
                g_EffectManager.SpawnEffect(em->deathEffect, &em->pos, 1, 0xffffffff);
                g_EffectManager.SpawnEffect(em->unk_e3d + 4, &em->pos, 4, 0xffffffff);
                if (em->deathSub >= 0)
                {
                    em->unk_cd4 = -0.5f;
                    em->unk_cd8 = 0.5f;
                    em->unk_cdc = 0;
                    em->unk_cde = 0;
                    em->unk_ce0 = 0;
                    em->unk_ce2 = 0;
                    em->unk_c3c = 0;
                    g_EnemyScripts.StartSub(&em->ctx, em->deathSub);
                    em->deathSub = -1;
                }
            }
            if (em->flags.unk11 && !g_Gui.Func4195a2())
            {
                g_Gui.SetBossHealth((float)em->life / (float)em->maxLife);
            }
            if (em->hitFlashTimer != 0)
            {
                em->hitFlashTimer--;
                em->sprite.flags.colorOp = 0;
            }
            else if (lifeBefore > em->life)
            {
                g_Unknown6d3f50.QueueSound(0x14, 0);
                em->sprite.flags.colorOp = 1;
                em->hitFlashTimer = 4;
            }
            else
            {
                em->sprite.flags.colorOp = 0;
            }
        }
        MoveEnemyEffects(em);
        if (g_GameManager.unk_002c == 0)
        {
            em->unk_cf0.Tick();
        }
    }
    mgr->unk_ee5e0.Tick();
    return 1;
}

void MoveEnemyEffects(Enemy *enemy)
{
    Effect *eff;
    int i;

    for (i = 0; i < enemy->effectCount; i++)
    {
        eff = enemy->effects[i];
        if (eff == NULL)
        {
            continue;
        }
        eff->unk_134 = enemy->pos;
        if (eff->unk_15c < enemy->unk_ea4)
        {
            eff->unk_15c += 0.3f;
        }
        eff->unk_160 = AddNormalizeAngle(eff->unk_160, 0.031415928f);
    }
}

int EnemyManager::RegisterChain(char *anmPath, char *anmPath2)
{
    EnemyManager *mgr = &g_EnemyManager;

    mgr->Initialize();
    mgr->anmPath = anmPath;
    mgr->anmPath2 = anmPath2;
    g_EnemyCalcChain.SetCallback((ChainCallback)EnemyManager::OnUpdate);
    g_EnemyCalcChain.addedCallback = (ChainCallback)EnemyManager::AddedCallback;
    g_EnemyCalcChain.deletedCallback = (ChainCallback)EnemyManager::DeletedCallback;
    g_EnemyCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_EnemyCalcChain, 9) != 0)
    {
        return -1;
    }
    g_EnemyDrawChain.SetCallback((ChainCallback)EnemyManager::OnDraw);
    g_EnemyDrawChain.arg = mgr;
    if (g_Chain.AddToDrawChain(&g_EnemyDrawChain, 6) != 0)
    {
        return -1;
    }
    return 0;
}

int EnemyManager::OnDraw(EnemyManager *mgr)
{
    int i;
    Sprite *sprite;
    int k;
    Enemy *curEnemy;

    for (curEnemy = mgr->enemies, i = 0; i < 0x100; i++, curEnemy++)
    {
        if (!curEnemy->flags.active)
        {
            continue;
        }
        if (curEnemy->flags.hidden)
        {
            continue;
        }
        for (sprite = curEnemy->sprites, k = 0; k < 4; k++, sprite++)
        {
            if (sprite->scriptIndex >= 0)
            {
                if (sprite->autoRotate != 0)
                {
                    sprite->rotationZ = curEnemy->angle;
                }
                sprite->pos = curEnemy->pos + sprite->posOffset;
                sprite->pos.z = 0.495f;
                g_SpriteEngine->Draw3DRounded(sprite);
            }
        }
        if (curEnemy->flags.rotateSprite)
        {
            curEnemy->sprite.rotationZ = curEnemy->angle;
        }
        curEnemy->sprite.pos = curEnemy->pos + curEnemy->sprite.posOffset;
        curEnemy->sprite.pos.z = 0.494f;
        g_SpriteEngine->Draw3DRounded(&curEnemy->sprite);
        for (k = 4; k < 8; k++, sprite++)
        {
            if (sprite->scriptIndex >= 0)
            {
                if (sprite->autoRotate != 0)
                {
                    sprite->rotationZ = curEnemy->angle;
                }
                sprite->pos = curEnemy->pos + sprite->posOffset;
                sprite->pos.z = 0.495f;
                g_SpriteEngine->Draw3DRounded(sprite);
            }
        }
    }
    return 1;
}

int EnemyManager::AddedCallback(EnemyManager *mgr)
{
    Enemy *unused = mgr->enemies;

    if (mgr->anmPath != NULL && g_SpriteEngine->LoadAnm(8, mgr->anmPath, 0x100) != 0)
    {
        return -1;
    }
    if (mgr->anmPath2 != NULL && g_SpriteEngine->LoadAnm(9, mgr->anmPath2, 0x100) != 0)
    {
        return -1;
    }
    mgr->unk_ee5b8 = g_Rng.GetRandomU16InRange(3);
    mgr->unk_ee5ba = g_Rng.GetRandomU16InRange(8);
    mgr->unk_ee5c8 = 0;
    mgr->unk_ee5dc = 0;
    return 0;
}
