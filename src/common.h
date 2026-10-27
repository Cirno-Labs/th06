// Shared helpers for the decompiled code.

#pragma once

#include <stdlib.h>

// Fails to compile unless sizeof(type) == size. Generates no code.
#define ASSERT_SIZE(type, size) typedef char assert_size_##type[sizeof(type) == (size) ? 1 : -1]

// Every free() in the game passes through a temporary, the trace an
// inlined wrapper leaves under /Ob1.
inline void MemFree(void *p)
{
    free(p);
}
