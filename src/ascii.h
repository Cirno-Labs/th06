// Text drawn in the ASCII font (data/ascii.anm).

#pragma once

#include <d3dx8.h>

#include "chain.h"
#include "common.h"
#include "gamemanager.h"
#include "sprite.h"

// A string queued by AsciiText::AddString.
struct AsciiString
{
    char text[0x40];
    D3DXVECTOR3 pos;
    D3DCOLOR color;
    float scaleX;
    float scaleY;
    // Drawn with the second glyph set (asciis.anm), in white.
    int highlight;
    // Drawn inside the play area's viewport.
    int inArena;
};
ASSERT_SIZE(AsciiString, 0x60);

// A part of AsciiText, twice: at 0x6244 and 0x69bc, the pause and the
// continue menus. Its inline constructor is too large to expand: emitted
// at 0x4010e0, and AsciiText's constructor reserves slots for it.
// AsciiText::OnDraw runs DrawPauseMenu on the first and DrawContinueMenu
// on the second.
class PauseMenu
{
public:
    PauseMenu()
    {
    }

    // The pause menu, while g_GameManager.isPaused is set. `state`: 0
    // starts the menu's scripts (and a copy of the play area into texture
    // 3 for the background), 1 and 2 have resume or quit selected, 4 and
    // 5 the no and yes of quitting; 3 resumes and 6 returns to the title
    // (scene 1) after 20 frames. The pause button resumes and 0x200 quits
    // from any state. Returns 0.
    int UpdatePauseMenu();
    // Draws, while g_GameManager.isPaused is set, inside the viewport given
    // by g_GameManager.arena: the background without z writes if
    // g_App.lockableBackBuffer is set and state is nonzero,
    // then those of the six sprites with flag 0.
    void DrawPauseMenu();
    // The continue menu, while g_GameManager.isInContinueMenu is set; leaves at
    // once (scene 7, or 8 in a replay; returns 1) in practice, replays,
    // after 3 continues or on Extra. `state`: 0 starts the scripts
    // (and the play area's copy as in UpdatePauseMenu), 1 and 2
    // have yes or no selected; 3 continues after 30 frames, refilling
    // lives and bombs and resetting the score to the continues used; 4
    // gives up (scene 7) after 20. Returns 0 then.
    int UpdateContinueMenu();
    // Draws, while g_GameManager.isInContinueMenu is set, inside the viewport
    // given by g_GameManager.arena: the background if lockableBackBuffer
    // is set and state is nonzero or stateFrames above 2, then, when state
    // is 1 or 2, a copy of sprite 1 as sprite 4, shifted right and showing
    // sprite entry 0x1e - g_GameManager.continuesUsed, then those of the
    // first four sprites with flag 0.
    void DrawContinueMenu();

    int state;
    // Frames since the state changed.
    int stateFrames;
    // The menu's lines: the title and two choices, then the confirmation's
    // (the continue menu uses the first four).
    Sprite sprites[6];
    // The copy of the play area behind the menu.
    Sprite background;
};
ASSERT_SIZE(PauseMenu, 0x778);

// A number rising from where something scored, one of AsciiText's
// 0x203: AddPopup uses 0x200 of them, AddPopup2 the other 3. OnUpdate
// moves it up and retires it after 60 frames.
struct AsciiPopup
{
    // Least significant first; 10 stands for a minus sign.
    BYTE digits[8];
    D3DXVECTOR3 pos;
    D3DCOLOR color;
    Timer timer;
    BYTE active;
    BYTE digitCount;
    unsigned char unk_26[0x2];
};
ASSERT_SIZE(AsciiPopup, 0x28);

// Queues strings to draw in the ASCII font, the global at 0x47b900. Its
// code starts at 0x401000 and loads "data/ascii.anm".
class AsciiText
{
public:
    // Constructs the members, for g_AsciiText's static initialization.
    AsciiText();
    // Adds AsciiText's update and draw chain elements. Returns 0, or -1
    // if adding fails.
    static int RegisterChain();
    // The second draw element's callback (priority 10): draws the
    // popups with DrawPopups while g_App.hardwareTnL is set, otherwise
    // with DrawPopups2. Returns 1.
    static int OnDraw2(AsciiText *text);
    // The first draw element's callback (priority 0xf): draws the queued
    // strings and empties the queue. Returns 1.
    static int OnDraw(AsciiText *text);
    // Draws the queued strings with `glyph`, character by character: 14
    // pixels a glyph and 16 a line, times the string's scale. Strings
    // with inArena set are drawn inside the viewport given by
    // g_GameManager.arena, the others in the whole 640x480 screen; those
    // with highlight set use the
    // second glyph set, in white.
    void DrawStrings();
    // The update element's callback: moves the active popups along, unless
    // g_GameManager.isPaused or g_GameManager.isInContinueMenu is set; then it
    // updates the matching part. Returns 1.
    static int OnUpdate(AsciiText *text);
    // The update element's added callback: loads the font's ANM files
    // and initializes. Returns 0, or -1 if loading fails.
    static int AddedCallback(AsciiText *text);
    // Zeroes everything, then sets the color to white, the scale to 1
    // and the two sprites up. Inline, but too large to expand, like
    // Sprite's constructor.
    void Initialize()
    {
        memset(this, 0, sizeof(AsciiText));
        this->color = 0xffffffff;
        this->scaleX = 1.0f;
        this->scaleY = 1.0f;
        this->digit.flags.anchor = 3;
        g_SpriteEngine->InitSprite(&this->digit, 0);
        g_SpriteEngine->InitSprite(&this->glyph, 0x20);
        this->digit.pos.z = 0.1f;
        this->highlight = 0;
    }
    // Draws the active popups digit by digit with digit, inside the
    // viewport given by g_GameManager.arena, through
    // SpriteEngine::Draw3DRounded.
    void DrawPopups();
    // Like DrawPopups, but draws the minus sign through
    // SpriteEngine::Draw.
    void DrawPopups2();

    // The update element's deleted callback: frees the ANM files the
    // added callback loaded. Returns 0.
    static int DeletedCallback(AsciiText *text);
    // Removes AsciiText's update element and its first draw element;
    // the second draw element stays.
    static void RemoveChain();

    void SetColor(D3DCOLOR color)
    {
        this->color = color;
    }
    void SetHighlight(int highlight)
    {
        this->highlight = highlight;
    }
    // Queues a string at `pos` in the current color. Up to 256 strings
    // a frame; more are dropped.
    void AddString(D3DXVECTOR3 *pos, char *text);
    // Like AddString, for a printf-style text of up to 512 bytes.
    void AddFormat(D3DXVECTOR3 *pos, char *fmt, ...);
    // Shows `value` rising from `pos` in `color` for 60 frames. Negative
    // values show as a minus sign.
    void AddPopup(D3DXVECTOR3 *pos, int value, D3DCOLOR color);
    // Like AddPopup, but in the last three popups, kept apart from
    // AddPopup's.
    void AddPopup2(D3DXVECTOR3 *pos, int value, D3DCOLOR color);

    // The glyph DrawStrings draws each character with.
    Sprite glyph;
    // The digit the popups are drawn with.
    Sprite digit;
    AsciiString strings[0x100];
    // How many strings are queued.
    int stringCount;
    // The color, and the scale, of the strings queued next.
    D3DCOLOR color;
    float scaleX;
    float scaleY;
    // The strings queued next are drawn inside the play area: set while
    // the Gui queues its own.
    int inArena;
    // The strings queued next are highlighted (if HasFlag0Or8): the menus
    // set it for the line selected.
    int highlight;
    // The popup AddPopup uses next.
    int nextPopup;
    // The one of the last three popups AddPopup2 uses next.
    int nextPopup2;
    unsigned char unk_6240[0x4];
    PauseMenu pauseMenu;
    PauseMenu continueMenu;
    AsciiPopup popups[0x203];
};
ASSERT_SIZE(AsciiText, 0xc1ac);
extern AsciiText g_AsciiText;

// AsciiText's chain elements, statically allocated: the update element,
// and draw elements at priorities 0xf and 10.
extern ChainElem g_AsciiCalcChain;
extern ChainElem g_AsciiDrawChain;
extern ChainElem g_AsciiDrawChain2;
