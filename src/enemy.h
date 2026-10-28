// Enemies, drawn with the stage's data/stgNenm.anm and stgNenm2.anm.

#pragma once

#include <windows.h>

#include "chain.h"
#include "bullet.h"
#include "common.h"
#include "effect.h"
#include "enemyscript.h"
#include "sprite.h"
#include "timer.h"

// Enemy's flags, in bytes.
struct EnemyFlags
{
    // How the enemy moves: 1 by angle and speed, 2 towards
    // moveStart + moveDelta over moveDuration.
    unsigned char moveMode : 2;
    unsigned char unk2 : 4;
    // Mirrors the horizontal movement.
    unsigned char mirrored : 1;
    // In use: EnemyManager::Spawn takes the first enemy without it.
    unsigned char active : 1;
    // Can be shot and killed.
    unsigned char unk8 : 1;
    // Touching the player costs it 10 life.
    unsigned char unk9 : 1;
    // Has been inside the play area; leaving it again removes the
    // enemy.
    unsigned char onScreen : 1;
    // A boss: its life shows in the interface's bar.
    unsigned char unk11 : 1;
    // Takes the player's shots' damage.
    unsigned char unk12 : 1;
    // What dying does, in EnemyManager::OnUpdate.
    unsigned char unk13 : 3;
    // Keeps the position within lowerBound and upperBound.
    unsigned char clampPos : 1;
    // Turns the main sprite with the enemy's angle.
    unsigned char rotateSprite : 1;
    unsigned char unk18 : 1;
    // OnDraw skips the enemy.
    unsigned char hidden : 1;
    unsigned char unk20 : 4;
    BYTE unk_e53;
};

// One enemy (0xec8 bytes).
struct Enemy
{
    // Inline, but too large to expand: callers call the copy at 0x411240
    // and still reserve its inlines' slots.
    Enemy()
    {
    }

    Sprite sprite;
    Sprite sprites[8];
    EnemyScriptState ctx;
    // Contexts saved by calls.
    EnemyScriptState savedCtx[8];
    DWORD unk_c3c;
    DWORD unk_c40;
    // The subroutine the script runs when the enemy dies; negative for
    // none.
    int deathSub;
    DWORD unk_c48[0x8];
    // Set by the timeline.
    int unk_c68;
    D3DXVECTOR3 pos;
    // The size of the enemy's hitbox.
    D3DXVECTOR3 hitboxSize;
    D3DXVECTOR3 velocity;
    // Movement with moveMode 1.
    float angle;
    float angularVelocity;
    float speed;
    float acceleration;
    DWORD unk_ca0[0x3];
    // Movement with moveMode 2: from moveStart by moveDelta over
    // moveDuration frames, counted down by moveTimer.
    D3DXVECTOR3 moveDelta;
    D3DXVECTOR3 moveStart;
    Timer moveTimer;
    int moveDuration;
    float unk_cd4;
    float unk_cd8;
    short unk_cdc;
    short unk_cde;
    short unk_ce0;
    short unk_ce2;
    // Script operand -10024.
    int life;
    int maxLife;
    int score;
    // Counts the frames, unless the game is paused.
    Timer unk_cf0;
    // The sprite's color when spawned.
    D3DCOLOR color;
    BulletShooter shooter;
    DWORD unk_d54;
    Timer unk_d58;
    BulletShooter laserShooter;
    DWORD unk_db8[0x21];
    // The effects the enemy's death spawns.
    BYTE deathEffect;
    BYTE unk_e3d;
    BYTE unk_e3e;
    // Passed to EnemyManager::Spawn.
    char unk_e3f;
    BYTE unk_e40;
    // Counts down the frames the sprite flashes for after a hit.
    BYTE hitFlashTimer;
    BYTE unk_e42[0x2];
    Timer unk_e44;
    EnemyFlags flags;
    DWORD unk_e54[0x3];
    // The bounds of the position, with flags.clampPos.
    float lowerBound[2];
    float upperBound[2];
    // Effects that follow the enemy.
    Effect *effects[12];
    int effectCount;
    float unk_ea4;
    // When the life falls below lifeCallbackLife, the script runs
    // lifeCallbackSub; -1 for none.
    int lifeCallbackLife;
    int lifeCallbackSub;
    // When unk_cf0 reaches timerCallbackTime, the script runs
    // timerCallbackSub; -1 for none.
    int timerCallbackTime;
    int timerCallbackSub;
    DWORD unk_eb8;
    Timer unk_ebc;

    void Func4121b0();
    // Once the life falls below lifeCallbackLife: sets it to that,
    // starts lifeCallbackSub, makes the death subroutine the timer
    // callback's and clears the other enemies. Returns 1 then, else 0.
    int CheckLifeCallback();
    int Func411f40();
    // Keeps the position within the bounds, with flags.clampPos.
    void ClampPos();
    // Moves by the velocity, mirrored with flags.mirrored.
    void Move();
};
ASSERT_SIZE(Enemy, 0xec8);

// Moves the effects following `enemy` with it, widening them up to
// unk_ea4 and turning them.
void MoveEnemyEffects(Enemy *enemy);
// Releases the effects following `enemy`.
void ReleaseEnemyEffects(Enemy *enemy);

// Runs the enemies, the global at 0x4b79c8.
class EnemyManager
{
public:
    // Constructs the members and initializes.
    EnemyManager();

    // Sets everything up for a stage.
    void Initialize();
    // Runs the stage's enemy timeline up to the current frame: spawns
    // enemies, starts dialogue, waits for bosses. Raises the rank now
    // and then.
    void RunTimeline();

    // Initializes, remembers the stage's two enemy sprite files and adds
    // the update element (priority 9) and the draw element (priority 6).
    // Returns 0, or -1 if adding fails.
    static int RegisterChain(char *anmPath, char *anmPath2);
    static int OnUpdate(EnemyManager *mgr);
    static int OnDraw(EnemyManager *mgr);
    // Loads the enemy sprites (either path may be NULL) and picks
    // unk_ee5b8 and unk_ee5ba at random. Returns 0, or -1 if loading
    // fails.
    static int AddedCallback(EnemyManager *mgr);
    // Frees the enemy sprites. Returns 0.
    static int DeletedCallback(EnemyManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();

    // Spawns an enemy from the template at `pos`, starting script
    // subroutine `sub` and running its first frame. `life` and `score`
    // replace the script's when not negative. Returns the enemy, or the
    // last one if all are in use.
    Enemy *Spawn(int sub, D3DXVECTOR3 *pos, short life, short unk_e3f, int score);

    // The stage's two enemy sprite files.
    char *anmPath;
    char *anmPath2;
    // The template new enemies start from, and the enemies.
    Enemy enemyTemplate;
    Enemy enemies[0x101];
    // Enemies the timeline refers to.
    Enemy *bosses[0x8];
    // Below 3 and below 8.
    WORD unk_ee5b8;
    WORD unk_ee5ba;
    // Counts the enemies in use, each frame.
    int enemyCount;
    DWORD unk_ee5c0;
    int unk_ee5c4;
    int unk_ee5c8;
    DWORD unk_ee5cc[0x2];
    int unk_ee5d4;
    DWORD unk_ee5d8;
    // The next timeline entry, and the timeline's frame count.
    EnemyTimelineEntry *timelinePos;
    Timer timelineTime;
};
ASSERT_SIZE(EnemyManager, 0xee5ec);
extern EnemyManager g_EnemyManager;
// The items enemies without their own drop, every third one, in turn.
extern BYTE g_EnemyItemDrops[0x20];

// EnemyManager's statically allocated chain elements.
extern ChainElem g_EnemyCalcChain;
extern ChainElem g_EnemyDrawChain;
