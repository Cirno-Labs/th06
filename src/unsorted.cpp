#include "unsorted.h"

int Unknown43be50::Func43be50()
{
    return this->field_30;
}

void DebugPrint(const char *fmt, ...)
{
}

void DebugPrint2(const char *fmt, ...)
{
}

int Func42a670(void *arg)
{
    return 1;
}

SpriteEngine::~SpriteEngine()
{
}

int Func41b1a4(Unknown417d29 *arg)
{
    return arg->Func417d29();
}

DWORD Rng::GetRandomU32()
{
    return (this->GetRandomU16() << 16) | this->GetRandomU16();
}

float Rng::GetRandomF32ZeroToOne()
{
    return this->GetRandomU32() / 4294967296.0f;
}

float AddNormalizeAngle(float a, float b)
{
    int i = 0;

    a += b;
    while (a > 3.1415927f)
    {
        a -= 6.2831855f;
        if (i++ > 16)
        {
            break;
        }
    }
    while (a < -3.1415927f)
    {
        a += 6.2831855f;
        if (i++ > 16)
        {
            break;
        }
    }
    return a;
}
