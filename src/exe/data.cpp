// The game's initialized data: tables of strings, script numbers, colors,
// shot patterns and sounds. Built into the standalone exe only: the DLL
// build uses the original's copies, which the original code shares.
// Values are the 1.02h exe's initial bytes.

#include <d3d8.h>

#include "bullet.h"
#include "effect.h"
#include "enemy.h"
#include "enemyscript.h"
#include "features.h"
#include "gamemanager.h"
#include "item.h"
#include "mainmenu.h"
#include "player.h"
#include "resultscreen.h"
#include "sprite.h"
#include "stage.h"
#include "unsorted.h"

// Defined in enemyscript.cpp and effect.cpp; only these tables name them.
void ScriptFreezeBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootFromRandomPoint(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptStarPattern(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptGetShotTypeValues(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptFreezeOrSendOff(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSideArcFans(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSparkCircle(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptTwoRingShots(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootFromLargeBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSwirlStoppedBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptSparksAndBombShield(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptScatterStoppedBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootFromLasers(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptArenaCircleShots(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptShootAlongLasers(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptBurstAroundLargeBullets(Enemy *enemy, EnemyScriptInstr *instr);
void ScriptFollowLife(Enemy *enemy, EnemyScriptInstr *instr);
int EffectUpdateDrift(Effect *effect);
int EffectUpdateDriftFast(Effect *effect);
int EffectUpdateMove(Effect *effect);
int EffectUpdateOrbit(Effect *effect);
int EffectUpdateConverge(Effect *effect);
int EffectUpdateConvergeSlow(Effect *effect);

StageFiles g_StageFiles[8] = {
    {"dummy", "dummy"},
    {"data/stg1bg.anm", "data/stage1.std"},
    {"data/stg2bg.anm", "data/stage2.std"},
    {"data/stg3bg.anm", "data/stage3.std"},
    {"data/stg4bg.anm", "data/stage4.std"},
    {"data/stg5bg.anm", "data/stage5.std"},
    {"data/stg6bg.anm", "data/stage6.std"},
    {"data/stg7bg.anm", "data/stage7.std"},
};
int g_SpellCardBonuses[0x40] = {
    200000, 200000, 200000, 200000, 200000, 200000, 200000, 250000,
    250000, 250000, 250000, 250000, 250000, 250000, 300000, 300000,
    300000, 300000, 300000, 300000, 300000, 300000, 300000, 300000,
    300000, 300000, 300000, 300000, 300000, 300000, 300000, 300000,
    400000, 400000, 400000, 400000, 400000, 400000, 400000, 400000,
    500000, 500000, 500000, 500000, 500000, 500000, 600000, 600000,
    600000, 600000, 600000, 700000, 700000, 700000, 700000, 700000,
    700000, 700000, 700000, 700000, 700000, 700000, 700000, 700000,
};
void (*g_EnemyScriptFuncs[])(Enemy *enemy, EnemyScriptInstr *instr) = {
    ScriptFreezeBullets,
    ScriptShootFromRandomPoint,
    ScriptStarPattern,
    ScriptGetShotTypeValues,
    ScriptFreezeOrSendOff,
    ScriptSideArcFans,
    ScriptSparkCircle,
    ScriptTwoRingShots,
    ScriptShootFromLargeBullets,
    ScriptSwirlStoppedBullets,
    ScriptSparksAndBombShield,
    ScriptScatterStoppedBullets,
    ScriptShootFromLasers,
    ScriptArenaCircleShots,
    ScriptShootAlongLasers,
    ScriptBurstAroundLargeBullets,
    ScriptFollowLife,
};
int g_ShotTypeValues[2][2][3] = {
    {{0, 3, 1}, {2, 3, 4}},
    {{1, 4, 0}, {4, 2, 3}},
};
EffectInfo g_EffectInfos[] = {
    {669, NULL},
    {670, NULL},
    {671, NULL},
    {672, EffectUpdateDriftFast},
    {675, EffectUpdateDrift},
    {676, EffectUpdateDrift},
    {677, EffectUpdateDrift},
    {678, EffectUpdateDrift},
    {679, EffectUpdateDrift},
    {680, EffectUpdateDrift},
    {681, EffectUpdateDrift},
    {682, EffectUpdateDrift},
    {683, NULL},
    {684, EffectUpdateOrbit},
    {684, EffectUpdateOrbit},
    {684, EffectUpdateOrbit},
    {691, NULL},
    {673, EffectUpdateConverge},
    {674, EffectUpdateConvergeSlow},
    {685, EffectUpdateMove},
};
BYTE g_EnemyItemDrops[0x20] = {
    0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 2,
};
DWORD g_EffectColorsFull[0x1c] = {
    0xff000000, 0xff303030, 0xff606060, 0xff500000,
    0xff900000, 0xffff2020, 0xff400040, 0xff800080,
    0xffff30ff, 0xff000050, 0xff000090, 0xff2020ff,
    0xff203060, 0xff304090, 0xff3080ff, 0xff005000,
    0xff009000, 0xff20ff20, 0xff206000, 0xff409010,
    0xff80ff20, 0xff505000, 0xff909000, 0xffffff20,
    0xff603000, 0xff904010, 0xfff08020, 0xffffffff,
};
DWORD g_EffectColorsPale[0x1c] = {
    0xfff0f0f0, 0xfff0f0f0, 0xffffffff, 0xffffe0e0,
    0xffffe0e0, 0xffffe0e0, 0xffffe0ff, 0xffffe0ff,
    0xffffe0ff, 0xffe0e0ff, 0xffe0e0ff, 0xffe0e0ff,
    0xffe0ffff, 0xffe0ffff, 0xffe0ffff, 0xffe0ffe0,
    0xffe0ffe0, 0xffe0ffe0, 0xffe0ffe0, 0xffe0ffe0,
    0xffe0ffe0, 0xffffffe0, 0xffffffe0, 0xffffffe0,
    0xffffe0e0, 0xffffe0e0, 0xffffe0e0, 0xffffffff,
};
DWORD *g_EffectColors = g_EffectColorsFull;
int g_BulletColorOffsets16[16] = {
    0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 0,
};
// Only the first eight are the table; the original reads on into
// g_BossTimerColors and g_ExtendScores, so those values follow.
int g_BulletColorOffsets32[16] = {
    0, 1, 1, 2, 2, 3, 4, 0,
    10539263, 10518783, 14713024, 16728128, 10000000, 20000000, 40000000, 60000000,
};
D3DCOLOR g_BossTimerColors[4] = {
    0x00a0d0ff, 0x00a080ff, 0x00e080c0, 0x00ff4040,
};
unsigned int g_ExtendScores[] = {
    10000000, 20000000, 40000000, 60000000, 1900000000,
};
char *g_EclPaths[8] = {
    "dummy",
    "data/ecldata1.ecl",
    "data/ecldata2.ecl",
    "data/ecldata3.ecl",
    "data/ecldata4.ecl",
    "data/ecldata5.ecl",
    "data/ecldata6.ecl",
    "data/ecldata7.ecl",
};
EnemyAnmPaths g_EnemyAnmPaths[8] = {
    {"dummy", "dummy"},
    {"data/stg1enm.anm", "data/stg1enm2.anm"},
    {"data/stg2enm.anm", "data/stg2enm2.anm"},
    {"data/stg3enm.anm", NULL},
    {"data/stg4enm.anm", NULL},
    {"data/stg5enm.anm", "data/stg5enm2.anm"},
    {"data/stg6enm.anm", "data/stg6enm2.anm"},
    {"data/stg7enm.anm", "data/stg7enm2.anm"},
};
RankSettings g_ReplayRanks[5] = {
    {16, 12, 20},
    {16, 10, 32},
    {16, 10, 32},
    {16, 10, 32},
    {16, 14, 18},
};
RankSettings g_Ranks[5] = {
    {16, 12, 20},
    {16, 10, 32},
    {16, 10, 32},
    {16, 10, 32},
    {16, 14, 18},
};
ControllerMapping g_ControllerMapping = {0, 1, 2, 4, -1, -1, -1, -1, 3};
FormatInfo g_FormatInfos[] = {
    {22, 32, 0x00000000, 0x00ff0000, 0x0000ff00, 0x000000ff},
    {21, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff},
    {24, 16, 0x00000000, 0x00007c00, 0x000003e0, 0x0000001f},
    {23, 16, 0x00000000, 0x0000f800, 0x000007e0, 0x0000001f},
    {25, 16, 0x00008000, 0x00007c00, 0x000003e0, 0x0000001f},
    {26, 16, 0x0000f000, 0x00000f00, 0x000000f0, 0x0000000f},
    {-1, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
};
int g_PowerItemBonus[31] = {
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
    200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000,
    3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000,
    51200,
};
int g_PowerLevels[] = {
    8, 16, 32, 48, 64, 80, 96, 128, 999,
};
BombInfo g_Bombs[4] = {
    {UpdateReimuABomb, DrawReimuABomb},
    {UpdateReimuBBomb, DrawReimuBBomb},
    {UpdateMarisaABomb, DrawMarisaABomb},
    {UpdateMarisaBBomb, DrawMarisaBBomb},
};
ShotTypeInfo g_ShotTypes[4] = {
    {4.0f, 2.0f, 4.0f, 2.0f, FireShotType0, FireShotType0},
    {4.0f, 2.0f, 4.0f, 2.0f, FireShotType1, FireShotType1},
    {5.0f, 2.5f, 5.0f, 2.5f, FireShotType2, FireShotType2},
    {5.0f, 2.5f, 5.0f, 2.5f, FireShotType3, FireShotType3},
};
static ShotData s_Shots0_0[1] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 48, 0, 0, 1088, 0},
};
static ShotData s_Shots0_1[3] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 48, 0, 0, 1088, 0},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -2.0943952f, 10.0f, 14, 1, 1, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.0471976f, 10.0f, 14, 2, 1, 1089, -1},
};
static ShotData s_Shots0_2[4] = {
    {5, 0, -4.0f, 0.0f, 12.0f, 12.0f, -1.5882497f, 12.0f, 30, 0, 0, 1088, -1},
    {5, 0, 4.0f, 0.0f, 12.0f, 12.0f, -1.553343f, 12.0f, 30, 0, 0, 1088, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -2.0943952f, 10.0f, 14, 1, 1, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.0471976f, 10.0f, 14, 2, 1, 1089, -1},
};
static ShotData s_Shots0_3[5] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6755161f, 12.0f, 24, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 30, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4660766f, 12.0f, 24, 0, 0, 1088, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -2.0943952f, 10.0f, 14, 1, 1, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.0471976f, 10.0f, 14, 2, 1, 1089, -1},
};
static ShotData s_Shots0_4[5] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6929694f, 12.0f, 24, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 30, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4486233f, 12.0f, 24, 0, 0, 1088, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -2.0943952f, 10.0f, 12, 1, 1, 1089, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.0471976f, 10.0f, 12, 2, 1, 1089, -1},
};
static ShotData s_Shots0_5[7] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6929694f, 12.0f, 24, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 29, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4486233f, 12.0f, 24, 0, 0, 1088, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -2.0943952f, 10.0f, 9, 1, 1, 1089, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.0471976f, 10.0f, 9, 2, 1, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -2.6179938f, 10.0f, 12, 1, 1, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -0.5235988f, 10.0f, 12, 2, 1, 1089, -1},
};
static ShotData s_Shots0_6[9] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6929694f, 12.0f, 24, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 28, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4486233f, 12.0f, 24, 0, 0, 1088, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.9198623f, 10.0f, 10, 1, 1, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.2217305f, 10.0f, 10, 2, 1, 1089, -1},
    {30, 10, 0.0f, 0.0f, 12.0f, 12.0f, -2.268928f, 10.0f, 9, 1, 1, 1089, -1},
    {30, 10, 0.0f, 0.0f, 12.0f, 12.0f, -0.87266463f, 10.0f, 9, 2, 1, 1089, -1},
    {30, 20, 0.0f, 0.0f, 12.0f, 12.0f, -2.6179938f, 10.0f, 11, 1, 1, 1089, -1},
    {30, 20, 0.0f, 0.0f, 12.0f, 12.0f, -0.5235988f, 10.0f, 11, 2, 1, 1089, -1},
};
static ShotData s_Shots0_7[9] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6929694f, 12.0f, 24, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 28, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4486233f, 12.0f, 24, 0, 0, 1088, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.9198623f, 10.0f, 8, 1, 1, 1089, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.2217305f, 10.0f, 8, 2, 1, 1089, -1},
    {15, 5, 0.0f, 0.0f, 12.0f, 12.0f, -2.268928f, 10.0f, 8, 1, 1, 1089, -1},
    {15, 5, 0.0f, 0.0f, 12.0f, 12.0f, -0.87266463f, 10.0f, 8, 2, 1, 1089, -1},
    {15, 10, 0.0f, 0.0f, 12.0f, 12.0f, -2.6179938f, 10.0f, 8, 1, 1, 1089, -1},
    {15, 10, 0.0f, 0.0f, 12.0f, 12.0f, -0.5235988f, 10.0f, 8, 2, 1, 1089, -1},
};
static ShotData s_Shots0_8[12] = {
    {5, 0, -8.0f, 0.0f, 12.0f, 12.0f, -1.6929694f, 12.0f, 23, 0, 0, 1088, 0},
    {5, 0, -8.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 24, 0, 0, 1088, -1},
    {5, 0, 8.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 24, 0, 0, 1088, -1},
    {5, 0, 8.0f, 0.0f, 12.0f, 12.0f, -1.4486233f, 12.0f, 23, 0, 0, 1088, -1},
    {16, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.9198623f, 10.0f, 10, 1, 1, 1089, -1},
    {16, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.2217305f, 10.0f, 10, 2, 1, 1089, -1},
    {16, 4, 0.0f, 0.0f, 12.0f, 12.0f, -2.268928f, 10.0f, 8, 1, 1, 1089, -1},
    {16, 4, 0.0f, 0.0f, 12.0f, 12.0f, -0.87266463f, 10.0f, 8, 2, 1, 1089, -1},
    {16, 8, 0.0f, 0.0f, 12.0f, 12.0f, -2.6179938f, 10.0f, 7, 1, 1, 1089, -1},
    {16, 8, 0.0f, 0.0f, 12.0f, 12.0f, -0.5235988f, 10.0f, 7, 2, 1, 1089, -1},
    {16, 12, 0.0f, 0.0f, 12.0f, 12.0f, -2.9670599f, 10.0f, 10, 1, 1, 1089, -1},
    {16, 12, 0.0f, 0.0f, 12.0f, 12.0f, -0.17453289f, 10.0f, 10, 2, 1, 1089, -1},
};
ShotTable g_ShotTable0[9] = {
    {1, 8, s_Shots0_0},
    {3, 16, s_Shots0_1},
    {4, 32, s_Shots0_2},
    {5, 48, s_Shots0_3},
    {5, 64, s_Shots0_4},
    {7, 80, s_Shots0_5},
    {9, 96, s_Shots0_6},
    {9, 127, s_Shots0_7},
    {12, 999, s_Shots0_8},
};
static ShotData s_Shots1_0[1] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 48, 0, 0, 1088, 0},
};
static ShotData s_Shots1_1[3] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 48, 0, 0, 1088, 0},
    {15, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 1, 0, 1090, -1},
    {15, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 2, 0, 1090, -1},
};
static ShotData s_Shots1_2[4] = {
    {5, 0, -4.0f, 0.0f, 12.0f, 12.0f, -1.5882497f, 12.0f, 32, 0, 0, 1088, 0},
    {5, 0, 4.0f, 0.0f, 12.0f, 12.0f, -1.553343f, 12.0f, 32, 0, 0, 1088, -1},
    {10, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 1, 0, 1090, -1},
    {10, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 2, 0, 1090, -1},
};
static ShotData s_Shots1_3[4] = {
    {5, 0, -4.0f, 0.0f, 12.0f, 12.0f, -1.5882497f, 12.0f, 30, 0, 0, 1088, 0},
    {5, 0, 4.0f, 0.0f, 12.0f, 12.0f, -1.553343f, 12.0f, 30, 0, 0, 1088, -1},
    {8, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 1, 0, 1090, -1},
    {8, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 2, 0, 1090, -1},
};
static ShotData s_Shots1_4[5] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6929694f, 12.0f, 20, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 28, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4486233f, 12.0f, 20, 0, 0, 1088, -1},
    {8, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 1, 0, 1090, -1},
    {8, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 2, 0, 1090, -1},
};
static ShotData s_Shots1_5[7] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6929694f, 12.0f, 16, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 27, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4486233f, 12.0f, 16, 0, 0, 1088, -1},
    {5, 0, 8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 1, 0, 1090, -1},
    {5, 0, 8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 12, 2, 0, 1090, -1},
    {8, 0, -8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, -1},
    {8, 0, -8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
};
static ShotData s_Shots1_6[7] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 12.0f, 16, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 22, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 12.0f, 16, 0, 0, 1088, -1},
    {3, 0, 8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, 0},
    {3, 0, 8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
    {5, 0, -8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, -1},
    {5, 0, -8.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
};
static ShotData s_Shots1_7[11] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.850049f, 12.0f, 9, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 12.0f, 17, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 20, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 12.0f, 17, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.2915437f, 12.0f, 9, 0, 0, 1088, -1},
    {3, 0, 12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, 0},
    {3, 0, 12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
    {5, 0, -12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, -1},
    {5, 0, -12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
    {10, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, -1},
    {10, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
};
static ShotData s_Shots1_8[11] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.850049f, 12.0f, 9, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 12.0f, 17, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 20, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 12.0f, 17, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.2915437f, 12.0f, 9, 0, 0, 1088, -1},
    {3, 0, 12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, 0},
    {3, 0, 12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
    {3, 0, -12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, -1},
    {3, 0, -12.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
    {5, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 1, 0, 1090, -1},
    {5, 0, 0.0f, -16.0f, 12.0f, 40.0f, -1.5707964f, 22.0f, 10, 2, 0, 1090, -1},
};
ShotTable g_ShotTable1[9] = {
    {1, 8, s_Shots1_0},
    {3, 16, s_Shots1_1},
    {4, 32, s_Shots1_2},
    {4, 48, s_Shots1_3},
    {5, 64, s_Shots1_4},
    {7, 80, s_Shots1_5},
    {7, 96, s_Shots1_6},
    {11, 127, s_Shots1_7},
    {11, 999, s_Shots1_8},
};
static ShotData s_Shots2_0[1] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 48, 0, 0, 1088, 0},
};
static ShotData s_Shots2_1[3] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 36, 0, 0, 1088, 0},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 3.0f, 18, 1, 2, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 3.0f, 18, 2, 2, 1089, -1},
};
static ShotData s_Shots2_2[5] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 32, 0, 0, 1088, 0},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 16, 1, 2, 1089, -1},
    {30, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 16, 2, 2, 1089, -1},
    {30, 15, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 10, 1, 2, 1089, -1},
    {30, 15, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 10, 2, 2, 1089, -1},
};
static ShotData s_Shots2_3[5] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 32, 0, 0, 1088, 0},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 15, 1, 2, 1089, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 15, 2, 2, 1089, -1},
    {15, 15, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 10, 1, 2, 1089, -1},
    {15, 15, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 10, 2, 2, 1089, -1},
};
static ShotData s_Shots2_4[5] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 32, 0, 0, 1088, 0},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 16, 1, 2, 1090, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 16, 2, 2, 1090, -1},
    {15, 20, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 11, 1, 2, 1090, -1},
    {15, 20, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 11, 2, 2, 1090, -1},
};
static ShotData s_Shots2_5[6] = {
    {5, 0, -8.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 16, 0, 0, 1088, 0},
    {5, 0, 8.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 16, 0, 0, 1088, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 16, 1, 2, 1090, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 16, 2, 2, 1090, -1},
    {15, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 3.0f, 10, 1, 2, 1090, -1},
    {15, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 3.0f, 10, 2, 2, 1090, -1},
};
static ShotData s_Shots2_6[6] = {
    {5, 0, -8.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 13, 0, 0, 1088, 0},
    {5, 0, 8.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 13, 0, 0, 1088, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 3.0f, 16, 1, 2, 1091, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 3.0f, 16, 2, 2, 1091, -1},
    {10, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 3.0f, 10, 1, 2, 1091, -1},
    {10, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 3.0f, 10, 2, 2, 1091, -1},
};
static ShotData s_Shots2_7[9] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6406095f, 12.0f, 8, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 12, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5009832f, 12.0f, 8, 0, 0, 1088, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 3.0f, 15, 1, 2, 1091, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 3.0f, 15, 2, 2, 1091, -1},
    {10, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 3.0f, 10, 1, 2, 1091, -1},
    {10, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 3.0f, 10, 2, 2, 1091, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.3613569f, 3.0f, 9, 1, 2, 1091, -1},
    {15, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7802359f, 3.0f, 9, 2, 2, 1091, -1},
};
static ShotData s_Shots2_8[9] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6406095f, 12.0f, 8, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 12, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5009832f, 12.0f, 8, 0, 0, 1088, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 3.0f, 14, 1, 2, 1092, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 3.0f, 14, 2, 2, 1092, -1},
    {10, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.43117f, 3.0f, 10, 1, 2, 1092, -1},
    {10, 5, 0.0f, 0.0f, 12.0f, 12.0f, -1.7104228f, 3.0f, 10, 2, 2, 1092, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.3089969f, 3.0f, 10, 1, 2, 1092, -1},
    {10, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.8325958f, 3.0f, 10, 2, 2, 1092, -1},
};
ShotTable g_ShotTable2[9] = {
    {1, 8, s_Shots2_0},
    {3, 16, s_Shots2_1},
    {5, 32, s_Shots2_2},
    {5, 48, s_Shots2_3},
    {5, 64, s_Shots2_4},
    {6, 80, s_Shots2_5},
    {6, 96, s_Shots2_6},
    {9, 127, s_Shots2_7},
    {9, 999, s_Shots2_8},
};
static ShotData s_Shots3_0[1] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 48, 0, 0, 1088, 0},
};
static ShotData s_Shots3_1[3] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 32, 0, 0, 1088, 0},
    {120, 0, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 1, 3, 1093, -1},
    {120, 1, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 2, 3, 1093, -1},
};
static ShotData s_Shots3_2[3] = {
    {5, 0, 0.0f, -8.0f, 12.0f, 24.0f, -1.5707964f, 12.0f, 32, 0, 0, 1088, 0},
    {170, 0, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 1, 3, 1093, -1},
    {170, 1, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 2, 3, 1093, -1},
};
static ShotData s_Shots3_3[4] = {
    {5, 0, -8.0f, -8.0f, 12.0f, 24.0f, -1.605703f, 12.0f, 22, 0, 0, 1088, 0},
    {5, 0, 8.0f, -8.0f, 12.0f, 24.0f, -1.5358897f, 12.0f, 22, 0, 0, 1088, -1},
    {200, 0, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 1, 3, 1093, -1},
    {200, 1, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 2, 3, 1093, -1},
};
static ShotData s_Shots3_4[4] = {
    {5, 0, -8.0f, -8.0f, 12.0f, 24.0f, -1.605703f, 12.0f, 22, 0, 0, 1088, 0},
    {5, 0, 8.0f, -8.0f, 12.0f, 24.0f, -1.5358897f, 12.0f, 22, 0, 0, 1088, -1},
    {210, 0, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 1, 3, 1094, -1},
    {210, 1, 0.0f, 0.0f, 10.0f, 480.0f, -1.5707964f, 3.0f, 3, 2, 3, 1094, -1},
};
static ShotData s_Shots3_5[4] = {
    {5, 0, -8.0f, -8.0f, 12.0f, 24.0f, -1.605703f, 12.0f, 20, 0, 0, 1088, 0},
    {5, 0, 8.0f, -8.0f, 12.0f, 24.0f, -1.5358897f, 12.0f, 20, 0, 0, 1088, -1},
    {230, 0, 0.0f, 0.0f, 15.0f, 480.0f, -1.5707964f, 3.0f, 4, 1, 3, 1094, -1},
    {230, 1, 0.0f, 0.0f, 15.0f, 480.0f, -1.5707964f, 3.0f, 4, 2, 3, 1094, -1},
};
static ShotData s_Shots3_6[5] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 12.0f, 15, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 20, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 12.0f, 15, 0, 0, 1088, -1},
    {250, 0, 0.0f, 0.0f, 15.0f, 480.0f, -1.5707964f, 3.0f, 4, 1, 3, 1094, -1},
    {250, 1, 0.0f, 0.0f, 15.0f, 480.0f, -1.5707964f, 3.0f, 4, 2, 3, 1094, -1},
};
static ShotData s_Shots3_7[5] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 12.0f, 15, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 20, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 12.0f, 15, 0, 0, 1088, -1},
    {270, 0, 0.0f, 0.0f, 20.0f, 480.0f, -1.5707964f, 3.0f, 5, 1, 3, 1095, -1},
    {270, 1, 0.0f, 0.0f, 20.0f, 480.0f, -1.5707964f, 3.0f, 5, 2, 3, 1095, -1},
};
static ShotData s_Shots3_8[7] = {
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.7453293f, 12.0f, 12, 0, 0, 1088, 0},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.6580628f, 12.0f, 15, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.5707964f, 12.0f, 20, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.4835299f, 12.0f, 15, 0, 0, 1088, -1},
    {5, 0, 0.0f, 0.0f, 12.0f, 12.0f, -1.3962635f, 12.0f, 12, 0, 0, 1088, -1},
    {330, 0, 0.0f, 0.0f, 20.0f, 480.0f, -1.5707964f, 3.0f, 6, 1, 3, 1095, -1},
    {330, 1, 0.0f, 0.0f, 20.0f, 480.0f, -1.5707964f, 3.0f, 6, 2, 3, 1095, -1},
};
ShotTable g_ShotTable3[9] = {
    {1, 8, s_Shots3_0},
    {3, 16, s_Shots3_1},
    {3, 32, s_Shots3_2},
    {4, 48, s_Shots3_3},
    {4, 64, s_Shots3_4},
    {4, 80, s_Shots3_5},
    {5, 96, s_Shots3_6},
    {5, 127, s_Shots3_7},
    {7, 999, s_Shots3_8},
};
char *g_NameAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ.,:;･@abcdefghijklmnopqrstuvwxyz+-/*=%0123456789(){}[]<>#!?'\"$      --";
char *g_ShotNames[6] = {
#if FEAT_ENGLISH
    EN_SHOT_REIMU_A,
    EN_SHOT_REIMU_B,
    EN_SHOT_MARISA_A,
    EN_SHOT_MARISA_B,
#else
    "博麗 霊夢 (霊)",
    "博麗 霊夢 (夢)",
    "霧雨 魔理沙 (魔)",
    "霧雨 魔理沙 (恋)",
#endif
    "冴月 麟 (花)",
    "冴月 麟 (風)",
};
float g_SpellCardRatings[5] = {
    1.0f, 1.5f, 1.5f, 2.0f, 2.5f,
};
char *g_DifficultyNames[5] = {
    "     Easy",
    "   Normal",
    "     Hard",
    "  Lunatic",
    "    Extra",
};
char *g_ShotTypeNames[4] = {
    "ReimuA ",
    "ReimuB ",
    "MarisaA",
    "MarisaB",
};
SoundInfo g_SoundInfos[0x20] = {
    {0, -1500, 0}, {0, -2000, 0}, {1, -1200, 5}, {1, -1400, 5},
    {2, -1000, 100}, {3, -500, 100}, {4, -500, 100}, {5, -1700, 50},
    {6, -1700, 50}, {7, -1700, 50}, {8, -1000, 100}, {9, -1000, 100},
    {10, -1900, 10}, {11, -1200, 10}, {12, -900, 100}, {5, -1500, 50},
    {13, -900, 50}, {14, -900, 50}, {15, -600, 100}, {16, -400, 100},
    {17, -1100, 0}, {18, -900, 0}, {5, -1800, 20}, {6, -1800, 20},
    {7, -1800, 20}, {19, -300, 50}, {20, -600, 50}, {21, -800, 50},
    {22, -100, 140}, {23, -500, 100}, {24, -1000, 20}, {25, -1000, 90},
};
char *g_SoundPaths[] = {
    "data/wav/plst00.wav",
    "data/wav/enep00.wav",
    "data/wav/pldead00.wav",
    "data/wav/power0.wav",
    "data/wav/power1.wav",
    "data/wav/tan00.wav",
    "data/wav/tan01.wav",
    "data/wav/tan02.wav",
    "data/wav/ok00.wav",
    "data/wav/cancel00.wav",
    "data/wav/select00.wav",
    "data/wav/gun00.wav",
    "data/wav/cat00.wav",
    "data/wav/lazer00.wav",
    "data/wav/lazer01.wav",
    "data/wav/enep01.wav",
    "data/wav/nep00.wav",
    "data/wav/damage00.wav",
    "data/wav/item00.wav",
    "data/wav/kira00.wav",
    "data/wav/kira01.wav",
    "data/wav/kira02.wav",
    "data/wav/extend.wav",
    "data/wav/timeout.wav",
    "data/wav/graze.wav",
    "data/wav/powerup.wav",
};
short g_LastPressedKey = 32;
char *g_MenuStageNames[7] = {
    "Stage1",
    "Stage2",
    "Stage3",
    "Stage4",
    "Stage5",
    "Stage6",
    "Extra ",
};
char *g_MenuShotTypeNames[4] = {
    "ReimuA ",
    "ReimuB ",
    "MarisaA",
    "MarisaB",
};
char *g_MenuDifficultyNames[5] = {
    "Easy   ",
    "Normal ",
    "Hard   ",
    "Lunatic",
    "Extra  ",
};
