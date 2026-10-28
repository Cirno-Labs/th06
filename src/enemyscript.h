// Enemy scripts, loaded from the stage's enemy data file.

#pragma once

#include <windows.h>

#include "common.h"
#include "timer.h"

struct Enemy;
struct EnemyScriptInstr;

// Where one enemy is in its script.
struct EnemyScriptState
{
    // The next instruction.
    void *code;
    Timer timer;
    // Called with the enemy and NULL every frame while it lives, when
    // set; one of g_EnemyScriptFuncs.
    void (*func)(Enemy *enemy, EnemyScriptInstr *instr);
    // The script's variables (operands -10001 to -10012): ints, floats,
    // and ints again.
    int intVars[4];
    float floatVars[4];
    int intVars2[4];
    // What the last comparison found: -1 less, 0 equal, 1 greater.
    int compareResult;
    // The subroutine running.
    short sub;
    short unk_4a;
};

// Arguments: moves at an angle over a time, slowing down.
struct EnemyMoveAngleArgs
{
    int duration;
    float angle;
    float speed;
};

// Arguments: moves to a position over a time, slowing down.
struct EnemyMovePosArgs
{
    int duration;
    float x;
    float y;
    float z;
};

// Arguments: moves at the enemy's angle and speed over a time, slowing
// down.
struct EnemyMoveTimeArgs
{
    int duration;
};

// Arguments: fires a pattern of bullets. The counts, speeds and angles
// are operands; the counts and speeds grow with the rank.
struct EnemyBulletArgs
{
    short sprite;
    short color;
    int count1;
    int count2;
    float speed1;
    float speed2;
    float angle1;
    float angle2;
    int flags;
};

// Arguments: spawns an enemy running subroutine `sub` at `pos`
// (operands).
struct EnemySpawnArgs
{
    int sub;
    D3DXVECTOR3 pos;
    short life;
    short item;
    int score;
};

// An instruction's arguments: the named layouts, or plain ints, floats,
// shorts and bytes.
union EnemyScriptArgs
{
    int i[16];
    float f[16];
    short s[32];
    BYTE b[64];
    EnemyMoveAngleArgs moveAngle;
    EnemyMovePosArgs movePos;
    EnemyMoveTimeArgs moveTime;
    EnemyBulletArgs bullet;
};

// One instruction of an enemy script.
struct EnemyScriptInstr
{
    // The frame it runs on.
    int time;
    short opcode;
    // The size of the instruction, arguments included.
    short size;
    BYTE unk_08;
    // The difficulties it runs on, a bit each.
    BYTE difficultyMask;
    short unk_0a;
    EnemyScriptArgs args;
};

// The start of an enemy script file.
struct EnemyScriptHeader
{
    short subCount;
    short unk_02;
    // An offset from the start of the file; Load makes it a pointer.
    int unk_04;
    DWORD unk_08[0x2];
    // Then the subroutines' offsets.
};

// The arguments of a timeline entry that spawns an enemy: where (-990 or
// less for a random place), the life and item (negative for the
// script's) and the score. Other entries read them as ints.
struct EnemyTimelineArgs
{
    D3DXVECTOR3 pos;
    short life;
    short item;
    int score;
};

// One entry of the stage's enemy timeline, ended by a negative time.
struct EnemyTimelineEntry
{
    // The frame it runs on.
    short time;
    short arg0;
    short opcode;
    // The size of the entry, arguments included.
    short size;
    EnemyTimelineArgs args;
};

// The enemy script file, the global at 0x487e50: a header, then a table
// of subroutines. Load turns the file's offsets into pointers.
class EnemyScripts
{
public:
    // Loads the file at `path` and turns its offsets into pointers.
    // Returns 0, or -1 after logging "敵データの読み込みに失敗しました".
    int Load(char *path);
    // Frees the file.
    void Release();
    // Starts subroutine `sub` on `state`. Returns 0.
    int StartSub(EnemyScriptState *state, short sub);
    // Runs `enemy`'s script for a frame: the instructions due (calls,
    // returns and interrupts included), then the movement, the timed
    // shots, the sprite's direction and `func`. Returns -1 when the
    // script deletes the enemy, else 0.
    int RunEnemy(Enemy *enemy);

    // The file, as loaded.
    EnemyScriptHeader *file;
    // Points at the subroutine table in `file`.
    void **subs;
    // The timeline, at the header's unk_04.
    EnemyTimelineEntry *timeline;
};
extern EnemyScripts g_EnemyScripts;

// Resolves script operand *operand for `enemy`: one of the variables
// -10001 to -10025 (the context's, the enemy's and player's positions,
// difficulty, rank, the angle and distance to the player, the shot
// type, ...) or else the value itself. Sets *type, if given, to 0 for
// an int variable, 1 for a float variable, 2 for a read-only value and
// 3 for the immediate. Returns where the value is.
int *ResolveOperand(Enemy *enemy, int *operand, int *type);

// Resolves a float operand: a variable when *operand, as an int, names
// one, else *operand itself.
float *ResolveFloatOperand(Enemy *enemy, float *operand, int *type);
// Sets variable `dst` to the value of operand *src.
void ScriptSet(Enemy *enemy, int dst, int *src);

// Moves `enemy` at instr's angle (an operand) and speed over its
// duration, slowing down to a stop: moveMode 2.
void ScriptMoveAngle(Enemy *enemy, EnemyScriptInstr *instr);
// Moves `enemy` to instr's position (operands) over its duration,
// slowing down to a stop: moveMode 2.
void ScriptMovePos(Enemy *enemy, EnemyScriptInstr *instr);
// Moves `enemy` at its own angle and speed over instr's duration,
// slowing down to a stop: moveMode 2.
void ScriptMoveTime(Enemy *enemy, EnemyScriptInstr *instr);
// Sets variable `dst` to operand *a plus operand *b, as ints or floats
// by the variable's type.
void ScriptAdd(Enemy *enemy, int dst, int *a, int *b);
// Sets variable `dst` to operand *a minus operand *b, like ScriptAdd.
void ScriptSub(Enemy *enemy, int dst, int *a, int *b);
// Sets variable `dst` to operand *a times operand *b, like ScriptAdd.
void ScriptMul(Enemy *enemy, int dst, int *a, int *b);
// Sets variable `dst` to operand *a divided by operand *b, like
// ScriptAdd.
void ScriptDiv(Enemy *enemy, int dst, int *a, int *b);
// Sets variable `dst` to operand *a modulo operand *b, like ScriptAdd
// (fmod for floats).
void ScriptMod(Enemy *enemy, int dst, int *a, int *b);
// Sets float variable `dst` to the angle from point (*x1, *y1) to
// point (*x2, *y2), all float operands.
void ScriptAngle(Enemy *enemy, int dst, float *x1, float *y1, float *x2, float *y2);

// Where ResolveOperand computes its read-only values.
extern int g_Unknown487e44;
extern float g_Unknown487e48;
extern float g_Unknown487e4c;

// The functions scripts call or set as EnemyScriptState::func.
extern void (*g_EnemyScriptFuncs[])(Enemy *enemy, EnemyScriptInstr *instr);
// The bonus of each spell card.
extern int g_SpellCardBonuses[0x40];
