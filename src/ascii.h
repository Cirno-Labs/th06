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
    int unk_58;
    int unk_5c;
};
ASSERT_SIZE(AsciiString, 0x60);

// A part of AsciiText, twice: at 0x6244 and 0x69bc. Constructed at
// 0x4010e0; AsciiText::OnDraw runs Func402740 on the first and
// Func403060 on the second, so they may be two types whose identical
// constructors were folded.
class Unknown4010e0
{
public:
    Unknown4010e0();

    void Func401b80();
    // Draws, while g_GameManager.unk_181f is set, inside the viewport given
    // by g_GameManager.arena: unk_668 without z writes if g_Unknown6c7115
    // is set and unk_000 is nonzero, then those of the six sprites with
    // flag 0.
    void Func402740();
    void Func402870();
    // Draws, while g_GameManager.unk_1820 is set, inside the viewport given
    // by g_GameManager.arena: unk_668 if g_Unknown6c7115 is set and unk_000
    // is nonzero or unk_004 above 2, then, when unk_000 is 1 or 2, a copy
    // of sprite 1 as sprite 4, shifted right and showing sprite entry
    // 0x1e - g_GameManager.unk_1818, then those of the first four sprites
    // with flag 0.
    void Func403060();

    int unk_000;
    int unk_004;
    Sprite unk_008[6];
    Sprite unk_668;
};
ASSERT_SIZE(Unknown4010e0, 0x778);

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
    // Adds AsciiText's update and draw chain elements. Returns 0, or -1
    // if adding fails.
    static int RegisterChain();
    // The second draw element's callback (priority 10): draws the
    // popups with DrawPopups while g_Unknown6c7114 is set, otherwise
    // with DrawPopups2. Returns 1.
    static int OnDraw2(AsciiText *text);
    // The first draw element's callback (priority 0xf): draws the queued
    // strings and empties the queue. Returns 1.
    static int OnDraw(AsciiText *text);
    // Draws the queued strings with unk_0000, glyph by glyph: 14 pixels a
    // glyph and 16 a line, times the string's scale. Strings with unk_5c
    // set are drawn inside the viewport given by g_GameManager.arena, the
    // others in the whole 640x480 screen; those with unk_58 set use the
    // second glyph set, in white.
    void DrawStrings();
    // The update element's callback: moves the active popups along, unless
    // g_GameManager.unk_181f or g_GameManager.unk_1820 is set; then it
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
        this->unk_0110.flags.anchor = 3;
        g_SpriteEngine->InitSprite(&this->unk_0110, 0);
        g_SpriteEngine->InitSprite(&this->unk_0000, 0x20);
        this->unk_0110.pos.z = 0.1f;
        this->unk_6234 = 0;
    }
    // Draws the active popups digit by digit with unk_0110, inside the
    // viewport given by g_GameManager.arena, through
    // SpriteEngine::Func433590.
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

    Sprite unk_0000;
    Sprite unk_0110;
    AsciiString strings[0x100];
    // How many strings are queued.
    int stringCount;
    // The color, and the scale, of the strings queued next.
    D3DCOLOR color;
    float scaleX;
    float scaleY;
    int unk_6230;
    int unk_6234;
    // The popup AddPopup uses next.
    int nextPopup;
    // The one of the last three popups AddPopup2 uses next.
    int nextPopup2;
    unsigned char unk_6240[0x4];
    Unknown4010e0 unk_6244;
    Unknown4010e0 unk_69bc;
    AsciiPopup popups[0x203];
};
ASSERT_SIZE(AsciiText, 0xc1ac);
extern AsciiText g_AsciiText;

extern BYTE g_Unknown6c7114;
extern BYTE g_Unknown6c7115;

// AsciiText's chain elements, statically allocated: the update element,
// and draw elements at priorities 0xf and 10.
extern ChainElem g_AsciiCalcChain;
extern ChainElem g_AsciiDrawChain;
extern ChainElem g_AsciiDrawChain2;
