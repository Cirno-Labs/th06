// The file with the player's bombs, from 0x405260 ("霊符「夢想封印」",
// "恋符「マスタースパーク」", ...).

#include "player.h"
#include "unsorted.h"

void DimPlayAreaForBomb(Player *player)
{
    int alpha;
    float area[4];
    float t;

    area[0] = 32.0f;
    area[1] = 16.0f;
    area[2] = 416.0f;
    area[3] = 464.0f;
    if (player->bombTimer < 60)
    {
        t = player->bombTimer.AsFloat() * 176.0f / 60.0f;
        alpha = (t >= 176.0f) ? 176 : (int)t;
    }
    else if (player->bombTimer >= player->bombDuration - 60)
    {
        t = (player->bombDuration - player->bombTimer.AsFloat()) * 176.0f / 60.0f;
        alpha = (t < 0.0f) ? 0 : (int)t;
    }
    else
    {
        alpha = 176;
    }
    Func42f8b0(area, alpha << 24);
}
