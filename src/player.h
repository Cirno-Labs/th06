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
    D3DXVECTOR3 pos;
    D3DXVECTOR3 size;
    // A laser keeps its offset from its option in velocity.z and
    // unk_134.
    D3DXVECTOR3 velocity;
    float unk_134;
    float speed;
    // The direction it flies in.
    float angle;
    Timer timer;
    short unk_14c;
    // 0 unused, 1 flying, 2 hit (drawn by DrawHitBullets).
    short state;
    // How it moves: 1 homing on unk_0a1c, 2 accelerating up, 3 a laser
    // kept on an option.
    short type;
    // A laser's: its entry in laserTimers, and its option, from 1.
    short laserIndex;
    short option;
    short unk_156;
};
ASSERT_SIZE(PlayerBullet, 0x158);

class Player;

// A shot type's movement speeds (straight, straight focused, diagonal,
// diagonal focused) and its fire functions, unfocused and focused.
// They fill `bullet` with shot `index` and return 0, or 1 for the
// frame's last shot; -1 skips the shot, keeping `bullet`, and -2 stops.
struct ShotTypeInfo
{
    float speed;
    float speedFocused;
    float speedDiagonal;
    float speedDiagonalFocused;
    int (*fire)(Player *player, PlayerBullet *bullet, int index, int frame);
    int (*fireFocused)(Player *player, PlayerBullet *bullet, int index, int frame);
};
ASSERT_SIZE(ShotTypeInfo, 0x18);

// A shot type's bomb callbacks.
struct BombInfo
{
    void (*update)(Player *player);
    void (*draw)(Player *player);
};

// The player, the global at 0x6ca628. Its added callback loads
// "data/player00.anm" or "player01.anm".
class Player
{
public:
    // Zeroes the player, remembers `unk` and adds the update element
    // (priority 7) and two draw elements (priorities 5 and 7). Returns
    // 0, or -1 if adding fails.
    static int RegisterChain(BYTE unk);
    // The update element's callback, unless g_GameManager.unk_002c is
    // set: runs the bomb or starts one (X pressed, bombs left), plays
    // the death (state 2: drops items, takes a life, respawns at the
    // bottom) and the respawn (state 1) and the blinking invulnerability
    // after it (state 3), then moves the player. Returns 1.
    static int OnUpdate(Player *player);
    // Moves the player by the direction held, at the focused speeds
    // while button 4 is held, within g_GameManager.playerArea; picks the
    // banking script; moves the hitboxes and the options, sliding them
    // in while focusing; fires while button 1 is held. Returns 0.
    int HandleInput();
    // Starts fireTimer unless it runs.
    static void StartFiring(Player *player);
    // Tests a `size` box centred on `pos` (an enemy bullet's): returns 2
    // if it touches a bullet-clearing area, else 1 if it touches the
    // hitbox, killing the player unless it is dying or invulnerable,
    // else 0.
    int CheckHit(D3DXVECTOR3 *pos, D3DXVECTOR3 *size);
    // Like CheckHit with the box grown by 20 on each side: returns 2 in a
    // bullet-clearing area, else 1 if it touches the hitbox while the
    // player is alive, grazing it, else 0.
    int CheckGraze(D3DXVECTOR3 *pos, D3DXVECTOR3 *size);
    // Grazes a bullet at `pos`: counts it (unless bombing), bursts an
    // effect between it and the player, scores 500 and raises the rank.
    void Graze(D3DXVECTOR3 *pos);
    // Kills the player: bursts effects, starts the death (state 2), plays
    // sound 4 and counts the death.
    void Die();
    // Fires the shot type's shots for `frame` of fireTimer into free
    // bullets.
    static void FireShots(Player *player, int frame);
    // Moves the player's bullets: homing ones steer like Reimu A's orbs,
    // others speed up to 10, rising ones accelerate, lasers stay on
    // their option, stretched to the top. Ends those that leave the
    // play area or whose script ends.
    static void UpdateBullets(Player *player);
    // Runs fireTimer: fires on each new frame, except during Marisa B's
    // bomb, and stops it after 30 frames or on dying. Returns 0.
    static int UpdateFiring(Player *player);
    // The draw element's callback: draws the flying bullets, the bomb,
    // and unless the game is over (g_GameManager.unk_1820) the player
    // and, while alive, its options. Returns 1.
    static int OnDraw(Player *player);
    // Runs DrawHitBullets. Returns 1.
    static int OnDraw2(Player *player);
    // Draws the flying bullets (state 1), turned along their angle.
    static void DrawBullets(Player *player);
    // Draws the bullets that hit (state 2).
    static void DrawHitBullets(Player *player);
    // Loads the character's sprites ("data/player00.anm" or
    // "player01.anm", unless the scene is 3) and sets the player up at
    // the bottom of the play area, respawning, with the shot type's
    // speeds, fire functions and bomb. Returns 0, or -1 if loading fails.
    static int AddedCallback(Player *player);
    // Frees the player's sprites, unless the scene is 3. Returns 0.
    static int DeletedCallback(Player *player);
    // Removes the player's three chain elements.
    static void RemoveChain();

    // The player's sprite, and the options' (stepped while optionState
    // is set).
    Sprite sprite;
    Sprite optionSprites[2];
    DWORD unk_0330[0x44];
    D3DXVECTOR3 position;
    DWORD unk_044c[0x3];
    // The corners of the hitbox and of the graze box, and their half
    // sizes.
    D3DXVECTOR3 hitboxMin;
    D3DXVECTOR3 hitboxMax;
    D3DXVECTOR3 grazeMin;
    D3DXVECTOR3 grazeMax;
    D3DXVECTOR3 hitboxSize;
    D3DXVECTOR3 grazeSize;
    // Where the two options are.
    D3DXVECTOR3 optionPos[2];
    // The bomb's hitboxes against enemies: centres, sizes, the damage
    // each deals and the hits each has scored.
    D3DXVECTOR3 bombHitPos[32];
    D3DXVECTOR3 bombHitSize[32];
    int bombHitDamage[32];
    int bombHitCount[32];
    // Areas that cancel bullets, as centre x and y, width and height;
    // unused while the width is a sentinel.
    float bulletClearAreas[16][4];
    // Clock the lasers; counted down while nonzero.
    Timer laserTimers[2];
    // Scale the player's movement: 0.3 during Marisa B's bomb, else 1.
    float moveScaleX;
    float moveScaleY;
    // While the player dies: frames left to bomb, as a deathbomb.
    int unk_09d8;
    // Frames left of clearing all bullets after a respawn.
    int unk_09dc;
    // 0 normally, 1 respawning, 2 dying, 3 invulnerable (the bombs set
    // it every frame).
    char unk_09e0;
    // RegisterChain's argument.
    BYTE unk_09e1;
    // The options: 0 none (power below 8), 1 out, 2 sliding in while
    // focusing, 3 in, 4 sliding out.
    char optionState;
    // Set while focusing (button 4).
    char focused;
    DWORD unk_09e4;
    // Clocks the options sliding in and out.
    Timer optionTimer;
    // The shot type's, from g_ShotTypes; AddedCallback works out the
    // diagonal speeds.
    ShotTypeInfo shotTypeInfo;
    // The direction held: 0 none, 1 up, 2 down, 3 left, 4 right, 5 up
    // left, 6 up right, 7 down left, 8 down right.
    int direction;
    // The last frame's movement.
    float velocityX;
    float velocityY;
    // The last frame's buttons.
    WORD prevButtons;
    WORD unk_0a1a;
    // What Reimu A's orbs home in on; x is -100 or less when there is
    // nothing.
    D3DXVECTOR3 unk_0a1c;
    PlayerBullet bullets[0x50];
    // Runs while firing: -1 when idle, set going by StartFiring and
    // stopped after 30 frames.
    Timer fireTimer;
    // Counts the death and respawn; the invulnerability counts down
    // (set to 360 frames when a bomb starts).
    Timer unk_75b4;
    // Copies of shotTypeInfo's.
    int (*fire)(Player *player, PlayerBullet *bullet, int index, int frame);
    int (*fireFocused)(Player *player, PlayerBullet *bullet, int index, int frame);
    // Whether a bomb runs; cleared when it ends.
    int bombing;
    // The bomb running: its length in frames, and its clock.
    int bombDuration;
    Timer bombTimer;
    // The shot type's bomb callbacks.
    void (*bombUpdate)(Player *player);
    void (*bombDraw)(Player *player);
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
// One shot of a shot type's power level.
struct ShotData
{
    // Fired on frames where frame % interval is phase; a laser instead
    // runs laserTimers[phase] for `interval` frames.
    short interval;
    short phase;
    // Where it starts, from the player or its option.
    float offsetX;
    float offsetY;
    float sizeX;
    float sizeY;
    float angle;
    float speed;
    short unk_1c;
    // 0 for the player, else the option, from 1.
    BYTE option;
    // PlayerBullet::type.
    BYTE type;
    // Its script, and its sound or -1.
    short script;
    short sound;
};
ASSERT_SIZE(ShotData, 0x24);

// A shot type's shots by power level: entries of a count, the power
// they last up to and their data.
struct ShotTable
{
    int count;
    int maxPower;
    ShotData *shots;
};

// Fires shot `index` of a shot type's table for the current power into
// `bullet`, if it fires on `frame`. Returns 0, or 1 for the level's last
// shot; when it does not fire, -1, or -2 for the last.
int FireShot(Player *player, PlayerBullet *bullet, int index, int frame, ShotTable *table);

// The four shot types' fire functions, in g_ShotTypes.
int FireShotType0(Player *player, PlayerBullet *bullet, int index, int frame);
int FireShotType1(Player *player, PlayerBullet *bullet, int index, int frame);
int FireShotType2(Player *player, PlayerBullet *bullet, int index, int frame);
int FireShotType3(Player *player, PlayerBullet *bullet, int index, int frame);

// By character * 2 + shot type.
extern ShotTypeInfo g_ShotTypes[4];
extern BombInfo g_Bombs[4];

// Their shot tables.
extern ShotTable g_ShotTable0;
extern ShotTable g_ShotTable1;
extern ShotTable g_ShotTable2;
extern ShotTable g_ShotTable3;
