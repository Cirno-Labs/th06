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

class Unknown43be50
{
public:
    int Func43be50();

    unsigned char unk_00[0x30];
    int field_30;
};
