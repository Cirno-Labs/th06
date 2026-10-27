// Classes and functions not yet assigned to a source file. Names are
// placeholders carrying an address until behavior shows what they are.

#pragma once

#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "ascii.h"
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
    // Loads the sound effects ("data/wav/plst00.wav", ...).
    int LoadSounds();

    unsigned char unk_000[0x62c];
    // The wave music playing; NULL when stopped.
    CStreamingSound *bgm;
};
extern Unknown6d3f50 g_Unknown6d3f50;

// One of the game's data archives (紅魔郷*.DAT). Its code, from
// 0x43c8d0, is built with optimization like the libraries after the
// game's code.
class DatArchive
{
public:
    // Calls Release.
    ~DatArchive();

    // Closes the archive and frees its entry table. Returns 1.
    int Release();
};

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
    // Opens the archive at `path` (such as "紅魔郷IN.dat") as archive
    // `index`. Returns 0, or -1 on failure.
    int LoadArchive(int index, char *path);
    // Closes and deletes archive `index`, if it is open.
    void ReleaseArchive(int index);
    // Fades the music out over `seconds`. Returns -1 if the music mode is
    // neither wave nor MIDI, otherwise 0.
    int FadeOutMusic(float seconds);
    // Called with a tune's path ("bgm/th06_01.mid") but ignores it: does
    // nothing in either mode. Returns -1 if the music mode is neither
    // wave nor MIDI, otherwise 0.
    int LoadMusic(char *path);
    // Plays the tune at `path`, a MIDI file, from the start. In wave mode
    // it plays the .wav of the same name instead, looping at the point
    // the .pos file gives. Returns -1 if the music mode is neither wave
    // nor MIDI, otherwise 0.
    int PlayMusic(char *path);

    // Resets the scene and adds the game's main elements to the chains:
    // OnUpdate at priority 0 with its added and deleted callbacks, and
    // OnDraw at 0xe. Returns 0, or -1 if adding fails.
    static int RegisterChain();
    // The main update chain callback and its added and deleted
    // callbacks.
    static int OnUpdate(Unknown6c6d18 *arg);
    static int AddedCallback(Unknown6c6d18 *arg);
    static int DeletedCallback(Unknown6c6d18 *arg);
    // The draw chain callback: resets the cached device state and draws
    // the frame rate. Returns 1.
    static int OnDraw(Unknown6c6d18 *arg);

    unsigned char unk_000[0x8];
    IDirect3DDevice8 *d3dDevice;
    // Created by DirectInput8Create.
    IDirectInput8 *dinput;
    // The keyboard's DirectInput device.
    IDirectInputDevice8 *keyboard;
    // The first joystick found; NULL without one.
    IDirectInputDevice8 *joystick;
    unsigned char unk_018[0x2c];
    // Set from CreateWindowExA's result; MidiDevice::Open has MIDI
    // messages sent to it (CALLBACK_WINDOW).
    HWND hwnd;
    unsigned char unk_048[0x98];
    // What the device was created with; AddedCallback resets the device
    // with them when presenting fails.
    D3DPRESENT_PARAMETERS presentParams;
    unsigned char unk_114[0x1b];
    // How music is played: 1 is wave, 2 is MIDI. Set to 2 after logging
    // "wave データが無いので、midi にします" ("no wave data, switching to
    // MIDI").
    BYTE musicMode;
    unsigned char unk_130[0x3];
    // Frames skipped per frame drawn: DrawFps counts 1 + this a frame.
    BYTE frameSkip;
    unsigned char unk_134[0x50];
    int unk_184;
    int unk_188;
    // The scene the game is in or switches to: many places set it (1
    // through 10), and the main loop's callbacks compare it with 3. The
    // music screen sets it to 1 to leave; RegisterChain to -1.
    int scene;
    unsigned char unk_190[0x8];
    int unk_198;
    // DrawFps shows the frame rate while it is 0.
    int unk_19c;
    unsigned char unk_1a0[0x8];
    // The part of a frame a Timer advances per frame while the game runs
    // slower than full speed.
    float frameAdvance;
    // 1.0 at full speed; at 0.99 or below, Timers count fractions of
    // frames.
    float gameSpeed;
    // Plays music in MIDI mode; may be NULL.
    MidiPlayer *midi;
    // While g_Unknown69d4c1 is set, DrawFps adds up every half second
    // the frames there should have been (unk_1b8) and a share of them
    // that shrinks as the frame rate drops (unk_1b4).
    float unk_1b4;
    float unk_1b8;
    // The open data archives; NULL where none is open.
    DatArchive *archives[0x10];
    unsigned char unk_1fc[0x204];
    // timeGetTime() when AddedCallback ran.
    DWORD unk_400;
};
extern Unknown6c6d18 g_Unknown6c6d18;

// The buttons held this frame and the frame before, as bits: 0x10 up,
// 0x20 down, 0x1001 select, 0xa cancel.
extern WORD g_Buttons;
extern WORD g_PrevButtons;

// Whether any of the buttons in `mask` went down this frame.
#define WAS_PRESSED(mask) ((g_Buttons & (mask)) && (g_Buttons & (mask)) != (g_PrevButtons & (mask)))

// The game's random number generator, at 0x69d8f8.
class Rng
{
public:
    void SetSeed(WORD seed)
    {
        this->seed = seed;
        this->unk_04 = 0;
    }

    WORD seed;
    unsigned char unk_02[0x2];
    int unk_04;
};
extern Rng g_Rng;

// Points at Unknown6c6d18::archives.
extern DatArchive **g_Archives;

int Func423b14(Unknown6c6d18 *game);
void Func41f008();

// Counts frames and draws the frame rate ("%.02ffps"), updated every
// half second, at the bottom right.
void DrawFps();
// Makes DrawFps measure how far the frame rate falls short.
extern BYTE g_Unknown69d4c1;

void Func42ab30(int unk1, char *unk2);
// Releases the COM object at 0x69e230, if there is one.
void Func41f02b();

// IDirectInput8::EnumDevices's callback for joysticks, from the DirectX
// 8 SDK's Joystick sample: creates a device for the first joystick that
// works, unless there already is one.
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext);

// IDirectInputDevice8::EnumObjects's callback for the joystick's axes,
// from the same sample without its dialog updates: sets each axis's
// range to -1000 to 1000.
BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE *pdidoi, VOID *pContext);

// Loads a whole file into a malloc'd buffer; NULL on failure.
void *Func41e290(char *path, int unk);
// The size of the file Func41e290 loaded last.
extern int g_FileSize;

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
