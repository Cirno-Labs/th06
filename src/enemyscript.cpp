// The file with EnemyScripts, from 0x407340.

#include "enemy.h"
#include "enemyscript.h"
#include "features.h"
#include "gamemanager.h"
#include "item.h"
#include "player.h"
#include "stage.h"
#include "unsorted.h"

// The game's data, as the original defines it here. The DLL build uses
// the original's copies instead.
#ifdef TH06_EXE
// Defined below; only this table names them.
void ScriptFreezeBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootFromRandomPoint(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptStarPattern(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptGetShotTypeValues(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptFreezeOrSendOff(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSideArcFans(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSparkCircle(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptTwoRingShots(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootFromLargeBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSwirlStoppedBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSparksAndBombShield(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptScatterStoppedBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootFromLasers(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptArenaCircleShots(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootAlongLasers(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptBurstAroundLargeBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptFollowLife(Enemy *enemy, EnemyScriptInstr *instr);

int g_SpellCardBonuses[0x40] = {
    200000, 200000, 200000, 200000, 200000, 200000, 200000, 250000,
    250000, 250000, 250000, 250000, 250000, 250000, 300000, 300000,
    300000, 300000, 300000, 300000, 300000, 300000, 300000, 300000,
    300000, 300000, 300000, 300000, 300000, 300000, 300000, 300000,
    400000, 400000, 400000, 400000, 400000, 400000, 400000, 400000,
    500000, 500000, 500000, 500000, 500000, 500000, 600000, 600000,
    600000, 600000, 600000, 700000, 700000, 700000, 700000, 700000,
    700000, 700000, 700000, 700000, 700000, 700000, 700000, 700000,
};

void (*g_EnemyScriptFuncs[])(Enemy *enemy, EnemyScriptInstr *instr) = {
    ScriptFreezeBullets,
    ScriptShootFromRandomPoint,
    ScriptStarPattern,
    ScriptGetShotTypeValues,
    ScriptFreezeOrSendOff,
    ScriptSideArcFans,
    ScriptSparkCircle,
    ScriptTwoRingShots,
    ScriptShootFromLargeBullets,
    ScriptSwirlStoppedBullets,
    ScriptSparksAndBombShield,
    ScriptScatterStoppedBullets,
    ScriptShootFromLasers,
    ScriptArenaCircleShots,
    ScriptShootAlongLasers,
    ScriptBurstAroundLargeBullets,
    ScriptFollowLife,
};

int g_ShotTypeValues[2][2][3] = {
    {{0, 3, 1}, {2, 3, 4}},
    {{1, 4, 0}, {4, 2, 3}},
};

// Constructed and never used.
ChainElem g_UnusedEclChain;
EnemyScripts g_EnemyScripts;
#endif // TH06_EXE

// D3DXVec3Length's body, but the game's own inline: the original has its
// out-of-line copy after ScriptAngle, where it is defined below, not after
// ResolveOperand, its only caller (an inline comes out after its first
// caller or its definition, whichever is later). Declared inline here, so
// ResolveOperand still reserves the stack slot of an inline call /Ob1
// gives up on.
inline float Vec3Length(const D3DXVECTOR3 *v);

int EnemyScripts::Load(char *path)
{
    int i;

    this->file = (EnemyScriptHeader *)LoadFileData(path, 0);
    if (this->file == NULL)
    {
        LogPrintf(&g_Log, "敵データの読み込みに失敗しました、データが壊れてるか失われています\n");
        return -1;
    }
    this->file->timelineOffset += (int)this->file;
    this->subs = (void **)((char *)this->file + sizeof(EnemyScriptHeader));
    for (i = 0; i < this->file->subCount; i++)
    {
        this->subs[i] = (char *)this->subs[i] + (int)this->file;
    }
    this->timeline = (EnemyTimelineEntry *)this->file->timelineOffset;
    return 0;
}

void EnemyScripts::Release()
{
    if (this->file != NULL)
    {
        MemFree(this->file);
    }
    this->file = NULL;
}

int EnemyScripts::StartSub(EnemyScriptState *state, short sub)
{
    state->code = this->subs[sub];
    state->timer.Set(0);
    state->sub = sub;
    return 0;
}

// Moves `lo` towards `hi` with the rank: lo at rank 0, hi at rank 32.
inline int RankLerp(int lo, int hi)
{
    return g_GameManager.rank * (hi - lo) / 32 + lo;
}

inline float RankLerpF(float lo, float hi)
{
    return g_GameManager.rank * (hi - lo) / 32.0f + lo;
}

int EnemyScripts::RunEnemy(Enemy *enemy)
{
    EnemyScriptInstr *instr;
    EnemyScriptArgs *params;
    float f;
    int value;
    D3DXVECTOR3 vec;

restart:
    instr = (EnemyScriptInstr *)enemy->ctx.code;
    if (enemy->pendingInterrupt >= 0)
    {
        goto interrupt;
    }
    while (enemy->ctx.timer == instr->time)
    {
        if ((instr->difficultyMask & (1 << g_GameManager.difficulty)) == 0)
        {
            goto next;
        }
        params = &instr->args;
        switch (instr->opcode)
        {
        case 1:
            return -1;
        case 3:
            value = *ResolveOperand(enemy, &params->i[2], NULL);
            value--;
            ScriptSet(enemy, params->i[2], &value);
            if (value <= 0)
            {
                break;
            }
        case 2:
        jump:
            enemy->ctx.timer.current = instr->args.i[0];
            instr = (EnemyScriptInstr *)((char *)instr + params->i[1]);
            continue;
        case 4:
        case 5:
            ScriptSet(enemy, instr->args.i[0], &params->i[1]);
            break;
        case 26:
            f = *(float *)ResolveOperand(enemy, &instr->args.i[0], NULL);
            f = AddNormalizeAngle(f, 0.0f);
            ScriptSet(enemy, instr->args.i[0], (int *)&f);
            break;
        case 6:
        {
            DWORD range = *ResolveOperand(enemy, &params->i[1], NULL);
            value = g_Random.GetRandomU32InRange(range);
            ScriptSet(enemy, instr->args.i[0], &value);
            break;
        }
        case 7:
        {
            DWORD range = *ResolveOperand(enemy, &params->i[1], NULL);
            int low = *ResolveOperand(enemy, &params->i[2], NULL);
            value = g_Random.GetRandomU32InRange(range);
            value += low;
            ScriptSet(enemy, instr->args.i[0], &value);
            break;
        }
        case 8:
        {
            float range = *ResolveFloatOperand(enemy, &params->f[1], NULL);
            f = g_Random.GetRandomF32InRange(range);
            ScriptSet(enemy, instr->args.i[0], (int *)&f);
            break;
        }
        case 9:
        {
            float range = *ResolveFloatOperand(enemy, &params->f[1], NULL);
            float low = *ResolveFloatOperand(enemy, &params->f[2], NULL);
            f = g_Random.GetRandomF32InRange(range);
            f += low;
            ScriptSet(enemy, instr->args.i[0], (int *)&f);
            break;
        }
        case 10:
            ScriptSet(enemy, instr->args.i[0], (int *)&enemy->pos.x);
            break;
        case 11:
            ScriptSet(enemy, instr->args.i[0], (int *)&enemy->pos.y);
            break;
        case 12:
            ScriptSet(enemy, instr->args.i[0], (int *)&enemy->pos.z);
            break;
        case 13:
        case 20:
            ScriptAdd(enemy, instr->args.i[0], &params->i[1], &params->i[2]);
            break;
        case 18:
        {
            int *var = ResolveOperand(enemy, &instr->args.i[0], NULL);
            *var += 1;
            break;
        }
        case 19:
        {
            int *var = ResolveOperand(enemy, &instr->args.i[0], NULL);
            *var -= 1;
            break;
        }
        case 14:
        case 21:
            ScriptSub(enemy, instr->args.i[0], &params->i[1], &params->i[2]);
            break;
        case 15:
        case 22:
            ScriptMul(enemy, instr->args.i[0], &params->i[1], &params->i[2]);
            break;
        case 16:
        case 23:
            ScriptDiv(enemy, instr->args.i[0], &params->i[1], &params->i[2]);
            break;
        case 17:
        case 24:
            ScriptMod(enemy, instr->args.i[0], &params->i[1], &params->i[2]);
            break;
        case 25:
            ScriptAngle(enemy, instr->args.i[0], &params->f[1], &params->f[2], &params->f[3], &params->f[4]);
            break;
        case 27:
        {
            int x = *ResolveOperand(enemy, &instr->args.i[0], NULL);
            int y = *ResolveOperand(enemy, &instr->args.i[1], NULL);
            enemy->ctx.compareResult = x == y ? 0 : (x < y ? -1 : 1);
            break;
        }
        case 28:
        {
            float a = *ResolveFloatOperand(enemy, &instr->args.f[0], NULL);
            float b = *ResolveFloatOperand(enemy, &instr->args.f[1], NULL);
            enemy->ctx.compareResult = a == b ? 0 : (a < b ? -1 : 1);
            break;
        }
        case 29:
            if (enemy->ctx.compareResult < 0)
            {
                goto jump;
            }
            break;
        case 30:
            if (enemy->ctx.compareResult <= 0)
            {
                goto jump;
            }
            break;
        case 31:
            if (enemy->ctx.compareResult == 0)
            {
                goto jump;
            }
            break;
        case 32:
            if (enemy->ctx.compareResult > 0)
            {
                goto jump;
            }
            break;
        case 33:
            if (enemy->ctx.compareResult >= 0)
            {
                goto jump;
            }
            break;
        case 34:
            if (enemy->ctx.compareResult != 0)
            {
                goto jump;
            }
            break;
        case 35:
        call:
            value = instr->args.i[0];
            enemy->ctx.code = (char *)instr + instr->size;
            if (!enemy->flags.noCallStack)
            {
                enemy->savedCtx[enemy->stackDepth] = enemy->ctx;
            }
            g_EnemyScripts.StartSub(&enemy->ctx, value);
            if (!enemy->flags.noCallStack && enemy->stackDepth < 7)
            {
                enemy->stackDepth++;
            }
            enemy->ctx.intVars[0] = instr->args.i[1];
            enemy->ctx.floatVars[0] = instr->args.f[2];
            goto restart;
        case 36:
            if (enemy->flags.noCallStack)
            {
                DebugPrint("error : no Stack Ret\n");
            }
            enemy->stackDepth--;
            enemy->ctx = enemy->savedCtx[enemy->stackDepth];
            goto restart;
        case 37:
            value = *ResolveOperand(enemy, &params->i[3], NULL);
            if (value < params->i[4])
            {
                goto call;
            }
            break;
        case 38:
            value = *ResolveOperand(enemy, &params->i[3], NULL);
            if (value <= params->i[4])
            {
                goto call;
            }
            break;
        case 39:
            value = *ResolveOperand(enemy, &params->i[3], NULL);
            if (value == params->i[4])
            {
                goto call;
            }
            break;
        case 40:
            value = *ResolveOperand(enemy, &params->i[3], NULL);
            if (value > params->i[4])
            {
                goto call;
            }
            break;
        case 41:
            value = *ResolveOperand(enemy, &params->i[3], NULL);
            if (value >= params->i[4])
            {
                goto call;
            }
            break;
        case 42:
            value = *ResolveOperand(enemy, &params->i[3], NULL);
            if (value != params->i[4])
            {
                goto call;
            }
            break;
        case 97:
            g_SpriteEngine->StartScript(&enemy->sprite, instr->args.i[0] + 0x100);
            break;
        case 99:
            if (instr->args.i[0] >= 8)
            {
                DebugPrint("error : sub anim overflow\n");
            }
            g_SpriteEngine->StartScript(&enemy->sprites[instr->args.i[0]], params->i[1] + 0x100);
            break;
        case 43:
            enemy->pos = *(D3DXVECTOR3 *)&instr->args;
            enemy->pos.x = *ResolveFloatOperand(enemy, &enemy->pos.x, NULL);
            enemy->pos.y = *ResolveFloatOperand(enemy, &enemy->pos.y, NULL);
            enemy->pos.z = *ResolveFloatOperand(enemy, &enemy->pos.z, NULL);
            enemy->ClampPos();
            break;
        case 44:
            enemy->velocity = *(D3DXVECTOR3 *)&instr->args;
            enemy->velocity.x = *ResolveFloatOperand(enemy, &enemy->velocity.x, NULL);
            enemy->velocity.y = *ResolveFloatOperand(enemy, &enemy->velocity.y, NULL);
            enemy->velocity.z = *ResolveFloatOperand(enemy, &enemy->velocity.z, NULL);
            enemy->flags.moveMode = 0;
            break;
        case 45:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->angle = *ResolveFloatOperand(enemy, &vec.x, NULL);
            enemy->speed = *ResolveFloatOperand(enemy, &vec.y, NULL);
            enemy->flags.moveMode = 1;
            break;
        case 46:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->angularVelocity = *ResolveFloatOperand(enemy, &vec.x, NULL);
            enemy->flags.moveMode = 1;
            break;
        case 51:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->angle = g_Player.AngleFrom(&enemy->pos) + vec.x;
            enemy->speed = *ResolveFloatOperand(enemy, &vec.y, NULL);
            enemy->flags.moveMode = 1;
            break;
        case 47:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->speed = *ResolveFloatOperand(enemy, &vec.x, NULL);
            enemy->flags.moveMode = 1;
            break;
        case 48:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->acceleration = *ResolveFloatOperand(enemy, &vec.x, NULL);
            enemy->flags.moveMode = 1;
            break;
        case 67:
        case 68:
        case 69:
        case 70:
        case 71:
        case 72:
        case 73:
        case 74:
        case 75:
        {
            EnemyBulletArgs *bulletArgs = &instr->args.bullet;
            BulletShooter *bs = &enemy->shooter;
            bs->sprite = bulletArgs->sprite;
            bs->aimMode = instr->opcode - 67;
            bs->count1 = *ResolveOperand(enemy, &bulletArgs->count1, NULL);
            bs->count1 += RankLerp(enemy->rankCount1Low, enemy->rankCount1High);
            if (bs->count1 <= 0)
            {
                bs->count1 = 1;
            }
            bs->count2 = *ResolveOperand(enemy, &bulletArgs->count2, NULL);
            bs->count2 += RankLerp(enemy->rankCount2Low, enemy->rankCount2High);
            if (bs->count2 <= 0)
            {
                bs->count2 = 1;
            }
            bs->pos = enemy->pos + enemy->shootOffset;
            bs->angle1 = *ResolveFloatOperand(enemy, &bulletArgs->angle1, NULL);
            bs->angle1 = AddNormalizeAngle(bs->angle1, 0.0f);
            bs->speed1 = *ResolveFloatOperand(enemy, &bulletArgs->speed1, NULL);
            if (bs->speed1 != 0.0f)
            {
                bs->speed1 += RankLerpF(enemy->rankSpeedLow, enemy->rankSpeedHigh);
                if (bs->speed1 < 0.3f)
                {
                    bs->speed1 = 0.3f;
                }
            }
            bs->angle2 = *ResolveFloatOperand(enemy, &bulletArgs->angle2, NULL);
            bs->speed2 = *ResolveFloatOperand(enemy, &bulletArgs->speed2, NULL);
            bs->speed2 += RankLerpF(enemy->rankSpeedLow, enemy->rankSpeedHigh) / 2.0f;
            if (bs->speed2 < 0.3f)
            {
                bs->speed2 = 0.3f;
            }
            bs->unk_4a = 0;
            bs->flags = bulletArgs->flags;
            value = bulletArgs->color;
            bs->color = *ResolveOperand(enemy, &value, NULL);
            if (!enemy->flags.holdFire)
            {
                g_BulletManager.FirePattern(bs);
            }
            break;
        }
        case 82:
            enemy->shooter.exInts[0] = *ResolveOperand(enemy, &params->i[0], NULL);
            enemy->shooter.exInts[1] = *ResolveOperand(enemy, &params->i[1], NULL);
            enemy->shooter.exInts[2] = *ResolveOperand(enemy, &params->i[2], NULL);
            enemy->shooter.exInts[3] = *ResolveOperand(enemy, &params->i[3], NULL);
            enemy->shooter.exFloats[0] = *ResolveFloatOperand(enemy, &params->f[4], NULL);
            enemy->shooter.exFloats[1] = *ResolveFloatOperand(enemy, &params->f[5], NULL);
            enemy->shooter.exFloats[2] = *ResolveFloatOperand(enemy, &params->f[6], NULL);
            enemy->shooter.exFloats[3] = *ResolveFloatOperand(enemy, &params->f[7], NULL);
            break;
        case 100:
        {
            EnemyScriptArgs *effectArgs = &instr->args;
            enemy->deathEffect = effectArgs->b[0];
            enemy->unk_e3d = effectArgs->b[1];
            enemy->unk_e3e = effectArgs->b[2];
            break;
        }
        case 76:
            enemy->shootInterval = instr->args.i[0];
            enemy->shootInterval += RankLerp(enemy->shootInterval / 5, -enemy->shootInterval / 5);
            enemy->shootTimer.Set(0);
            break;
        case 77:
            enemy->shootInterval = instr->args.i[0];
            enemy->shootInterval += RankLerp(enemy->shootInterval / 5, -enemy->shootInterval / 5);
            if (enemy->shootInterval != 0)
            {
                enemy->shootTimer.Set(g_Random.GetRandomU32InRange(enemy->shootInterval));
            }
            break;
        case 78:
            enemy->flags.holdFire = 1;
            break;
        case 79:
            enemy->flags.holdFire = 0;
            break;
        case 80:
            enemy->shooter.pos = enemy->pos + enemy->shootOffset;
            g_BulletManager.FirePattern(&enemy->shooter);
            break;
        case 81:
            enemy->shootOffset.x = *ResolveFloatOperand(enemy, &params->f[0], NULL);
            enemy->shootOffset.y = *ResolveFloatOperand(enemy, &params->f[1], NULL);
            enemy->shootOffset.z = *ResolveFloatOperand(enemy, &params->f[2], NULL);
            break;
        case 85:
        case 86:
        {
            EnemyScriptArgs *laserArgs = &instr->args;
            BulletShooter *shooter = &enemy->laserShooter;
            shooter->pos = enemy->pos + enemy->shootOffset;
            shooter->sprite = laserArgs->s[0];
            shooter->color = laserArgs->s[1];
            shooter->angle1 = *ResolveFloatOperand(enemy, &laserArgs->f[1], NULL);
            shooter->speed1 = *ResolveFloatOperand(enemy, &laserArgs->f[2], NULL);
            shooter->exFloats[0] = *ResolveFloatOperand(enemy, &laserArgs->f[3], NULL);
            shooter->exFloats[1] = *ResolveFloatOperand(enemy, &laserArgs->f[4], NULL);
            shooter->exFloats[2] = *ResolveFloatOperand(enemy, &laserArgs->f[5], NULL);
            shooter->exFloats[3] = laserArgs->f[6];
            shooter->exInts[0] = laserArgs->i[7];
            shooter->exInts[1] = laserArgs->i[8];
            shooter->exInts[2] = laserArgs->i[9];
            shooter->exInts[3] = laserArgs->i[10];
            shooter->laserEndHitFrames = laserArgs->i[11];
            shooter->flags = laserArgs->i[12];
            if (instr->opcode == 86)
            {
                shooter->aimMode = 0;
            }
            else
            {
                shooter->aimMode = 1;
            }
            enemy->lasers[enemy->laserIndex] = g_BulletManager.SpawnLaser(shooter);
            break;
        }
        case 87:
            enemy->laserIndex = *ResolveOperand(enemy, &instr->args.i[0], NULL);
            break;
        case 88:
            if (enemy->lasers[instr->args.i[0]] != NULL)
            {
                enemy->lasers[instr->args.i[0]]->angle = enemy->lasers[instr->args.i[0]]->angle + *ResolveFloatOperand(enemy, &instr->args.f[1], NULL);
            }
            break;
        case 89:
            if (enemy->lasers[instr->args.i[0]] != NULL)
            {
                enemy->lasers[instr->args.i[0]]->angle = g_Player.AngleFrom(&enemy->lasers[instr->args.i[0]]->pos) + *ResolveFloatOperand(enemy, &instr->args.f[1], NULL);
            }
            break;
        case 90:
            if (enemy->lasers[instr->args.i[0]] != NULL)
            {
                enemy->lasers[instr->args.i[0]]->pos = enemy->pos + *(D3DXVECTOR3 *)&instr->args.f[1];
            }
            break;
        case 91:
            if (enemy->lasers[instr->args.i[0]] != NULL && enemy->lasers[instr->args.i[0]]->active)
            {
                enemy->ctx.compareResult = 0;
            }
            else
            {
                enemy->ctx.compareResult = 1;
            }
            break;
        case 92:
            if (enemy->lasers[instr->args.i[0]] != NULL && enemy->lasers[instr->args.i[0]]->active && enemy->lasers[instr->args.i[0]]->state < 2)
            {
                enemy->lasers[instr->args.i[0]]->state = 2;
                enemy->lasers[instr->args.i[0]]->timer.Set(0);
            }
            break;
        case 134:
        {
            int i;
            for (i = 0; i < 0x20; i++)
            {
                enemy->lasers[i] = NULL;
            }
            break;
        }
        case 101:
            if (instr->args.i[0] >= 0)
            {
                g_EnemyManager.bosses[instr->args.i[0]] = enemy;
                g_Gui.bossPresent = 1;
                g_Gui.bossHealth = 1.0f;
                enemy->flags.isBoss = 1;
                enemy->bossIndex = instr->args.i[0];
            }
            else
            {
                g_Gui.bossPresent = 0;
                g_EnemyManager.bosses[enemy->bossIndex] = NULL;
                enemy->flags.isBoss = 0;
            }
            break;
        case 102:
        {
            EnemyScriptArgs *effectArgs = &instr->args;
            enemy->effects[enemy->effectCount] = g_EffectManager.SpawnEffect(0xd, &enemy->pos, 1, g_EffectColors[effectArgs->i[0]]);
            enemy->effects[enemy->effectCount]->axis = *(D3DXVECTOR3 *)&effectArgs->f[1];
            enemy->effectRadius = effectArgs->f[4];
            enemy->effectCount++;
            break;
        }
        case 52:
            ScriptMoveAngle(enemy, instr);
            enemy->flags.moveEasing = 1;
            break;
        case 53:
            ScriptMoveAngle(enemy, instr);
            enemy->flags.moveEasing = 2;
            break;
        case 54:
            ScriptMoveAngle(enemy, instr);
            enemy->flags.moveEasing = 3;
            break;
        case 55:
            ScriptMoveAngle(enemy, instr);
            enemy->flags.moveEasing = 4;
            break;
        case 56:
            ScriptMovePos(enemy, instr);
            enemy->flags.moveEasing = 0;
            break;
        case 57:
            ScriptMovePos(enemy, instr);
            enemy->flags.moveEasing = 1;
            break;
        case 58:
            ScriptMovePos(enemy, instr);
            enemy->flags.moveEasing = 2;
            break;
        case 59:
            ScriptMovePos(enemy, instr);
            enemy->flags.moveEasing = 3;
            break;
        case 60:
            ScriptMovePos(enemy, instr);
            enemy->flags.moveEasing = 4;
            break;
        case 61:
            ScriptMoveTime(enemy, instr);
            enemy->flags.moveEasing = 1;
            break;
        case 62:
            ScriptMoveTime(enemy, instr);
            enemy->flags.moveEasing = 2;
            break;
        case 63:
            ScriptMoveTime(enemy, instr);
            enemy->flags.moveEasing = 3;
            break;
        case 64:
            ScriptMoveTime(enemy, instr);
            enemy->flags.moveEasing = 4;
            break;
        case 65:
            enemy->lowerBound[0] = instr->args.f[0];
            enemy->lowerBound[1] = instr->args.f[1];
            enemy->upperBound[0] = instr->args.f[2];
            enemy->upperBound[1] = instr->args.f[3];
            enemy->flags.clampPos = 1;
            break;
        case 66:
            enemy->flags.clampPos = 0;
            break;
        case 49:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->angle = g_Random.GetRandomF32InRange(vec.y - vec.x) + vec.x;
            break;
        case 50:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->angle = g_Random.GetRandomF32InRange(vec.y - vec.x) + vec.x;
            if (enemy->lowerBound[0] + 96.0f > enemy->pos.x)
            {
                if (enemy->angle > 1.5707964f)
                {
                    enemy->angle = 3.1415927f - enemy->angle;
                }
                else if (enemy->angle < -1.5707964f)
                {
                    enemy->angle = -3.1415927f - enemy->angle;
                }
            }
            if (enemy->upperBound[0] - 96.0f < enemy->pos.x)
            {
                if (enemy->angle < 1.5707964f && enemy->angle >= 0.0f)
                {
                    enemy->angle = 3.1415927f - enemy->angle;
                }
                else if (enemy->angle > -1.5707964f && enemy->angle <= 0.0f)
                {
                    enemy->angle = -3.1415927f - enemy->angle;
                }
            }
            if (enemy->lowerBound[1] + 48.0f > enemy->pos.y)
            {
                if (enemy->angle < 0.0f)
                {
                    enemy->angle = -enemy->angle;
                }
            }
            if (enemy->upperBound[1] - 48.0f < enemy->pos.y)
            {
                if (enemy->angle > 0.0f)
                {
                    enemy->angle = -enemy->angle;
                }
            }
            break;
        case 98:
            enemy->turnStill = instr->args.s[0];
            enemy->turnStillFromLeft = instr->args.s[1];
            enemy->turnStillFromRight = instr->args.s[2];
            enemy->turnLeft = instr->args.s[3];
            enemy->turnRight = instr->args.s[4];
            enemy->flags.spriteDirection = 0xff;
            break;
        case 103:
            enemy->hitboxSize.x = instr->args.f[0];
            enemy->hitboxSize.y = instr->args.f[1];
            enemy->hitboxSize.z = instr->args.f[2];
            break;
        case 104:
            enemy->flags.contactDamage = instr->args.i[0];
            break;
        case 105:
            enemy->flags.takesDamage = instr->args.i[0];
            break;
        case 106:
            g_SoundPlayer.QueueSound(instr->args.i[0], 0);
            break;
        case 107:
            enemy->flags.deathMode = instr->args.i[0];
            break;
        case 108:
            enemy->deathSub = instr->args.i[0];
            break;
        case 109:
            enemy->interrupts[params->i[1]] = params->i[0];
            break;
        case 110:
            enemy->pendingInterrupt = instr->args.i[0];
        interrupt:
            enemy->ctx.code = (char *)instr + instr->size;
            if (!enemy->flags.noCallStack)
            {
                enemy->savedCtx[enemy->stackDepth] = enemy->ctx;
            }
            g_EnemyScripts.StartSub(&enemy->ctx, enemy->interrupts[enemy->pendingInterrupt]);
            if (enemy->stackDepth < 7)
            {
                enemy->stackDepth++;
            }
            enemy->pendingInterrupt = -1;
            goto restart;
        case 111:
            enemy->maxLife = instr->args.i[0];
            enemy->life = enemy->maxLife;
            break;
        case 93:
        {
            Catk *rec;
            int len;
            int sum;
#if FEAT_ENGLISH
            g_Gui.ShowSpellCardName(instr->args.s[0], ENGLISH_BOSS_SPELL_CARD(instr->args.s[1]));
#else
            g_Gui.ShowSpellCardName(instr->args.s[0], (char *)&instr->args.i[1]);
#endif
            g_EnemyManager.spellCardCapture = 1;
            g_EnemyManager.spellCardActive = 1;
            g_EnemyManager.spellCardIndex = instr->args.s[1];
            g_EnemyManager.spellCardBonus = g_SpellCardBonuses[g_EnemyManager.spellCardIndex];
            g_BulletManager.ClearAllToItems();
            g_Stage.spellCardState = 1;
            g_Stage.spellCardFrames = 0;
            enemy->rankSpeedLow = -0.5f;
            enemy->rankSpeedHigh = 0.5f;
            enemy->rankCount1Low = 0;
            enemy->rankCount1High = 0;
            enemy->rankCount2Low = 0;
            enemy->rankCount2High = 0;
            rec = &g_GameManager.catks[g_EnemyManager.spellCardIndex];
            sum = 0;
            if (g_GameManager.isInReplay == 0)
            {
                strcpy(rec->name, (char *)&instr->args.i[1]);
                len = strlen(rec->name);
                while (len > 0)
                {
                    len--;
                    sum += rec->name[len];
                }
                if (rec->nameChecksum != (sum & 0xff))
                {
                    rec->captures = 0;
                    rec->attempts = 0;
                    rec->nameChecksum = sum;
                }
                rec->bonus = g_EnemyManager.spellCardBonus;
                if (rec->attempts < 9999)
                {
                    rec->attempts++;
                }
            }
            break;
        }
        case 94:
            if (g_EnemyManager.spellCardActive != 0)
            {
                g_Gui.EndSpellCardName();
                if (g_EnemyManager.spellCardActive == 1)
                {
                    int bonus = g_BulletManager.ClearAllWithBonus(0x3200, 1);
                    if (g_EnemyManager.spellCardCapture != 0)
                    {
                        Catk *rec = &g_GameManager.catks[g_EnemyManager.spellCardIndex];
                        int k;
                        int tenth = g_EnemyManager.spellCardBonus >= 500000 ? 50000 : g_EnemyManager.spellCardBonus / 10;
                        bonus = g_EnemyManager.spellCardBonus + g_EnemyManager.spellCardBonus * g_Gui.GetBossTimer() / 10;
                        g_Gui.ShowSpellCardBonus(bonus);
                        g_GameManager.score += bonus;
                        if (g_GameManager.isInReplay == 0)
                        {
                            rec->captures++;
                            for (k = 4; k > 0; k--)
                            {
                                rec->history[k] = rec->history[k - 1];
                            }
                            rec->history[0] = g_GameManager.character * 2 + g_GameManager.shotType;
                        }
                        g_GameManager.spellCardsCaptured++;
                    }
                }
                g_EnemyManager.spellCardActive = 0;
            }
            g_Stage.spellCardState = 0;
            break;
        case 112:
            enemy->callbackTimer.Set(instr->args.i[0]);
            break;
        case 113:
            enemy->lifeCallbackLife = instr->args.i[0];
            break;
        case 114:
            enemy->lifeCallbackSub = instr->args.i[0];
            break;
        case 115:
            enemy->timerCallbackTime = instr->args.i[0];
            enemy->callbackTimer.Set(0);
            break;
        case 116:
            enemy->timerCallbackSub = instr->args.i[0];
            break;
        case 117:
            enemy->flags.shootable = instr->args.i[0];
            break;
        case 118:
            g_EffectManager.SpawnEffect(instr->args.i[0], &enemy->pos, instr->args.i[1], instr->args.i[2]);
            break;
        case 119:
        {
            int i;
            D3DXVECTOR3 pos;
            for (i = 0; i < instr->args.i[0]; i++)
            {
                pos = enemy->pos;
                pos[0] += g_Random.GetRandomF32InRange(144.0f) - 72.0f;
                pos[1] += g_Random.GetRandomF32InRange(144.0f) - 72.0f;
                if (g_GameManager.power < 0x80)
                {
                    g_ItemManager.SpawnItem(&pos, i == 0 ? 2 : 0, 0);
                }
                else
                {
                    g_ItemManager.SpawnItem(&pos, 1, 0);
                }
            }
            break;
        }
        case 120:
            enemy->flags.rotateSprite = instr->args.i[0];
            break;
        case 121:
            g_EnemyScriptFuncs[instr->args.i[0]](enemy, instr);
            break;
        case 122:
            if (instr->args.i[0] >= 0)
            {
                enemy->ctx.func = g_EnemyScriptFuncs[instr->args.i[0]];
            }
            else
            {
                enemy->ctx.func = NULL;
            }
            break;
        case 123:
            enemy->ctx.timer += *ResolveOperand(enemy, &instr->args.i[0], NULL);
            break;
        case 124:
            g_ItemManager.SpawnItem(&enemy->pos, instr->args.i[0], 0);
            break;
        case 125:
            g_Stage.scriptResume = 1;
            break;
        case 126:
            g_Gui.SetBossLives(instr->args.i[0]);
            g_GameManager.progressFrames += 1800;
            break;
        case 95:
        {
            EnemySpawnArgs spawn = *(EnemySpawnArgs *)&instr->args;
            spawn.pos.x = *ResolveFloatOperand(enemy, &spawn.pos.x, NULL);
            spawn.pos.y = *ResolveFloatOperand(enemy, &spawn.pos.y, NULL);
            spawn.pos.z = *ResolveFloatOperand(enemy, &spawn.pos.z, NULL);
            g_EnemyManager.Spawn(spawn.sub, &spawn.pos, spawn.life, spawn.item, spawn.score);
            break;
        }
        case 96:
        {
            Enemy *en;
            int i;
            for (en = g_EnemyManager.enemies, i = 0; i < 0x100; i++, en++)
            {
                if (!en->flags.active)
                {
                    continue;
                }
                if (en->flags.isBoss)
                {
                    continue;
                }
                en->life = 0;
                if (!en->flags.shootable && en->deathSub >= 0)
                {
                    g_EnemyScripts.StartSub(&en->ctx, en->deathSub);
                    en->deathSub = -1;
                }
            }
            break;
        }
        case 128:
            enemy->sprite.pendingInterrupt = instr->args.s[0];
            break;
        case 129:
            enemy->sprites[params->i[0]].pendingInterrupt = params->s[2];
            break;
        case 83:
            g_BulletManager.ClearAllToItems();
            break;
        case 84:
            if (instr->args.i[0] >= 0)
            {
                enemy->shooter.sound = instr->args.i[0];
                enemy->shooter.flags |= 0x200;
            }
            else
            {
                enemy->shooter.flags &= ~0x200;
            }
            break;
        case 130:
            enemy->flags.noCallStack = instr->args.i[0];
            break;
        case 131:
            enemy->rankSpeedLow = params->f[0];
            enemy->rankSpeedHigh = params->f[1];
            enemy->rankCount1Low = params->i[2];
            enemy->rankCount1High = params->i[3];
            enemy->rankCount2Low = params->i[4];
            enemy->rankCount2High = params->i[5];
            break;
        case 132:
            enemy->flags.hidden = instr->args.i[0];
            break;
        case 133:
            enemy->timerCallbackSub = enemy->deathSub;
            enemy->callbackTimer.Set(0);
            break;
        case 135:
            enemy->flags.keepSpellCard = instr->args.i[0];
            break;
        }
    next:
        instr = (EnemyScriptInstr *)((char *)instr + instr->size);
    }
    switch (enemy->flags.moveMode)
    {
    case 1:
        enemy->angle = AddNormalizeAngle(enemy->angle, g_App.frameAdvance * enemy->angularVelocity);
        enemy->speed = g_App.frameAdvance * enemy->acceleration + enemy->speed;
        SinCosScale(&enemy->velocity, enemy->angle, enemy->speed);
        enemy->velocity.z = 0.0f;
        break;
    case 2:
    {
        float t;
        enemy->moveTimer.Sub(1);
        t = enemy->moveTimer.AsFloat() / enemy->moveDuration;
        if (t >= 1.0f)
        {
            t = 1.0f;
        }
        switch (enemy->flags.moveEasing)
        {
        case 0:
            t = 1.0f - t;
            break;
        case 1:
            t = 1.0f - t * t;
            break;
        case 2:
            t = 1.0f - t * t * t * t;
            break;
        case 3:
            t = 1.0f - t;
            t = t * t;
            break;
        case 4:
            t = 1.0f - t;
            t = t * t * t * t;
            break;
        }
        enemy->velocity = t * enemy->moveDelta + enemy->moveStart - enemy->pos;
        enemy->angle = atan2f(enemy->velocity.y, enemy->velocity.x);
        if (enemy->moveTimer <= 0)
        {
            enemy->flags.moveMode = 0;
            enemy->pos = enemy->moveStart + enemy->moveDelta;
            enemy->velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        }
        break;
    }
    }
    if (enemy->life > 0)
    {
        if (enemy->shootInterval > 0)
        {
            enemy->shootTimer.Tick();
            if (enemy->shootTimer >= enemy->shootInterval)
            {
                enemy->shooter.pos = enemy->pos + enemy->shootOffset;
                g_BulletManager.FirePattern(&enemy->shooter);
                enemy->shootTimer.Set(0);
            }
        }
        if (enemy->turnLeft >= 0)
        {
            int dir = 0;
            if (enemy->velocity.x < 0.0f)
            {
                dir = 1;
            }
            else if (enemy->velocity.x > 0.0f)
            {
                dir = 2;
            }
            if (enemy->flags.spriteDirection != dir)
            {
                switch (dir)
                {
                case 0:
                    if (enemy->flags.spriteDirection == 0xff)
                    {
                        g_SpriteEngine->StartScript(&enemy->sprite, enemy->turnStill + 0x100);
                    }
                    else if (enemy->flags.spriteDirection == 1)
                    {
                        g_SpriteEngine->StartScript(&enemy->sprite, enemy->turnStillFromLeft + 0x100);
                    }
                    else
                    {
                        g_SpriteEngine->StartScript(&enemy->sprite, enemy->turnStillFromRight + 0x100);
                    }
                    break;
                case 1:
                    g_SpriteEngine->StartScript(&enemy->sprite, enemy->turnLeft + 0x100);
                    break;
                case 2:
                    g_SpriteEngine->StartScript(&enemy->sprite, enemy->turnRight + 0x100);
                    break;
                }
                enemy->flags.spriteDirection = dir;
            }
        }
        if (enemy->ctx.func != NULL)
        {
            enemy->ctx.func(enemy, NULL);
        }
    }
    enemy->ctx.code = instr;
    enemy->ctx.timer.Tick();
    return 0;
}

void ScriptMoveAngle(Enemy *enemy, EnemyScriptInstr *instr)
{
    EnemyMoveAngleArgs *args;
    float angle;

    args = &instr->args.moveAngle;
    angle = *ResolveFloatOperand(enemy, &args->angle, NULL);
    enemy->moveDelta.x = cosf(angle) * args->speed * (float)args->duration / 2.0f;
    enemy->moveDelta.y = sinf(angle) * args->speed * (float)args->duration / 2.0f;
    enemy->moveDelta.z = 0.0f;
    enemy->moveStart = enemy->pos;
    enemy->moveDuration = args->duration;
    enemy->moveTimer.Set(enemy->moveDuration);
    enemy->flags.moveMode = 2;
}

void ScriptMovePos(Enemy *enemy, EnemyScriptInstr *instr)
{
    EnemyMovePosArgs *args;
    D3DXVECTOR3 target;

    args = &instr->args.movePos;
    target.x = *ResolveFloatOperand(enemy, &args->x, NULL);
    target.y = *ResolveFloatOperand(enemy, &args->y, NULL);
    target.z = *ResolveFloatOperand(enemy, &args->z, NULL);
    enemy->moveDelta = target - enemy->pos;
    enemy->moveStart = enemy->pos;
    enemy->moveDuration = args->duration;
    enemy->moveTimer.Set(enemy->moveDuration);
    enemy->flags.moveMode = 2;
    enemy->velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void ScriptMoveTime(Enemy *enemy, EnemyScriptInstr *instr)
{
    EnemyMoveTimeArgs *args;
    float angle;

    args = &instr->args.moveTime;
    angle = *ResolveFloatOperand(enemy, &enemy->angle, NULL);
    enemy->moveDelta.x = cosf(angle) * enemy->speed * (float)args->duration / 2.0f;
    enemy->moveDelta.y = sinf(angle) * enemy->speed * (float)args->duration / 2.0f;
    enemy->moveDelta.z = 0.0f;
    enemy->moveStart = enemy->pos;
    enemy->moveDuration = args->duration;
    enemy->moveTimer.Set(enemy->moveDuration);
    enemy->flags.moveMode = 2;
}

int *ResolveOperand(Enemy *enemy, int *operand, int *type)
{
#ifdef TH06_EXE
    // The original's are statics of this function.
    static int g_ShotTypeOperand;
    static float g_PlayerDistance;
    static float g_PlayerAngle;
#endif // TH06_EXE
    if (type != NULL)
    {
        *type = 3;
    }
    switch (*operand)
    {
    case -10001:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars[0];
    case -10002:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars[1];
    case -10003:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars[2];
    case -10004:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars[3];
    case -10005:
        if (type != NULL)
        {
            *type = 1;
        }
        return (int *)&enemy->ctx.floatVars[0];
    case -10006:
        if (type != NULL)
        {
            *type = 1;
        }
        return (int *)&enemy->ctx.floatVars[1];
    case -10007:
        if (type != NULL)
        {
            *type = 1;
        }
        return (int *)&enemy->ctx.floatVars[2];
    case -10008:
        if (type != NULL)
        {
            *type = 1;
        }
        return (int *)&enemy->ctx.floatVars[3];
    case -10009:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars2[0];
    case -10010:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars2[1];
    case -10011:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars2[2];
    case -10012:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->ctx.intVars2[3];
    case -10013:
        if (type != NULL)
        {
            *type = 2;
        }
        return &g_GameManager.difficulty;
    case -10014:
        if (type != NULL)
        {
            *type = 2;
        }
        return &g_GameManager.rank;
    case -10015:
        if (type != NULL)
        {
            *type = 1;
        }
        return (int *)&enemy->pos.x;
    case -10016:
        if (type != NULL)
        {
            *type = 1;
        }
        return (int *)&enemy->pos.y;
    case -10017:
        if (type != NULL)
        {
            *type = 1;
        }
        return (int *)&enemy->pos.z;
    case -10018:
        if (type != NULL)
        {
            *type = 2;
        }
        return (int *)&g_Player.position.x;
    case -10019:
        if (type != NULL)
        {
            *type = 2;
        }
        return (int *)&g_Player.position.y;
    case -10020:
        if (type != NULL)
        {
            *type = 2;
        }
        return (int *)&g_Player.position.z;
    case -10021:
        g_PlayerAngle = g_Player.AngleFrom(&enemy->pos);
        if (type != NULL)
        {
            *type = 2;
        }
        return (int *)&g_PlayerAngle;
    case -10022:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->callbackTimer.current;
    case -10023:
    {
        D3DXVECTOR3 diff = g_Player.position - enemy->pos;
        g_PlayerDistance = Vec3Length(&diff);
        if (type != NULL)
        {
            *type = 2;
        }
        return (int *)&g_PlayerDistance;
    }
    case -10024:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->life;
    case -10025:
        g_ShotTypeOperand = g_GameManager.character * 2 + g_GameManager.shotType;
        if (type != NULL)
        {
            *type = 0;
        }
        return &g_ShotTypeOperand;
    }
    return operand;
}

float *ResolveFloatOperand(Enemy *enemy, float *operand, int *type)
{
    int *ptr;
    int val;

    val = *operand;
    ptr = ResolveOperand(enemy, &val, type);
    if (ptr == &val)
    {
        return operand;
    }
    return (float *)ptr;
}

void ScriptSet(Enemy *enemy, int dst, int *src)
{
    int *dstPtr;
    int *rhs;
    int t;

    rhs = ResolveOperand(enemy, src, NULL);
    dstPtr = ResolveOperand(enemy, &dst, &t);
    if (t == 0)
    {
        *dstPtr = *rhs;
    }
    else if (t == 1)
    {
        *(float *)dstPtr = *(float *)rhs;
    }
}

#ifdef TH06_EXE
// Unused (dropped by /OPT:REF), exe only. Dead code still goes into
// /OPT:ICF's sort of the code sections, so with the others marked like
// it, it decides which of several identical D3DX8 functions the linker
// keeps: the original's choices. Its size and its number of references
// are what matter (4 to 19 references, any size).
//
// It also mentions four KERNEL32 functions that D3DX8 uses and the game
// doesn't: the linker then loads their import objects with the game's
// instead of after D3DX8's members, and that is what the original's
// import sort needs to put CreateFileA, LocalFree and IsBadReadPtr where
// it has them. Any four of D3DX8's dropped KERNEL32 imports would do (the
// resource and file mapping ones); these stand in.
int UnusedScriptKind(HMODULE module, char *name, DWORD *size)
{
    HRSRC info;
    HGLOBAL data;

    info = FindResourceA(module, name, RT_RCDATA);
    data = LoadResource(module, info);
    *size = SizeofResource(module, info);
    if (LockResource(data) == NULL)
    {
        return 0;
    }
    return g_GameManager.difficulty * 8 + g_GameManager.character * 2 + g_GameManager.shotType +
           g_GameManager.continuesUsed + g_GameManager.deaths + g_GameManager.bombsUsed +
           g_GameManager.graze + g_GameManager.spellCardsCaptured;
}
#endif // TH06_EXE

void ScriptAdd(Enemy *enemy, int dst, int *a, int *b)
{
    int *res;
    int *x;
    int *y;
    int t;

    res = ResolveOperand(enemy, &dst, &t);
    if (t == 0)
    {
        x = ResolveOperand(enemy, a, NULL);
        y = ResolveOperand(enemy, b, NULL);
        *res = *x + *y;
    }
    else if (t == 1)
    {
        x = (int *)ResolveFloatOperand(enemy, (float *)a, NULL);
        y = (int *)ResolveFloatOperand(enemy, (float *)b, NULL);
        *(float *)res = *(float *)x + *(float *)y;
    }
}

void ScriptSub(Enemy *enemy, int dst, int *a, int *b)
{
    int *res;
    int *x;
    int *y;
    int t;

    res = ResolveOperand(enemy, &dst, &t);
    if (t == 0)
    {
        x = ResolveOperand(enemy, a, NULL);
        y = ResolveOperand(enemy, b, NULL);
        *res = *x - *y;
    }
    else if (t == 1)
    {
        x = (int *)ResolveFloatOperand(enemy, (float *)a, NULL);
        y = (int *)ResolveFloatOperand(enemy, (float *)b, NULL);
        *(float *)res = *(float *)x - *(float *)y;
    }
}

void ScriptMul(Enemy *enemy, int dst, int *a, int *b)
{
    int *res;
    int *x;
    int *y;
    int t;

    x = ResolveOperand(enemy, a, NULL);
    y = ResolveOperand(enemy, b, NULL);
    res = ResolveOperand(enemy, &dst, &t);
    if (t == 0)
    {
        x = ResolveOperand(enemy, a, NULL);
        y = ResolveOperand(enemy, b, NULL);
        *res = *x * *y;
    }
    else if (t == 1)
    {
        x = (int *)ResolveFloatOperand(enemy, (float *)a, NULL);
        y = (int *)ResolveFloatOperand(enemy, (float *)b, NULL);
        *(float *)res = *(float *)x * *(float *)y;
    }
}

void ScriptDiv(Enemy *enemy, int dst, int *a, int *b)
{
    int *res;
    int *x;
    int *y;
    int t;

    res = ResolveOperand(enemy, &dst, &t);
    if (t == 0)
    {
        x = ResolveOperand(enemy, a, NULL);
        y = ResolveOperand(enemy, b, NULL);
        *res = *x / *y;
    }
    else if (t == 1)
    {
        x = (int *)ResolveFloatOperand(enemy, (float *)a, NULL);
        y = (int *)ResolveFloatOperand(enemy, (float *)b, NULL);
        *(float *)res = *(float *)x / *(float *)y;
    }
}

void ScriptMod(Enemy *enemy, int dst, int *a, int *b)
{
    int *res;
    int *x;
    int *y;
    int t;

    res = ResolveOperand(enemy, &dst, &t);
    if (t == 0)
    {
        x = ResolveOperand(enemy, a, NULL);
        y = ResolveOperand(enemy, b, NULL);
        *res = *x % *y;
    }
    else if (t == 1)
    {
        x = (int *)ResolveFloatOperand(enemy, (float *)a, NULL);
        y = (int *)ResolveFloatOperand(enemy, (float *)b, NULL);
        *(float *)res = fmodf(*(float *)x, *(float *)y);
    }
}

void ScriptAngle(Enemy *enemy, int dst, float *fromX, float *fromY, float *toX, float *toY)
{
    float *resPtr;
    int dstType;
    float *x1;
    float *y1;
    float *x2;
    float *by;

    resPtr = (float *)ResolveOperand(enemy, &dst, &dstType);
    if (dstType == 1)
    {
        x1 = ResolveFloatOperand(enemy, fromX, NULL);
        y1 = ResolveFloatOperand(enemy, fromY, NULL);
        x2 = ResolveFloatOperand(enemy, toX, NULL);
        by = ResolveFloatOperand(enemy, toY, NULL);
        *resPtr = atan2f(*by - *y1, *x2 - *x1);
    }
}

inline float Vec3Length(const D3DXVECTOR3 *v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}
