// Text drawn in the ASCII font (data/ascii.anm).

#pragma once

#include <d3dx8.h>

#include "chain.h"
#include "common.h"
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
    void Func402740();
    void Func402870();
    void Func403060();

    unsigned char unk_000[0x8];
    Sprite unk_008[6];
    Sprite unk_668;
};
ASSERT_SIZE(Unknown4010e0, 0x778);

// One of AsciiText's 0x203 entries that OnUpdate moves up the screen
// while active, and deactivates after 60 frames.
struct Unknown7134
{
    unsigned char unk_00[0xc];
    float unk_0c;
    unsigned char unk_10[0x8];
    Timer timer;
    BYTE active;
    unsigned char unk_25[0x3];
};
ASSERT_SIZE(Unknown7134, 0x28);

// Queues strings to draw in the ASCII font, the global at 0x47b900. Its
// code starts at 0x401000 and loads "data/ascii.anm".
class AsciiText
{
public:
    // Adds AsciiText's update and draw chain elements. Returns 0, or -1
    // if adding fails.
    static int RegisterChain();
    // The second draw element's callback (priority 10): runs Func4031e0
    // while g_Unknown6c7114 is set, otherwise Func4033b0. Returns 1.
    static int OnDraw2(AsciiText *text);
    // The first draw element's callback (priority 0xf): draws the queued
    // strings and empties the queue. Returns 1.
    static int OnDraw(AsciiText *text);
    // Draws the queued strings.
    void DrawStrings();
    // The update element's callback: moves the active Unknown7134
    // entries along, unless g_Unknown69d4bf or g_Unknown69d4c0 is set;
    // then it updates the matching part. Returns 1.
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
        this->unk_0110.flags |= 0x300;
        g_SpriteEngine->InitSprite(&this->unk_0110, 0);
        g_SpriteEngine->InitSprite(&this->unk_0000, 0x20);
        this->unk_0110.pos.z = 0.1f;
        this->unk_6234 = 0;
    }
    void Func4031e0();
    void Func4033b0();

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
    unsigned char unk_6238[0xc];
    Unknown4010e0 unk_6244;
    Unknown4010e0 unk_69bc;
    Unknown7134 unk_7134[0x203];
};
ASSERT_SIZE(AsciiText, 0xc1ac);
extern AsciiText g_AsciiText;

extern BYTE g_Unknown6c7114;
extern BYTE g_Unknown69d4bf;
extern BYTE g_Unknown69d4c0;

// AsciiText's chain elements, statically allocated: the update element,
// and draw elements at priorities 0xf and 10.
extern ChainElem g_AsciiCalcChain;
extern ChainElem g_AsciiDrawChain;
extern ChainElem g_AsciiDrawChain2;
