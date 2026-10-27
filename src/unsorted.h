// Classes and functions not yet assigned to a source file. Names are
// placeholders carrying an address until behavior shows what they are.

#pragma once

#include <windows.h>

#include "sprite.h"
#include "dsutil.h"
#include "midi.h"

// The global at 0x6c6d18 plays wave music through it.
class Unknown6d3f50
{
public:
    // Stops the wave music: ends the notification thread and releases
    // the stream ("stop BGM").
    void StopBgm();
    // Fades the wave music out over `seconds`, counted in frames at 60
    // per second.
    void FadeOutBgm(float seconds);

    unsigned char unk_000[0x62c];
    // The wave music playing; NULL when stopped.
    CStreamingSound *bgm;
};
extern Unknown6d3f50 g_Unknown6d3f50;

// The global at 0x6c6d18: the window, settings and game speed. Larger
// than its known fields.
class Unknown6c6d18
{
public:
    // Advances a frame counter split into whole frames and the fraction
    // of a frame, like Timer::Add(1). Callers pass a Timer's `current` and
    // `subFrame` after setting its `previous`: an inlined per-frame tick.
    void TickTimer(int *frames, float *subFrame);
    // Loads a MIDI file into a slot of `midi` in MIDI mode. Returns 0 in
    // MIDI mode, otherwise 1.
    int LoadMidi(int slot, char *path);
    // Plays the MIDI file loaded into a slot from the start, in MIDI mode.
    // Returns 0 in MIDI mode, otherwise 1.
    int PlayMidi(int slot);
    // Stops the music. Returns -1 if the music mode is neither wave (1)
    // nor MIDI (2), otherwise 0.
    int StopMusic();
    // Fades the music out over `seconds`. Returns -1 if the music mode is
    // neither wave nor MIDI, otherwise 0.
    int FadeOutMusic(float seconds);
    // Plays the tune at `path`, a MIDI file, from the start. In wave mode
    // it plays the .wav of the same name instead, looping at the point
    // the .pos file gives. Returns -1 if the music mode is neither wave
    // nor MIDI, otherwise 0.
    int PlayMusic(char *path);

    unsigned char unk_000[0x44];
    // Set from CreateWindowExA's result; MidiDevice::Open has MIDI
    // messages sent to it (CALLBACK_WINDOW).
    HWND hwnd;
    unsigned char unk_048[0xe7];
    // How music is played: 1 is wave, 2 is MIDI. Set to 2 after logging
    // "wave データが無いので、midi にします" ("no wave data, switching to
    // MIDI").
    BYTE musicMode;
    unsigned char unk_130[0x5c];
    // The scene the game is in or switches to: many places set it (1
    // through 10), and the main loop's callbacks compare it with 3. The
    // music screen sets it to 1 to leave.
    int scene;
    unsigned char unk_190[0x18];
    // The part of a frame a Timer advances per frame while the game runs
    // slower than full speed.
    float frameAdvance;
    // 1.0 at full speed; at 0.99 or below, Timers count fractions of
    // frames.
    float gameSpeed;
    // Plays music in MIDI mode; may be NULL.
    MidiPlayer *midi;
};
extern Unknown6c6d18 g_Unknown6c6d18;

// The buttons held this frame and the frame before, as bits: 0x10 up,
// 0x20 down, 0x1001 select, 0xa cancel.
extern WORD g_Buttons;
extern WORD g_PrevButtons;

// Whether any of the buttons in `mask` went down this frame.
#define WAS_PRESSED(mask) ((g_Buttons & (mask)) && (g_Buttons & (mask)) != (g_PrevButtons & (mask)))

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
