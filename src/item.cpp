// The file with the items, 0x41f230-0x420328: complete.

#include "ascii.h"
#include "bullet.h"
#include "gamemanager.h"
#include "item.h"
#include "player.h"
#include "unsorted.h"

// The game's data, as the original defines it here. The DLL build uses
// the original's copies instead.
#ifdef TH06_EXE
int g_PowerItemBonus[31] = {
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
    200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000,
    3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000,
    51200,
};

int g_PowerLevels[] = {
    8, 16, 32, 48, 64, 80, 96, 128, 999,
};

ItemManager g_ItemManager;
// Constructed and never used.
ChainElem g_UnusedChainA;
ChainElem g_UnusedChainB;
#endif // TH06_EXE

EXE_INLINE ItemManager::ItemManager()
{
}

void ItemManager::SpawnItem(D3DXVECTOR3 *pos, int type, int state)
{
    int i;
    Item *item;

    item = &this->items[this->nextIndex];
    for (i = 0; i < 0x200; i++)
    {
        this->nextIndex++;
        if (item->active)
        {
            if (this->nextIndex >= 0x200)
            {
                this->nextIndex = 0;
                item = this->items;
            }
            else
            {
                item++;
            }
            continue;
        }
        if (this->nextIndex >= 0x200)
        {
            this->nextIndex = 0;
        }
        item->active = 1;
        item->pos = *pos;
        item->velocity.x = 0.0f;
        item->velocity.y = -2.2f;
        item->velocity.z = 0.0f;
        item->type = type;
        item->state = state;
        item->timer.Set(0);
        if (state == 2)
        {
            item->target.x = g_Random.GetRandomF32ZeroToOne() * 288.0f + 48.0f;
            item->target.y = g_Random.GetRandomF32ZeroToOne() * 192.0f - 64.0f;
            item->target.z = 0.0f;
            item->velocity = item->pos;
        }
        g_SpriteEngine->StartScript(&item->sprite, type + 0x215);
        item->sprite.color = 0xffffffff;
        item->onScreen = 1;
        return;
    }
}

void ItemManager::UpdateItems()
{
    int i;
    int amount;
    float angle;
    // Whether any item was collected this frame.
    BOOL any;
    Item *entry = this->items;
    float t;
    static D3DXVECTOR3 itemSize(16.0f, 16.0f, 16.0f);

    any = FALSE;
    this->activeCount = 0;
    for (i = 0; i < 0x200; i++, entry++)
    {
        if (!entry->active)
        {
            continue;
        }
        this->activeCount++;
        if (entry->state == 2)
        {
            if (entry->timer < 60)
            {
                t = entry->timer.AsFloat() / 60.0f;
                entry->pos = t * entry->target + entry->velocity * (1.0f - t);
                goto touch;
            }
            if (entry->timer == 60)
            {
                entry->velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            }
            goto move;
        }
        else
        {
            if (entry->state == 1 || (g_GameManager.power >= 128 && g_Player.position.y < 128.0f))
            {
                angle = g_Player.AngleFrom(&entry->pos);
                SinCosScale(&entry->velocity, angle, 8.0f);
                entry->state = 1;
            }
            else
            {
                entry->velocity.x = 0.0f;
                entry->velocity.z = 0.0f;
                if (entry->velocity.y < -2.2f)
                {
                    entry->velocity.y = -2.2f;
                }
            }
        move:
            entry->pos += entry->velocity * g_App.frameAdvance;
            if (g_GameManager.arena[3] + 16.0f <= entry->pos.y)
            {
                entry->active = 0;
                g_GameManager.SubSubRank(3);
                continue;
            }
            if (entry->velocity.y < 3.0f)
            {
                entry->velocity.y += 0.03f * g_App.frameAdvance;
            }
            else
            {
                entry->velocity.y = 3.0f;
            }
        }
    touch:
        if (g_Player.TouchesGrazeBox(&entry->pos, &itemSize))
        {
            switch (entry->type)
            {
            case 0:
                if (g_GameManager.power >= 128)
                {
                    g_GameManager.powerItemCombo++;
                    if (g_GameManager.powerItemCombo >= ARRAY_SIZE(g_PowerItemBonus))
                    {
                        g_GameManager.powerItemCombo = ARRAY_SIZE(g_PowerItemBonus) - 1;
                    }
                    amount = g_PowerItemBonus[g_GameManager.powerItemCombo];
                    g_GameManager.score += amount;
                    g_AsciiText.AddPopup(&entry->pos, amount, amount >= 12800 ? 0xffffff00 : 0xffffffff);
                }
                else
                {
                    int prevLevel;
                    int level;

                    level = 0;
                    while (g_GameManager.power >= g_PowerLevels[level])
                    {
                        level++;
                    }
                    prevLevel = level;
                    g_GameManager.powerItemCombo = 0;
                    g_GameManager.power++;
                    if (g_GameManager.power >= 128)
                    {
                        g_GameManager.power = 128;
                        g_BulletManager.ClearAllToItems();
                        g_Gui.ShowFullPowerPopup(0);
                    }
                    g_GameManager.AddScore(10);
                    g_Gui.flags.powerChanged = 2;
                    while (g_GameManager.power >= g_PowerLevels[level])
                    {
                        level++;
                    }
                    if (level != prevLevel)
                    {
                        g_AsciiText.AddPopup(&entry->pos, -1, 0xff80c0ff);
                        g_SoundPlayer.QueueSound(0x1f, 0);
                    }
                    else
                    {
                        g_AsciiText.AddPopup(&entry->pos, 10, 0xffffffff);
                    }
                }
                g_GameManager.AddSubRank(1);
                break;
            case 1:
                switch (g_GameManager.difficulty)
                {
                case 0:
                case 1:
                    amount = ((int)entry->pos.y < 128) ? 100000 : 60000 - ((int)entry->pos.y - 128) * 100;
                    g_AsciiText.AddPopup(&entry->pos, amount, amount >= 100000 ? 0xffffff00 : 0xffffffff);
                    break;
                case 2:
                    amount = ((int)entry->pos.y < 128) ? 150000 : 100000 - ((int)entry->pos.y - 128) * 180;
                    g_AsciiText.AddPopup(&entry->pos, amount, amount >= 150000 ? 0xffffff00 : 0xffffffff);
                    break;
                case 3:
                    amount = ((int)entry->pos.y < 128) ? 200000 : 150000 - ((int)entry->pos.y - 128) * 270;
                    g_AsciiText.AddPopup(&entry->pos, amount, amount >= 200000 ? 0xffffff00 : 0xffffffff);
                    break;
                case 4:
                    amount = ((int)entry->pos.y < 128) ? 300000 : 200000 - ((int)entry->pos.y - 128) * 400;
                    g_AsciiText.AddPopup(&entry->pos, amount, amount >= 300000 ? 0xffffff00 : 0xffffffff);
                    break;
                }
                g_GameManager.score += amount;
                g_GameManager.pointItems++;
                g_GameManager.pointItemsTotal++;
                g_Gui.flags.pointsChanged = 2;
                if (entry->pos.y < 128.0f)
                {
                    g_GameManager.AddSubRank(30);
                }
                else
                {
                    g_GameManager.AddSubRank(3);
                }
                break;
            case 2:
                if (g_GameManager.power >= 128)
                {
                    g_GameManager.powerItemCombo += 8;
                    if (g_GameManager.powerItemCombo >= ARRAY_SIZE(g_PowerItemBonus))
                    {
                        g_GameManager.powerItemCombo = ARRAY_SIZE(g_PowerItemBonus) - 1;
                    }
                    amount = g_PowerItemBonus[g_GameManager.powerItemCombo];
                    g_GameManager.score += amount;
                    g_AsciiText.AddPopup(&entry->pos, amount, amount >= 12800 ? 0xffffff00 : 0xffffffff);
                }
                else
                {
                    int prevLevel;
                    int level;

                    level = 0;
                    while (g_GameManager.power >= g_PowerLevels[level])
                    {
                        level++;
                    }
                    prevLevel = level;
                    g_GameManager.power += 8;
                    if (g_GameManager.power >= 128)
                    {
                        g_GameManager.power = 128;
                        g_BulletManager.ClearAllToItems();
                        g_Gui.ShowFullPowerPopup(0);
                    }
                    g_Gui.flags.powerChanged = 2;
                    g_GameManager.AddScore(10);
                    while (g_GameManager.power >= g_PowerLevels[level])
                    {
                        level++;
                    }
                    if (level != prevLevel)
                    {
                        g_AsciiText.AddPopup(&entry->pos, -1, 0xff80c0ff);
                        g_SoundPlayer.QueueSound(0x1f, 0);
                    }
                    else
                    {
                        g_AsciiText.AddPopup(&entry->pos, 10, 0xffffffff);
                    }
                }
                break;
            case 3:
                if (g_GameManager.bombs < 8)
                {
                    g_GameManager.bombs++;
                    g_Gui.flags.bombsChanged = 2;
                }
                g_GameManager.AddSubRank(5);
                break;
            case 5:
                if (g_GameManager.lives < 8)
                {
                    g_GameManager.lives++;
                    g_Gui.flags.livesChanged = 2;
                }
                g_GameManager.AddSubRank(200);
                g_SoundPlayer.QueueSound(0x1c, 0);
                break;
            case 4:
                if (g_GameManager.power < 128)
                {
                    g_BulletManager.ClearAllToItems();
                    g_Gui.ShowFullPowerPopup(0);
                    g_SoundPlayer.QueueSound(0x1f, 0);
                    g_AsciiText.AddPopup(&entry->pos, -1, 0xff80c0ff);
                }
                g_GameManager.power = 128;
                g_GameManager.AddScore(1000);
                g_AsciiText.AddPopup(&entry->pos, 1000, 0xffffffff);
                g_Gui.flags.powerChanged = 2;
                break;
            case 6:
                amount = g_GameManager.graze / 3 * 10 + 500;
                if (g_Player.bomb.running)
                {
                    amount = 100;
                }
                g_GameManager.score += amount;
                g_AsciiText.AddPopup2(&entry->pos, amount, 0xffffffff);
                break;
            }
            entry->active = 0;
            any = TRUE;
            continue;
        }
        entry->timer.Tick();
        g_SpriteEngine->Step(&entry->sprite);
    }
    if (any)
    {
        g_SoundPlayer.QueueSound(0x15, 0);
    }
}

void ItemManager::CollectAll()
{
    Item *entry = this->items;
    int i;

    for (i = 0; i < 0x200; i++, entry++)
    {
        if (!entry->active)
        {
            continue;
        }
        entry->state = 1;
    }
}

void ItemManager::DrawItems()
{
    Item *entry = this->items;
    int i;
    int alpha;

    for (i = 0; i < 0x200; i++, entry++)
    {
        if (!entry->active)
        {
            continue;
        }
        entry->sprite.pos.x = g_GameManager.arena[0] + entry->pos.x;
        entry->sprite.pos.y = g_GameManager.arena[1] + entry->pos.y;
        entry->sprite.pos.z = 0.01f;
        if (entry->pos.y < -8.0f)
        {
            entry->sprite.pos.y = 8.0f + g_GameManager.arena[1];
            if (entry->onScreen)
            {
                g_SpriteEngine->SetSprite(&entry->sprite, entry->type + 0x207);
                entry->onScreen = 0;
            }
            alpha = 255 - (int)((8.0f - entry->pos.y) * 255.0f / 128.0f);
            if (alpha < 64)
            {
                alpha = 64;
            }
            entry->sprite.color = entry->sprite.color & 0xffffff | alpha << 24;
        }
        else if (!entry->onScreen)
        {
            g_SpriteEngine->SetSprite(&entry->sprite, entry->type + 0x200);
            entry->onScreen = 1;
            entry->sprite.color = 0xffffffff;
        }
        g_SpriteEngine->Draw(&entry->sprite);
    }
}
