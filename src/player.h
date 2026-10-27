// The player and its shots.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "timer.h"

struct PlayerBullet;

// The player, the global at 0x6ca628. Its added callback loads
// "data/player00.anm" or "player01.anm".
class Player
{
public:
    // Zeroes the player, remembers `unk` and adds the update element
    // (priority 7) and two draw elements (priorities 5 and 7). Returns
    // 0, or -1 if adding fails.
    static int RegisterChain(BYTE unk);
    static int OnUpdate(Player *player);
    static int OnDraw(Player *player);
    // Runs Func429bb0. Returns 1.
    static int OnDraw2(Player *player);
    // Draws the player's bullets of type 2.
    static void Func429bb0(Player *player);
    // Loads the player's sprites.
    static int AddedCallback(Player *player);
    // Frees the player's sprites, unless the scene is 3. Returns 0.
    static int DeletedCallback(Player *player);
    // Removes the player's three chain elements.
    static void RemoveChain();

    DWORD unk_0000[0x278];
    BYTE unk_09e0;
    // RegisterChain's argument.
    BYTE unk_09e1;
    WORD unk_09e2;
    DWORD unk_09e4[0x1af4];
    Timer unk_75b4;
    DWORD unk_75c0[0x8c9];
    // The update element and the two draw elements.
    ChainElem *calcChain;
    ChainElem *drawChain;
    ChainElem *drawChain2;
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
