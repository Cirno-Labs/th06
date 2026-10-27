// Enemy scripts, loaded from the stage's enemy data file.

#pragma once

#include <windows.h>

#include "common.h"
#include "timer.h"

// Where one enemy is in its script.
struct EnemyScriptState
{
    // The next instruction.
    void *code;
    Timer timer;
    DWORD unk_10;
    // The script's variables (operands -10001 to -10012): ints, floats,
    // and ints again.
    int intVars[4];
    float floatVars[4];
    int intVars2[4];
    DWORD unk_44;
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

// One instruction of an enemy script.
struct EnemyScriptInstr
{
    // The frame it runs on.
    int time;
    short opcode;
    // The size of the instruction, arguments included.
    short size;
    DWORD unk_08;
    union {
        EnemyMoveAngleArgs moveAngle;
        EnemyMovePosArgs movePos;
        EnemyMoveTimeArgs moveTime;
    } args;
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

    // The file, as loaded.
    EnemyScriptHeader *file;
    // Points at the subroutine table in `file`.
    void **subs;
    // The header's unk_04, as a pointer.
    void *unk_08;
};
extern EnemyScripts g_EnemyScripts;

struct Enemy;

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

// Where ResolveOperand computes its read-only values.
extern int g_Unknown487e44;
extern float g_Unknown487e48;
extern float g_Unknown487e4c;
