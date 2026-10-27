// The player and its shots.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"
#include "timer.h"

// One of the player's bullets.
struct PlayerBullet
{
    Sprite sprite;
    DWORD unk_110[0xb];
    // The direction it flies in.
    float angle;
    DWORD unk_140[0x3];
    short unk_14c;
    // 2 for those DrawHitBullets draws.
    short type;
    DWORD unk_150[0x2];
};
ASSERT_SIZE(PlayerBullet, 0x158);

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
    // Runs DrawHitBullets. Returns 1.
    static int OnDraw2(Player *player);
    // Draws the player's bullets of type 2.
    static void DrawHitBullets(Player *player);
    // Loads the player's sprites.
    static int AddedCallback(Player *player);
    // Frees the player's sprites, unless the scene is 3. Returns 0.
    static int DeletedCallback(Player *player);
    // Removes the player's three chain elements.
    static void RemoveChain();

    DWORD unk_0000[0x110];
    D3DXVECTOR3 position;
    DWORD unk_044c[0x1b];
    // The bomb's hitboxes against enemies: centres, sizes, the damage
    // each deals and the hits each has scored.
    D3DXVECTOR3 bombHitPos[32];
    D3DXVECTOR3 bombHitSize[32];
    int bombHitDamage[32];
    int bombHitCount[32];
    // Areas that cancel bullets, as centre x and y, width and height;
    // unused while the width is a sentinel.
    float bulletClearAreas[16][4];
    DWORD unk_09b8[0x6];
    // Scale the player's movement: 0.3 during Marisa B's bomb, else 1.
    float moveScaleX;
    float moveScaleY;
    DWORD unk_09d8[0x2];
    // 3 while a bomb runs.
    BYTE unk_09e0;
    // RegisterChain's argument.
    BYTE unk_09e1;
    WORD unk_09e2;
    DWORD unk_09e4[0xe];
    // What Reimu A's orbs home in on; x is -100 or less when there is
    // nothing.
    D3DXVECTOR3 unk_0a1c;
    PlayerBullet bullets[0x50];
    DWORD unk_75a8[0x3];
    // Set to 360 frames when a bomb starts.
    Timer unk_75b4;
    DWORD unk_75c0[0x2];
    // Cleared when the bomb ends.
    int unk_75c8;
    // The bomb running: its length in frames, and its clock.
    int bombDuration;
    Timer bombTimer;
    DWORD unk_75dc[0x2];
    // Per-bomb state for up to 8 of a bomb's parts (Reimu A's orbs,
    // ...): whether each is live (Reimu A: 1 flying, then counting up to
    // 30 after it bursts), its speed, position and velocity, and four
    // sprites each.
    int bombActive[8];
    float bombSpeed[8];
    D3DXVECTOR3 bombPos[8];
    D3DXVECTOR3 bombVelocity[8];
    Sprite bombSprites[32];
    // The update element and the two draw elements.
    ChainElem *calcChain;
    ChainElem *drawChain;
    ChainElem *drawChain2;
};
ASSERT_SIZE(Player, 0x98f0);

// Reimu A's bomb update callback: ends the bomb after its duration;
// otherwise starts it (the spell card name, 300 frames), launches an orb
// every 16 frames from frame 76 to 172, steers the live orbs towards
// unk_0a1c or the player, keeping their hitboxes and bullet clearing
// on them, and bursts each after 100 hits or in the bomb's last 30
// frames.
void UpdateReimuABomb(Player *player);
// Reimu A's bomb draw callback: dims the play area and draws each live
// orb's four sprites at its position.
void DrawReimuABomb(Player *player);
// Darkens the play area while a bomb runs: to alpha 176 over its first
// 60 frames, and back over its last 60.
void DimPlayAreaForBomb(Player *player);
// Reimu B's bomb update callback: ends the bomb after its duration;
// otherwise starts it (the spell card name, 140 frames) with two
// vertical and two horizontal pillars through the player, then on every
// other frame keeps their hitboxes and bullet clearing on them.
void UpdateReimuBBomb(Player *player);
// Reimu B's bomb draw callback: dims the play area and draws the four
// pillars at their positions.
void DrawReimuBBomb(Player *player);
// Marisa A's bomb update callback: ends the bomb after its duration;
// otherwise starts it (the spell card name, 250 frames) with eight stars
// flying out from the player, then moves them, keeping their hitboxes
// and bullet clearing on them two frames in three.
void UpdateMarisaABomb(Player *player);
// Marisa A's bomb draw callback: dims the play area and draws each star
// three times, shrinking along its trail.
void DrawMarisaABomb(Player *player);
// Marisa B's bomb update callback: ends the bomb after its duration,
// restoring the player's speed; otherwise starts it (the spell card
// name, 300 frames) slowing the player, then keeps a beam's hitbox and
// bullet clearing above the player three frames in four.
void UpdateMarisaBBomb(Player *player);
// Marisa B's bomb draw callback: dims the play area and draws the four
// beam sprites fanned out above the player, each turned along its
// direction.
void DrawMarisaBBomb(Player *player);
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
