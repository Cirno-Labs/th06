// Enemy bullets, drawn with data/etama3.anm and etama4.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "sprite.h"
#include "timer.h"

// Runs the enemy bullets, the global at 0x5a5ff8.
// A bullet type's sprites: the bullet's and those of its spawning
// effects.
struct BulletTypeSprites
{
    Sprite sprite0;
    Sprite sprite1;
    Sprite sprite2;
    Sprite sprite3;
    Sprite sprite4;
    // The size of the bullet's hitbox, and its sprite's height.
    D3DXVECTOR3 hitboxSize;
    BYTE unk_55c;
    BYTE height;
    BYTE unk_55e[0x2];
};
ASSERT_SIZE(BulletTypeSprites, 0x560);

// One enemy bullet.
struct Bullet
{
    BulletTypeSprites sprites;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 velocity;
    // Added to the velocity (flag 0x10).
    D3DXVECTOR3 accel;
    float speed;
    float unk_588;
    float unk_58c;
    float angle;
    float unk_594;
    float unk_598;
    Timer timer;
    int unk_5a8;
    int unk_5ac;
    int unk_5b0;
    int unk_5b4;
    // BulletShooter::flags.
    WORD flags;
    short color;
    WORD unk_5bc;
    // 0 unused; 1 flying; 2 to 4 spawning (flags 2, 4, 8); 5 fading out.
    WORD state;
    BYTE unk_5c0;
    BYTE unk_5c1;
    BYTE unk_5c2;
    BYTE unk_5c3;
};
ASSERT_SIZE(Bullet, 0x5c4);

// What a bullet pattern fires: count1 bullets around, count2 in a row,
// aimed by `aimMode`.
struct BulletShooter
{
    // The bullet type, and its color.
    short sprite;
    short color;
    D3DXVECTOR3 pos;
    float angle1;
    float angle2;
    // The speed of the first and the last of a row.
    float speed1;
    float speed2;
    // Extra values for the flags' behaviors.
    float exFloats[4];
    int exInts[4];
    DWORD unk_40;
    short count1;
    short count2;
    WORD aimMode;
    WORD unk_4a;
    // Bullet::flags; 0x200 plays `sound`.
    int flags;
    int sound;
};

// The scripts of each bullet type's five sprites.
extern int g_BulletTypeScripts[10][5];

// Sprite offsets by color, for bullets up to 16 and up to 32 high.
extern int g_BulletColorOffsets16[16];
extern int g_BulletColorOffsets32[16];

// One enemy laser: a beam from `pos` along `angle`, between startOffset
// and endOffset.
struct Laser
{
    Sprite sprite0;
    Sprite sprite1;
    D3DXVECTOR3 pos;
    float angle;
    float startOffset;
    float endOffset;
    float unk_238;
    float width;
    float speed;
    int unk_244;
    int unk_248;
    int unk_24c;
    int unk_250;
    int unk_254;
    int active;
    Timer timer;
    // BulletShooter::flags and color.
    WORD flags;
    short color;
    // 2 and up: ending.
    BYTE state;
    BYTE unk_26d[0x3];
};
ASSERT_SIZE(Laser, 0x270);

class BulletManager
{
public:
    // Constructs the members and resets.
    BulletManager();

    // Zeroes everything.
    void Reset();
    // Ends every bullet and laser: with `toItems` zero they fade out;
    // with it nonzero bullets turn into items of type 6, as do points
    // every 32 along each laser.
    void ClearAll(int toItems);
    // Spawns bullet (`col`, `row`) of `shooter`'s pattern, aimed at
    // `aimAngle` by the aiming modes that use it. Returns 0, or 1 if no
    // bullet is free.
    int SpawnBullet(BulletShooter *shooter, int col, int row, float aimAngle);
    // Fires `shooter`'s pattern, aimed at the player, until no bullet is
    // free, and plays its sound with flag 0x200. Returns 0.
    int FirePattern(BulletShooter *shooter);
    // Starts a laser from `shooter` in a free slot: exFloats give its
    // start and end offsets and width, exInts its timings. Returns it.
    Laser *SpawnLaser(BulletShooter *shooter);
    // ClearAll(1).
    void ClearAllToItems();
    // Like ClearAll, but each bullet scores a bonus, from 2000 up by 10
    // per bullet to `maxBonus`, shown as a popup, and the total is shown
    // through g_Gui.Func41735a. Returns the total.
    int ClearAllWithBonus(int maxBonus, int toItems);

    // Resets, remembers `unk`, picks g_Unknown476438's table by option
    // bit 0, and adds the update element (priority 0xb) and the draw
    // element (priority 9). Returns 0, or -1 if adding fails.
    static int RegisterChain(int unk);
    static int OnUpdate(BulletManager *mgr);
    static int OnDraw(BulletManager *mgr);
    // Loads the bullet sprites ("data/etama3.anm" and "etama4.anm", unless
    // the scene is 3), sets up the bullet types' sprites and hitboxes and
    // clears the items. Returns 0, or -1 if loading fails.
    static int AddedCallback(BulletManager *mgr);
    // Frees the bullet sprites, unless the scene is 3. Returns 0.
    static int DeletedCallback(BulletManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();

    BulletTypeSprites typeSprites[0x10];
    Bullet bullets[0x280];
    Laser lasers[0x40];
    // Where SpawnBullet looks for a free bullet first.
    int nextBullet;
    DWORD unk_f5c04;
    Timer unk_f5c08;
    int unk_f5c14;
};
ASSERT_SIZE(BulletManager, 0xf5c18);
extern BulletManager g_BulletManager;

// Draws a bullet with the sprite for its state (spawning effects, fading
// out), turned along its angle where the sprite asks for it; Func416c50
// places it in the play area.
void DrawBullet(Bullet *bullet);
void Func416c50(Bullet *bullet);

// Two tables; BulletManager::RegisterChain points g_Unknown476438 at the
// first while option bit 0 is clear, otherwise at the second.
extern DWORD g_Unknown476358[0x1c];
extern DWORD g_Unknown4763c8[0x1c];
extern DWORD *g_Unknown476438;

// BulletManager's statically allocated chain elements.
extern ChainElem g_BulletCalcChain;
extern ChainElem g_BulletDrawChain;
