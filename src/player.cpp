// The file with the player's shots.

#include "bullet.h"
#include "effect.h"
#include "enemy.h"
#include "gamemanager.h"
#include "player.h"
#include "unsorted.h"

Player::Player()
{
}

int FireShotType0(Player *player, PlayerBullet *bullet, int index, int frame)
{
    return FireShot(player, bullet, index, frame, &g_ShotTable0);
}

int FireShot(Player *player, PlayerBullet *bullet, int index, int frame, ShotTable *table)
{
    ShotData *sd;
    int laser;
    D3DXVECTOR3 unused;

    while (g_GameManager.power >= table->maxPower)
    {
        table++;
    }
    sd = &table->shots[index];
    if (sd->type == 3)
    {
        laser = sd->phase;
        if (player->laserTimers[laser].AsFrames() == 0)
        {
            player->laserTimers[laser].Set(sd->interval);
            bullet->laserIndex = laser;
            bullet->option = sd->option;
            bullet->velocity.z = sd->offsetX;
            bullet->unk_134 = sd->offsetY;
            goto fire;
        }
    }
    else if (frame % sd->interval == sd->phase)
    {
    fire:
        g_SpriteEngine->StartScript(&bullet->sprite, sd->script);
        if (sd->option == 0)
        {
            bullet->pos = player->position;
        }
        else
        {
            bullet->pos = player->optionPos[sd->option - 1];
        }
        bullet->pos[0] += sd->offsetX;
        bullet->pos[1] += sd->offsetY;
        bullet->pos.z = 0.495f;
        bullet->size.x = sd->sizeX;
        bullet->size.y = sd->sizeY;
        bullet->size.z = 1.0f;
        bullet->angle = sd->angle;
        bullet->speed = sd->speed;
        bullet->velocity.x = cosf(sd->angle) * sd->speed;
        bullet->velocity.y = sinf(sd->angle) * sd->speed;
        bullet->timer.Set(0);
        bullet->type = sd->type;
        bullet->damage = sd->damage;
        if (sd->sound >= 0)
        {
            g_Unknown6d3f50.Func4311e0(sd->sound, 0);
        }
        return index >= table->count - 1;
    }
    if (index >= table->count - 1)
    {
        return -2;
    }
    return -1;
}

int FireShotType1(Player *player, PlayerBullet *bullet, int index, int frame)
{
    return FireShot(player, bullet, index, frame, &g_ShotTable1);
}

int FireShotType2(Player *player, PlayerBullet *bullet, int index, int frame)
{
    return FireShot(player, bullet, index, frame, &g_ShotTable2);
}

int FireShotType3(Player *player, PlayerBullet *bullet, int index, int frame)
{
    return FireShot(player, bullet, index, frame, &g_ShotTable3);
}

int Player::DeletedCallback(Player *player)
{
    if ((g_Unknown6c6d18.scene != 3) != 0)
    {
        g_SpriteEngine->ReleaseAnm(5);
    }
    return 0;
}

void Player::RemoveChain()
{
    g_Chain.Remove(g_Player.calcChain);
    g_Player.calcChain = NULL;
    g_Chain.Remove(g_Player.drawChain);
    g_Player.drawChain = NULL;
    g_Chain.Remove(g_Player.drawChain2);
    g_Player.drawChain2 = NULL;
}

// The corners of a `size` box centred on `pos`.
inline void GetBox(D3DXVECTOR3 *pos, D3DXVECTOR3 *size, D3DXVECTOR3 &min, D3DXVECTOR3 &max)
{
    min.x = pos->x - size->x / 2.0f;
    min.y = pos->y - size->y / 2.0f;
    max.x = size->x / 2.0f + pos->x;
    max.y = size->y / 2.0f + pos->y;
}

int Player::CalcDamage(D3DXVECTOR3 *pos, D3DXVECTOR3 *size, int *hitByBomb)
{
    PlayerBullet *cur;
    int idx;
    // The enemy's box, and the box of the cur or bomb part tested.
    D3DXVECTOR3 eMin;
    D3DXVECTOR3 eMax;
    D3DXVECTOR3 targetLo;
    D3DXVECTOR3 targetHi;
    int damage;

    damage = 0;
    GetBox(pos, size, eMin, eMax);
    cur = this->bullets;
    if (hitByBomb != NULL)
    {
        *hitByBomb = 0;
    }
    for (idx = 0; idx < 0x50; idx++, cur++)
    {
        if (cur->state == 0 || (cur->state != 1 && cur->type != 2))
        {
            continue;
        }
        GetBox(&cur->pos, &cur->size, targetLo, targetHi);
        if (targetLo.y > eMax.y || targetLo.x > eMax.x || targetHi.y < eMin.y || targetHi.x < eMin.x)
        {
            continue;
        }
        if (!this->bomb.running)
        {
            damage += cur->damage;
        }
        else
        {
            damage += (cur->damage / 3 != 0) ? cur->damage / 3 : 1;
        }
        if (cur->type == 2)
        {
            cur->damage = cur->damage / 4;
            if (cur->damage == 0)
            {
                cur->damage = 1;
            }
            switch (cur->sprite.scriptIndex)
            {
            case 0x441:
                cur->size.x = 32.0f;
                cur->size.y = 32.0f;
                break;
            case 0x442:
                cur->size.x = 42.0f;
                cur->size.y = 42.0f;
                break;
            case 0x443:
                cur->size.x = 48.0f;
                cur->size.y = 48.0f;
                break;
            case 0x444:
                cur->size.x = 48.0f;
                cur->size.y = 48.0f;
                break;
            }
            if (cur->timer.AsFrames() % 6 == 0)
            {
                g_EffectManager.Func40ef50(5, &cur->pos, 1, 0xffffffff);
            }
        }
        if (cur->type != 3)
        {
            if (cur->state == 1)
            {
                g_SpriteEngine->StartScript(&cur->sprite, cur->sprite.scriptIndex + 0x20);
                g_EffectManager.Func40ef50(5, &cur->pos, 1, 0xffffffff);
                cur->pos.z = 0.1f;
            }
            cur->state = 2;
            cur->velocity.x /= 8.0f;
            cur->velocity.y /= 8.0f;
        }
        else
        {
            this->hitCounter++;
            if (this->hitCounter % 8 == 0)
            {
                targetLo = *pos;
                targetLo.x = cur->pos.x;
                g_EffectManager.Func40ef50(5, &targetLo, 1, 0xffffffff);
            }
        }
    }
    for (idx = 0; idx < 32; idx++)
    {
        if (this->bombHitSize[idx].x <= 0.0f)
        {
            continue;
        }
        targetLo = this->bombHitPos[idx] - this->bombHitSize[idx] / 2.0f;
        targetHi = this->bombHitSize[idx] / 2.0f + this->bombHitPos[idx];
        if (targetLo.x > eMax.x || targetHi.x < eMin.x || targetLo.y > eMax.y || targetHi.y < eMin.y)
        {
            continue;
        }
        damage += this->bombHitDamage[idx];
        this->bombHitCount[idx] += this->bombHitDamage[idx];
        this->hitCounter++;
        if (this->hitCounter % 4 == 0)
        {
            g_EffectManager.Func40ef50(3, pos, 1, 0xffffffff);
        }
        if (this->bomb.running && hitByBomb != NULL)
        {
            *hitByBomb = 1;
        }
    }
    return damage;
}

int Player::CheckHit(D3DXVECTOR3 *pos, D3DXVECTOR3 *size)
{
    D3DXVECTOR3 areaMin;
    int idx;
    D3DXVECTOR3 start;
    D3DXVECTOR3 end;
    float (*area)[4];
    D3DXVECTOR3 areaMax;

    area = this->bulletClearAreas;
    start.x = pos->x - size->x / 2.0f;
    start.y = pos->y - size->y / 2.0f;
    end.x = size->x / 2.0f + pos->x;
    end.y = size->y / 2.0f + pos->y;
    for (idx = 0; idx < 16; idx++, area++)
    {
        if ((*area)[2] == 0.0f)
        {
            continue;
        }
        areaMin.x = (*area)[0] - (*area)[2] / 2.0f;
        areaMin.y = (*area)[1] - (*area)[3] / 2.0f;
        areaMax.x = (*area)[2] / 2.0f + (*area)[0];
        areaMax.y = (*area)[3] / 2.0f + (*area)[1];
        if (!(areaMin.x > end.x || areaMax.x < start.x || areaMin.y > end.y || areaMax.y < start.y))
        {
            return 2;
        }
    }
    if (this->hitboxMin.x > end.x || this->hitboxMin.y > end.y || this->hitboxMax.x < start.x ||
        this->hitboxMax.y < start.y)
    {
        return 0;
    }
    if (this->unk_09e0 != 0)
    {
        return 1;
    }
    this->Die();
    return 1;
}

int Player::CheckGraze(D3DXVECTOR3 *pos, D3DXVECTOR3 *size)
{
    D3DXVECTOR3 areaMin;
    int idx;
    D3DXVECTOR3 start;
    D3DXVECTOR3 end;
    float (*area)[4];
    D3DXVECTOR3 areaMax;

    start.x = pos->x - size->x / 2.0f - 20.0f;
    start.y = pos->y - size->y / 2.0f - 20.0f;
    end.x = size->x / 2.0f + pos->x + 20.0f;
    end.y = size->y / 2.0f + pos->y + 20.0f;
    area = this->bulletClearAreas;
    for (idx = 0; idx < 16; idx++, area++)
    {
        if ((*area)[2] == 0.0f)
        {
            continue;
        }
        areaMin.x = (*area)[0] - (*area)[2] / 2.0f;
        areaMin.y = (*area)[1] - (*area)[3] / 2.0f;
        areaMax.x = (*area)[2] / 2.0f + (*area)[0];
        areaMax.y = (*area)[3] / 2.0f + (*area)[1];
        if (!(areaMin.x > end.x || areaMax.x < start.x || areaMin.y > end.y || areaMax.y < start.y))
        {
            return 2;
        }
    }
    if (this->unk_09e0 == 2 || this->unk_09e0 == 1)
    {
        return 0;
    }
    if (this->hitboxMin.x > end.x || this->hitboxMax.x < start.x || this->hitboxMin.y > end.y ||
        this->hitboxMax.y < start.y)
    {
        return 0;
    }
    this->Graze(pos);
    return 1;
}

int Player::TouchesGrazeBox(D3DXVECTOR3 *pos, D3DXVECTOR3 *size)
{
    D3DXVECTOR3 start;
    D3DXVECTOR3 end;

    if (this->unk_09e0 != 0 && this->unk_09e0 != 3)
    {
        return 0;
    }
    start = *pos - *size / 2.0f;
    end = *size / 2.0f + *pos;
    if (this->grazeMin.x > end.x || this->grazeMax.x < start.x || this->grazeMin.y > end.y ||
        this->grazeMax.y < start.y)
    {
        return 0;
    }
    return 1;
}

int Player::CheckLaserHit(D3DXVECTOR3 *pos, D3DXVECTOR3 *size, D3DXVECTOR3 *origin, float angle, int canGraze)
{
    D3DXVECTOR3 boxMin;
    D3DXVECTOR3 start;
    D3DXVECTOR3 end;
    D3DXVECTOR3 boxMax;

    // The hitbox, turned into the laser's frame.
    start = this->position - *origin;
    Rotate2D(&end, &start, angle);
    end.z = 0.0f;
    start = end + *origin;
    boxMin = start - this->hitboxSize;
    boxMax = start + this->hitboxSize;
    start = *pos - *size / 2.0f;
    end = *size / 2.0f + *pos;
    if (!(boxMin.x > end.x || boxMax.x < start.x || boxMin.y > end.y || boxMax.y < start.y))
    {
        goto hit;
    }
    if (!canGraze)
    {
        return 0;
    }
    start.x -= 48.0f;
    start.y -= 48.0f;
    end.x += 48.0f;
    end.y += 48.0f;
    if (boxMin.x > end.x || boxMax.x < start.x || boxMin.y > end.y || boxMax.y < start.y)
    {
        return 0;
    }
    if (this->unk_09e0 == 2 || this->unk_09e0 == 1)
    {
        return 0;
    }
    this->Graze(&this->position);
    return 2;

hit:
    if (this->unk_09e0 != 0)
    {
        return 0;
    }
    this->Die();
    return 1;
}

void Player::Graze(D3DXVECTOR3 *pos)
{
    D3DXVECTOR3 mid;

    if (!g_Player.bomb.running)
    {
        if (g_GameManager.graze < 9999)
        {
            g_GameManager.graze++;
        }
        if (g_GameManager.grazeTotal < 999999)
        {
            g_GameManager.grazeTotal++;
        }
    }
    mid = (this->position + *pos) / 2.0f;
    g_EffectManager.Func40ef50(8, &mid, 1, 0xffffffff);
    g_GameManager.AddScore(500);
    g_GameManager.AddSubRank(6);
    g_Gui.flags.grazeChanged = 2;
    g_Unknown6d3f50.Func4311e0(0x1e, 0);
}

void Player::Die()
{
    int i;

    g_EnemyManager.unk_ee5c4 = 0;
    g_EffectManager.Func40ef50(0xc, &this->position, 1, 0xff4040ff);
    g_EffectManager.Func40ef50(6, &this->position, 16, 0xffffffff);
    this->unk_09e0 = 2;
    this->unk_75b4.Set(0);
    g_Unknown6d3f50.Func4311e0(4, 0);
    g_GameManager.deaths++;
    for (i = 0; i < 2; i++)
    {
        this->laserTimers[i].Set(2);
    }
}

int Player::HandleInput()
{
    int previous;
    float deltaY;
    float deltaX;
    float sideY;
    float sideX;
    float t;

    deltaX = 0.0f;
    deltaY = 0.0f;
    previous = this->direction;
    this->direction = 0;
    if (g_Buttons & 0x10)
    {
        this->direction = 1;
        if (g_Buttons & 0x40)
        {
            this->direction = 5;
        }
        if (g_Buttons & 0x80)
        {
            this->direction = 6;
        }
    }
    else if (g_Buttons & 0x20)
    {
        this->direction = 2;
        if (g_Buttons & 0x40)
        {
            this->direction = 7;
        }
        if (g_Buttons & 0x80)
        {
            this->direction = 8;
        }
    }
    else
    {
        if (g_Buttons & 0x40)
        {
            this->direction = 3;
        }
        if (g_Buttons & 0x80)
        {
            this->direction = 4;
        }
    }
    if (g_Buttons & 4)
    {
        this->focused = 1;
    }
    else
    {
        this->focused = 0;
    }
    switch (this->direction)
    {
    case 4:
        if (g_Buttons & 4)
        {
            deltaX = this->shotTypeInfo.speedFocused;
        }
        else
        {
            deltaX = this->shotTypeInfo.speed;
        }
        break;
    case 3:
        if (g_Buttons & 4)
        {
            deltaX = -this->shotTypeInfo.speedFocused;
        }
        else
        {
            deltaX = -this->shotTypeInfo.speed;
        }
        break;
    case 1:
        if (g_Buttons & 4)
        {
            deltaY = -this->shotTypeInfo.speedFocused;
        }
        else
        {
            deltaY = -this->shotTypeInfo.speed;
        }
        break;
    case 2:
        if (g_Buttons & 4)
        {
            deltaY = this->shotTypeInfo.speedFocused;
        }
        else
        {
            deltaY = this->shotTypeInfo.speed;
        }
        break;
    case 5:
        if (g_Buttons & 4)
        {
            deltaX = -this->shotTypeInfo.speedDiagonalFocused;
        }
        else
        {
            deltaX = -this->shotTypeInfo.speedDiagonal;
        }
        deltaY = deltaX;
        break;
    case 7:
        if (g_Buttons & 4)
        {
            deltaX = -this->shotTypeInfo.speedDiagonalFocused;
        }
        else
        {
            deltaX = -this->shotTypeInfo.speedDiagonal;
        }
        deltaY = -deltaX;
        break;
    case 6:
        if (g_Buttons & 4)
        {
            deltaX = this->shotTypeInfo.speedDiagonalFocused;
        }
        else
        {
            deltaX = this->shotTypeInfo.speedDiagonal;
        }
        deltaY = -deltaX;
        break;
    case 8:
        if (g_Buttons & 4)
        {
            deltaX = this->shotTypeInfo.speedDiagonalFocused;
        }
        else
        {
            deltaX = this->shotTypeInfo.speedDiagonal;
        }
        deltaY = deltaX;
        break;
    }
    if (deltaX < 0.0f && this->velocityX >= 0.0f)
    {
        g_SpriteEngine->StartScript(&this->sprite, 0x401);
    }
    else if (deltaX == 0.0f && this->velocityX < 0.0f)
    {
        g_SpriteEngine->StartScript(&this->sprite, 0x402);
    }
    if (deltaX > 0.0f && this->velocityX <= 0.0f)
    {
        g_SpriteEngine->StartScript(&this->sprite, 0x403);
    }
    else if (deltaX == 0.0f && this->velocityX > 0.0f)
    {
        g_SpriteEngine->StartScript(&this->sprite, 0x404);
    }
    this->velocityX = deltaX;
    this->velocityY = deltaY;
    this->position[0] += deltaX * this->moveScaleX * g_Unknown6c6d18.frameAdvance;
    this->position[1] += deltaY * this->moveScaleY * g_Unknown6c6d18.frameAdvance;
    if (this->position.x < g_GameManager.playerArea[0])
    {
        this->position.x = g_GameManager.playerArea[0];
    }
    else if (g_GameManager.playerArea[0] + g_GameManager.playerArea[2] < this->position.x)
    {
        this->position.x = g_GameManager.playerArea[0] + g_GameManager.playerArea[2];
    }
    if (this->position.y < g_GameManager.playerArea[1])
    {
        this->position.y = g_GameManager.playerArea[1];
    }
    else if (g_GameManager.playerArea[1] + g_GameManager.playerArea[3] < this->position.y)
    {
        this->position.y = g_GameManager.playerArea[1] + g_GameManager.playerArea[3];
    }
    this->hitboxMin = this->position - this->hitboxSize;
    this->hitboxMax = this->position + this->hitboxSize;
    this->grazeMin = this->position - this->grazeSize;
    this->grazeMax = this->position + this->grazeSize;
    this->optionPos[0] = this->position;
    this->optionPos[1] = this->position;
    sideX = sideY = 0.0f;
    if (g_GameManager.power < 8)
    {
        this->optionState = 0;
    }
    else if (this->optionState == 0)
    {
        this->optionState = 1;
    }
    switch (this->optionState)
    {
    case 0:
        this->optionTimer.Set(0);
        break;
    case 1:
        sideX = 24.0f;
        this->optionTimer.Set(0);
        if (this->focused)
        {
            this->optionState = 2;
        }
        else
        {
            break;
        }
    case 2:
    slideIn:
        this->optionTimer.Tick();
        t = this->optionTimer.AsFloat() / 8.0f;
        sideY = (1.0f - t) * 32.0f + -32.0f;
        t = t * t;
        sideX = -16.0f * t + 24.0f;
        if (this->optionTimer >= 8)
        {
            this->optionState = 3;
        }
        if (!this->focused)
        {
            this->optionState = 4;
            this->optionTimer.Set(8 - this->optionTimer.AsFrames());
            goto slideOut;
        }
        break;
    case 3:
        sideX = 8.0f;
        sideY = -32.0f;
        this->optionTimer.Set(0);
        if (!this->focused)
        {
            this->optionState = 4;
        }
        else
        {
            break;
        }
    case 4:
    slideOut:
        this->optionTimer.Tick();
        t = this->optionTimer.AsFloat() / 8.0f;
        sideY = 32.0f * t + -32.0f;
        t = t * t;
        t = 1.0f - t;
        sideX = -16.0f * t + 24.0f;
        if (this->optionTimer >= 8)
        {
            this->optionState = 1;
        }
        if (this->focused)
        {
            this->optionState = 2;
            this->optionTimer.Set(8 - this->optionTimer.AsFrames());
            goto slideIn;
        }
        break;
    }
    this->optionPos[0].x -= sideX;
    this->optionPos[1].x += sideX;
    this->optionPos[0].y += sideY;
    this->optionPos[1].y += sideY;
    if ((g_Buttons & 1) && !g_Gui.Func4195a2())
    {
        Player::StartFiring(this);
    }
    this->prevButtons = g_Buttons;
    return 0;
}

void Player::StartFiring(Player *player)
{
    if (player->fireTimer.AsFrames() < 0)
    {
        player->fireTimer.Set(0);
    }
}

float Player::AngleTo(D3DXVECTOR3 *pos)
{
    float x;
    float y;

    x = pos->x - this->position.x;
    y = pos->y - this->position.y;
    if (y == 0.0f && x == 0.0f)
    {
        return 1.5707964f;
    }
    return atan2f(y, x);
}

float Player::AngleFrom(D3DXVECTOR3 *pos)
{
    float x;
    float y;

    x = this->position.x - pos->x;
    y = this->position.y - pos->y;
    if (y == 0.0f && x == 0.0f)
    {
        return 1.5707964f;
    }
    return atan2f(y, x);
}

int Player::RegisterChain(BYTE unk)
{
    Player *player = &g_Player;

    memset(player, 0, sizeof(Player));
    player->unk_75b4.Set(0);
    player->unk_09e1 = unk;
    player->calcChain = g_Chain.CreateElem((ChainCallback)Player::OnUpdate);
    player->drawChain = g_Chain.CreateElem((ChainCallback)Player::OnDraw);
    player->drawChain2 = g_Chain.CreateElem((ChainCallback)Player::OnDraw2);
    player->calcChain->arg = player;
    player->drawChain->arg = player;
    player->drawChain2->arg = player;
    player->calcChain->addedCallback = (ChainCallback)Player::AddedCallback;
    player->calcChain->deletedCallback = (ChainCallback)Player::DeletedCallback;
    if (g_Chain.AddToCalcChain(player->calcChain, 7) != 0)
    {
        return -1;
    }
    g_Chain.AddToDrawChain(player->drawChain, 5);
    g_Chain.AddToDrawChain(player->drawChain2, 7);
    return 0;
}

int Player::OnUpdate(Player *player)
{
    int i;
    float progress;
    float progress2;

    if (g_GameManager.unk_002c)
    {
        return 1;
    }
    for (i = 0; i < 32; i++)
    {
        player->bombHitSize[i].x = 0.0f;
    }
    for (i = 0; i < 16; i++)
    {
        player->bulletClearAreas[i][2] = 0.0f;
    }
    if (player->bomb.running != 0)
    {
        player->bomb.update(player);
    }
    else if (!g_Gui.Func4195a2() && player->unk_09d8 != 0 && g_GameManager.bombs > 0 && WAS_PRESSED(2) &&
             player->bomb.update != NULL)
    {
        g_GameManager.bombsUsed++;
        g_GameManager.bombs--;
        g_Gui.flags.bombsChanged = 2;
        player->bomb.running = 1;
        player->bomb.timer.Set(0);
        player->bomb.duration = 999;
        player->bomb.update(player);
        g_EnemyManager.unk_ee5c4 = 0;
        g_GameManager.SubSubRank(200);
        g_EnemyManager.unk_ee5d4 = g_EnemyManager.unk_ee5c8;
    }
    if (player->unk_09e0 == 2)
    {
        if (player->unk_09d8 != 0)
        {
            player->unk_09d8--;
            if (player->unk_09d8 == 0)
            {
                g_GameManager.unk_1819 = 0;
                if (g_GameManager.lives > 0)
                {
                    g_Unknown69e268.Func41f290(&player->position, 2, 2);
                    g_Unknown69e268.Func41f290(&player->position, 0, 2);
                    g_Unknown69e268.Func41f290(&player->position, 0, 2);
                    g_Unknown69e268.Func41f290(&player->position, 0, 2);
                    g_Unknown69e268.Func41f290(&player->position, 0, 2);
                    g_Unknown69e268.Func41f290(&player->position, 0, 2);
                    if (g_GameManager.power <= 16)
                    {
                        g_GameManager.power = 0;
                    }
                    else
                    {
                        g_GameManager.power -= 16;
                    }
                    g_Gui.flags.powerChanged = 2;
                }
                else
                {
                    g_Unknown69e268.Func41f290(&player->position, 4, 2);
                    g_Unknown69e268.Func41f290(&player->position, 4, 2);
                    g_Unknown69e268.Func41f290(&player->position, 4, 2);
                    g_Unknown69e268.Func41f290(&player->position, 4, 2);
                    g_Unknown69e268.Func41f290(&player->position, 4, 2);
                    g_GameManager.power = 0;
                    g_Gui.flags.powerChanged = 2;
                    g_GameManager.unk_181c = 0xff;
                }
                g_GameManager.SubSubRank(1600);
            }
        }
        else
        {
            progress = player->unk_75b4.AsFloat() / 30.0f;
            player->sprite.scaleY = 3.0f * progress + 1.0f;
            player->sprite.scaleX = 1.0f - 1.0f * progress;
            player->sprite.color = (int)(255.0f - player->unk_75b4.AsFloat() * 255.0f / 30.0f) << 24 | 0xffffff;
            player->sprite.flags.blendMode = 1;
            player->velocityX = 0.0f;
            player->velocityY = 0.0f;
            if (player->unk_75b4.AsFrames() >= 30)
            {
                player->unk_09e0 = 1;
                player->position.x = g_GameManager.arena[2] / 2.0f;
                player->position.y = g_GameManager.arena[3] - 64.0f;
                player->position.z = 0.2f;
                player->unk_75b4.Set(0);
                player->sprite.scaleX = 3.0f;
                player->sprite.scaleY = 3.0f;
                g_SpriteEngine->StartScript(&player->sprite, 0x400);
                if (g_GameManager.lives <= 0)
                {
                    g_GameManager.unk_1820 = 1;
                }
                else
                {
                    g_GameManager.lives--;
                    g_Gui.flags.livesChanged = 2;
                    if (g_GameManager.difficulty < 4 && g_GameManager.unk_1823 == 0)
                    {
                        g_GameManager.bombs = g_Unknown6c6d18.unk_14c.bombs;
                    }
                    else
                    {
                        g_GameManager.bombs = 3;
                    }
                    g_Gui.flags.bombsChanged = 2;
                    goto respawning;
                }
            }
        }
    }
    else if (player->unk_09e0 == 1)
    {
    respawning:
        player->unk_09dc = 90;
        progress2 = 1.0f - player->unk_75b4.AsFloat() / 30.0f;
        player->sprite.scaleY = 2.0f * progress2 + 1.0f;
        player->sprite.scaleX = 1.0f - 1.0f * progress2;
        player->sprite.flags.blendMode = 1;
        player->moveScaleY = 1.0f;
        player->moveScaleX = 1.0f;
        player->sprite.color = player->unk_75b4.AsFrames() * 255 / 30 << 24 | 0xffffff;
        player->unk_09d8 = 0;
        if (player->unk_75b4.AsFrames() >= 30)
        {
            player->unk_09e0 = 3;
            player->sprite.scaleX = 1.0f;
            player->sprite.scaleY = 1.0f;
            player->sprite.color = 0xffffffff;
            player->sprite.flags.blendMode = 0;
            player->unk_75b4.Set(240);
            player->unk_09d8 = 6;
        }
    }
    if (player->unk_09dc != 0)
    {
        player->unk_09dc--;
        g_BulletManager.Func414160(0);
    }
    if (player->unk_09e0 == 3)
    {
        player->unk_75b4.Sub(1);
        if (player->unk_75b4.AsFrames() <= 0)
        {
            player->unk_09e0 = 0;
            player->unk_75b4.Set(0);
            player->sprite.flags.colorOp = 0;
            player->sprite.color = 0xffffffff;
        }
        else if (player->unk_75b4.AsFrames() % 8 < 2)
        {
            player->sprite.flags.colorOp = 1;
            player->sprite.color = 0xff404040;
        }
        else
        {
            player->sprite.flags.colorOp = 0;
            player->sprite.color = 0xffffffff;
        }
    }
    else
    {
        player->unk_75b4.Tick();
    }
    if (player->unk_09e0 != 2 && player->unk_09e0 != 1)
    {
        player->HandleInput();
    }
    g_SpriteEngine->Step(&player->sprite);
    Player::UpdateBullets(player);
    if (player->optionState)
    {
        g_SpriteEngine->Step(&player->optionSprites[0]);
        g_SpriteEngine->Step(&player->optionSprites[1]);
    }
    player->unk_0a1c = D3DXVECTOR3(-999.0f, -999.0f, 0.0f);
    Player::UpdateFiring(player);
    return 1;
}

void Player::UpdateBullets(Player *player)
{
    float x;
    float y;
    int i;
    float len;
    PlayerBullet *bullet;

    for (i = 0; i < 2; i++)
    {
        if (player->laserTimers[i].AsFrames() != 0)
        {
            player->laserTimers[i].Sub(1);
        }
    }
    bullet = player->bullets;
    for (i = 0; i < 0x50; i++, bullet++)
    {
        if (bullet->state == 0)
        {
            continue;
        }
        switch (bullet->type)
        {
        case 1:
            if (bullet->state == 1)
            {
                if (player->unk_0a1c.x > -100.0f && bullet->timer.AsFrames() < 40 && bullet->timer.HasTicked())
                {
                    x = player->unk_0a1c.x - bullet->pos.x;
                    y = player->unk_0a1c.y - bullet->pos.y;
                    len = sqrtf(x * x + y * y) / (bullet->speed / 4.0f);
                    if (len < 1.0f)
                    {
                        len = 1.0f;
                    }
                    x = x / len + bullet->velocity.x;
                    y = y / len + bullet->velocity.y;
                    len = sqrtf(x * x + y * y);
                    bullet->speed = (len > 10.0f) ? 10.0f : len;
                    if (bullet->speed < 1.0f)
                    {
                        bullet->speed = 1.0f;
                    }
                    bullet->velocity.x = x * bullet->speed / len;
                    bullet->velocity.y = y * bullet->speed / len;
                }
                else if (bullet->speed < 10.0f)
                {
                    bullet->speed += 0.33333334f;
                    x = bullet->velocity.x;
                    y = bullet->velocity.y;
                    len = sqrtf(x * x + y * y);
                    bullet->velocity.x = x * bullet->speed / len;
                    bullet->velocity.y = y * bullet->speed / len;
                }
            }
            break;
        case 2:
            if (bullet->state == 1)
            {
                bullet->velocity.y -= 0.3f;
            }
            break;
        case 3:
            if (player->laserTimers[bullet->laserIndex] == 70)
            {
                bullet->sprite.unk_08a = 1;
            }
            else if (player->laserTimers[bullet->laserIndex] == 1)
            {
                bullet->sprite.unk_08a = 1;
            }
            bullet->pos = player->optionPos[bullet->option - 1];
            bullet->pos.x += bullet->velocity.z;
            bullet->pos.y /= 2.0f;
            bullet->pos.z = 0.44f;
            bullet->sprite.scaleY = bullet->pos.y * 2.0f / 14.0f;
            bullet->size.y = bullet->pos.y * 2.0f;
            break;
        }
        bullet->sprite.pos.x = bullet->pos[0] += g_Unknown6c6d18.frameAdvance * bullet->velocity.x;
        bullet->sprite.pos.y = bullet->pos[1] += g_Unknown6c6d18.frameAdvance * bullet->velocity.y;
        bullet->sprite.pos.z = bullet->pos.z;
        if (bullet->type != 3 &&
            !g_GameManager.Func41b5e1(bullet->pos.x, bullet->pos.y, bullet->sprite.spriteData->width,
                                      bullet->sprite.spriteData->height))
        {
            bullet->state = 0;
        }
        if (g_SpriteEngine->Step(&bullet->sprite))
        {
            bullet->state = 0;
        }
        bullet->timer.Tick();
    }
}

int Player::UpdateFiring(Player *player)
{
    if (player->fireTimer.AsFrames() < 0)
    {
        return 0;
    }
    if (player->fireTimer.HasTicked() &&
        (!g_Player.bomb.running || g_GameManager.character != 1 || g_GameManager.shotType != 1))
    {
        Player::FireShots(player, player->fireTimer.AsFrames());
    }
    player->fireTimer.Tick();
    if (player->fireTimer.AsFrames() >= 30 || player->unk_09e0 == 2 || player->unk_09e0 == 1)
    {
        player->fireTimer.Set(-1);
    }
    return 0;
}

void Player::FireShots(Player *player, int frame)
{
    int shotIndex;
    int i;
    PlayerBullet *curBullet;
    int ret;

    shotIndex = 0;
    curBullet = player->bullets;
    for (i = 0; i < 0x50; i++, curBullet++)
    {
        if (curBullet->state != 0)
        {
            continue;
        }
    again:
        if (!player->focused)
        {
            ret = player->fire(player, curBullet, shotIndex, frame);
        }
        else
        {
            ret = player->fireFocused(player, curBullet, shotIndex, frame);
        }
        if (ret >= 0)
        {
            curBullet->sprite.pos.x = curBullet->pos.x;
            curBullet->sprite.pos.y = curBullet->pos.y;
            curBullet->sprite.pos.z = 0.495f;
            curBullet->state = 1;
        }
        if (ret == -2)
        {
            break;
        }
        if (ret > 0)
        {
            break;
        }
        shotIndex++;
        if (ret == -1)
        {
            goto again;
        }
    }
}

int Player::OnDraw2(Player *player)
{
    Player::DrawHitBullets(player);
    return 1;
}

int Player::OnDraw(Player *player)
{
    Player::DrawBullets(player);
    if (player->bomb.running && player->bomb.draw != NULL)
    {
        player->bomb.draw(player);
    }
    player->sprite.pos.x = g_GameManager.arena[0] + player->position.x;
    player->sprite.pos.y = g_GameManager.arena[1] + player->position.y;
    player->sprite.pos.z = 0.49f;
    if (!g_GameManager.unk_1820)
    {
        g_SpriteEngine->Draw(&player->sprite);
        if (player->optionState && (player->unk_09e0 == 0 || player->unk_09e0 == 3))
        {
            player->optionSprites[0].pos = player->optionPos[0];
            player->optionSprites[1].pos = player->optionPos[1];
            player->optionSprites[0].pos[0] += g_GameManager.arena[0];
            player->optionSprites[0].pos[1] += g_GameManager.arena[1];
            player->optionSprites[1].pos[0] += g_GameManager.arena[0];
            player->optionSprites[1].pos[1] += g_GameManager.arena[1];
            player->optionSprites[0].pos.z = 0.491f;
            player->optionSprites[1].pos.z = 0.491f;
            g_SpriteEngine->Func432cc0(&player->optionSprites[0]);
            g_SpriteEngine->Func432cc0(&player->optionSprites[1]);
        }
    }
    return 1;
}

void Player::DrawBullets(Player *player)
{
    PlayerBullet *bullet = player->bullets;
    int i;

    for (i = 0; i < 0x50; i++, bullet++)
    {
        if (bullet->state != 1)
        {
            continue;
        }
        if (bullet->sprite.unk_088 != 0)
        {
            bullet->sprite.rotation = 1.5707964f - AddNormalizeAngle(bullet->angle, 3.1415927f);
        }
        g_SpriteEngine->Func433590(&bullet->sprite);
    }
}

void Player::DrawHitBullets(Player *player)
{
    PlayerBullet *bullet = player->bullets;
    int i;

    for (i = 0; i < 0x50; i++, bullet++)
    {
        if (bullet->state != 2)
        {
            continue;
        }
        if (bullet->sprite.unk_088 != 0)
        {
            bullet->sprite.rotation = 1.5707964f - AddNormalizeAngle(bullet->angle, 3.1415927f);
        }
        bullet->sprite.pos.z = 0.4f;
        g_SpriteEngine->Func433590(&bullet->sprite);
    }
}

int Player::AddedCallback(Player *player)
{
    int i;
    PlayerBullet *bullet;

    switch (g_GameManager.character)
    {
    case 0:
        if ((g_Unknown6c6d18.scene != 3) != 0 && g_SpriteEngine->LoadAnm(5, "data/player00.anm", 0x400) != 0)
        {
            return -1;
        }
        g_SpriteEngine->StartScript(&player->sprite, 0x400);
        break;
    case 1:
        if ((g_Unknown6c6d18.scene != 3) != 0 && g_SpriteEngine->LoadAnm(5, "data/player01.anm", 0x400) != 0)
        {
            return -1;
        }
        g_SpriteEngine->StartScript(&player->sprite, 0x400);
        break;
    }
    player->position.x = g_GameManager.arena[2] / 2.0f;
    player->position.y = g_GameManager.arena[3] - 64.0f;
    player->position.z = 0.49f;
    player->optionPos[0].z = 0.49f;
    player->optionPos[1].z = 0.49f;
    for (i = 0; i < 32; i++)
    {
        player->bombHitSize[i].x = 0.0f;
    }
    player->hitboxSize.x = 1.25f;
    player->hitboxSize.y = 1.25f;
    player->hitboxSize.z = 5.0f;
    player->grazeSize.x = 12.0f;
    player->grazeSize.y = 12.0f;
    player->grazeSize.z = 5.0f;
    player->direction = 0;
    player->shotTypeInfo = g_ShotTypes[g_GameManager.character * 2 + g_GameManager.shotType];
    player->shotTypeInfo.speedDiagonal = player->shotTypeInfo.speed / sqrtf(2.0f);
    player->shotTypeInfo.speedDiagonalFocused = player->shotTypeInfo.speedFocused / sqrtf(2.0f);
    player->fire = player->shotTypeInfo.fire;
    player->fireFocused = player->shotTypeInfo.fireFocused;
    player->unk_09e0 = 1;
    player->unk_75b4.Set(120);
    player->optionState = 0;
    g_SpriteEngine->StartScript(&player->optionSprites[0], 0x480);
    g_SpriteEngine->StartScript(&player->optionSprites[1], 0x481);
    bullet = player->bullets;
    for (i = 0; i < 0x50; i++, bullet++)
    {
        bullet->state = 0;
    }
    player->fireTimer.Set(-1);
    player->bomb.update = g_Bombs[g_GameManager.character * 2 + g_GameManager.shotType].update;
    player->bomb.draw = g_Bombs[g_GameManager.character * 2 + g_GameManager.shotType].draw;
    player->bomb.running = 0;
    for (i = 0; i < 2; i++)
    {
        player->laserTimers[i].Set(0);
    }
    player->moveScaleY = 1.0f;
    player->moveScaleX = 1.0f;
    player->unk_09d8 = 8;
    return 0;
}
