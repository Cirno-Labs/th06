// The application: configuration, window, Direct3D and the main loop.
// Included by unsorted.h where these declarations were; it relies on what
// unsorted.h declares before it.

#pragma once

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
    // Color add emulation ("色加算エミュレートモード"): set whenever the
    // configuration is loaded or reset, and when the device lacks
    // D3DTEXOPCAPS_ADD. Picks the colors of the bullets' effects and of
    // other sprites.
    unsigned int emulateColorAdd : 1;
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
    unsigned int pad_12 : 20;
};

// The configuration file's contents. The comments give LoadConfig's
// default and the largest value it accepts.
struct GameConfig
{
    ControllerMapping controllerMapping;
    unsigned char pad_12[0x2];
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
    unsigned char pad_24[0x10];
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
    // Zeroes the application. Inline: the original runs it as g_App's
    // static initializer.
    App()
    {
        memset(this, 0, sizeof(App));
    }

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
    // Whether option bit 0 (emulateColorAdd) or 8 (noColorBlending) is
    // set. Inline; the operands are evaluated right to left.
    int HasFlag0Or8()
    {
        return this->cfg.flags.noColorBlending | this->cfg.flags.emulateColorAdd;
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
    unsigned char pad_194[0x4];
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
    unsigned char pad_3ff[0x1];
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
extern WORD g_PreviousButtons;

// 1 on every 8th frame once the buttons have been held unchanged for 30
// frames, for key repeat.
extern WORD g_RepeatButtons;
// How many frames the buttons have been held unchanged, kept between 30
// and 38 once it reaches 30.
extern WORD g_ButtonHeldFrames;
// Sets `bit` in `buttons` if pad button `button` (none when negative) is
// down in DIJOYSTATE2::rgbButtons. Returns the bit set, or 0.
int SetButtonFromDirectInput(WORD *buttons, short button, WORD bit, BYTE *rgbButtons);
// Like SetButtonFromDirectInput, for joyGetPosEx's dwButtons.
int SetButtonFromJoystick(WORD *buttons, short button, WORD bit, DWORD joyButtons);
// Reads g_JoyDeviceCaps if joystick 1 answers; otherwise logs that there is
// no pad. Returns 0, or 1 without a pad.
WORD InitJoystick();
// The joystick's capabilities, from joyGetDevCaps.
extern JOYCAPSA g_JoyDeviceCaps;
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
#define WAS_PRESSED(mask) ((g_Buttons & (mask)) && (g_Buttons & (mask)) != (g_PreviousButtons & (mask)))
// Also while they are held, once the repeat delay has passed.
#define WAS_PRESSED_OR_HELD(mask) (WAS_PRESSED(mask) || ((g_Buttons & (mask)) && g_RepeatButtons))
