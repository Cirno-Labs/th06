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
    if (player->unk_75c8 != 0)
    {
        player->bombUpdate(player);
    }
    else if (!g_Gui.Func4195a2() && player->unk_09d8 != 0 && g_GameManager.bombs > 0 && WAS_PRESSED(2) &&
             player->bombUpdate != NULL)
    {
        g_GameManager.bombsUsed++;
        g_GameManager.bombs--;
        g_Gui.flags.bombsChanged = 2;
        player->unk_75c8 = 1;
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
            player->unk_0a10 = 0;
            player->unk_0a14 = 0;
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
        player->Func427860();
    }
    g_SpriteEngine->Step(&player->sprite);
    Player::Func4291b0(player);
    if (player->unk_09e2)
    {
        g_SpriteEngine->Step(&player->unk_0110[0]);
        g_SpriteEngine->Step(&player->unk_0110[1]);
    }
    player->unk_0a1c = D3DXVECTOR3(-999.0f, -999.0f, 0.0f);
    Player::Func429710(player);
    return 1;
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
        if (bullet->type != 2)
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
