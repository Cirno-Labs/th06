// The player and its shots.

#pragma once

#include <windows.h>

#include "common.h"

struct PlayerBullet;

// The player, the global at 0x6ca628. Its added callback loads
// "data/player00.anm" or "player01.anm".
class Player
{
public:
    // Frees the player's sprites, unless the scene is 3. Returns 0.
    static int DeletedCallback(Player *player);

    DWORD unk_0000[0x263c];
};
ASSERT_SIZE(Player, 0x98f0);
extern Player g_Player;
// A shot type's shots by power level: entries of a count, the power
// they last up to and their data.
struct ShotTable;

// Fires shot `index` of a shot type for `frame` at the current power.
int FireShot(Player *player, PlayerBullet *bullet, int index, int frame, ShotTable *table);

// The four shot types' fire functions, in the per-shot-type table at
// 0x476728 that also holds their movement speeds.
int FireShotType0(Player *player, PlayerBullet *bullet, int index, int frame);
int FireShotType1(Player *player, PlayerBullet *bullet, int index, int frame);
int FireShotType2(Player *player, PlayerBullet *bullet, int index, int frame);
int FireShotType3(Player *player, PlayerBullet *bullet, int index, int frame);

// Their shot tables.
extern ShotTable g_ShotTable0;
extern ShotTable g_ShotTable1;
extern ShotTable g_ShotTable2;
extern ShotTable g_ShotTable3;
