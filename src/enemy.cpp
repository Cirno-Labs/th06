// The file with EnemyManager.

#include "enemy.h"
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
