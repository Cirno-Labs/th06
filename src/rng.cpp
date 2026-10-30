// Rng, 0x41e780 to 0x41e84c (g_Random is defined in system.cpp).

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "chain.h"
#include "unsorted.h"
#include "features.h"

WORD Rng::GetRandomU16()
{
    WORD a;

    a = (this->seed ^ 0x9630) - 0x6553;
    this->seed = (((a & 0xc000) >> 14) + a * 4) & 0xffff;
    this->generated++;
    return this->seed;
}

DWORD Rng::GetRandomU32()
{
    return (this->GetRandomU16() << 16) | this->GetRandomU16();
}

float Rng::GetRandomF32ZeroToOne()
{
    return this->GetRandomU32() / 4294967296.0f;
}
