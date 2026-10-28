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

// The mutex CheckSingleInstance creates, held while the game runs.
extern HANDLE g_AppMutex;
// Creates g_AppMutex. Returns 0, or -1 (after logging that two cannot
// run) if another copy of the game holds it.
int CheckSingleInstance();

// Sets the device's render and texture stage states for drawing
// sprites, as the options allow; again after the device is reset. Makes
// the SpriteEngine and the stage set their own states again.
void SetDefaultRenderStates();
// Creates the Direct3D device for the configuration, falling back from
// hardware vertex processing to HAL to the reference rasterizer, and
// from 60 Hz to presenting immediately or from a lockable back buffer
// to none when that fails; then sets up the camera, reads the caps and
// adjusts the options to them. Returns 0, or 1 if no device can be
// created.
int InitD3dDevice();
// Presents the frame. If that fails, resets the device (the surfaces
// are released first and the states set again) and has unk_198 count
// 2 frames; then does the SpriteEngine's pending copy and counts
// unk_198 down.
void PresentFrame();

// The game's window and the main loop's state.
struct GameWindow
{
    // Runs a frame: the calc chain, and unless the frame is skipped, the
    // draw chain and PresentFrame; in windowed mode (or when presenting
    // immediately) it waits for 1/60 s to pass. Returns 0, or what the
    // calc chain asks for: 1 to quit, 2 to restart. Does nothing while
    // the game is not the active app.
    int RunFrame();

    // The window, also kept in g_Unknown6c6d18.hwnd.
    HWND hwnd;
    // Set when the window gets WM_CLOSE: the main loop ends.
    int isClosing;
    // WM_ACTIVATEAPP's wParam: nonzero while the game is the active app.
    WPARAM isAppActive;
    // Set while the game is not the active app: the window shows the
    // cursor then.
    int isAppInactive;
    // Frames run since the last one drawn, for the frame skip.
    BYTE curFrame;
    unsigned char unk_11[0x3];
    // The screen saver and power saving settings (SPI_GETSCREENSAVEACTIVE,
    // SPI_GETLOWPOWERACTIVE, SPI_GETPOWEROFFACTIVE), turned off while
    // the game runs and restored at exit.
    int screenSaveActive;
    int lowPowerActive;
    int powerOffActive;
};
ASSERT_SIZE(GameWindow, 0x20);
extern GameWindow g_GameWindow;
// The window procedure of the window class "BASE".
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// Registers the window class "BASE" and creates the game's window: at
// 640 by 480 in full screen mode, or with room for its frame and caption
// in windowed mode.
void CreateGameWindow(HINSTANCE instance);

// The global at 0x6c6d18 plays wave music through it.
class Unknown6d3f50
{
public:
    // Zeroes everything; no sound is loaded.
    Unknown6d3f50();
    // Sets up DirectSound for `hwnd` (stereo, 44.1 kHz, 16 bits) and
    // starts a silent looping buffer and a 250 ms timer. Returns 0 or -1.
    int Initialize(HWND hwnd);
    // Releases the sounds, the wave music and DirectSound. Returns 0.
    int Release();
    // Loads the wave file at `path` as the streaming wave music, after
    // stopping what plays, and starts the thread that streams it.
    // Returns 0 or -1.
    int LoadBgm(char *path);
    // Sets the wave music's loop from the .pos file at `path`: its start
    // and end, in samples of 4 bytes. Returns 0 or -1.
    int LoadBgmLoop(char *path);
    // Plays the wave music from the start, filling the buffer with
    // `bLoop` as the repeat flag. Returns 0 or -1.
    int PlayBgm(BOOL bLoop);
    // Stops the wave music: ends the notification thread and releases
    // the stream ("stop BGM").
    void StopBgm();
    // Fades the wave music out over `seconds`, counted in frames at 60
    // per second.
    void FadeOutBgm(float seconds);
    // Loads the sound effects ("data/wav/plst00.wav", ...).
    int LoadSounds();
    // Loads the wave file at `path` into soundBuffers[`index`]. Returns
    // 0, also when there is no sound, or -1 on failure.
    int LoadSound(int index, char *path);
    // Queues sound effect `sound`, unless it is already queued or three
    // are. `unused` is not used.
    void QueueSound(int sound, int unused);
    // Plays the queued sound effects from the start, if sound effects
    // are on, and empties the queue.
    void PlayQueuedSounds();

    IDirectSound8 *dsound;
    DWORD unk_004;
    // The loaded sound files' buffers.
    IDirectSoundBuffer *soundBuffers[0x80];
    // Each sound effect's own buffer.
    IDirectSoundBuffer *sounds[0x80];
    // Set from g_SoundInfos[].unk_6 when a sound is queued; -1 at
    // first.
    int unk_408[0x80];
    // A silent buffer, looping from start-up.
    IDirectSoundBuffer *silence;
    // The window the sound timer posts to.
    HWND hwnd;
    // NULL until Initialize succeeds.
    CSoundManager *manager;
    // The thread streaming the wave music and its ID.
    DWORD notifyThreadId;
    HANDLE notifyThread;
    DWORD unk_61c;
    // The sound effects to play, or -1.
    int queue[3];
    // The wave music playing; NULL when stopped.
    CStreamingSound *bgm;
    // Signalled as the wave music plays.
    HANDLE notifyEvent;
    // PlayBgm's `bLoop`.
    BOOL bgmLoop;
};
ASSERT_SIZE(Unknown6d3f50, 0x638);
extern Unknown6d3f50 g_Unknown6d3f50;

// The thread LoadBgm starts, with the main window: streams the wave music
// each time notifyEvent is signalled, until WM_QUIT or the music is gone.
// The DX8 SDK's StreamData sample's NotificationProc, looping always.
DWORD WINAPI BgmNotificationProc(LPVOID lpParameter);

// Each sound effect's sound buffer and volume.
struct SoundInfo
{
    int buffer;
    short volume;
    short unk_6;
};
extern SoundInfo g_SoundInfos[0x20];
// The sound files LoadSounds loads, by index.
extern char *g_SoundPaths[];

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
    // The lives and bombs to start with. 2, at most 4; 3, at most 3.
    BYTE lives;
    BYTE bombs;
    // Whether full screen mode uses 16-bit colors rather than 32: 0xff
    // (at most 1) until the first start picks one from the desktop's.
    BYTE colorMode16Bits;
    // How music is played: 1 is wave, 2 is MIDI. LoadConfig picks 1 if
    // "bgm/th06_01.wav" opens, otherwise 2 after logging "wave データが
    // 無いので、midi にします" ("no wave data, switching to MIDI").
    BYTE musicMode;
    // Whether sound effects play. 1, at most 1.
    BYTE playSounds;
    // 1, at most 4.
    BYTE unk_1d;
    // Whether the game runs in a window rather than full screen. 0, at
    // most 1.
    BYTE windowed;
    // Frames skipped per frame drawn: DrawFps counts 1 + this a frame.
    // 0, at most 2.
    BYTE frameSkip;
    // How far a DirectInput pad axis (-1000 to 1000) goes before it
    // counts as a direction. 600 each.
    short padThresholdX;
    short padThresholdY;
    unsigned char unk_24[0x10];
    GameConfigFlags flags;
};
ASSERT_SIZE(GameConfig, 0x38);

// Writes `size` bytes of `data` to the file at `path`. Returns 0, -1 if
// the file cannot be opened, -2 if writing fails.
int WriteToFile(char *path, void *data, int size);

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

    // Whether the 60 frame mode presents without waiting for vsync.
    BOOL UsesPresentImmediate()
    {
        return this->cfg.flags.force60Fps && this->presentImmediate;
    }

    // WinMain's instance.
    HINSTANCE hInstance;
    // Created by Direct3DCreate8 (CreateD3d).
    IDirect3D8 *d3d;
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
    // Set on the device by SetupCamera.
    D3DXMATRIX viewMatrix;
    D3DXMATRIX projectionMatrix;
    D3DVIEWPORT8 viewport;
    // What the device was created with; AddedCallback resets the device
    // with them when presenting fails.
    D3DPRESENT_PARAMETERS presentParams;
    // Loaded from and saved to "東方紅魔郷.cfg" by LoadConfig.
    GameConfig cfg;
    // Another copy of the configuration; Player::OnUpdate refills the
    // bombs from it.
    GameConfig unk_14c;
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
    // Set when the 60 frame mode presents without waiting for vsync
    // (D3DPRESENT_INTERVAL_IMMEDIATE), because the refresh rate could
    // not be set to 60 Hz. Cleared by LoadConfig when the 60 frame mode
    // is forced.
    int presentImmediate;
    DWORD unk_1a4;
    // The part of a frame a Timer advances per frame while the game runs
    // slower than full speed. Movement per frame is scaled by it too.
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
    // Set when the device was created with hardware vertex processing
    // ("T&L HAL"); the popups and bullets are drawn differently without.
    BYTE hardwareTnL;
    // Whether the back buffer was made lockable
    // (D3DPRESENTFLAG_LOCKABLE_BACKBUFFER).
    BYTE lockableBackBuffer;
    // Whether D3DFMT_A8R8G8B8 textures can be created.
    BYTE hasA8R8G8B8Textures;
    unsigned char unk_3ff[0x1];
    // timeGetTime() when AddedCallback ran.
    DWORD unk_400;
    // The device's capabilities.
    D3DCAPS8 caps;
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
// Sets `bit` in `buttons` if pad button `button` (none when negative) is
// down in DIJOYSTATE2::rgbButtons. Returns the bit set, or 0.
int SetButtonFromDirectInput(WORD *buttons, short button, WORD bit, BYTE *rgbButtons);
// Like SetButtonFromDirectInput, for joyGetPosEx's dwButtons.
int SetButtonFromJoystick(WORD *buttons, short button, WORD bit, DWORD joyButtons);
// Reads g_JoyCaps if joystick 1 answers; otherwise logs that there is
// no pad. Returns 0, or 1 without a pad.
WORD InitJoystick();
// The joystick's capabilities, from joyGetDevCaps.
extern JOYCAPSA g_JoyCaps;
// How many frames the shot button has been held, up to 16, when shooting
// and focusing share a pad button: focus counts from 8.
extern WORD g_ShotHeldFrames;
// The pad's buttons as GetPadButtons last read them: 0x80 when down.
extern BYTE g_PadButtons[0x80];
// Reads which pad buttons are down into g_PadButtons and returns it.
BYTE *GetPadButtons();
// Adds the joystick's buttons and directions to `buttons` and returns
// them: through DirectInput when there is a device, otherwise through
// joyGetPosEx.
WORD ReadJoystick(WORD buttons);
// Reads the keyboard and the joystick into button bits.
WORD ReadButtons();

// Whether any of the buttons in `mask` went down this frame.
#define WAS_PRESSED(mask) ((g_Buttons & (mask)) && (g_Buttons & (mask)) != (g_PrevButtons & (mask)))
// Also while they are held, once the repeat delay has passed.
#define WAS_PRESSED_OR_HELD(mask) (WAS_PRESSED(mask) || ((g_Buttons & (mask)) && g_ButtonRepeat))

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
    // A random float from 0 up to `range`.
    float GetRandomF32InRange(float range)
    {
        return this->GetRandomF32ZeroToOne() * range;
    }
    // A random number below `range`, or 0 if `range` is 0.
    WORD GetRandomU16InRange(WORD range)
    {
        return range != 0 ? this->GetRandomU16() % range : 0;
    }
    DWORD GetRandomU32InRange(DWORD range)
    {
        return range != 0 ? this->GetRandomU32() % range : 0;
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


// Creates g_Unknown6c6d18.d3d. Returns 0, or 1 after logging an error.
int CreateD3d();

// Scene set-up functions Unknown6c6d18::OnUpdate calls. They return
// nonzero on failure.
int Func4107b0();
int Func43a338(int unk);

// Releases ANMs 21 to 27.
void Func43a70a();

// The 640 by 64 surface text is drawn on with GDI before it is copied
// into a texture.
extern IDirect3DSurface8 *g_TextSurface;

// Creates g_TextSurface.
void CreateTextSurface();

// Returns `a + b`, brought back into -pi to pi by whole turns (at most
// 18 each way).
float AddNormalizeAngle(float a, float b);

// Rotates the 2D vector `in` by `angle` into `out`.
void Rotate2D(D3DXVECTOR3 *out, D3DXVECTOR3 *in, float angle);
// Sets *sinOut and *cosOut to the sine and cosine of `angle`, with one
// fsincos.
inline void SinCos(float *sinOut, float *cosOut, float angle)
{
    __asm
    {
        fld angle
        fsincos
        mov eax, cosOut
        fstp [eax]
        mov eax, sinOut
        fstp [eax]
    }
}
// Sets vec's x and y to `radius` along `angle`, with one fsincos.
inline void SinCosScale(D3DXVECTOR3 *vec, float angle, float radius)
{
    __asm
    {
        mov eax, vec
        fld angle
        fsincos
        fmul radius
        fstp [eax]
        fmul radius
        fstp [eax+4]
    }
}

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

// Clears the back buffer and the depth buffer to `color` and presents,
// twice, so both buffers of the swap chain are cleared; resets the
// device when presenting fails.
void ClearScreen(D3DCOLOR color);
// Sets the viewport to the whole 640x480 screen, then ClearScreen.
void ClearViewport(D3DCOLOR color);
// Fills `rect` (left, top, right and bottom) with `color`.
void DrawColorRect(float *rect, D3DCOLOR color);
void Func41c28c(int unk);

// Clears the high bit of every key's state, so no key reads as held.
void ResetKeyboard();

// Counts frames and draws the frame rate ("%.02ffps"), updated every
// half second, at the bottom right.
void DrawFps();

// Ends the replay being recorded: unless `path` is NULL, saves it there
// under `name`, with the date and the slowdown, checksummed and
// encrypted; then frees its stages. Removes its update element either
// way.
void SaveReplay(char *path, char *name);
// Releases g_TextSurface, if there is one.
void ReleaseTextSurface();

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

// The object error messages go to, e.g. "error : MIDI File が読み込めない":
// a text buffer written to "./log.txt" at exit.
class Unknown69d998
{
public:
    // If anything was logged, ends the text with a line of dashes, shows
    // it in a message box if an error was logged, and writes it to
    // "./log.txt".
    void Flush();

    char buffer[0x800];
    // The end of the text in `buffer`.
    char *cursor;
    // Set by Func41e620, the error logger.
    char hasError;
};
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



// A loaded message file ("data/msg1.dat", ...): a count, then the
// offsets of its scripts, made pointers once loaded.
struct MsgFile
{
    int count;
};

// A message Gui shows for a while at a fixed place, with a number.
struct GuiPopup
{
    D3DXVECTOR3 pos;
    int value;
    int shown;
    Timer timer;
};

// The bulk of Gui, allocated by its added callback (0x2c44 bytes,
// constructed at 0x41b34b).
struct GuiImpl
{
    // Starts script `index` of msgFile.
    void Func41878b(int index);
    void Func4188c9();

    Sprite unk_0000[0x1a];
    DWORD unk_1ba0;
    Sprite unk_1ba4;
    Sprite unk_1cb4;
    Sprite unk_1dc4;
    // Func417bfd shows a sprite and a line of text on these two.
    Sprite unk_1ed4;
    Sprite unk_1fe4;
    Sprite unk_20f4;
    Sprite unk_2204;
    Sprite unk_2314;
    Sprite unk_2424;
    // The message file loaded.
    MsgFile *msgFile;
    DWORD unk_2538;
    int unk_253c;
    DWORD unk_2540[0x1a5];
    DWORD unk_2bd4;
    BYTE unk_2bd8;
    BYTE unk_2bd9[0x3];
    DWORD unk_2bdc[0x2];
    // The bonus for bullets cleared, the full power message and one more.
    GuiPopup bonusPopup;
    GuiPopup fullPowerPopup;
    GuiPopup unk_2c24;
};
ASSERT_SIZE(GuiImpl, 0x2c44);

// The in-game interface, the global at 0x69bc30: it loads
// "data/front.anm" and the dialogue portraits ("data/face00a.anm", ...)
// and draws the stage clear bonus ("Stage Clear", "Graze *   10 = %5d").
// Gui's flags, a bitfield. The player and the items set these to 2 when
// the lives, the bombs, the power, the graze and the point items
// change.
struct GuiFlags
{
    unsigned int livesChanged : 2;
    unsigned int bombsChanged : 2;
    unsigned int powerChanged : 2;
    unsigned int grazeChanged : 2;
    unsigned int pointsChanged : 2;
    unsigned int unk10 : 22;
};

class Gui
{
public:
    // Loads the interface's sprites.
    int Func417d29();

    // The update element's callback: unless g_GameManager.unk_002c is
    // set, runs Func4195bf and impl->Func4188c9. Returns 1.
    static int OnUpdate(Gui *gui);
    // The update element's added callback (the element at 0x69bc7c):
    // runs Func417d29.
    static int AddedCallback(Gui *gui);
    // Removes the update element and the draw element.
    static void RemoveChain();

    // Runs impl->Func41878b(`index`) and sets
    // g_Unknown6c6d18.unk_198 to 3.
    void StartMsg(int index);
    // Whether impl->unk_2424 shows a sprite and has flag 13.
    BOOL Func4172d3();
    // Shows spell card `name` with sprite `sprite`.
    void Func417acd(int sprite, char *name);
    // Shows sprite `sprite` + 0x4a8 with `text` (scripts 0x4a3 and 0x707)
    // and plays sound 14.
    void Func417bfd(int sprite, char *text);
    // Sets impl->unk_1fe4.pendingInterrupt.
    void Func417314();
    // Shows fullPowerPopup, when the power reaches its maximum.
    void ShowFullPowerPopup(int unk);
    // Shows `value` in unk_2c24, from enemy scripts.
    void Func417458(int value);
    // Shows `bonus` in bonusPopup, the bonus for bullets cleared.
    void ShowBonusPopup(int bonus);
    // Sets impl->unk_20f4.pendingInterrupt.
    void Func41732c();
    // Returns impl->unk_2bd8.
    BOOL Func417344();
    // Whether impl->unk_2bd4 is 0 and impl->unk_253c is not negative.
    BOOL Func419572();
    // Whether impl->unk_253c is not negative.
    BOOL Func4195a2();
    // Frees impl->msgFile, if there is one.
    void FreeMsgFile();
    BYTE IsBossPresent()
    {
        return this->bossPresent;
    }
    void SetBossTimer(int seconds)
    {
        this->bossTimer = seconds;
    }
    // Sets the boss's life bar, as a fraction of its maximum.
    void SetBossHealth(float health)
    {
        this->bossHealth = health;
    }
    void Func4195bf();

    GuiFlags flags;
    GuiImpl *impl;
    DWORD unk_08;
    // Set by Func417bfd from the length of its text.
    float unk_0c;
    DWORD unk_10[0x2];
    // The seconds left of the boss's timer.
    int bossTimer;
    DWORD unk_1c;
    // Set while a boss fight runs.
    BYTE bossPresent;
    BYTE unk_21[0x3];
    // The boss's life as a fraction of its maximum, for its bar.
    float bossHealth;
    DWORD unk_28;
};
ASSERT_SIZE(Gui, 0x2c);
extern Gui g_Gui;

// Gui's statically allocated chain elements.
extern ChainElem g_GuiCalcChain;
extern ChainElem g_GuiDrawChain;

// How a D3DFORMAT's pixels are laid out, for a DIB section.
struct FormatInfo
{
    // A D3DFORMAT; -1 ends the table.
    int format;
    int bitCount;
    DWORD alphaMask;
    DWORD redMask;
    DWORD greenMask;
    DWORD blueMask;
};
// The formats GdiSurface supports: X8R8G8B8, A8R8G8B8, X1R5G5B5, R5G6B5,
// A1R5G5B5 and A4R4G4B4.
extern FormatInfo g_FormatInfos[];

// A pixel in D3DFMT_A1R5G5B5.
struct A1R5G5B5
{
    WORD b : 5;
    WORD g : 5;
    WORD r : 5;
    WORD a : 1;
};

// A GDI DIB section text is drawn into before it is copied to a Direct3D
// surface; `format` is the D3DFORMAT its pixels follow.
class GdiSurface
{
public:
    GdiSurface();
    ~GdiSurface();
    // Selects the old object back into `hdc`, then deletes the DC and the
    // bitmap. Returns whether there was a DC.
    bool Release();
    // Creates the DIB section in `format`, or failing that in a 32-bit
    // format with the same channels. Returns whether it was created.
    bool Create(int width, int height, int format);
    // Creates the DIB section and its DC, after releasing any old ones.
    bool CreateInFormat(int width, int height, int format);
    // The entry of g_FormatInfos for `format`, or the end of the table
    // when it is missing; NULL for -1.
    FormatInfo *FindFormat(int format);
    // Copies the pixels into `surface` when it has the same format.
    // Returns false when there is no bitmap or the surface can't be
    // locked.
    bool CopyToSurface(IDirect3DSurface8 *surface);
    // Flips the alpha of the pixels in rows `y` on (GDI leaves alpha at
    // 0), taking `width` and `height` in 16-bit pixels. In A1R5G5B5 it
    // also shades the colors darker towards the bottom. Returns false
    // for other formats.
    bool InvertAlpha(int x, int y, int width, int height);

    void *GetBits()
    {
        return this->bits;
    }
    bool IsCreated()
    {
        return this->bitmap != NULL;
    }
    int GetWidth()
    {
        return this->width;
    }
    int GetHeight()
    {
        return this->height;
    }
    int GetFormat()
    {
        return this->format;
    }

    int format;
    int width;
    int height;
    DWORD imageSize;
    int widthBytes;
    HDC hdc;
    HGDIOBJ oldObject;
    HBITMAP bitmap;
    void *bits;
};
ASSERT_SIZE(GdiSurface, 0x24);

// Draws `text` at `x` in "ＭＳ ゴシック" `fontHeight` high, with a shadow
// unless `shadowColor` is -1, on g_TextSurface at twice the size, then
// scales `width` by `fontHeight` of it into `texture` at row `y`.
void RenderTextToTexture(int x, int y, int width, int unk4, int fontHeight, int unk6, COLORREF textColor,
                         COLORREF shadowColor, char *text, IDirect3DTexture8 *texture);
