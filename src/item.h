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
    char type;
    char active;
    // Set while it shows its own sprite; clear while above the top of
    // the play area, where it shows as a fading marker.
    char onScreen;
    // 0 falling, 1 drawn to the player, 2 thrown out by the player's
    // death.
    char state;
};
ASSERT_SIZE(Item, 0x144);

// The items, the global at 0x69e268.
class ItemManager
{
public:
    // Constructs the items (0x41f230).
    ItemManager();

    // Drops item `type` at `pos` in `state` into the next free slot.
    void SpawnItem(D3DXVECTOR3 *pos, int type, int state);
    // Moves the items: those the player dropped fly out over 60 frames,
    // those in state 1 or at full power above y 128 home in on the
    // player, others fall; collects those touching the graze box
    // (scoring, power, bombs, lives) and drops those that fall out.
    void UpdateItems();
    // Draws every active item to the player (state 1).
    void CollectAll();
    // Draws the items; those above the top as markers at its edge,
    // fading out with the distance.
    void DrawItems();

    Item items[0x201];
    // Where SpawnItem looks for a free slot first.
    int nextIndex;
    int unk_28948;
};
extern ItemManager g_ItemManager;

// The score of a power item at full power, by powerItemCombo.
extern int g_PowerItemBonus[31];
// The power each level of the shot types starts from.
extern int g_PowerLevels[];
