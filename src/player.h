// The player and its shots.

#pragma once

#include "common.h"

class Player;
struct PlayerBullet;
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
