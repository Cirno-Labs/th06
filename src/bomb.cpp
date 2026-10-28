// The file with the player's bombs, from 0x405260 ("霊符「夢想封印」",
// "恋符「マスタースパーク」", ...).

#include "effect.h"
#include "gamemanager.h"
#include "item.h"
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

    if (player->bomb.timer >= player->bomb.duration)
    {
        g_Gui.Func417314();
        player->bomb.running = 0;
        return;
    }
    if (player->bomb.timer.HasTicked() && player->bomb.timer == 0)
    {
        g_Gui.Func417acd(0x4a1, "霊符「夢想封印」");
        player->bomb.duration = 300;
        player->unk_75b4.Set(360);
        for (i = 0; i < 8; i++)
        {
            player->bomb.partActive[i] = 0;
        }
        g_ItemManager.CollectAll();
        g_EffectManager.SpawnEffect(0xc, &player->position, 1, 0xff4040ff);
        player->bulletClearAreas[8][0] = player->position.x;
        player->bulletClearAreas[8][1] = player->position.y;
        player->bulletClearAreas[8][2] = 256.0f;
        player->bulletClearAreas[8][3] = 256.0f;
    }
    if (player->bomb.timer >= 60 && player->bomb.timer < 180)
    {
        if (player->bomb.timer.AsFrames() % 16 == 0)
        {
            i = (player->bomb.timer.AsFrames() - 60) / 16;
            if (i != 0)
            {
                player->bomb.partActive[i] = 1;
                player->bomb.partSpeed[i] = 4.0f;
                player->bomb.partPos[i] = player->position;
                x = g_Rng.GetRandomF32ZeroToOne() * 6.2831855f - 3.1415927f;
                player->bomb.partVelocity[i].x = cosf(x) * player->bomb.partSpeed[i];
                player->bomb.partVelocity[i].y = sinf(x) * player->bomb.partSpeed[i];
                player->bombHitCount[i] = 0;
                for (spr = &player->bomb.sprites[i * 4], k = 0; k < 4; k++, spr++)
                {
                    g_SpriteEngine->Func4051b0(spr, k + 0x485);
                }
                g_Unknown6d3f50.QueueSound(0xd, 0);
            }
        }
    }
    player->unk_09e0 = 3;
    for (i = 0; i < 8; i++)
    {
        if (player->bomb.partActive[i] == 0)
        {
            continue;
        }
        if (player->bomb.partActive[i] == 1)
        {
            if (player->bomb.timer.HasTicked())
            {
                if (player->unk_0a1c.x > -100.0f)
                {
                    aim = player->unk_0a1c;
                }
                else
                {
                    aim = player->position;
                }
                x = aim.x - player->bomb.partPos[i].x;
                y = aim.y - player->bomb.partPos[i].y;
                len = sqrtf(x * x + y * y) / (player->bomb.partSpeed[i] / 8.0f);
                if (len < 1.0f)
                {
                    len = 1.0f;
                }
                x = x / len + player->bomb.partVelocity[i].x;
                y = y / len + player->bomb.partVelocity[i].y;
                len = sqrtf(x * x + y * y);
                player->bomb.partSpeed[i] = (len > 10.0f) ? 10.0f : len;
                if (player->bomb.partSpeed[i] < 1.0f)
                {
                    player->bomb.partSpeed[i] = 1.0f;
                }
                player->bomb.partVelocity[i].x = x * player->bomb.partSpeed[i] / len;
                player->bomb.partVelocity[i].y = y * player->bomb.partSpeed[i] / len;
                player->bombHitSize[i].x = 48.0f;
                player->bombHitSize[i].y = 48.0f;
                player->bombHitPos[i] = player->bomb.partPos[i];
                player->bombHitDamage[i] = 8;
                player->bulletClearAreas[i][0] = player->bomb.partPos[i].x;
                player->bulletClearAreas[i][1] = player->bomb.partPos[i].y;
                player->bulletClearAreas[i][2] = 48.0f;
                player->bulletClearAreas[i][3] = 48.0f;
                if (player->bombHitCount[i] >= 100 || player->bomb.timer >= player->bomb.duration - 30)
                {
                    g_EffectManager.SpawnEffect(6, &player->bomb.partPos[i], 8, 0xffffffff);
                    g_EffectManager.SpawnEffect(0xc, &player->bomb.partPos[i], 1, 0xff4040ff);
                    player->bomb.partActive[i] = 2;
                    player->bomb.sprites[i * 4].unk_08a = 1;
                    player->bomb.sprites[i * 4 + 1].unk_08a = 1;
                    player->bomb.sprites[i * 4 + 2].unk_08a = 1;
                    player->bomb.sprites[i * 4 + 3].unk_08a = 1;
                    player->bombHitSize[i].x = 256.0f;
                    player->bombHitSize[i].y = 256.0f;
                    player->bombHitDamage[i] = 200;
                    player->bulletClearAreas[i][2] = 256.0f;
                    player->bulletClearAreas[i][3] = 256.0f;
                    D3DXVECTOR3 velocity = player->bomb.partVelocity[i] / 8.0f;
                    g_Unknown6d3f50.QueueSound(0xf, 0);
                    ScreenEffect::Func42fd30(1, 0x10, 8, 0, 0);
                }
            }
        }
        else if (player->bomb.partActive[i] != 0)
        {
            if (player->bomb.timer.HasTicked())
            {
                player->bomb.partActive[i]++;
                if (player->bomb.partActive[i] >= 30)
                {
                    player->bomb.partActive[i] = 0;
                }
            }
        }
        player->bomb.partPos[i].x += g_Unknown6c6d18.frameAdvance * player->bomb.partVelocity[i].x;
        player->bomb.partPos[i].y += g_Unknown6c6d18.frameAdvance * player->bomb.partVelocity[i].y;
        g_SpriteEngine->Step(&player->bomb.sprites[i * 4]);
        g_SpriteEngine->Step(&player->bomb.sprites[i * 4 + 1]);
        g_SpriteEngine->Step(&player->bomb.sprites[i * 4 + 2]);
        g_SpriteEngine->Step(&player->bomb.sprites[i * 4 + 3]);
    }
    player->bomb.timer.Tick();
}

// Places curSprite at orb i's position in the play area, draws it and
// moves on to the next.
#define DRAW_BOMB_SPRITE()                                              \
    curSprite->pos = player->bomb.partPos[i] + curSprite->unk_0e4;           \
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
    curSprite = player->bomb.sprites;
    for (i = 0; i < 8; i++)
    {
        if (player->bomb.partActive[i] == 0)
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
    if (player->bomb.timer < 60)
    {
        t = player->bomb.timer.AsFloat() * 176.0f / 60.0f;
        alpha = (t >= 176.0f) ? 176 : (int)t;
    }
    else if (player->bomb.timer >= player->bomb.duration - 60)
    {
        t = (player->bomb.duration - player->bomb.timer.AsFloat()) * 176.0f / 60.0f;
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

    if (player->bomb.timer >= player->bomb.duration)
    {
        g_Gui.Func417314();
        player->bomb.running = 0;
        return;
    }
    if (player->bomb.timer.HasTicked() && player->bomb.timer == 0)
    {
        g_ItemManager.CollectAll();
        g_Gui.Func417acd(0x4a3, "夢符「封魔陣」");
        player->bomb.duration = 140;
        player->unk_75b4.Set(200);
        for (sprite = player->bomb.sprites, i = 0; i < 4; i++, sprite++)
        {
            g_SpriteEngine->Func4051b0(sprite, i + 0x489);
        }
        g_Unknown6d3f50.QueueSound(6, 0);
        player->bomb.partPos[0].x = player->position.x;
        player->bomb.partPos[0].y = 224.0f;
        player->bomb.partPos[0].z = 0.42f;
        player->bomb.partPos[1].x = 192.0f;
        player->bomb.partPos[1].y = player->position.y;
        player->bomb.partPos[1].z = 0.415f;
        player->bomb.partPos[2].x = player->position.x;
        player->bomb.partPos[2].y = 224.0f;
        player->bomb.partPos[2].z = 0.41f;
        player->bomb.partPos[3].x = 192.0f;
        player->bomb.partPos[3].y = player->position.y;
        player->bomb.partPos[3].z = 0.405f;
        ScreenEffect::Func42fd30(1, 60, 2, 6, 0);
    }
    else
    {
        if (player->bomb.timer == 60)
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
            g_SpriteEngine->Step(&player->bomb.sprites[i]);
            if (player->bomb.timer.HasTicked() && player->bomb.timer.AsFrames() % 2 != 0)
            {
                player->bulletClearAreas[i][0] = player->bomb.partPos[i].x + player->bomb.sprites[i].unk_0e4.x;
                player->bulletClearAreas[i][1] = player->bomb.partPos[i].y + player->bomb.sprites[i].unk_0e4.y;
                player->bombHitSize[i].x = player->bulletClearAreas[i][2];
                player->bombHitSize[i].y = player->bulletClearAreas[i][3];
                player->bombHitPos[i] = player->bomb.partPos[i] + player->bomb.sprites[i].unk_0e4;
                player->bombHitDamage[i] = 8;
            }
        }
    }
    player->unk_09e0 = 3;
    player->bomb.timer.Tick();
}

void DrawReimuBBomb(Player *player)
{
    Sprite *curSprite;
    int i;

    DimPlayAreaForBomb(player);
    for (curSprite = player->bomb.sprites, i = 0; i < 4; i++, curSprite++)
    {
        curSprite->pos = player->bomb.partPos[i] + curSprite->unk_0e4;
        curSprite->pos.x += g_GameManager.arena[0];
        curSprite->pos.y += g_GameManager.arena[1];
        curSprite->pos.z = 0.0f;
        g_SpriteEngine->DrawRotated(curSprite);
    }
}

void UpdateMarisaABomb(Player *player)
{
    int i;
    Sprite *sprite;
    D3DXVECTOR3 unused;
    float theta;

    if (player->bomb.timer >= player->bomb.duration)
    {
        g_Gui.Func417314();
        player->bomb.running = 0;
        return;
    }
    if (player->bomb.timer.HasTicked() && player->bomb.timer == 0)
    {
        g_ItemManager.CollectAll();
        g_Gui.Func417acd(0x4a3, "魔符「スターダストレヴァリエ」");
        player->bomb.duration = 250;
        player->unk_75b4.Set(300);
        for (sprite = player->bomb.sprites, i = 0; i < 8; i++, sprite++)
        {
            g_SpriteEngine->Func4051b0(sprite, i % 3 + 0x405);
            player->bomb.partPos[i] = player->position;
            theta = (float)i * 6.2831855f / 8.0f;
            player->bomb.partVelocity[i].x = cosf(theta) * 2.0f;
            player->bomb.partVelocity[i].y = sinf(theta) * 2.0f;
            player->bomb.partVelocity[i].z = 0.0f;
        }
        g_Unknown6d3f50.QueueSound(6, 0);
        ScreenEffect::Func42fd30(1, 120, 4, 1, 0);
    }
    else
    {
        for (i = 0; i < 8; i++)
        {
            player->bomb.partPos[i] += player->bomb.partVelocity[i] * g_Unknown6c6d18.frameAdvance;
            if (player->bomb.timer.HasTicked() && player->bomb.timer.AsFrames() % 3 != 0)
            {
                player->bulletClearAreas[i][0] = player->bomb.partPos[i].x;
                player->bulletClearAreas[i][1] = player->bomb.partPos[i].y;
                player->bulletClearAreas[i][2] = 128.0f;
                player->bulletClearAreas[i][3] = 128.0f;
                player->bombHitSize[i].x = 128.0f;
                player->bombHitSize[i].y = 128.0f;
                player->bombHitPos[i] = player->bomb.partPos[i];
                player->bombHitDamage[i] = 8;
            }
            g_SpriteEngine->Step(&player->bomb.sprites[i]);
        }
    }
    player->unk_09e0 = 3;
    player->bomb.timer.Tick();
}

void DrawMarisaABomb(Player *player)
{
    Sprite *curSprite;
    int i;

    DimPlayAreaForBomb(player);
    curSprite = player->bomb.sprites;
    for (i = 0; i < 8; i++)
    {
        curSprite->pos = player->bomb.partPos[i];
        curSprite->pos.x += g_GameManager.arena[0];
        curSprite->pos.y += g_GameManager.arena[1];
        curSprite->pos.z = 0.0f;
        curSprite->scaleX = 3.2f;
        curSprite->scaleY = 3.2f;
        g_SpriteEngine->DrawRotated(curSprite);
        curSprite->pos -= player->bomb.partVelocity[i] * 6.0f;
        curSprite->pos.x += -32.0f;
        curSprite->pos.y += -32.0f;
        curSprite->pos.z = 0.0f;
        curSprite->scaleX = 2.2f;
        curSprite->scaleY = 2.2f;
        g_SpriteEngine->DrawRotated(curSprite);
        curSprite->pos -= player->bomb.partVelocity[i] * 2.0f;
        curSprite->pos.x += 64.0f;
        curSprite->pos.y += 64.0f;
        curSprite->pos.z = 0.0f;
        curSprite->pos -= player->bomb.partVelocity[i] * 2.0f;
        curSprite->pos.x += -32.0f;
        curSprite->pos.y += -32.0f;
        curSprite->pos.z = 0.0f;
        curSprite->scaleX = 1.0f;
        curSprite->scaleY = 1.0f;
        g_SpriteEngine->DrawRotated(curSprite);
        curSprite++;
    }
}

void UpdateMarisaBBomb(Player *player)
{
    int i;
    Sprite *sprite;
    D3DXVECTOR3 unused;

    if (player->bomb.timer >= player->bomb.duration)
    {
        g_Gui.Func417314();
        player->bomb.running = 0;
        player->moveScaleY = 1.0f;
        player->moveScaleX = 1.0f;
        return;
    }
    if (player->bomb.timer.HasTicked() && player->bomb.timer == 0)
    {
        g_ItemManager.CollectAll();
        g_Gui.Func417acd(0x4a1, "恋符「マスタースパーク」");
        player->bomb.duration = 300;
        player->unk_75b4.Set(360);
        for (sprite = player->bomb.sprites, i = 0; i < 4; i++, sprite++)
        {
            g_SpriteEngine->Func4051b0(sprite, i + 0x408);
            player->bomb.partPos[i] = player->position;
        }
        g_Unknown6d3f50.QueueSound(0x13, 0);
        player->moveScaleY = 0.3f;
        player->moveScaleX = 0.3f;
    }
    else
    {
        if (player->bomb.timer == 60)
        {
            ScreenEffect::Func42fd30(1, 60, 1, 7, 0);
        }
        else if (player->bomb.timer == 120)
        {
            ScreenEffect::Func42fd30(1, 200, 24, 0, 0);
        }
        if (player->bomb.timer.HasTicked() && player->bomb.timer.AsFrames() % 4 != 0)
        {
            player->bulletClearAreas[0][0] = 192.0f;
            player->bulletClearAreas[0][1] = player->position.y / 2.0f;
            player->bulletClearAreas[0][2] = 384.0f;
            player->bulletClearAreas[0][3] = player->position.y;
            player->bombHitSize[0].x = 384.0f;
            player->bombHitSize[0].y = player->position.y;
            player->bombHitPos[0].x = player->bulletClearAreas[0][0];
            player->bombHitPos[0].y = player->bulletClearAreas[0][1];
            player->bombHitDamage[0] = 12;
        }
        g_SpriteEngine->Step(&player->bomb.sprites[0]);
        g_SpriteEngine->Step(&player->bomb.sprites[1]);
        g_SpriteEngine->Step(&player->bomb.sprites[2]);
        g_SpriteEngine->Step(&player->bomb.sprites[3]);
    }
    player->unk_09e0 = 3;
    player->bomb.timer.Tick();
}

void DrawMarisaBBomb(Player *player)
{
    Sprite *curSprite;
    int i;
    float angle;

    DimPlayAreaForBomb(player);
    curSprite = player->bomb.sprites;
    for (i = 0; i < 4; i++)
    {
        angle = 0.62831855f * (float)i / 3.0f - 3.1415927f + 1.2566371f;
        curSprite->pos = player->position;
        curSprite->pos.x += cosf(angle) * curSprite->spriteData->height * curSprite->scaleY / 2.0f;
        curSprite->pos.y += sinf(angle) * curSprite->spriteData->height * curSprite->scaleY / 2.0f;
        angle = 1.5707964f - angle;
        curSprite->rotation = AddNormalizeAngle(angle, 3.1415927f);
        curSprite->pos.x += g_GameManager.arena[0];
        curSprite->pos.y += g_GameManager.arena[1];
        curSprite->pos.z = 0.0f;
        g_SpriteEngine->DrawRotated(curSprite);
        curSprite++;
    }
}
