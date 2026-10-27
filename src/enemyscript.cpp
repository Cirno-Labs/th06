// The file with EnemyScripts, from 0x407340.

#include "enemy.h"
#include "enemyscript.h"
#include "gamemanager.h"
#include "player.h"
#include "unsorted.h"

int EnemyScripts::Load(char *path)
{
    int i;

    this->file = (EnemyScriptHeader *)Func41e290(path, 0);
    if (this->file == NULL)
    {
        Func41e4d0(&g_Unknown69d998, "敵データの読み込みに失敗しました、データが壊れてるか失われています\n");
        return -1;
    }
    this->file->unk_04 += (int)this->file;
    this->subs = (void **)((char *)this->file + sizeof(EnemyScriptHeader));
    for (i = 0; i < this->file->subCount; i++)
    {
        this->subs[i] = (char *)this->subs[i] + (int)this->file;
    }
    this->unk_08 = (void *)this->file->unk_04;
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
        return &enemy->unk_ce4;
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
