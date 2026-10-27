// Sprites animated by ANM scripts.

#pragma once

#include <d3dx8.h>

#include "common.h"
#include "timer.h"

// The state of one sprite's animation script.
class Sprite
{
public:
    // Inline, but too large to expand: callers call the copy at 0x403720,
    // and arrays of Sprites still reserve its inlined Timers' slots.
    Sprite()
    {
        this->spriteIndex = -1;
    }

    // Resets the sprite's state, keeping its texture.
    void Initialize();

    unsigned char unk_000[0x30];
    Timer unk_030;
    unsigned char unk_03c[0x40];
    // SpriteEngine::Draw skips the sprite while it is 0.
    D3DCOLOR color;
    // Bit 1 shows the sprite.
    DWORD flags;
    unsigned char unk_084[0xc];
    D3DXVECTOR3 pos;
    unsigned char unk_09c[0x8];
    Timer unk_0a4;
    // The sprite SpriteEngine::SetSprite set; -1 for none.
    short spriteIndex;
    unsigned char unk_0b2[0x2];
    // The script SpriteEngine::StartScript started.
    short scriptIndex;
    unsigned char unk_0b6[0x3a];
    Timer unk_0f0;
    unsigned char unk_0fc[0x4];
    Timer unk_100;
    unsigned char unk_10c[0x4];
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

    // Loads an ANM file into `slot`, numbering its sprites from
    // `firstSprite`. Returns 0, or -1 on failure.
    int LoadAnm(int slot, char *path, int firstSprite);
    // Loads an image into one of the surfaces. Returns 0, or -1 on
    // failure.
    int LoadSurface(int surface, char *path);
    // Shows sprite `sprite` of the loaded ANM files on `vm`. Returns 0,
    // or -1 if there is no such sprite.
    int SetSprite(Sprite *vm, int sprite);
    // Resets `vm` and runs `script` on it from the start.
    void SetScript(Sprite *vm, void *script);

    // Runs script `script` of the loaded ANM files on `vm`.
    void StartScript(Sprite *vm, int script)
    {
        vm->scriptIndex = script;
        this->SetScript(vm, this->scripts[script]);
    }

    // Resets `vm` and shows sprite `sprite` on it.
    void InitSprite(Sprite *vm, int sprite)
    {
        vm->Initialize();
        this->SetSprite(vm, sprite);
    }

    // Releases the vertex buffer, if there is one.
    void ReleaseVertexBuffer();

    // Runs a sprite's script for one frame.
    int Step(Sprite *vm);
    // Draws a sprite if it is shown.
    int Draw(Sprite *sprite);
    // Frees the ANM file loaded into `slot` and clears its scripts.
    void ReleaseAnm(int slot);
    // Releases one of the loaded surfaces.
    void ReleaseSurface(int surface);
    // Copies a rectangle of one of the loaded surfaces to the back buffer.
    void Func435300(int surface, int unk1, int unk2, int unk3, int unk4);
    // Prints a printf-style text with a shadow into the sprite's texture
    // (formatted into 64 bytes) and marks the sprite for upload.
    void RenderText(Sprite *sprite, D3DCOLOR textColor, D3DCOLOR shadowColor, char *fmt, ...);

    unsigned char unk_00000[0x1c934];
    // The scripts of the loaded ANM files.
    void *scripts[0x800];
    unsigned char unk_1e934[0x2784];
    // The texture last set on the device (IDirect3DDevice8::SetTexture).
    IDirect3DTexture8 *currentTexture;
    unsigned char unk_210bc[0x8];
    // Created with IDirect3DDevice8::CreateVertexBuffer and set as the
    // stream source.
    IDirect3DVertexBuffer8 *vertexBuffer;
};
extern SpriteEngine *g_SpriteEngine;
