// The file with the player's bombs, from 0x405260 ("霊符「夢想封印」",
// "恋符「マスタースパーク」", ...).

#include "effect.h"
#include "gamemanager.h"
#include "player.h"
#include "screeneffect.h"
#include "sprite.h"
#include "unsorted.h"

void UpdateReimuABomb(Player *player)
{
    float x;
    float y;
    int i;
    Sprite *spr;
    float len;
    D3DXVECTOR3 aim;
    int k;

    if (player->bombTimer >= player->bombDuration)
    {
        g_Gui.Func417314();
        player->unk_75c8 = 0;
        return;
    }
    if (player->bombTimer.HasTicked() && player->bombTimer == 0)
    {
        g_Gui.Func417acd(0x4a1, "霊符「夢想封印」");
        player->bombDuration = 300;
        player->unk_75b4.Set(360);
        for (i = 0; i < 8; i++)
        {
            player->bombActive[i] = 0;
        }
        g_Unknown69e268.Func420130();
        g_EffectManager.Func40ef50(0xc, &player->position, 1, 0xff4040ff);
        player->bulletClearAreas[8][0] = player->position.x;
        player->bulletClearAreas[8][1] = player->position.y;
        player->bulletClearAreas[8][2] = 256.0f;
        player->bulletClearAreas[8][3] = 256.0f;
    }
    if (player->bombTimer >= 60 && player->bombTimer < 180)
    {
        if (player->bombTimer.AsFrames() % 16 == 0)
        {
            i = (player->bombTimer.AsFrames() - 60) / 16;
            if (i != 0)
            {
                player->bombActive[i] = 1;
                player->bombSpeed[i] = 4.0f;
                player->bombPos[i] = player->position;
                x = g_Rng.GetRandomF32ZeroToOne() * 6.2831855f - 3.1415927f;
                player->bombVelocity[i].x = cosf(x) * player->bombSpeed[i];
                player->bombVelocity[i].y = sinf(x) * player->bombSpeed[i];
                player->bombHitCount[i] = 0;
                for (spr = &player->bombSprites[i * 4], k = 0; k < 4; k++, spr++)
                {
                    g_SpriteEngine->Func4051b0(spr, k + 0x485);
                }
                g_Unknown6d3f50.Func4311e0(0xd, 0);
            }
        }
    }
    player->unk_09e0 = 3;
    for (i = 0; i < 8; i++)
    {
        if (player->bombActive[i] == 0)
        {
            continue;
        }
        if (player->bombActive[i] == 1)
        {
            if (player->bombTimer.HasTicked())
            {
                if (player->unk_0a1c.x > -100.0f)
                {
                    aim = player->unk_0a1c;
                }
                else
                {
                    aim = player->position;
                }
                x = aim.x - player->bombPos[i].x;
                y = aim.y - player->bombPos[i].y;
                len = sqrtf(x * x + y * y) / (player->bombSpeed[i] / 8.0f);
                if (len < 1.0f)
                {
                    len = 1.0f;
                }
                x = x / len + player->bombVelocity[i].x;
                y = y / len + player->bombVelocity[i].y;
                len = sqrtf(x * x + y * y);
                player->bombSpeed[i] = (len > 10.0f) ? 10.0f : len;
                if (player->bombSpeed[i] < 1.0f)
                {
                    player->bombSpeed[i] = 1.0f;
                }
                player->bombVelocity[i].x = x * player->bombSpeed[i] / len;
                player->bombVelocity[i].y = y * player->bombSpeed[i] / len;
                player->bombHitSize[i].x = 48.0f;
                player->bombHitSize[i].y = 48.0f;
                player->bombHitPos[i] = player->bombPos[i];
                player->bombHitDamage[i] = 8;
                player->bulletClearAreas[i][0] = player->bombPos[i].x;
                player->bulletClearAreas[i][1] = player->bombPos[i].y;
                player->bulletClearAreas[i][2] = 48.0f;
                player->bulletClearAreas[i][3] = 48.0f;
                if (player->bombHitCount[i] >= 100 || player->bombTimer >= player->bombDuration - 30)
                {
                    g_EffectManager.Func40ef50(6, &player->bombPos[i], 8, 0xffffffff);
                    g_EffectManager.Func40ef50(0xc, &player->bombPos[i], 1, 0xff4040ff);
                    player->bombActive[i] = 2;
                    player->bombSprites[i * 4].unk_08a = 1;
                    player->bombSprites[i * 4 + 1].unk_08a = 1;
                    player->bombSprites[i * 4 + 2].unk_08a = 1;
                    player->bombSprites[i * 4 + 3].unk_08a = 1;
                    player->bombHitSize[i].x = 256.0f;
                    player->bombHitSize[i].y = 256.0f;
                    player->bombHitDamage[i] = 200;
                    player->bulletClearAreas[i][2] = 256.0f;
                    player->bulletClearAreas[i][3] = 256.0f;
                    D3DXVECTOR3 velocity = player->bombVelocity[i] / 8.0f;
                    g_Unknown6d3f50.Func4311e0(0xf, 0);
                    ScreenEffect::Func42fd30(1, 0x10, 8, 0, 0);
                }
            }
        }
        else if (player->bombActive[i] != 0)
        {
            if (player->bombTimer.HasTicked())
            {
                player->bombActive[i]++;
                if (player->bombActive[i] >= 30)
                {
                    player->bombActive[i] = 0;
                }
            }
        }
        player->bombPos[i].x += g_Unknown6c6d18.frameAdvance * player->bombVelocity[i].x;
        player->bombPos[i].y += g_Unknown6c6d18.frameAdvance * player->bombVelocity[i].y;
        g_SpriteEngine->Step(&player->bombSprites[i * 4]);
        g_SpriteEngine->Step(&player->bombSprites[i * 4 + 1]);
        g_SpriteEngine->Step(&player->bombSprites[i * 4 + 2]);
        g_SpriteEngine->Step(&player->bombSprites[i * 4 + 3]);
    }
    player->bombTimer.Tick();
}

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
    curSprite = player->bombSprites;
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

void UpdateReimuBBomb(Player *player)
{
    int i;
    Sprite *sprite;
    D3DXVECTOR3 unused;

    if (player->bombTimer >= player->bombDuration)
    {
        g_Gui.Func417314();
        player->unk_75c8 = 0;
        return;
    }
    if (player->bombTimer.HasTicked() && player->bombTimer == 0)
    {
        g_Unknown69e268.Func420130();
        g_Gui.Func417acd(0x4a3, "夢符「封魔陣」");
        player->bombDuration = 140;
        player->unk_75b4.Set(200);
        for (sprite = player->bombSprites, i = 0; i < 4; i++, sprite++)
        {
            g_SpriteEngine->Func4051b0(sprite, i + 0x489);
        }
        g_Unknown6d3f50.Func4311e0(6, 0);
        player->bombPos[0].x = player->position.x;
        player->bombPos[0].y = 224.0f;
        player->bombPos[0].z = 0.42f;
        player->bombPos[1].x = 192.0f;
        player->bombPos[1].y = player->position.y;
        player->bombPos[1].z = 0.415f;
        player->bombPos[2].x = player->position.x;
        player->bombPos[2].y = 224.0f;
        player->bombPos[2].z = 0.41f;
        player->bombPos[3].x = 192.0f;
        player->bombPos[3].y = player->position.y;
        player->bombPos[3].z = 0.405f;
        ScreenEffect::Func42fd30(1, 60, 2, 6, 0);
    }
    else
    {
        if (player->bombTimer == 60)
        {
            ScreenEffect::Func42fd30(1, 80, 20, 0, 0);
        }
        player->bulletClearAreas[0][2] = 62.0f;
        player->bulletClearAreas[0][3] = 448.0f;
        player->bulletClearAreas[1][2] = 384.0f;
        player->bulletClearAreas[1][3] = 62.0f;
        player->bulletClearAreas[2][2] = 62.0f;
        player->bulletClearAreas[2][3] = 448.0f;
        player->bulletClearAreas[3][2] = 384.0f;
        player->bulletClearAreas[3][3] = 62.0f;
        for (i = 0; i < 4; i++)
        {
            g_SpriteEngine->Step(&player->bombSprites[i]);
            if (player->bombTimer.HasTicked() && player->bombTimer.AsFrames() % 2 != 0)
            {
                player->bulletClearAreas[i][0] = player->bombPos[i].x + player->bombSprites[i].unk_0e4.x;
                player->bulletClearAreas[i][1] = player->bombPos[i].y + player->bombSprites[i].unk_0e4.y;
                player->bombHitSize[i].x = player->bulletClearAreas[i][2];
                player->bombHitSize[i].y = player->bulletClearAreas[i][3];
                player->bombHitPos[i] = player->bombPos[i] + player->bombSprites[i].unk_0e4;
                player->bombHitDamage[i] = 8;
            }
        }
    }
    player->unk_09e0 = 3;
    player->bombTimer.Tick();
}
