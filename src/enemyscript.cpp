// The file with EnemyScripts, from 0x407340.

#include "enemy.h"
#include "enemyscript.h"
#include "gamemanager.h"
#include "item.h"
#include "player.h"
#include "stage.h"
#include "unsorted.h"

int EnemyScripts::Load(char *path)
{
    int i;

    this->file = (EnemyScriptHeader *)LoadFileData(path, 0);
    if (this->file == NULL)
    {
        LogPrintf(&g_Unknown69d998, "敵データの読み込みに失敗しました、データが壊れてるか失われています\n");
        return -1;
    }
    this->file->unk_04 += (int)this->file;
    this->subs = (void **)((char *)this->file + sizeof(EnemyScriptHeader));
    for (i = 0; i < this->file->subCount; i++)
    {
        this->subs[i] = (char *)this->subs[i] + (int)this->file;
    }
    this->timeline = (EnemyTimelineEntry *)this->file->unk_04;
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
            value = g_Rng.GetRandomU32InRange(range);
            ScriptSet(enemy, instr->args.i[0], &value);
            break;
        }
        case 7:
        {
            DWORD range = *ResolveOperand(enemy, &params->i[1], NULL);
            int low = *ResolveOperand(enemy, &params->i[2], NULL);
            value = g_Rng.GetRandomU32InRange(range);
            value += low;
            ScriptSet(enemy, instr->args.i[0], &value);
            break;
        }
        case 8:
        {
            float range = *ResolveFloatOperand(enemy, &params->f[1], NULL);
            f = g_Rng.GetRandomF32InRange(range);
            ScriptSet(enemy, instr->args.i[0], (int *)&f);
            break;
        }
        case 9:
        {
            float range = *ResolveFloatOperand(enemy, &params->f[1], NULL);
            float low = *ResolveFloatOperand(enemy, &params->f[2], NULL);
            f = g_Rng.GetRandomF32InRange(range);
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
            if (!enemy->flags.unk18)
            {
                enemy->savedCtx[enemy->stackDepth] = enemy->ctx;
            }
            g_EnemyScripts.StartSub(&enemy->ctx, value);
            if (!enemy->flags.unk18 && enemy->stackDepth < 7)
            {
                enemy->stackDepth++;
            }
            enemy->ctx.intVars[0] = instr->args.i[1];
            enemy->ctx.floatVars[0] = instr->args.f[2];
            goto restart;
        case 36:
            if (enemy->flags.unk18)
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
            bs->count1 += RankLerp(enemy->unk_cdc, enemy->unk_cde);
            if (bs->count1 <= 0)
            {
                bs->count1 = 1;
            }
            bs->count2 = *ResolveOperand(enemy, &bulletArgs->count2, NULL);
            bs->count2 += RankLerp(enemy->unk_ce0, enemy->unk_ce2);
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
                bs->speed1 += RankLerpF(enemy->unk_cd4, enemy->unk_cd8);
                if (bs->speed1 < 0.3f)
                {
                    bs->speed1 = 0.3f;
                }
            }
            bs->angle2 = *ResolveFloatOperand(enemy, &bulletArgs->angle2, NULL);
            bs->speed2 = *ResolveFloatOperand(enemy, &bulletArgs->speed2, NULL);
            bs->speed2 += RankLerpF(enemy->unk_cd4, enemy->unk_cd8) / 2.0f;
            if (bs->speed2 < 0.3f)
            {
                bs->speed2 = 0.3f;
            }
            bs->unk_4a = 0;
            bs->flags = bulletArgs->flags;
            value = bulletArgs->color;
            bs->color = *ResolveOperand(enemy, &value, NULL);
            if (!enemy->flags.unk5)
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
                enemy->shootTimer.Set(g_Rng.GetRandomU32InRange(enemy->shootInterval));
            }
            break;
        case 78:
            enemy->flags.unk5 = 1;
            break;
        case 79:
            enemy->flags.unk5 = 0;
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
            shooter->unk_40 = laserArgs->i[11];
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
                enemy->flags.unk11 = 1;
                enemy->unk_e40 = instr->args.i[0];
            }
            else
            {
                g_Gui.bossPresent = 0;
                g_EnemyManager.bosses[enemy->unk_e40] = NULL;
                enemy->flags.unk11 = 0;
            }
            break;
        case 102:
        {
            EnemyScriptArgs *effectArgs = &instr->args;
            enemy->effects[enemy->effectCount] = g_EffectManager.SpawnEffect(0xd, &enemy->pos, 1, g_Unknown476438[effectArgs->i[0]]);
            enemy->effects[enemy->effectCount]->unk_140 = *(D3DXVECTOR3 *)&effectArgs->f[1];
            enemy->unk_ea4 = effectArgs->f[4];
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
            enemy->angle = g_Rng.GetRandomF32InRange(vec.y - vec.x) + vec.x;
            break;
        case 50:
            vec = *(D3DXVECTOR3 *)&instr->args;
            enemy->angle = g_Rng.GetRandomF32InRange(vec.y - vec.x) + vec.x;
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
            enemy->unk_e54 = instr->args.s[0];
            enemy->unk_e56 = instr->args.s[1];
            enemy->unk_e58 = instr->args.s[2];
            enemy->unk_e5a = instr->args.s[3];
            enemy->unk_e5c = instr->args.s[4];
            enemy->flags.spriteDirection = 0xff;
            break;
        case 103:
            enemy->hitboxSize.x = instr->args.f[0];
            enemy->hitboxSize.y = instr->args.f[1];
            enemy->hitboxSize.z = instr->args.f[2];
            break;
        case 104:
            enemy->flags.unk9 = instr->args.i[0];
            break;
        case 105:
            enemy->flags.unk12 = instr->args.i[0];
            break;
        case 106:
            g_Unknown6d3f50.QueueSound(instr->args.i[0], 0);
            break;
        case 107:
            enemy->flags.unk13 = instr->args.i[0];
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
            if (!enemy->flags.unk18)
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
            g_Gui.Func417bfd(instr->args.s[0], (char *)&instr->args.i[1]);
            g_EnemyManager.spellCardCapture = 1;
            g_EnemyManager.spellCardActive = 1;
            g_EnemyManager.spellCardIndex = instr->args.s[1];
            g_EnemyManager.spellCardBonus = g_SpellCardBonuses[g_EnemyManager.spellCardIndex];
            g_BulletManager.ClearAllToItems();
            g_Stage.unk_080 = 1;
            g_Stage.unk_084 = 0;
            enemy->unk_cd4 = -0.5f;
            enemy->unk_cd8 = 0.5f;
            enemy->unk_cdc = 0;
            enemy->unk_cde = 0;
            enemy->unk_ce0 = 0;
            enemy->unk_ce2 = 0;
            rec = &g_GameManager.catks[g_EnemyManager.spellCardIndex];
            sum = 0;
            if (g_GameManager.unk_001c == 0)
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
                    rec->unk_3e = 0;
                    rec->unk_3c = 0;
                    rec->nameChecksum = sum;
                }
                rec->bonus = g_EnemyManager.spellCardBonus;
                if (rec->unk_3c < 9999)
                {
                    rec->unk_3c++;
                }
            }
            break;
        }
        case 94:
            if (g_EnemyManager.spellCardActive != 0)
            {
                g_Gui.Func41732c();
                if (g_EnemyManager.spellCardActive == 1)
                {
                    int bonus = g_BulletManager.ClearAllWithBonus(0x3200, 1);
                    if (g_EnemyManager.spellCardCapture != 0)
                    {
                        Catk *rec = &g_GameManager.catks[g_EnemyManager.spellCardIndex];
                        int k;
                        int tenth = g_EnemyManager.spellCardBonus >= 500000 ? 50000 : g_EnemyManager.spellCardBonus / 10;
                        bonus = g_EnemyManager.spellCardBonus + g_EnemyManager.spellCardBonus * g_Gui.GetBossTimer() / 10;
                        g_Gui.Func417458(bonus);
                        g_GameManager.score += bonus;
                        if (g_GameManager.unk_001c == 0)
                        {
                            rec->unk_3e++;
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
            g_Stage.unk_080 = 0;
            break;
        case 112:
            enemy->unk_cf0.Set(instr->args.i[0]);
            break;
        case 113:
            enemy->lifeCallbackLife = instr->args.i[0];
            break;
        case 114:
            enemy->lifeCallbackSub = instr->args.i[0];
            break;
        case 115:
            enemy->timerCallbackTime = instr->args.i[0];
            enemy->unk_cf0.Set(0);
            break;
        case 116:
            enemy->timerCallbackSub = instr->args.i[0];
            break;
        case 117:
            enemy->flags.unk8 = instr->args.i[0];
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
                pos[0] += g_Rng.GetRandomF32InRange(144.0f) - 72.0f;
                pos[1] += g_Rng.GetRandomF32InRange(144.0f) - 72.0f;
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
            g_Stage.unk_2a8 = 1;
            break;
        case 126:
            g_Gui.SetUnk14(instr->args.i[0]);
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
                if (en->flags.unk11)
                {
                    continue;
                }
                en->life = 0;
                if (!en->flags.unk8 && en->deathSub >= 0)
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
            enemy->flags.unk18 = instr->args.i[0];
            break;
        case 131:
            enemy->unk_cd4 = params->f[0];
            enemy->unk_cd8 = params->f[1];
            enemy->unk_cdc = params->i[2];
            enemy->unk_cde = params->i[3];
            enemy->unk_ce0 = params->i[4];
            enemy->unk_ce2 = params->i[5];
            break;
        case 132:
            enemy->flags.hidden = instr->args.i[0];
            break;
        case 133:
            enemy->timerCallbackSub = enemy->deathSub;
            enemy->unk_cf0.Set(0);
            break;
        case 135:
            enemy->flags.unk20 = instr->args.i[0];
            break;
        }
    next:
        instr = (EnemyScriptInstr *)((char *)instr + instr->size);
    }
    switch (enemy->flags.moveMode)
    {
    case 1:
        enemy->angle = AddNormalizeAngle(enemy->angle, g_Unknown6c6d18.frameAdvance * enemy->angularVelocity);
        enemy->speed = g_Unknown6c6d18.frameAdvance * enemy->acceleration + enemy->speed;
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
        if (enemy->unk_e5a >= 0)
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
                        g_SpriteEngine->StartScript(&enemy->sprite, enemy->unk_e54 + 0x100);
                    }
                    else if (enemy->flags.spriteDirection == 1)
                    {
                        g_SpriteEngine->StartScript(&enemy->sprite, enemy->unk_e56 + 0x100);
                    }
                    else
                    {
                        g_SpriteEngine->StartScript(&enemy->sprite, enemy->unk_e58 + 0x100);
                    }
                    break;
                case 1:
                    g_SpriteEngine->StartScript(&enemy->sprite, enemy->unk_e5a + 0x100);
                    break;
                case 2:
                    g_SpriteEngine->StartScript(&enemy->sprite, enemy->unk_e5c + 0x100);
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

int *ResolveOperand(Enemy *enemy, int *operand, int *type)
{
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
        g_Unknown487e4c = g_Player.AngleFrom(&enemy->pos);
        if (type != NULL)
        {
            *type = 2;
        }
        return (int *)&g_Unknown487e4c;
    case -10022:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->unk_cf0.current;
    case -10023:
    {
        D3DXVECTOR3 diff = g_Player.position - enemy->pos;
        g_Unknown487e48 = D3DXVec3Length(&diff);
        if (type != NULL)
        {
            *type = 2;
        }
        return (int *)&g_Unknown487e48;
    }
    case -10024:
        if (type != NULL)
        {
            *type = 0;
        }
        return &enemy->life;
    case -10025:
        g_Unknown487e44 = g_GameManager.character * 2 + g_GameManager.shotType;
        if (type != NULL)
        {
            *type = 0;
        }
        return &g_Unknown487e44;
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

// g_EnemyScriptFuncs[0]: turns every flying bullet into color 15, then
// stops them (instr's second argument 0) or sends them drifting off in
// random directions (1).
void Func40b8e0(Enemy *enemy, EnemyScriptInstr *instr)
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
        g_SpriteEngine->SetSprite(&bullet->sprites.sprite0, bullet->sprites.sprite0.unk_0b2 + bullet->color);
        switch (choice)
        {
        case 0:
            bullet->speed = 0.0f;
            bullet->velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            break;
        case 1:
            bullet->flags |= 0x10;
            bullet->unk_5a8 = 220;
            bullet->timer.Set(0);
            SinCosScale(&bullet->accel, g_Rng.GetRandomF32InRange(6.2831855f) - 3.1415927f, 0.01f);
            break;
        }
    }
}

// g_EnemyScriptFuncs[1]: fires the enemy's shooter from a random point
// in a box around it, instr's second argument wide and three quarters
// as high.
void Func40ba80(Enemy *enemy, EnemyScriptInstr *instr)
{
    float range = instr->args.i[1];

    enemy->shooter.pos = enemy->pos + enemy->shootOffset;
    enemy->shooter.pos.x = g_Rng.GetRandomF32InRange(range) + enemy->pos.x - range / 2.0f;
    range *= 0.75f;
    enemy->shooter.pos.y = g_Rng.GetRandomF32InRange(range) + enemy->pos.y - range / 2.0f;
    g_BulletManager.FirePattern(&enemy->shooter);
}

// g_EnemyScriptFuncs[2], set as the context's func: every sixth frame
// until the count in intVars[2] reaches intVars[3], fires five patterns
// from points on a star (corners g_Unknown487edc, radius floatVars[3])
// whose center moves from the enemy towards where the player was.
void Func40bb80(Enemy *enemy, EnemyScriptInstr *instr)
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
        g_Unknown487f5c = enemy->pos;
        g_Unknown487f68 = g_Player.position;
        g_Unknown487edc[0] = g_Rng.GetRandomF32InRange(6.2831855f) - 3.1415927f;
        g_Unknown487edc[1] = AddNormalizeAngle(g_Unknown487edc[0], 2.5132742f);
    }
    if (enemy->ctx.intVars[2] % 30 == 0)
    {
        g_Unknown487edc[0] = g_Unknown487edc[1];
        g_Unknown487edc[1] = AddNormalizeAngle(g_Unknown487edc[0], 2.5132742f);
        g_Unknown487edc[2] = AddNormalizeAngle(g_Unknown487edc[1], 2.5132742f);
        g_Unknown487edc[3] = AddNormalizeAngle(g_Unknown487edc[2], 2.5132742f);
        g_Unknown487edc[4] = AddNormalizeAngle(g_Unknown487edc[3], 2.5132742f);
        g_Unknown487edc[5] = AddNormalizeAngle(g_Unknown487edc[4], 2.5132742f);
    }
    if (enemy->ctx.intVars[2] % 6 == 0)
    {
        progress2 = (float)enemy->ctx.intVars[2] / (float)enemy->ctx.intVars[3];
        t = progress2 * 0.1f;
        center2 = (g_Unknown487f68 - g_Unknown487f5c) * t + g_Unknown487f5c;
        center2.z = 0.0f;
        progress2 += 0.5f;
        enemy->shooter.angle1 = 1.0471976f * progress2;
        for (i = 0; i < 5; i++)
        {
            t = (float)(enemy->ctx.intVars[2] % 30) / 30.0f;
            SinCosScale(&from, g_Unknown487edc[i], enemy->ctx.floatVars[3]);
            SinCosScale(&to3, g_Unknown487edc[i + 1], enemy->ctx.floatVars[3]);
            from = (to3 - from) * t + from;
            from.z = 0.0f;
            enemy->shooter.pos = center2 + from;
            speed = enemy->shooter.speed1;
            enemy->shooter.speed1 += g_Rng.GetRandomF32InRange(enemy->shooter.speed2);
            g_BulletManager.FirePattern(&enemy->shooter);
            enemy->shooter.speed1 = speed;
            enemy->shooter.angle1 -= 0.5235988f * progress2;
        }
        g_Unknown6d3f50.QueueSound(0x16, 0);
    }
    enemy->ctx.intVars[2]++;
}

// g_EnemyScriptFuncs[3]: sets intVars[1] to [3] to the player's shot
// type's values from g_Unknown476264.
void Func40c100(Enemy *enemy, EnemyScriptInstr *instr)
{
    enemy->ctx.intVars[1] = g_Unknown476264[g_GameManager.character][g_GameManager.shotType][0];
    enemy->ctx.intVars[2] = g_Unknown476264[g_GameManager.character][g_GameManager.shotType][1];
    enemy->ctx.intVars[3] = g_Unknown476264[g_GameManager.character][g_GameManager.shotType][2];
}

// The angle from the player to `pos`, turned by a quarter turn and by
// `offset`.
inline float AngleAwayFromPlayer(D3DXVECTOR3 *pos, float offset)
{
    return g_Player.AngleTo(pos) + 1.5707964f + offset;
}

// g_EnemyScriptFuncs[4]: with instr's second argument below 2, sets
// g_GameManager.unk_002c to it. Otherwise turns some large bullets (14,
// or 52 above normal difficulty; one in four at random) into color 5 and
// sends them off: away from the player when close, else downwards (or
// anywhere above normal difficulty).
void Func40c180(Enemy *enemy, EnemyScriptInstr *instr)
{
    Bullet *bullet;
    int i;
    int count;
    float dx2;
    float dy;

    if (instr->args.i[1] < 2)
    {
        g_EffectManager.SpawnEffect(0xc, &enemy->pos, 1, 0xffffffff);
        g_GameManager.unk_002c = instr->args.i[1];
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
                    bullet->color != 5 && g_Rng.GetRandomU16() % 4 == 0)
                {
                    bullet->color = 5;
                    g_SpriteEngine->SetSprite(&bullet->sprites.sprite0, bullet->sprites.sprite0.unk_0b2 + bullet->color);
                    dx2 = bullet->pos.x - g_Player.position.x;
                    dy = bullet->pos.y - g_Player.position.y;
                    if ((float)sqrt(dy * dy + dx2 * dx2) > 128.0f)
                    {
                        bullet->angle = g_Rng.GetRandomF32InRange(2.3561945f) + 0.7853982f;
                    }
                    else
                    {
                        bullet->angle = AngleAwayFromPlayer(&bullet->pos, g_Rng.GetRandomF32InRange(6.2831855f));
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
                    bullet->color != 5 && g_Rng.GetRandomU16() % 4 == 0)
                {
                    bullet->color = 5;
                    g_SpriteEngine->SetSprite(&bullet->sprites.sprite0, bullet->sprites.sprite0.unk_0b2 + bullet->color);
                    dx2 = bullet->pos.x - g_Player.position.x;
                    dy = bullet->pos.y - g_Player.position.y;
                    if ((float)sqrt(dy * dy + dx2 * dx2) > 128.0f)
                    {
                        bullet->angle = g_Rng.GetRandomF32InRange(6.2831855f);
                    }
                    else
                    {
                        bullet->angle = AngleAwayFromPlayer(&bullet->pos, g_Rng.GetRandomF32InRange(6.2831855f));
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
void Func40c5b0(Enemy *enemy, EnemyScriptInstr *instr)
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
        g_Unknown6d3f50.QueueSound(7, 0);
    }
    enemy->ctx.intVars[2]++;
}

// g_EnemyScriptFuncs[6], set as the context's func: while the enemy is
// shown, spawns pairs of sparks (effect 0x13) around it on a turning
// circle, more often as unk_ebc counts up; frees its effects once
// hidden.
void Func40caf0(Enemy *enemy, EnemyScriptInstr *instr)
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
    enemy->unk_eb8 += 0.017453292f;
    if (enemy->unk_eb8 >= 0.7853982f)
    {
        enemy->unk_eb8 -= 1.5707964f;
    }
    if (enemy->unk_ebc.HasTicked())
    {
        if (enemy->unk_ebc > 120 || (enemy->unk_ebc > 60 && enemy->unk_ebc.current % 2 == 0) ||
            (enemy->unk_ebc > 30 && enemy->unk_ebc.current % 4 == 0) || enemy->unk_ebc.current % 8 == 0)
        {
            k = enemy->unk_ebc.current % 16;
            k = g_Rng.GetRandomU16InRange(k / 2) + k / 2;
            r = k * 160.0f / 16.0f + 32.0f;
            a2 = enemy->unk_eb8 - k * 3.1415927f / 40.0f;
            if (r < -0.7853982f)
            {
                r += 1.5707964f;
            }
            pos = enemy->pos;
            pos.x += cosf(a2) * r;
            pos.y += sinf(a2) * r;
            effect = g_EffectManager.SpawnEffect(0x13, &pos, 1, 0xff3030ff);
            effect->unk_11c.x = (g_Rng.GetRandomF32ZeroToOne() * 40.0f - 20.0f) / 60.0f;
            effect->unk_11c.y = 8.0f * k / 60.0f - 0.26666668f;
            effect->unk_11c.z = 0.0f;
            effect->unk_128 = -effect->unk_11c / 120.0f;
            pos = enemy->pos;
            pos.x -= cosf(a2) * r;
            pos.y += sinf(a2) * r;
            effect = g_EffectManager.SpawnEffect(0x13, &pos, 1, 0xff3030ff);
            effect->unk_11c.x = (g_Rng.GetRandomF32ZeroToOne() * 40.0f - 20.0f) / 60.0f;
            effect->unk_11c.y = 8.0f * k / 60.0f - 0.26666668f;
            effect->unk_11c.z = 0.0f;
            effect->unk_128 = -effect->unk_11c / 120.0f;
        }
    }
    enemy->unk_ebc.Tick();
}

// g_EnemyScriptFuncs[7]: from two rings of eight points around the enemy
// (turning opposite ways), fires lasers (instr's second argument 0) or
// the enemy's shooter outwards, three times.
void Func40d040(Enemy *enemy, EnemyScriptInstr *instr)
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
    origin2 = g_Rng.GetRandomF32InRange(6.2831855f);
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
                    shooter.unk_40 = 16;
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
void Func40d400(Enemy *enemy, EnemyScriptInstr *instr)
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
            shooter.angle1 = g_Rng.GetRandomF32InRange(6.2831855f) - 3.1415927f;
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
void Func40d530(Enemy *enemy, EnemyScriptInstr *instr)
{
    float base2;
    Bullet *bullet2 = g_BulletManager.bullets;
    BulletShooter shooter;
    int i;
    float dist;

    base2 = g_Rng.GetRandomF32InRange(6.2831855f) - 3.1415927f;
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
            g_SpriteEngine->SetSprite(&bullet2->sprites.sprite0, bullet2->sprites.sprite0.unk_0b2 + bullet2->color);
            bullet2->speed = 0.01f;
            bullet2->timer.Set(0);
            bullet2->unk_5a8 = 120;
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
