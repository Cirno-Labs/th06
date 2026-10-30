// The enemy script instructions' bullet patterns, from 0x40b8e0
// (ScriptFreezeBullets) to 0x40e2cc.

#include "enemy.h"
#include "enemyscript.h"
#include "features.h"
#include "gamemanager.h"
#include "item.h"
#include "player.h"
#include "stage.h"
#include "unsorted.h"

// g_EnemyScriptFuncs[0]: turns every flying bullet into color 15, then
// stops them (instr's second argument 0) or sends them drifting off in
// random directions (1).
void ScriptFreezeBullets(Enemy *enemy, EnemyScriptInstr *instr)
{
    int i;
    Bullet *bullet = g_BulletManager.bullets;
    int choice = instr->args.i[1];

    g_EffectManager.SpawnEffect(0xc, &enemy->pos, 1, 0xffffffff);
    for (i = 0; i < 0x280; i++, bullet++)
    {
        if (bullet->state == 0 || bullet->state == 5)
        {
            continue;
        }
        bullet->color = 0xf;
        g_SpriteEngine->SetSprite(&bullet->sprites.sprite0, bullet->sprites.sprite0.baseSpriteIndex + bullet->color);
        switch (choice)
        {
        case 0:
            bullet->speed = 0.0f;
            bullet->velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            break;
        case 1:
            bullet->flags |= 0x10;
            bullet->exDuration = 220;
            bullet->timer.Set(0);
            SinCosScale(&bullet->accel, g_Random.GetRandomF32InRange(6.2831855f) - 3.1415927f, 0.01f);
            break;
        }
    }
}

// g_EnemyScriptFuncs[1]: fires the enemy's shooter from a random point
// in a box around it, instr's second argument wide and three quarters
// as high.
void ScriptShootFromRandomPoint(Enemy *enemy, EnemyScriptInstr *instr)
{
    float range = instr->args.i[1];

    enemy->shooter.pos = enemy->pos + enemy->shootOffset;
    enemy->shooter.pos.x = g_Random.GetRandomF32InRange(range) + enemy->pos.x - range / 2.0f;
    range *= 0.75f;
    enemy->shooter.pos.y = g_Random.GetRandomF32InRange(range) + enemy->pos.y - range / 2.0f;
    g_BulletManager.FirePattern(&enemy->shooter);
}

// g_EnemyScriptFuncs[2], set as the context's func: every sixth frame
// until the count in intVars[2] reaches intVars[3], fires five patterns
// from points on a star (corners g_EnemyScripts.star.angles, radius floatVars[3])
// whose center moves from the enemy towards where the player was.
void ScriptStarPattern(Enemy *enemy, EnemyScriptInstr *instr)
{
    D3DXVECTOR3 center2;
    float progress2;
    D3DXVECTOR3 from;
    float t;
    D3DXVECTOR3 to3;
    float speed;
    int i;

    if (enemy->ctx.intVars[2] >= enemy->ctx.intVars[3])
    {
        enemy->ctx.func = NULL;
        return;
    }
    if (enemy->ctx.intVars[2] == 0)
    {
        g_EnemyScripts.star.ends[0] = enemy->pos;
        g_EnemyScripts.star.ends[1] = g_Player.position;
        g_EnemyScripts.star.angles[0] = g_Random.GetRandomF32InRange(6.2831855f) - 3.1415927f;
        g_EnemyScripts.star.angles[1] = AddNormalizeAngle(g_EnemyScripts.star.angles[0], 2.5132742f);
    }
    if (enemy->ctx.intVars[2] % 30 == 0)
    {
        g_EnemyScripts.star.angles[0] = g_EnemyScripts.star.angles[1];
        g_EnemyScripts.star.angles[1] = AddNormalizeAngle(g_EnemyScripts.star.angles[0], 2.5132742f);
        g_EnemyScripts.star.angles[2] = AddNormalizeAngle(g_EnemyScripts.star.angles[1], 2.5132742f);
        g_EnemyScripts.star.angles[3] = AddNormalizeAngle(g_EnemyScripts.star.angles[2], 2.5132742f);
        g_EnemyScripts.star.angles[4] = AddNormalizeAngle(g_EnemyScripts.star.angles[3], 2.5132742f);
        g_EnemyScripts.star.angles[5] = AddNormalizeAngle(g_EnemyScripts.star.angles[4], 2.5132742f);
    }
    if (enemy->ctx.intVars[2] % 6 == 0)
    {
        progress2 = (float)enemy->ctx.intVars[2] / (float)enemy->ctx.intVars[3];
        t = progress2 * 0.1f;
        center2 = (g_EnemyScripts.star.ends[1] - g_EnemyScripts.star.ends[0]) * t + g_EnemyScripts.star.ends[0];
        center2.z = 0.0f;
        progress2 += 0.5f;
        enemy->shooter.angle1 = 1.0471976f * progress2;
        for (i = 0; i < 5; i++)
        {
            t = (float)(enemy->ctx.intVars[2] % 30) / 30.0f;
            SinCosScale(&from, g_EnemyScripts.star.angles[i], enemy->ctx.floatVars[3]);
            SinCosScale(&to3, g_EnemyScripts.star.angles[i + 1], enemy->ctx.floatVars[3]);
            from = (to3 - from) * t + from;
            from.z = 0.0f;
            enemy->shooter.pos = center2 + from;
            speed = enemy->shooter.speed1;
            enemy->shooter.speed1 += g_Random.GetRandomF32InRange(enemy->shooter.speed2);
            g_BulletManager.FirePattern(&enemy->shooter);
            enemy->shooter.speed1 = speed;
            enemy->shooter.angle1 -= 0.5235988f * progress2;
        }
        g_SoundPlayer.QueueSound(0x16, 0);
    }
    enemy->ctx.intVars[2]++;
}

// g_EnemyScriptFuncs[3]: sets intVars[1] to [3] to the player's shot
// type's values from g_ShotTypeValues.
void ScriptGetShotTypeValues(Enemy *enemy, EnemyScriptInstr *instr)
{
    enemy->ctx.intVars[1] = g_ShotTypeValues[g_GameManager.character][g_GameManager.shotType][0];
    enemy->ctx.intVars[2] = g_ShotTypeValues[g_GameManager.character][g_GameManager.shotType][1];
    enemy->ctx.intVars[3] = g_ShotTypeValues[g_GameManager.character][g_GameManager.shotType][2];
}

// The angle from the player to `pos`, turned by a quarter turn and by
// `offset`.
inline float AngleAwayFromPlayer(D3DXVECTOR3 *pos, float offset)
{
    return g_Player.AngleTo(pos) + 1.5707964f + offset;
}

// g_EnemyScriptFuncs[4]: with instr's second argument below 2, sets
// g_GameManager.isFrozen to it. Otherwise turns some large bullets (14,
// or 52 above normal difficulty; one in four at random) into color 5 and
// sends them off: away from the player when close, else downwards (or
// anywhere above normal difficulty).
void ScriptFreezeOrSendOff(Enemy *enemy, EnemyScriptInstr *instr)
{
    Bullet *bullet;
    int i;
    int count;
    float dx2;
    float dy;

    if (instr->args.i[1] < 2)
    {
        g_EffectManager.SpawnEffect(0xc, &enemy->pos, 1, 0xffffffff);
        g_GameManager.isFrozen = instr->args.i[1];
    }
    else
    {
        count = 14;
        bullet = g_BulletManager.bullets;
        if (g_GameManager.difficulty <= 1)
        {
            for (i = 0; i < 0x280; i++, bullet++)
            {
                if (bullet->state == 0 || bullet->state == 5)
                {
                    continue;
                }
                if (bullet->sprites.sprite0.spriteData != NULL && bullet->sprites.sprite0.spriteData->height >= 30.0f &&
                    bullet->color != 5 && g_Random.GetRandomU16() % 4 == 0)
                {
                    bullet->color = 5;
                    g_SpriteEngine->SetSprite(&bullet->sprites.sprite0, bullet->sprites.sprite0.baseSpriteIndex + bullet->color);
                    dx2 = bullet->pos.x - g_Player.position.x;
                    dy = bullet->pos.y - g_Player.position.y;
                    if ((float)sqrt(dy * dy + dx2 * dx2) > 128.0f)
                    {
                        bullet->angle = g_Random.GetRandomF32InRange(2.3561945f) + 0.7853982f;
                    }
                    else
                    {
                        bullet->angle = AngleAwayFromPlayer(&bullet->pos, g_Random.GetRandomF32InRange(6.2831855f));
                    }
                    SinCosScale(&bullet->velocity, bullet->angle, bullet->speed);
                    count--;
                    if (count == 0)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            count = 52;
            for (i = 0; i < 0x280; i++, bullet++)
            {
                if (bullet->state == 0 || bullet->state == 5)
                {
                    continue;
                }
                if (bullet->sprites.sprite0.spriteData != NULL && bullet->sprites.sprite0.spriteData->height >= 30.0f &&
                    bullet->color != 5 && g_Random.GetRandomU16() % 4 == 0)
                {
                    bullet->color = 5;
                    g_SpriteEngine->SetSprite(&bullet->sprites.sprite0, bullet->sprites.sprite0.baseSpriteIndex + bullet->color);
                    dx2 = bullet->pos.x - g_Player.position.x;
                    dy = bullet->pos.y - g_Player.position.y;
                    if ((float)sqrt(dy * dy + dx2 * dx2) > 128.0f)
                    {
                        bullet->angle = g_Random.GetRandomF32InRange(6.2831855f);
                    }
                    else
                    {
                        bullet->angle = AngleAwayFromPlayer(&bullet->pos, g_Random.GetRandomF32InRange(6.2831855f));
                    }
                    SinCosScale(&bullet->velocity, bullet->angle, bullet->speed);
                    count--;
                    if (count == 0)
                    {
                        break;
                    }
                }
            }
        }
    }
    enemy->ctx.intVars[2] = 0;
}

// g_EnemyScriptFuncs[5], set as the context's func: every ninth frame,
// fires a fan of nine bullets from an arc to one side of the enemy (the
// side alternating), on the way towards the player.
void ScriptSideArcFans(Enemy *enemy, EnemyScriptInstr *instr)
{
    int i;
    int count;

    if (enemy->ctx.intVars[2] % 9 == 0)
    {
        D3DXVECTOR3 tmp4;
        float side;
        float cs2;
        float a2;
        D3DXVECTOR3 perp;
        float f2;
        D3DXVECTOR3 offset;
        float s;
        BulletShooter shooter;

        count = enemy->ctx.intVars[2] / 9;
        shooter.sprite = 8;
        shooter.aimMode = 0;
        if (g_GameManager.difficulty <= 1)
        {
            shooter.count1 = 1;
        }
        else
        {
            shooter.count1 = 3;
        }
        shooter.count2 = 1;
        shooter.angle2 = 0.5235988f;
        shooter.unk_4a = 0;
        shooter.flags = 0;
        f2 = 0.5f - count * 0.5f / 9.0f;
        tmp4 = g_Player.position - enemy->pos;
        D3DXVec3Normalize(&perp, &tmp4);
        if (count & 1)
        {
            side = -256.0f;
        }
        else
        {
            side = 256.0f;
        }
        perp *= side;
        tmp4 *= f2;
        offset = tmp4 + perp;
        perp = -perp;
        f2 = 0.7853982f;
        cs2 = cosf(f2);
        s = sinf(f2);
        tmp4 = perp;
        perp.x = tmp4.x * cs2 + tmp4.y * s;
        perp.y = -tmp4.x * s + tmp4.y * cs2;
        f2 = -0.17453294f;
        cs2 = cosf(f2);
        s = sinf(f2);
        shooter.angle1 = 0.0f;
        a2 = -0.7853982f;
        for (i = 0; i < 9; i++, a2 += 0.17453294f)
        {
            tmp4 = perp;
            perp.x = tmp4.x * cs2 + tmp4.y * s;
            perp.y = -tmp4.x * s + tmp4.y * cs2;
            shooter.pos = enemy->pos + perp + offset;
            shooter.speed1 = 2.0f;
            if ((count & 1) && g_GameManager.difficulty <= 1)
            {
                shooter.angle1 = a2;
            }
            shooter.color = 3;
            g_BulletManager.FirePattern(&shooter);
        }
        g_SoundPlayer.QueueSound(7, 0);
    }
    enemy->ctx.intVars[2]++;
}

// g_EnemyScriptFuncs[6], set as the context's func: while the enemy is
// shown, spawns pairs of sparks (effect 0x13) around it on a turning
// circle, more often as unk_ebc counts up; frees its effects once
// hidden.
void ScriptSparkCircle(Enemy *enemy, EnemyScriptInstr *instr)
{
    D3DXVECTOR3 pos;
    float a2;
    float r;
    int k;
    Effect *effect;

    if (enemy->flags.hidden)
    {
        ReleaseEnemyEffects(enemy);
        return;
    }
    enemy->sparkAngle += 0.017453292f;
    if (enemy->sparkAngle >= 0.7853982f)
    {
        enemy->sparkAngle -= 1.5707964f;
    }
    if (enemy->sparkTimer.HasTicked())
    {
        if (enemy->sparkTimer > 120 || (enemy->sparkTimer > 60 && enemy->sparkTimer.current % 2 == 0) ||
            (enemy->sparkTimer > 30 && enemy->sparkTimer.current % 4 == 0) || enemy->sparkTimer.current % 8 == 0)
        {
            k = enemy->sparkTimer.current % 16;
            k = g_Random.GetRandomU16InRange(k / 2) + k / 2;
            r = k * 160.0f / 16.0f + 32.0f;
            a2 = enemy->sparkAngle - k * 3.1415927f / 40.0f;
            if (r < -0.7853982f)
            {
                r += 1.5707964f;
            }
            pos = enemy->pos;
            pos.x += cosf(a2) * r;
            pos.y += sinf(a2) * r;
            effect = g_EffectManager.SpawnEffect(0x13, &pos, 1, 0xff3030ff);
            effect->velocity.x = (g_Random.GetRandomF32ZeroToOne() * 40.0f - 20.0f) / 60.0f;
            effect->velocity.y = 8.0f * k / 60.0f - 0.26666668f;
            effect->velocity.z = 0.0f;
            effect->acceleration = -effect->velocity / 120.0f;
            pos = enemy->pos;
            pos.x -= cosf(a2) * r;
            pos.y += sinf(a2) * r;
            effect = g_EffectManager.SpawnEffect(0x13, &pos, 1, 0xff3030ff);
            effect->velocity.x = (g_Random.GetRandomF32ZeroToOne() * 40.0f - 20.0f) / 60.0f;
            effect->velocity.y = 8.0f * k / 60.0f - 0.26666668f;
            effect->velocity.z = 0.0f;
            effect->acceleration = -effect->velocity / 120.0f;
        }
    }
    enemy->sparkTimer.Tick();
}

// g_EnemyScriptFuncs[7]: from two rings of eight points around the enemy
// (turning opposite ways), fires lasers (instr's second argument 0) or
// the enemy's shooter outwards, three times.
void ScriptTwoRingShots(Enemy *enemy, EnemyScriptInstr *instr)
{
    BulletShooter shooter;
    D3DXVECTOR3 at[8];
    float origin2;
    float a2;
    int half3;
    float step2;
    int k;
    int mode3;
    float radius;
    int i;

    mode3 = instr->args.i[1];
    origin2 = g_Random.GetRandomF32InRange(6.2831855f);
    for (half3 = 0; half3 < 2; half3++)
    {
        if (half3 == 0)
        {
            a2 = -3.1415927f + origin2;
            step2 = 0.7853982f;
        }
        else
        {
            a2 = -2.7488937f + origin2;
            step2 = -0.7853982f;
        }
        radius = 32.0f;
        for (i = 0; i < 8; i++)
        {
            at[i] = enemy->pos;
            at[i].x += cosf(a2) * radius;
            at[i].y += sinf(a2) * radius;
            a2 += 0.7853982f;
        }
        if (half3 == 0)
        {
            a2 = -3.1415927f + origin2;
        }
        else
        {
            a2 = -2.7488937f + origin2;
        }
        for (k = 0; k < 3; k++)
        {
            if (k < 2)
            {
                radius = 112.0f;
            }
            else
            {
                radius = 480.0f;
            }
            for (i = 0; i < 8; i++)
            {
                shooter.pos = at[i];
                shooter.sprite = 1;
                if (mode3 == 0)
                {
                    if (g_GameManager.difficulty <= 1)
                    {
                        shooter.color = 2;
                    }
                    else
                    {
                        shooter.color = 8;
                    }
                    shooter.angle1 = a2;
                    shooter.speed1 = 0.0f;
                    shooter.exFloats[0] = 0.0f;
                    if (g_GameManager.difficulty <= 1)
                    {
                        shooter.exFloats[1] = radius;
                        shooter.exFloats[2] = radius;
                    }
                    else
                    {
                        shooter.exFloats[1] = 440.0f;
                        shooter.exFloats[2] = 440.0f;
                    }
                    if (g_GameManager.difficulty <= 1)
                    {
                        shooter.exFloats[3] = 28.0f;
                    }
                    else
                    {
                        shooter.exFloats[3] = 20.0f;
                    }
                    shooter.exInts[0] = k * 16 + 60;
                    shooter.exInts[1] = 90 - k * 16;
                    shooter.exInts[2] = 16;
                    shooter.exInts[3] = 50;
                    shooter.laserEndHitFrames = 16;
                    shooter.flags = 2;
                    shooter.aimMode = 1;
                    g_BulletManager.SpawnLaser(&shooter);
                }
                else
                {
                    enemy->shooter.pos = shooter.pos;
                    g_BulletManager.FirePattern(&enemy->shooter);
                }
                at[i].x += cosf(a2) * radius;
                at[i].y += sinf(a2) * radius;
                a2 += 0.7853982f;
            }
            a2 += step2 - 6.2831855f;
        }
    }
}

// g_EnemyScriptFuncs[8]: fires one bullet in a random direction from
// every large bullet, and stores how many in intVars[3].
void ScriptShootFromLargeBullets(Enemy *enemy, EnemyScriptInstr *instr)
{
    int count = 0;
    Bullet *bullet = g_BulletManager.bullets;
    BulletShooter shooter;
    int i;

    for (i = 0; i < 0x280; i++, bullet++)
    {
        if (bullet->state == 0 || bullet->state == 5)
        {
            continue;
        }
        if (bullet->sprites.sprite0.spriteData != NULL && bullet->sprites.sprite0.spriteData->height >= 30.0f)
        {
            shooter.pos = bullet->pos;
            shooter.sprite = 3;
            shooter.color = 1;
            shooter.angle1 = g_Random.GetRandomF32InRange(6.2831855f) - 3.1415927f;
            shooter.speed1 = 0.0f;
            shooter.count1 = 1;
            shooter.count2 = 1;
            shooter.flags = 8;
            shooter.aimMode = 1;
            g_BulletManager.FirePattern(&shooter);
            count++;
        }
    }
    enemy->ctx.intVars[3] = count;
}

// g_EnemyScriptFuncs[9]: turns every small stopped bullet into color 2
// and starts it accelerating away, its direction turning with its
// distance from the enemy.
void ScriptSwirlStoppedBullets(Enemy *enemy, EnemyScriptInstr *instr)
{
    float base2;
    Bullet *bullet2 = g_BulletManager.bullets;
    BulletShooter shooter;
    int i;
    float dist;

    base2 = g_Random.GetRandomF32InRange(6.2831855f) - 3.1415927f;
    g_EffectManager.SpawnEffect(0xc, &enemy->pos, 1, 0xffffffff);
    for (i = 0; i < 0x280; i++, bullet2++)
    {
        if (bullet2->state == 0 || bullet2->state == 5)
        {
            continue;
        }
        if (bullet2->sprites.sprite0.spriteData != NULL && bullet2->sprites.sprite0.spriteData->height < 30.0f &&
            bullet2->speed == 0.0f)
        {
            bullet2->flags |= 0x10;
            bullet2->color = 2;
            g_SpriteEngine->SetSprite(&bullet2->sprites.sprite0, bullet2->sprites.sprite0.baseSpriteIndex + bullet2->color);
            bullet2->speed = 0.01f;
            bullet2->timer.Set(0);
            bullet2->exDuration = 120;
            dist = (enemy->pos.x - bullet2->pos.x) * (enemy->pos.x - bullet2->pos.x) +
                   (enemy->pos.y - bullet2->pos.y) * (enemy->pos.y - bullet2->pos.y);
            if (dist > 0.1f)
            {
                dist = sqrtf(dist);
            }
            else
            {
                dist = 0.0f;
            }
            SinCosScale(&bullet2->accel, dist * 3.1415927f / 256.0f + base2, 0.01f);
        }
    }
}

// g_EnemyScriptFuncs[10]: like ScriptSwirlStoppedBullets, each bullet accelerating in a
// random direction of its own.
void ScriptScatterStoppedBullets(Enemy *enemy, EnemyScriptInstr *instr)
{
    float base2;
    Bullet *bullet2 = g_BulletManager.bullets;
    BulletShooter shooter;
    int i;

    base2 = g_Random.GetRandomF32InRange(6.2831855f) - 3.1415927f;
    g_EffectManager.SpawnEffect(0xc, &enemy->pos, 1, 0xffffffff);
    for (i = 0; i < 0x280; i++, bullet2++)
    {
        if (bullet2->state == 0 || bullet2->state == 5)
        {
            continue;
        }
        if (bullet2->sprites.sprite0.spriteData != NULL && bullet2->sprites.sprite0.spriteData->height < 30.0f &&
            bullet2->speed == 0.0f)
        {
            bullet2->flags |= 0x10;
            bullet2->color = 2;
            g_SpriteEngine->SetSprite(&bullet2->sprites.sprite0, bullet2->sprites.sprite0.baseSpriteIndex + bullet2->color);
            bullet2->speed = 0.01f;
            bullet2->timer.Set(0);
            bullet2->exDuration = 120;
            SinCosScale(&bullet2->accel, g_Random.GetRandomF32InRange(6.2831855f) - 3.1415927f, 0.01f);
        }
    }
}

// g_EnemyScriptFuncs[11], set as the context's func: ScriptSparkCircle's sparks
// while the enemy lives; a bomb makes it unshootable (and turns its
// sprite to script 0x1a5) until 60 frames after the bomb ends.
void ScriptSparksAndBombShield(Enemy *enemy, EnemyScriptInstr *instr)
{
    if (enemy->life <= 0)
    {
        return;
    }
    ScriptSparkCircle(enemy, instr);
    if (g_Player.bomb.running != 0)
    {
        if (enemy->turnLeft >= 0)
        {
            g_SpriteEngine->StartScript(&enemy->sprite, 0x1a5);
            enemy->turnLeft = -1;
        }
        enemy->flags.shootable = 0;
        enemy->shieldTimer.Set(60);
    }
    else if (enemy->shieldTimer > 0)
    {
        enemy->shieldTimer.Sub(1);
        if (enemy->shieldTimer == 0)
        {
            if (enemy->turnLeft < 0)
            {
                g_SpriteEngine->StartScript(&enemy->sprite, 0x1a0);
                enemy->turnLeft = 0xa1;
            }
            enemy->flags.shootable = 1;
        }
    }
}

// g_EnemyScriptFuncs[12]: fires the enemy's shooter from 64 along each
// of its first eight lasers that is active.
void ScriptShootFromLasers(Enemy *enemy, EnemyScriptInstr *instr)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        if (enemy->lasers[i] != NULL && enemy->lasers[i]->active)
        {
            enemy->shooter.pos = D3DXVECTOR3(64.0f, 0.0f, 0.0f);
            Rotate2D(&enemy->shooter.pos, &enemy->shooter.pos, enemy->lasers[i]->angle);
            enemy->shooter.pos += enemy->pos;
            g_BulletManager.FirePattern(&enemy->shooter);
        }
    }
}

// g_EnemyScriptFuncs[13], set as the context's func: every sixth frame
// fires a copy of the enemy's shooter from instr's second argument
// points on a circle (radius floatVars[3], from angle floatVars[2])
// around the play area's center, each aimed outwards turned by
// floatVars[1].
void ScriptArenaCircleShots(Enemy *enemy, EnemyScriptInstr *instr)
{
    int i;
    BulletShooter sh = enemy->shooter;
    int n2 = instr->args.i[1];
    float theta = enemy->ctx.floatVars[2];
    int unused;

    if (enemy->ctx.intVars[3] % 6 == 0)
    {
        for (i = 0; i < n2; i++, theta += 6.2831855f / n2)
        {
            SinCosScale(&sh.pos, theta, enemy->ctx.floatVars[3]);
            sh.pos.x += 192.0f;
            sh.pos.y += 224.0f;
            sh.angle1 = theta + enemy->ctx.floatVars[1];
            g_BulletManager.FirePattern(&sh);
        }
    }
    enemy->ctx.intVars[3]++;
}

// g_EnemyScriptFuncs[14]: fires the enemy's shooter every 48 along each
// of its first eight lasers that is active, counting them in
// intVars[3].
void ScriptShootAlongLasers(Enemy *enemy, EnemyScriptInstr *instr)
{
    float r3;
    float cs2;
    Laser *l2;
    float sn3;
    int i2;
    D3DXVECTOR3 pos;

    enemy->ctx.intVars[3] = 0;
    for (i2 = 0; i2 < 8; i2++)
    {
        if (enemy->lasers[i2] != NULL && enemy->lasers[i2]->active)
        {
            l2 = enemy->lasers[i2];
            r3 = l2->startOffset;
            SinCos(&sn3, &cs2, l2->angle);
            while (l2->endOffset > r3)
            {
                pos.x = cs2 * r3 + l2->pos.x;
                pos.y = sn3 * r3 + l2->pos.y;
                pos.z = 0.0f;
                enemy->shooter.pos = pos;
                g_BulletManager.FirePattern(&enemy->shooter);
                r3 += 48.0f;
            }
            enemy->ctx.intVars[3]++;
        }
    }
}

// g_EnemyScriptFuncs[15]: around every large bullet, starts the small
// stopped bullets within reach accelerating away from the enemy, bent
// towards the large one's direction, and recolors them; then
// ScriptSparksAndBombShield. Stores the number of large bullets in intVars[3].
void ScriptBurstAroundLargeBullets(Enemy *enemy, EnemyScriptInstr *instr)
{
    int count = 0;
    Bullet *big3 = g_BulletManager.bullets;
    BulletShooter shooter;
    int i;
    Bullet *sm2;
    float bigAngle;
    float d;
    float theta;
    int j;

    for (i = 0; i < 0x280; i++, big3++)
    {
        if (big3->state == 0 || big3->state == 5)
        {
            continue;
        }
        if (big3->sprites.sprite0.spriteData != NULL && big3->sprites.sprite0.spriteData->height >= 30.0f)
        {
            count++;
            bigAngle = atan2f(big3->pos.y - enemy->pos.y, big3->pos.x - enemy->pos.x);
            for (j = 0, sm2 = g_BulletManager.bullets; j < 0x280; j++, sm2++)
            {
                if (sm2->state == 0 || sm2->state == 5)
                {
                    continue;
                }
                if (sm2->sprites.sprite0.spriteData != NULL && sm2->sprites.sprite0.spriteData->height < 30.0f &&
                    sm2->speed == 0.0f)
                {
                    d = sqrtf((sm2->pos.x - big3->pos.x) * (sm2->pos.x - big3->pos.x) +
                                 (sm2->pos.y - big3->pos.y) * (sm2->pos.y - big3->pos.y));
                    if (d < 64.0f)
                    {
                        sm2->flags |= 0x10;
                        sm2->speed = 0.01f;
                        sm2->timer.Set(0);
                        sm2->exDuration = 120;
                        theta = atan2f(sm2->pos.y - enemy->pos.y, sm2->pos.x - enemy->pos.x);
                        sm2->angle = (theta - bigAngle) * 2.2f + bigAngle;
                        SinCosScale(&sm2->accel, sm2->angle, 0.01f);
                        sm2->color = sm2->color + 1;
                        g_SpriteEngine->SetSprite(&sm2->sprites.sprite0, sm2->sprites.sprite0.baseSpriteIndex + sm2->color);
                    }
                }
            }
        }
    }
    ScriptSparksAndBombShield(enemy, instr);
    enemy->ctx.intVars[3] = count;
}

// g_EnemyScriptFuncs[16]: sets values that follow the enemy's life (0
// after 7200 frames): with instr's second argument 0, floatVars[3] and
// intVars2[1]; else a random x in floatVars[2] and y in floatVars[3],
// in a box that widens as the life falls.
void ScriptFollowLife(Enemy *enemy, EnemyScriptInstr *instr)
{
    int life = enemy->life;
    float range;

    if (enemy->callbackTimer >= 7200)
    {
        life = 0;
    }
    if (instr->args.i[1] == 0)
    {
        enemy->ctx.floatVars[3] = 2.0f - life * 1.0f / 6000.0f;
        enemy->ctx.intVars2[1] = life * 240 / 6000 + 40;
    }
    else
    {
        range = 320.0f - life * 160.0f / 6000.0f;
        enemy->ctx.floatVars[2] = 192.0f - range / 2.0f + g_Random.GetRandomF32InRange(range);
        range = 128.0f - life * 64.0f / 6000.0f;
        enemy->ctx.floatVars[3] = 96.0f - range / 2.0f + g_Random.GetRandomF32InRange(range);
    }
}
