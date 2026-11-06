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
    unsigned char pad_11[0x3];
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

#include "sound.h"

#include "app.h"

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
    WORD pad_02;
    // How many numbers were generated since the seed was set.
    int generated;
};
extern Rng g_Random;

// Points at App::archives.
extern DatArchive **g_DatArchives;

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
// functions directly: the original only constructs one global TextFont,
// whose vtable keeps them (and D3DX8's font code) in the exe.
class TextFont
{
public:
    TextFont()
    {
        this->font = NULL;
    }

    // Creates the font at `width` by `height`.
    virtual void Create(IDirect3DDevice8 *device, int width, int height);
    // Draws `text` from (`x`, `y`) in `color`.
    virtual int Draw(char *text, int x, int y, D3DCOLOR color);
    // Releases the font, if there is one.
    virtual void Release();

    ID3DXFont *font;
};

#ifdef TH06_EXE
extern TextFont g_TextFont;
#endif

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
extern int g_LastFileSize;

// The object error messages go to, e.g. "error : MIDI File が読み込めない":
// a text buffer written to "./log.txt" at exit.
class Log;
int LogPrintf(Log *log, const char *fmt, ...);
class Log
{
public:
    // Empties the buffer and logs the header line. Inline: the original
    // runs it as g_Logger's static initializer.
    Log()
    {
        this->cursor = this->buffer;
        *this->cursor = '\0';
        this->hasError = 0;
        LogPrintf(this, "東方動作記録 --------------------------------------------- \n");
    }
    // Empty, but registered with atexit for g_Logger like the original's.
    ~Log()
    {
    }

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
extern Log g_Logger;

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



#include "gui.h"

#include "gdisurface.h"
