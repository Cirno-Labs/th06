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
