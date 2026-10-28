// Classes and functions not yet assigned to a source file. Names are
// placeholders carrying an address until behavior shows what they are.

#pragma once

#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "archive.h"
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
// are released first and the states set again) and has redrawFrames
// count 2 frames; then does the SpriteEngine's pending copy and counts
// redrawFrames down.
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

    // The window, also kept in g_App.hwnd.
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

// The sound effects and the wave music, through DirectSound and the DX8
// SDK's CSoundManager; the global is at 0x6d3f50 and g_App plays wave
// music through it.
class SoundPlayer
{
public:
    // Zeroes everything; no sound is loaded.
    SoundPlayer();
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
    // Set from g_SoundInfos[].unk_6 when a sound is queued (nothing reads
    // either); -1 at first.
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
ASSERT_SIZE(SoundPlayer, 0x638);
extern SoundPlayer g_SoundPlayer;

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

// DatArchive is in archive.h.

// The pad buttons assigned to the game's actions, -1 where none is.
// Named fields rather than an array: a local copy (MainMenu::OnUpdate's
// defaults) gets no /GS cookie; the key configuration indexes them in
// order through a short pointer.
struct ControllerMapping
{
    short shootButton;
    short bombButton;
    short focusButton;
    short menuButton;
    short upButton;
    short downButton;
    short leftButton;
    short rightButton;
    short skipButton;
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
    // The difficulty the menu starts on, the last one picked. 1, at most
    // 4.
    BYTE defaultDifficulty;
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

// The application, at 0x6c6d18: the window, device, input, settings,
// scenes, music, archives and game speed. Larger than its known fields.
class App
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
    static int OnUpdate(App *arg);
    static int AddedCallback(App *arg);
    static int DeletedCallback(App *arg);
    // The draw chain callback: resets the cached device state and draws
    // the frame rate. Returns 1.
    static int OnDraw(App *arg);

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
    // Another copy of the configuration, of which the lives and bombs
    // count: set from the game's when a stage starts (outside the demo),
    // read back on a continue; Player::OnUpdate refills the bombs from
    // it and the Gui's penalty line reads its lives.
    GameConfig startCfg;
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
    // Frames left in which the Gui draws the items around the play area
    // even though the back buffer is kept: set to 3 when a stage starts,
    // on pausing, by Gui messages and after a device reset (2 when
    // presenting fails); counted down by PresentFrame.
    int redrawFrames;
    // DrawFps shows the frame rate while it is 0; the ending sets it.
    int hideFps;
    // Set when the 60 frame mode presents without waiting for vsync
    // (D3DPRESENT_INTERVAL_IMMEDIATE), because the refresh rate could
    // not be set to 60 Hz. Cleared by LoadConfig when the 60 frame mode
    // is forced.
    int presentImmediate;
    // timeGetTime() when RunFrame last measured the frame rate.
    DWORD lastFrameTime;
    // The part of a frame a Timer advances per frame while the game runs
    // slower than full speed. Movement per frame is scaled by it too.
    float frameAdvance;
    // 1.0 at full speed; at 0.99 or below, Timers count fractions of
    // frames.
    float gameSpeed;
    // Plays music in MIDI mode; may be NULL.
    MidiPlayer *midi;
    // While g_GameManager.isPlaying is set, DrawFps adds up every half
    // second the frames there should have been (expectedFrames) and a
    // share of them that shrinks as the frame rate drops
    // (achievedFrames); the result screen and replays show the slowdown
    // from them.
    float achievedFrames;
    float expectedFrames;
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
    // The title screen waits until 3 s after it to start the music.
    DWORD startTime;
    // The device's capabilities.
    D3DCAPS8 caps;
};
extern App g_App;

inline void Timer::Tick()
{
    this->previous = this->current;
    g_App.TickTimer(&this->current, &this->subFrame);
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

// Points at App::archives.
extern DatArchive **g_Archives;

// Sets up DirectInput like the DirectX 8 SDK's Keyboard and Joystick
// samples: the keyboard, then the first joystick found, if any. Returns
// 0, or -1 if DirectInput is off or the keyboard fails.
int InitDirectInput(App *game);


// Creates g_App.d3d. Returns 0, or 1 after logging an error.
int CreateD3d();

// Scene set-up functions App::OnUpdate calls. They return
// nonzero on failure.
// Zeroes g_MainMenu and adds its elements; `unk` starts it in state
// 12 instead of 0.
int SetUpTitleScene(int unk);

// Releases ANMs 21 to 27.
void ReleaseTitleAnms();

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
class TextFont
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
// Like SetupCamera, without the camera's offset and zoom: looks at the
// viewport's middle from straight in front, with the far plane at
// 10000 + `extraFar`.
void SetupFixedCamera(float extraFar);

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

// Loads a whole file into a malloc'd buffer; NULL on failure. Unless
// `isExternal` is set, looks it up by its name (the part after the
// last '/', or the whole path) in the open archives and decodes it from
// the first that has it; otherwise reads it from disk.
void *LoadFileData(char *path, int isExternal);
// The size of the file LoadFileData loaded last.
extern int g_FileSize;

// The object error messages go to, e.g. "error : MIDI File が読み込めない":
// a text buffer written to "./log.txt" at exit.
class Log
{
public:
    // If anything was logged, ends the text with a line of dashes, shows
    // it in a message box if an error was logged, and writes it to
    // "./log.txt".
    void Flush();

    char buffer[0x800];
    // The end of the text in `buffer`.
    char *cursor;
    // Set by LogError, the error logger.
    char hasError;
};
extern Log g_Log;

// Logs a printf-style message to a log object.
int LogPrintf(Log *log, const char *fmt, ...);
// Logs a printf-style error message to a log object.
int LogError(Log *log, const char *fmt, ...);

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
    void *scripts[1];
};

// The arguments of most message instructions: who speaks (0 the
// player, 1 the boss; 2 and 3 select the text lines for opcode 5) and a
// sprite, script or line number, then the text.
struct MsgArgs
{
    short who;
    union
    {
        short index;
        BYTE value;
    };
    char text[1];
};

// One instruction of a message script.
struct MsgInstr
{
    // The frame it runs on.
    WORD time;
    BYTE opcode;
    // The size of the arguments, in bytes.
    BYTE argsSize;
    union
    {
        MsgArgs args;
        int i;
        BYTE b;
    };
};

// The dialogue Gui runs (GuiImpl+0x2534, 0x6a8 bytes). Its implicit
// constructor is emitted out of line (0x41b4c4), called from GuiImpl's.
struct GuiMsg
{
    // The message file loaded.
    MsgFile *msgFile;
    // The next instruction of the running script.
    MsgInstr *instr;
    // The running script; negative when none runs.
    int scriptIndex;
    Timer timer;
    // Counts the frames since the last line of text showed, for the wait
    // instruction.
    int waitFrames;
    // The player's and the boss's portraits.
    Sprite portraits[2];
    // The two lines of text.
    Sprite text[2];
    // Lines shown by opcode 8 (scripts 0x704 and 0x705).
    Sprite unk_45c[2];
    // The text colors and their shadows, by speaker.
    D3DCOLOR textColors[4];
    D3DCOLOR shadowColors[4];
    // The text's height in pixels (unk_10c of the text sprites).
    int fontSize;
    // Raised by opcode 6, counted down every frame: while it is above 0
    // the enemy timeline's wait for the dialogue (MsgHoldsTimeline) ends.
    DWORD timelineRelease;
    // Lets the skip button (0x100) fast-forward the script; set by
    // opcode 13.
    BYTE skippable;
    BYTE unk_6a5[0x3];
};
ASSERT_SIZE(GuiMsg, 0x6a8);

// A message Gui shows for a while at a fixed place, with a number.
struct GuiPopup
{
    D3DXVECTOR3 pos;
    int value;
    int shown;
    Timer timer;
};

// The bulk of Gui, allocated by Gui::RegisterChain (0x2c44 bytes). Its
// implicit constructor is emitted out of line at 0x41b34b.
struct GuiImpl
{
    // Starts script `index` of msg.msgFile, if there is one: resets
    // the dialogue's state and the text's colors, and loads the effects
    // of stage 6 and the extra stage for their scripts 0 and 10.
    void StartMsgScript(int index);
    // Runs the dialogue for a frame: the instructions due (portraits,
    // text, waits for the shot button, music, the stage's end), then
    // steps its sprites. Returns 0, or -1 when no script runs.
    int RunMsg();
    // Draws the dialogue: the portraits, a dark box under the text that
    // opens over its first second, and the text. Returns 0, or -1 when no
    // script runs.
    int DrawMsg();

    // The interface's sprites (scripts from 0x600 on).
    Sprite sprites[0x1a];
    // The boss's bar (sprites[0x13]): 0 hidden, 1 appearing (script
    // 0x613), 2 shown, 3 leaving (script 0x614).
    BYTE bossBarState;
    BYTE unk_1ba1[0x3];
    // The stage's name and the song's (script 0x700 and 0x701).
    Sprite stageName;
    Sprite songName;
    // ShowBombName's portrait; ShowSpellCardName's.
    Sprite bombPortrait;
    Sprite spellPortrait;
    // The bomb's and the spell card's names, and the bars behind them.
    Sprite bombName;
    Sprite spellName;
    Sprite bombNameBar;
    Sprite spellNameBar;
    // The loading screen ("data/loading.anm", script 0x619), between
    // stages; spriteIndex is -1 when it is not shown.
    Sprite loading;
    GuiMsg msg;
    // Set by the dialogue's opcode 9: OnDraw then shows the stage clear
    // bonus, stageClearTotal.
    DWORD stageClearShown;
    DWORD stageClearTotal;
    // The bonus for bullets cleared, the full power message and the spell
    // card's bonus.
    GuiPopup bonusPopup;
    GuiPopup fullPowerPopup;
    GuiPopup spellBonusPopup;
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
    // Loads the interface's sprites ("data/front.anm"), the character's
    // and the stage's portraits and the stage's message file (all but
    // the stage's kept when the scene is 3), then starts the interface's
    // scripts and shows the stage and song names. Returns 0, or -1 if a
    // file fails to load.
    int LoadResources();

    // The update element's callback: unless g_GameManager.isFrozen is
    // set, runs UpdateInterface and impl->RunMsg. Returns 1.
    static int OnUpdate(Gui *gui);
    // The draw element's callback: the stage clear bonus once
    // impl->stageClearShown is set, the dialogue, DrawBanners, DrawHud and the
    // popups, with the depth test off. Returns 1.
    static int OnDraw(Gui *gui);
    // Draws the stage's title ("STAGE %d", "FINAL STAGE", "EXTRA STAGE",
    // or " DEMO PLAY"), the song's name, the sprites ShowSpellCardName
    // and ShowBombName show with their bars, and `loading` in the play
    // area.
    void DrawBanners();
    // Draws the interface: the boss's bar, bossLives and timer
    // (ticking under 10 seconds), the frame around the play area when
    // it changed, and the score, the high score, lives, bombs, power
    // (a bar), graze and point items, each while its flag counts down.
    void DrawHud();
    // The update element's added callback (the element at 0x69bc7c):
    // runs LoadResources.
    static int AddedCallback(Gui *gui);
    // Removes the update element and the draw element.
    static void RemoveChain();
    // Unless the scene is 3, zeroes the interface and allocates impl;
    // then adds the update element (priority 12) and the draw element
    // (priority 11). Returns 0, or -1 if adding fails.
    static int RegisterChain();
    // Frees the stage's portraits and message file and, unless the
    // scene is 3, the interface's sprites and impl. Returns 0.
    static int DeletedCallback(Gui *gui);

    // Runs impl->StartMsgScript(`index`) and sets
    // g_App.redrawFrames to 3.
    void StartMsg(int index);
    // Whether impl->loading shows a sprite and has stopped (flag 13): the
    // loading screen is fully up, and the stage is not drawn.
    BOOL IsLoadingShown();
    // Shows the player's bomb, spell card `name`, with sprite `sprite`
    // (scripts 0x4a1 and 0x706), sets g_App.redrawFrames to 3 and
    // plays sound 14.
    void ShowBombName(int sprite, char *name);
    // Shows a boss's spell card, `text`, with sprite `sprite` + 0x4a8
    // (scripts 0x4a3 and 0x707), from enemy scripts, and plays sound 14.
    void ShowSpellCardName(int sprite, char *text);
    // Sets impl->bombName.pendingInterrupt, as the bomb ends.
    void EndBombName();
    // Shows fullPowerPopup, when the power reaches its maximum.
    void ShowFullPowerPopup(int unk);
    // Shows `value` in spellBonusPopup, the bonus of a spell card
    // captured, from enemy scripts.
    void ShowSpellCardBonus(int value);
    // Shows `bonus` in bonusPopup, the bonus for bullets cleared.
    void ShowBonusPopup(int bonus);
    // Sets impl->spellName.pendingInterrupt, as the spell card ends.
    void EndSpellCardName();
    // Returns impl->msg.skippable.
    BOOL IsMsgSkippable();
    // Whether a dialogue script runs and impl->msg.timelineRelease is 0:
    // the enemy timeline waits for the dialogue then.
    BOOL MsgHoldsTimeline();
    // Whether a dialogue script runs.
    BOOL IsMsgRunning();
    // Loads the message file at `path` into impl->msgFile, turning its
    // offsets into pointers. Returns 0, or -1 after logging
    // "メッセージファイル %s が読み込めませんでした".
    int LoadMsg(char *path);
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
    int GetBossTimer()
    {
        return this->bossTimer;
    }
    void SetBossLives(int value)
    {
        this->bossLives = value;
    }
    void SetBossHealth(float health)
    {
        this->bossHealth = health;
    }
    // The update: the interface's sprites, the boss's bar, the popups,
    // and once the stage is cleared, the bonus (stage, graze, power and
    // point items, by difficulty and starting lives), added to the score.
    void UpdateInterface();

    GuiFlags flags;
    GuiImpl *impl;
    // The widths of the texts ShowBombName and ShowSpellCardName show,
    // from their lengths; DrawBanners fits the bars to them.
    float bombNameWidth;
    float spellNameWidth;
    // The boss bar's alpha, raised by 4 a frame up to 0xff while a boss
    // fight runs.
    DWORD bossBarAlpha;
    // The number shown at the boss bar's left, set by enemy scripts (the
    // boss's remaining bars, as far as can be told).
    int bossLives;
    // The seconds left of the boss's timer.
    int bossTimer;
    // bossTimer when DrawHud last ran: it ticks when the timer changes.
    DWORD lastBossTimer;
    // Set while a boss fight runs.
    BYTE bossPresent;
    BYTE unk_21[0x3];
    // The boss's life as a fraction of its maximum, for its bar.
    float bossHealth;
    // The life the bar shows, following bossHealth.
    float bossHealthShown;
};
ASSERT_SIZE(Gui, 0x2c);
extern Gui g_Gui;
// The boss timer's colors: 20 seconds and more, 10, 5, and less
// (0x4764a0).
extern D3DCOLOR g_BossTimerColors[4];

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
