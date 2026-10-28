// The file with EnemyManager.

#include <string.h>

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

void EnemyManager::Initialize()
{
    int i;
    Enemy *enemy = this->enemies;

    memset(this, 0, sizeof(EnemyManager));
    enemy = &this->enemyTemplate;
    memset(enemy, 0, sizeof(Enemy));
    for (i = 0; i < 8; i++)
    {
        enemy->sprites[i].scriptIndex = -1;
    }
    enemy->flags.active = 1;
    enemy->callbackTimer.Set(0);
    enemy->flags.unk8 = 1;
    enemy->flags.unk9 = 1;
    enemy->flags.onScreen = 0;
    enemy->hitboxSize = D3DXVECTOR3(12.0f, 12.0f, 12.0f);
    enemy->velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    enemy->angularVelocity = 0.0f;
    enemy->angle = 0.0f;
    enemy->acceleration = 0.0f;
    enemy->speed = 0.0f;
    enemy->flags.moveMode = 0;
    enemy->flags.unk5 = 0;
    enemy->flags.mirrored = 0;
    enemy->flags.unk11 = 0;
    enemy->stackDepth = 0;
    enemy->life = 1;
    enemy->score = 100;
    enemy->deathEffect = 0;
    enemy->unk_e3d = 0;
    enemy->unk_e3e = 0;
    enemy->shootInterval = 0;
    enemy->shootTimer.Set(0);
    enemy->shootOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    enemy->turnLeft = -1;
    enemy->turnRight = -1;
    enemy->turnStill = -1;
    enemy->flags.unk12 = 1;
    enemy->flags.unk13 = 0;
    enemy->deathSub = -1;
    enemy->flags.clampPos = 0;
    enemy->effectCount = 0;
    enemy->pendingInterrupt = -1;
    enemy->lifeCallbackLife = -1;
    enemy->timerCallbackTime = -1;
    enemy->laserIndex = 0;
    enemy->hitFlashTimer = 0;
    enemy->flags.rotateSprite = 0;
    enemy->rankSpeedLow = -0.5f;
    enemy->rankSpeedHigh = 0.5f;
}

EnemyManager::EnemyManager()
{
    this->Initialize();
}

Enemy *EnemyManager::Spawn(int sub, D3DXVECTOR3 *pos, short life, short item, int score)
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
        g_EnemyScripts.RunEnemy(enemy);
        enemy->color = enemy->sprite.color;
        enemy->item = item;
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

void EnemyManager::RunTimeline()
{
    Enemy *enemy;
    int delay;

    if (this->timelinePos == NULL)
    {
        this->timelinePos = g_EnemyScripts.timeline;
    }
    if (!g_Gui.IsMsgRunning())
    {
        delay = 2400;
        delay -= g_GameManager.lives * 4 * 60;
        if (this->timelineTime.HasTicked() && this->timelineTime.AsFrames() % delay == 0)
        {
            g_GameManager.AddSubRank(100);
        }
    }
    while (this->timelinePos->time >= 0)
    {
        if (this->timelineTime == this->timelinePos->time)
        {
            switch (this->timelinePos->opcode)
            {
            case 0:
                if (!g_Gui.IsBossPresent())
                {
                    EnemyTimelineArgs *args = &this->timelinePos->args;
                    this->Spawn(this->timelinePos->arg0, &args->pos, args->life, args->item,
                                args->score);
                }
                break;
            case 1:
                if (!g_Gui.IsBossPresent())
                {
                    this->Spawn(this->timelinePos->arg0, &this->timelinePos->args.pos, -1, -1, -1);
                }
                break;
            case 2:
                if (!g_Gui.IsBossPresent())
                {
                    EnemyTimelineArgs *args = &this->timelinePos->args;
                    enemy = this->Spawn(this->timelinePos->arg0, &args->pos, args->life, args->item,
                                        args->score);
                    enemy->flags.mirrored = 1;
                }
                break;
            case 3:
                if (!g_Gui.IsBossPresent())
                {
                    enemy = this->Spawn(this->timelinePos->arg0, &this->timelinePos->args.pos, -1, -1, -1);
                    enemy->flags.mirrored = 1;
                }
                break;
            case 4:
                if (!g_Gui.IsBossPresent())
                {
                    EnemyTimelineArgs *args = &this->timelinePos->args;
                    D3DXVECTOR3 pos;

                    pos = args->pos;
                    if (args->pos.x <= -990.0f)
                    {
                        pos.x = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[2]);
                    }
                    if (args->pos.y <= -990.0f)
                    {
                        pos.y = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[3]);
                    }
                    if (args->pos.z <= -990.0f)
                    {
                        pos.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    this->Spawn(this->timelinePos->arg0, &pos, args->life, args->item, args->score);
                }
                break;
            case 5:
                if (!g_Gui.IsBossPresent())
                {
                    D3DXVECTOR3 pos;

                    pos = this->timelinePos->args.pos;
                    if (pos.x <= -990.0f)
                    {
                        pos.x = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[2]);
                    }
                    if (pos.y <= -990.0f)
                    {
                        pos.y = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[3]);
                    }
                    if (pos.z <= -990.0f)
                    {
                        pos.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    this->Spawn(this->timelinePos->arg0, &pos, -1, -1, -1);
                }
                break;
            case 6:
                if (!g_Gui.IsBossPresent())
                {
                    EnemyTimelineArgs *args = &this->timelinePos->args;
                    D3DXVECTOR3 pos;

                    pos = args->pos;
                    if (args->pos.x <= -990.0f)
                    {
                        pos.x = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[2]);
                    }
                    if (args->pos.y <= -990.0f)
                    {
                        pos.y = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[3]);
                    }
                    if (args->pos.z <= -990.0f)
                    {
                        pos.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    enemy = this->Spawn(this->timelinePos->arg0, &pos, args->life, args->item,
                                        args->score);
                    enemy->flags.mirrored = 1;
                }
                break;
            case 7:
                if (!g_Gui.IsBossPresent())
                {
                    D3DXVECTOR3 pos;

                    pos = this->timelinePos->args.pos;
                    if (pos.x <= -990.0f)
                    {
                        pos.x = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[2]);
                    }
                    if (pos.y <= -990.0f)
                    {
                        pos.y = g_Rng.GetRandomF32InRange(g_GameManager.playerArea[3]);
                    }
                    if (pos.z <= -990.0f)
                    {
                        pos.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    enemy = this->Spawn(this->timelinePos->arg0, &pos, -1, -1, -1);
                    enemy->flags.mirrored = 1;
                }
                break;
            case 8:
                if (g_GameManager.difficulty == 0 && g_GameManager.stage == 5 && this->timelinePos->arg0 == 1)
                {
                    g_Gui.StartMsg(g_GameManager.character * 10 + 3);
                }
                else
                {
                    g_Gui.StartMsg(this->timelinePos->arg0 + g_GameManager.character * 10);
                }
                break;
            case 9:
                if (g_Gui.MsgHoldsTimeline())
                {
                    this->timelineTime.Sub(1);
                    return;
                }
                break;
            case 10:
                this->bosses[((int *)&this->timelinePos->args)[0]]->pendingInterrupt = ((int *)&this->timelinePos->args)[1];
                break;
            case 11:
                g_GameManager.power = this->timelinePos->arg0;
                break;
            case 12:
                if (this->bosses[this->timelinePos->arg0] != NULL && this->bosses[this->timelinePos->arg0]->flags.active)
                {
                    this->timelineTime.Sub(1);
                    return;
                }
                break;
            }
        }
        else if (this->timelineTime < this->timelinePos->time)
        {
            break;
        }
        this->timelinePos = (EnemyTimelineEntry *)((char *)this->timelinePos + this->timelinePos->size);
    }
    if (!g_Gui.IsMsgRunning())
    {
        g_GameManager.progressFrames++;
    }
}

int Enemy::CheckLifeCallback()
{
    Enemy *it;
    int i;

    if (this->life < this->lifeCallbackLife)
    {
        this->life = this->lifeCallbackLife;
        g_EnemyScripts.StartSub(&this->ctx, this->lifeCallbackSub);
        this->lifeCallbackLife = -1;
        this->timerCallbackSub = this->deathSub;
        this->rankSpeedLow = -0.5f;
        this->rankSpeedHigh = 0.5f;
        this->rankCount1Low = 0;
        this->rankCount1High = 0;
        this->rankCount2Low = 0;
        this->rankCount2High = 0;
        this->stackDepth = 0;
        for (it = g_EnemyManager.enemies, i = 0; i < 0x100; i++, it++)
        {
            if (!it->flags.active)
            {
                continue;
            }
            if (it->flags.unk11)
            {
                continue;
            }
            it->life = 0;
            if (!it->flags.unk8 && it->deathSub >= 0)
            {
                g_EnemyScripts.StartSub(&it->ctx, it->deathSub);
                it->deathSub = -1;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int Enemy::CheckTimerCallback()
{
    Enemy *it;
    int i;

    if (this->flags.unk11)
    {
        g_Gui.SetBossTimer((this->timerCallbackTime - this->callbackTimer.AsFrames()) / 60);
    }
    if (this->callbackTimer >= this->timerCallbackTime)
    {
        if (this->lifeCallbackLife > 0)
        {
            this->life = this->lifeCallbackLife;
            this->lifeCallbackLife = -1;
        }
        g_EnemyScripts.StartSub(&this->ctx, this->timerCallbackSub);
        this->timerCallbackTime = -1;
        this->timerCallbackSub = this->deathSub;
        this->callbackTimer.Set(0);
        if (!this->flags.unk20)
        {
            g_EnemyManager.spellCardCapture = 0;
            if (g_EnemyManager.spellCardActive != 0)
            {
                g_EnemyManager.spellCardActive++;
            }
            g_BulletManager.ClearAll(0);
        }
        for (it = g_EnemyManager.enemies, i = 0; i < 0x100; i++, it++)
        {
            if (!it->flags.active)
            {
                continue;
            }
            if (it->flags.unk11)
            {
                continue;
            }
            it->life = 0;
            if (!it->flags.unk8 && it->deathSub >= 0)
            {
                g_EnemyScripts.StartSub(&it->ctx, it->deathSub);
                it->deathSub = -1;
            }
        }
        this->rankSpeedLow = -0.5f;
        this->rankSpeedHigh = 0.5f;
        this->rankCount1Low = 0;
        this->rankCount1High = 0;
        this->rankCount2Low = 0;
        this->rankCount2High = 0;
        this->stackDepth = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

void Enemy::Despawn()
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
        g_Gui.bossPresent = 0;
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
        this->pos.x += g_App.frameAdvance * this->velocity.x;
    }
    else
    {
        this->pos.x -= g_App.frameAdvance * this->velocity.x;
    }
    this->pos.y += g_App.frameAdvance * this->velocity.y;
    this->pos.z += g_App.frameAdvance * this->velocity.z;
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

    mgr->RunTimeline();
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
            g_GameManager.IsInPlayArea(em->pos.x, em->pos.y, em->sprite.spriteData->width,
                                     em->sprite.spriteData->height) != 0)
        {
            em->flags.onScreen = 1;
        }
        if (em->flags.onScreen == 1 &&
            g_GameManager.IsInPlayArea(em->pos.x, em->pos.y, em->sprite.spriteData->width,
                                     em->sprite.spriteData->height) == 0)
        {
            em->flags.active = 0;
            em->Despawn();
            continue;
        }
        if (em->lifeCallbackLife >= 0)
        {
            em->CheckLifeCallback();
        }
        if (em->timerCallbackTime >= 0)
        {
            em->CheckTimerCallback();
        }
        if (g_EnemyScripts.RunEnemy(em) == -1)
        {
            em->flags.active = 0;
            em->Despawn();
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
                if (mgr->spellCardActive != 0)
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
                    else if (mgr->spellCardBombed != 0)
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
                em->lifeCallbackLife = -1;
                em->timerCallbackTime = -1;
                switch (em->flags.unk13)
                {
                case 3:
                    em->life = 1;
                    em->flags.unk12 = 0;
                    em->flags.unk13 = 0;
                    g_Gui.bossPresent = 0;
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
                        g_Gui.bossPresent = 0;
                        ReleaseEnemyEffects(em);
                    }
                case 2:
                    if (em->item >= 0)
                    {
                        g_EffectManager.SpawnEffect(em->unk_e3d + 4, &em->pos, 3, 0xffffffff);
                        g_ItemManager.SpawnItem(&em->pos, em->item, bomb);
                    }
                    else if (em->item == -1)
                    {
                        if (mgr->dropCounter % 3 == 0)
                        {
                            g_EffectManager.SpawnEffect(em->unk_e3d + 4, &em->pos, 6, 0xffffffff);
                            g_ItemManager.SpawnItem(&em->pos, g_EnemyItemDrops[mgr->dropIndex], bomb);
                            mgr->dropIndex++;
                            if (mgr->dropIndex >= 0x20)
                            {
                                mgr->dropIndex = 0;
                            }
                        }
                        mgr->dropCounter++;
                    }
                    if (em->flags.unk11 && g_EnemyManager.spellCardActive == 0)
                    {
                        g_BulletManager.ClearAllWithBonus(0x3200, 0);
                    }
                    em->life = 0;
                    break;
                }
                g_SoundPlayer.QueueSound(i % 2 + 2, 0);
                g_EffectManager.SpawnEffect(em->deathEffect, &em->pos, 1, 0xffffffff);
                g_EffectManager.SpawnEffect(em->unk_e3d + 4, &em->pos, 4, 0xffffffff);
                if (em->deathSub >= 0)
                {
                    em->rankSpeedLow = -0.5f;
                    em->rankSpeedHigh = 0.5f;
                    em->rankCount1Low = 0;
                    em->rankCount1High = 0;
                    em->rankCount2Low = 0;
                    em->rankCount2High = 0;
                    em->stackDepth = 0;
                    g_EnemyScripts.StartSub(&em->ctx, em->deathSub);
                    em->deathSub = -1;
                }
            }
            if (em->flags.unk11 && !g_Gui.IsMsgRunning())
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
                g_SoundPlayer.QueueSound(0x14, 0);
                em->sprite.flags.colorOp = 1;
                em->hitFlashTimer = 4;
            }
            else
            {
                em->sprite.flags.colorOp = 0;
            }
        }
        MoveEnemyEffects(em);
        if (g_GameManager.isFrozen == 0)
        {
            em->callbackTimer.Tick();
        }
    }
    mgr->timelineTime.Tick();
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
        if (eff->unk_15c < enemy->effectRadius)
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
    mgr->dropCounter = g_Rng.GetRandomU16InRange(3);
    mgr->dropIndex = g_Rng.GetRandomU16InRange(8);
    mgr->spellCardActive = 0;
    mgr->timelinePos = NULL;
    return 0;
}
