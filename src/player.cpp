// The file with the player's shots.

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

int Player::OnDraw2(Player *player)
{
    Player::Func429bb0(player);
    return 1;
}
