// The file with the items, from 0x41f230.

#include "gamemanager.h"
#include "item.h"
#include "unsorted.h"

ItemManager::ItemManager()
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
            item->target.x = g_Rng.GetRandomF32ZeroToOne() * 288.0f + 48.0f;
            item->target.y = g_Rng.GetRandomF32ZeroToOne() * 192.0f - 64.0f;
            item->target.z = 0.0f;
            item->velocity = item->pos;
        }
        g_SpriteEngine->StartScript(&item->sprite, type + 0x215);
        item->sprite.color = 0xffffffff;
        item->unk_142 = 1;
        return;
    }
}
