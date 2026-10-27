// Classes and functions not yet assigned to a source file. Names are
// placeholders carrying an address until behavior shows what they are.

#pragma once

#include <windows.h>

// Set from CreateWindowExA's result; MidiDevice::Open has MIDI messages
// sent to it (CALLBACK_WINDOW).
extern HWND g_Hwnd6c6d5c;

// How music is played; 2 is MIDI. Set to 2 after logging "wave データが無い
// ので、midi にします" ("no wave data, switching to MIDI").
extern BYTE g_MusicMode;

// Loads a whole file into a malloc'd buffer; NULL on failure.
void *Func41e290(char *path, int unk);

// The object error messages go to, e.g. "error : MIDI File が読み込めない".
class Unknown69d998;
extern Unknown69d998 g_Unknown69d998;

// Logs a printf-style message to a log object.
int Func41e4d0(Unknown69d998 *log, const char *fmt, ...);

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

// Holds GDI objects: 0x41e992 selects the old object back into the DC at
// +0x14, then deletes the DC and the object at +0x1c.
class Unknown41e981
{
public:
    ~Unknown41e981();
    void Func41e992();
};

class Unknown43be50
{
public:
    int Func43be50();

    unsigned char unk_00[0x30];
    int field_30;
};
