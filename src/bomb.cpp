// The file with the player's bombs, from 0x405260 ("霊符「夢想封印」",
// "恋符「マスタースパーク」", ...).

#include "gamemanager.h"
#include "player.h"
#include "sprite.h"
#include "unsorted.h"

// Places curSprite at orb i's position in the play area, draws it and
// moves on to the next.
#define DRAW_BOMB_SPRITE()                                              \
    curSprite->pos = player->bombPos[i] + curSprite->unk_0e4;           \
    curSprite->pos[0] += g_GameManager.arena[0];                        \
    curSprite->pos[1] += g_GameManager.arena[1];                        \
    curSprite->pos.z = 0.0f;                                            \
    g_SpriteEngine->Draw(curSprite);                                    \
    curSprite++;

void DrawReimuABomb(Player *player)
{
    Sprite *curSprite;
    int i;

    DimPlayAreaForBomb(player);
    curSprite = player->bombSprites[0];
    for (i = 0; i < 8; i++)
    {
        if (player->bombActive[i] == 0)
        {
            curSprite += 4;
            continue;
        }
        DRAW_BOMB_SPRITE();
        DRAW_BOMB_SPRITE();
        DRAW_BOMB_SPRITE();
        DRAW_BOMB_SPRITE();
    }
}

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
