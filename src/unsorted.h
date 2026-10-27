// Classes and functions not yet assigned to a source file. Names are
// placeholders carrying an address until behavior shows what they are.

#pragma once

// Logs a printf-style message. Compiled out in the release build: the body
// is empty, but callers still pass format strings such as
// "add calc chain (pri = %d)\n".
void DebugPrint(const char *fmt, ...);

// A second compiled-out logger, used by file, sound and display code
// ("%s open ...\n", "Reflesh Rate = %f\n").
void DebugPrint2(const char *fmt, ...);

// A callback registered with the chain at 0x69d918 by 0x41cd40. Chain
// callbacks are cdecl, take the element's user pointer and return a
// status; 1 keeps the element and moves on to the next.
int Func42a670(void *arg);

// The object behind the pointer at 0x6d4588. Its destructor is empty and
// defined out of line: `delete` calls it, then operator delete.
class Unknown423330
{
public:
    ~Unknown423330();
};

class Unknown417d29
{
public:
    int Func417d29();
};

// The main callback of the chain element at 0x69bc80: runs
// Unknown417d29::Func417d29 on the element's user pointer.
int Func41b1a4(Unknown417d29 *arg);

class Unknown43be50
{
public:
    int Func43be50();

    unsigned char unk_00[0x30];
    int field_30;
};
