// Sprites animated by ANM scripts.

#pragma once

#include <d3dx8.h>

#include "common.h"

// The state of one sprite's animation script. Constructed at 0x403720,
// which sets up its Timers.
class Sprite
{
public:
    Sprite();

    unsigned char unk_000[0x80];
    // Bit 1 shows the sprite.
    DWORD flags;
    unsigned char unk_084[0xc];
    D3DXVECTOR3 pos;
    unsigned char unk_09c[0x74];
};
ASSERT_SIZE(Sprite, 0x110);

// Runs and draws Sprites.
class SpriteEngine
{
public:
    // Empty, and defined out of line: `delete` calls it, then operator
    // delete.
    ~SpriteEngine();

    // Runs a sprite's script for one frame.
    int Step(Sprite *vm);
    // Prints a printf-style text with a shadow into the sprite's texture
    // (formatted into 64 bytes) and marks the sprite for upload.
    void RenderText(Sprite *sprite, D3DCOLOR textColor, D3DCOLOR shadowColor, char *fmt, ...);
};
extern SpriteEngine *g_SpriteEngine;
