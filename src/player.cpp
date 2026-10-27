// The file with the player's shots.

#include "bullet.h"
#include "enemy.h"
#include "gamemanager.h"
#include "player.h"
#include "unsorted.h"

int FireShotType0(Player *player, PlayerBullet *bullet, int index, int frame)
{
    return FireShot(player, bullet, index, frame, &g_ShotTable0);
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
            deltaX = this->speedFocused;
        }
        else
        {
            deltaX = this->speed;
        }
        break;
    case 3:
        if (g_Buttons & 4)
        {
            deltaX = -this->speedFocused;
        }
        else
        {
            deltaX = -this->speed;
        }
        break;
    case 1:
        if (g_Buttons & 4)
        {
            deltaY = -this->speedFocused;
        }
        else
        {
            deltaY = -this->speed;
        }
        break;
    case 2:
        if (g_Buttons & 4)
        {
            deltaY = this->speedFocused;
        }
        else
        {
            deltaY = this->speed;
        }
        break;
    case 5:
        if (g_Buttons & 4)
        {
            deltaX = -this->speedDiagonalFocused;
        }
        else
        {
            deltaX = -this->speedDiagonal;
        }
        deltaY = deltaX;
        break;
    case 7:
        if (g_Buttons & 4)
        {
            deltaX = -this->speedDiagonalFocused;
        }
        else
        {
            deltaX = -this->speedDiagonal;
        }
        deltaY = -deltaX;
        break;
    case 6:
        if (g_Buttons & 4)
        {
            deltaX = this->speedDiagonalFocused;
        }
        else
        {
            deltaX = this->speedDiagonal;
        }
        deltaY = -deltaX;
        break;
    case 8:
        if (g_Buttons & 4)
        {
            deltaX = this->speedDiagonalFocused;
        }
        else
        {
            deltaX = this->speedDiagonal;
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
    if (player->bombing != 0)
    {
        player->bombUpdate(player);
    }
    else if (!g_Gui.Func4195a2() && player->unk_09d8 != 0 && g_GameManager.bombs > 0 && WAS_PRESSED(2) &&
             player->bombUpdate != NULL)
    {
        g_GameManager.bombsUsed++;
        g_GameManager.bombs--;
        g_Gui.flags.bombsChanged = 2;
        player->bombing = 1;
        player->bombTimer.Set(0);
        player->bombDuration = 999;
        player->bombUpdate(player);
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
    Player::Func429710(player);
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

int Player::OnDraw2(Player *player)
{
    Player::DrawHitBullets(player);
    return 1;
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
