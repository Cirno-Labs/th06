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
    DatArchive();
    // Calls Release.
    ~DatArchive();

    // Opens the archive at `path` and reads its entry table. Returns
    // nonzero on success.
    int Open(char *path);
    // Returns the index of the entry named `name`, or -1.
    int FindEntry(char *name);

    // Closes the archive and frees its entry table. Returns 1.
    int Release();

    unsigned char unk_00[0x14];
};
ASSERT_SIZE(DatArchive, 0x14);

// The pad buttons assigned to the game's actions.
struct ControllerMapping
{
    short unk_00[9];
};
ASSERT_SIZE(ControllerMapping, 0x12);
// The mapping in effect, at 0x4765a0, kept in step with the config's.
extern ControllerMapping g_ControllerMapping;

// GameConfig's option bits; each logs a message when LoadConfig finds
// it set.
struct GameConfigFlags
{
    // Set whenever the configuration is loaded or reset.
    unsigned int unk0 : 1;
    // "頂点バッファの使用を抑制します"
    unsigned int noVertexBuffer : 1;
    // "16Bit のテクスチャの使用を強制します"
    unsigned int force16BitTextures : 1;
    // "バックバッファの消去を強制します"
    unsigned int clearBackBuffer : 1;
    // "ゲーム周りのアイテムの描画を抑制します"; also clears the back
    // buffer.
    unsigned int noFrameItems : 1;
    // "グーローシェーディングを抑制します"
    unsigned int noGouraudShading : 1;
    // "デプステストを抑制します"
    unsigned int noDepthTest : 1;
    // "６０フレーム強制モードにします"
    unsigned int force60Fps : 1;
    // "テクスチャの色合成を抑制します", and LoadConfig also logs
    // "ウィンドウモードで起動します" for it.
    unsigned int noColorBlending : 1;
    // "リファレンスラスタライザを強制します"
    unsigned int referenceRasterizer : 1;
    // "フォグの使用を抑制します"
    unsigned int noFog : 1;
    // "パッド、キーボードの入力に DirectInput を使用しません"
    unsigned int noDirectInput : 1;
    unsigned int unk12 : 20;
};

// The configuration file's contents. The comments give LoadConfig's
// default and the largest value it accepts.
struct GameConfig
{
    ControllerMapping controllerMapping;
    unsigned char unk_12[0x2];
    // 0x102; LoadConfig resets a file with another.
    int version;
    // 2, at most 4.
    BYTE unk_18;
    // 3, at most 3.
    BYTE unk_19;
    // 0xff, at most 1.
    BYTE unk_1a;
    // How music is played: 1 is wave, 2 is MIDI. LoadConfig picks 1 if
    // "bgm/th06_01.wav" opens, otherwise 2 after logging "wave データが
    // 無いので、midi にします" ("no wave data, switching to MIDI").
    BYTE musicMode;
    // 1, at most 1.
    BYTE unk_1c;
    // 1, at most 4.
    BYTE unk_1d;
    // 0, at most 1.
    BYTE unk_1e;
    // Frames skipped per frame drawn: DrawFps counts 1 + this a frame.
    // 0, at most 2.
    BYTE frameSkip;
    // 600 each.
    short unk_20;
    short unk_22;
    unsigned char unk_24[0x10];
    GameConfigFlags flags;
};
ASSERT_SIZE(GameConfig, 0x38);

// Writes `size` bytes of `data` to the file at `path`. Returns nonzero on
// failure.
int Func41e460(char *path, void *data, int size);

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
    // Whether the back buffer is cleared every frame: forced, or because
    // the items around the game are not drawn.
    int ClearsBackBuffer()
    {
        return this->cfg.flags.clearBackBuffer | this->cfg.flags.noFrameItems;
    }
    // Whether option bit 0 or 8 (noColorBlending) is set. Inline; the
    // operands are evaluated right to left.
    int HasFlag0Or8()
    {
        return this->cfg.flags.noColorBlending | this->cfg.flags.unk0;
    }

    // Loads the configuration from `path`, resetting it to the defaults
    // if the file is missing or invalid, logs the options in effect and
    // writes it back. Returns 0, or -1 if writing fails.
    int LoadConfig(char *path);
    // Stops the music. Returns -1 if the music mode is neither wave (1)
    // nor MIDI (2), otherwise 0.
    int StopMusic();
    // Opens the archive at `path` (such as "紅魔郷IN.dat") as archive
    // `index`, unless it already is. Returns 1 if the archive lacks the
    // version entry ("ver0102.dat"), otherwise 0, even when the archive
    // cannot be opened.
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
    // The joystick's capabilities.
    DIDEVCAPS joystickCaps;
    // Set from CreateWindowExA's result; MidiDevice::Open has MIDI
    // messages sent to it (CALLBACK_WINDOW).
    HWND hwnd;
    unsigned char unk_048[0x80];
    D3DVIEWPORT8 viewport;
    // What the device was created with; AddedCallback resets the device
    // with them when presenting fails.
    D3DPRESENT_PARAMETERS presentParams;
    // Loaded from and saved to "東方紅魔郷.cfg" by LoadConfig.
    GameConfig cfg;
    unsigned char unk_14c[0x38];
    // Counts OnUpdate's calls.
    int frames;
    // The scene set up; OnUpdate switches it to `scene` when they
    // differ.
    int curScene;
    // The scene the game is in or switches to: many places set it (1
    // through 10), and the main loop's callbacks compare it with 3. The
    // music screen sets it to 1 to leave; RegisterChain to -1.
    // OnUpdate sets up the scene requested here.
    int scene;
    // The scene before `curScene`.
    int prevScene;
    unsigned char unk_194[0x4];
    int unk_198;
    // DrawFps shows the frame rate while it is 0.
    int unk_19c;
    // Cleared by LoadConfig when the 60 frame mode is forced.
    int unk_1a0;
    unsigned char unk_1a4[0x4];
    // The part of a frame a Timer advances per frame while the game runs
    // slower than full speed.
    float frameAdvance;
    // 1.0 at full speed; at 0.99 or below, Timers count fractions of
    // frames.
    float gameSpeed;
    // Plays music in MIDI mode; may be NULL.
    MidiPlayer *midi;
    // While g_GameManager.unk_1821 is set, DrawFps adds up every half
    // second the frames there should have been (unk_1b8) and a share of
    // them that shrinks as the frame rate drops (unk_1b4).
    float unk_1b4;
    float unk_1b8;
    // The open data archives; NULL where none is open.
    DatArchive *archives[0x10];
    // The paths the archives were opened from.
    char archivePaths[0x10][0x20];
    unsigned char unk_3fc[0x4];
    // timeGetTime() when AddedCallback ran.
    DWORD unk_400;
};
extern Unknown6c6d18 g_Unknown6c6d18;

inline void Timer::Tick()
{
    this->previous = this->current;
    g_Unknown6c6d18.TickTimer(&this->current, &this->subFrame);
}

// The buttons held this frame and the frame before, as bits: 0x10 up,
// 0x20 down, 0x1001 select, 0xa cancel.
extern WORD g_Buttons;
extern WORD g_PrevButtons;

// 1 on every 8th frame once the buttons have been held unchanged for 30
// frames, for key repeat.
extern WORD g_ButtonRepeat;
// How many frames the buttons have been held unchanged, kept between 30
// and 38 once it reaches 30.
extern WORD g_ButtonFrames;
// Reads the keyboard and the joystick into button bits.
WORD ReadButtons();

// Whether any of the buttons in `mask` went down this frame.
#define WAS_PRESSED(mask) ((g_Buttons & (mask)) && (g_Buttons & (mask)) != (g_PrevButtons & (mask)))

// The game's random number generator, at 0x69d8f8.
class Rng
{
public:
    void SetSeed(WORD seed)
    {
        this->seed = seed;
        this->generated = 0;
    }

    // Advances the generator and returns 16 random bits.
    WORD GetRandomU16();
    // 32 random bits, from two GetRandomU16 calls.
    DWORD GetRandomU32();
    // A random float from 0 up to 1.
    float GetRandomF32ZeroToOne();
    // A random number below `range`, or 0 if `range` is 0.
    WORD GetRandomU16InRange(WORD range)
    {
        return range != 0 ? this->GetRandomU16() % range : 0;
    }

    WORD seed;
    WORD unk_02;
    // How many numbers were generated since the seed was set.
    int generated;
};
extern Rng g_Rng;

// Points at Unknown6c6d18::archives.
extern DatArchive **g_Archives;

// Sets up DirectInput like the DirectX 8 SDK's Keyboard and Joystick
// samples: the keyboard, then the first joystick found, if any. Returns
// 0, or -1 if DirectInput is off or the keyboard fails.
int InitDirectInput(Unknown6c6d18 *game);

// The object at 0x6d457c, run first in Unknown6c6d18::OnUpdate when it
// exists.
class Unknown6d457c
{
public:
    int Func43b4d0();
};
extern Unknown6d457c *g_Unknown6d457c;

// Scene set-up and tear-down functions Unknown6c6d18::OnUpdate calls.
// The non-void ones return nonzero on failure.
int Func41ba6a();
int Func42d773(int unk);
int Func4107b0();
void Func41c269();
int Func43a338(int unk);
void Func41f008();

// Returns `a + b`, brought back into -pi to pi by whole turns (at most
// 18 each way).
float AddNormalizeAngle(float a, float b);

// Rotates the 2D vector `in` by `angle` into `out`.
void Rotate2D(D3DXVECTOR3 *out, D3DXVECTOR3 *in, float angle);

// Text drawn with a D3DX font in "ＭＳ ゴシック". Nothing calls its
// functions directly.
class Unknown41e150
{
public:
    // Creates the font at `width` by `height`.
    void Create(IDirect3DDevice8 *device, int width, int height);
    // Draws `text` from (`x`, `y`) in `color`.
    int Draw(char *text, int x, int y, D3DCOLOR color);
    // Releases the font, if there is one.
    void Release();

    DWORD unk_0;
    ID3DXFont *font;
};

// Clears the high bit of every key's state, so no key reads as held.
void ResetKeyboard();

// Counts frames and draws the frame rate ("%.02ffps"), updated every
// half second, at the bottom right.
void DrawFps();

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
// Logs a printf-style error message to a log object.
int Func41e620(Unknown69d998 *log, const char *fmt, ...);

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


// The bulk of Gui, allocated by its added callback (0x2c44 bytes,
// constructed at 0x41b34b).
struct GuiImpl
{
    // Starts script `index` of the table at +0x2534.
    void Func41878b(int index);

    DWORD unk_0000[0x81b];
    WORD unk_206c;
    WORD unk_206e;
    DWORD unk_2070[0x43];
    WORD unk_217c;
    WORD unk_217e;
    DWORD unk_2180[0xef];
    int unk_253c;
    DWORD unk_2540[0x1a5];
    DWORD unk_2bd4;
    BYTE unk_2bd8;
    BYTE unk_2bd9[0x3];
    DWORD unk_2bdc[0x1a];
};
ASSERT_SIZE(GuiImpl, 0x2c44);

// The in-game interface, the global at 0x69bc30: it loads
// "data/front.anm" and the dialogue portraits ("data/face00a.anm", ...)
// and draws the stage clear bonus ("Stage Clear", "Graze *   10 = %5d").
class Gui
{
public:
    // Loads the interface's sprites.
    int Func417d29();

    // The update element's added callback (the element at 0x69bc7c):
    // runs Func417d29.
    static int AddedCallback(Gui *gui);
    // Removes the update element and the draw element.
    static void RemoveChain();

    // Runs impl->Func41878b(`index`) and sets
    // g_Unknown6c6d18.unk_198 to 3.
    void StartMsg(int index);
    // Sets impl->unk_206e.
    void Func417314();
    // Sets impl->unk_217e.
    void Func41732c();
    // Returns impl->unk_2bd8.
    BOOL Func417344();
    // Whether impl->unk_2bd4 is 0 and impl->unk_253c is not negative.
    BOOL Func419572();
    // Whether impl->unk_253c is not negative.
    BOOL Func4195a2();

    DWORD unk_00;
    GuiImpl *impl;
    DWORD unk_08[0x9];
};
ASSERT_SIZE(Gui, 0x2c);
extern Gui g_Gui;

// Gui's statically allocated chain elements.
extern ChainElem g_GuiCalcChain;
extern ChainElem g_GuiDrawChain;

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
