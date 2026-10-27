// The items enemies and the player drop (power, points, ...).

#pragma once

#include "common.h"
#include "sprite.h"
#include "timer.h"

// One item.
struct Item
{
    Sprite sprite;
    D3DXVECTOR3 pos;
    // How it moves; for one dropped by the player (state 2), where it
    // started instead.
    D3DXVECTOR3 velocity;
    // Where one dropped by the player flies to.
    D3DXVECTOR3 target;
    Timer timer;
    // Its kind: picks its script, 0x215 + type.
    BYTE type;
    char active;
    BYTE unk_142;
    // 0 falling, 1 drawn to the player, 2 thrown out by the player's
    // death.
    BYTE state;
};
ASSERT_SIZE(Item, 0x144);

// The items, the global at 0x69e268.
class ItemManager
{
public:
    // Constructs the items (0x41f230).
    ItemManager();

    // Drops item `type` at `pos` in `state` into the next free slot.
    void Func41f290(D3DXVECTOR3 *pos, int type, int state);
    // Draws every active item to the player (state 1).
    void Func420130();

    Item items[0x201];
    // Where Func41f290 looks for a free slot first.
    int nextIndex;
    int unk_28948;
};
extern ItemManager g_ItemManager;
