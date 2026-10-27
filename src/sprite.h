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

    unsigned char unk_000[0x7c];
    // SpriteEngine::Draw skips the sprite while it is 0.
    D3DCOLOR color;
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

    // Forgets the texture last set on the device, so the next sprite
    // drawn sets its own.
    void ClearTexture()
    {
        this->currentTexture = NULL;
    }

    // Runs a sprite's script for one frame.
    int Step(Sprite *vm);
    // Draws a sprite if it is shown.
    int Draw(Sprite *sprite);
    // Copies a rectangle of one of the loaded surfaces to the back buffer.
    void Func435300(int surface, int unk1, int unk2, int unk3, int unk4);
    // Prints a printf-style text with a shadow into the sprite's texture
    // (formatted into 64 bytes) and marks the sprite for upload.
    void RenderText(Sprite *sprite, D3DCOLOR textColor, D3DCOLOR shadowColor, char *fmt, ...);

    unsigned char unk_00000[0x210b8];
    // The texture last set on the device (IDirect3DDevice8::SetTexture).
    IDirect3DTexture8 *currentTexture;
};
extern SpriteEngine *g_SpriteEngine;
