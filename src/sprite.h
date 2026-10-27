// Sprites animated by ANM scripts.

#pragma once

#include <d3dx8.h>

#include "common.h"
#include "timer.h"

// One of SpriteEngine's sprites: a rectangle of a texture.
struct SpriteData
{
    // Negative when unused.
    int unk_00;
    DWORD unk_04[0xa];
    float height;
    float width;
    DWORD unk_34;
};
ASSERT_SIZE(SpriteData, 0x38);

// Sprite's flags, a bitfield.
struct SpriteFlags
{
    // SpriteEngine::Draw skips the sprite without it; RenderText sets
    // it.
    unsigned int unk0 : 1;
    // Shows the sprite.
    unsigned int visible : 1;
    // Selects SpriteEngine::Draw's D3DRS_DESTBLEND.
    unsigned int blendMode : 1;
    // Selects SpriteEngine::Draw's stage 0 D3DTSS_COLOROP.
    unsigned int colorOp : 1;
    unsigned int unk4 : 4;
    // Set to 3 throughout the music screen.
    unsigned int unk8 : 2;
    unsigned int unk10 : 2;
    // Turns D3DRS_ZWRITEENABLE off while drawing.
    unsigned int zWriteDisabled : 1;
    unsigned int unk13 : 1;
    unsigned int unk14 : 18;
};

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

    // The gaps are DWORDs, not bytes: a byte array would make /GS put a
    // security cookie in functions with a Sprite local.
    DWORD unk_000[0x2];
    // Turns the sprite (through SpriteEngine::Func433590).
    float rotation;
    DWORD unk_00c[0x3];
    // SpriteEngine::Draw scales the sprite's height and width by these.
    float scaleY;
    float scaleX;
    DWORD unk_020[0x4];
    Timer unk_030;
    // Set to the identity by Initialize.
    D3DXMATRIX matrix;
    // SpriteEngine::Draw skips the sprite while it is 0.
    D3DCOLOR color;
    SpriteFlags flags;
    short unk_084;
    short unk_086;
    short unk_088;
    short unk_08a;
    short unk_08c;
    short unk_08e;
    D3DXVECTOR3 pos;
    DWORD unk_09c[0x2];
    Timer unk_0a4;
    // The sprite SpriteEngine::SetSprite set; -1 for none.
    short spriteIndex;
    short unk_0b2;
    // The script SpriteEngine::StartScript started.
    short scriptIndex;
    short unk_0b6;
    DWORD unk_0b8[0x2];
    // The entry SetSprite chose.
    SpriteData *spriteData;
    DWORD unk_0c4[0x8];
    D3DXVECTOR3 unk_0e4;
    Timer unk_0f0;
    DWORD unk_0fc;
    Timer unk_100;
    // Used by RenderText.
    BYTE unk_10c;
    BYTE unk_10d;
    short unk_10e;
};
ASSERT_SIZE(Sprite, 0x110);

// Runs and draws Sprites.
class SpriteEngine
{
public:
    // Empty, and defined out of line: `delete` calls it, then operator
    // delete.
    ~SpriteEngine();

    // These forget a piece of the device state Draw last set, so the
    // next sprite drawn sets its own.
    void ClearTexture()
    {
        this->currentTexture = NULL;
    }
    void ClearBlendMode()
    {
        this->blendMode = 0xff;
    }
    void ClearColorOp()
    {
        this->colorOp = 0xff;
    }
    void ClearVertexFormat()
    {
        this->vertexFormat = 0xff;
    }
    void ClearZWrite()
    {
        this->zWriteDisabled = 0xff;
    }
    void ClearSpriteData()
    {
        this->currentSpriteData = NULL;
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
    int Func432cc0(Sprite *sprite);
    // Like StartScript, after zeroing the position and unk_0e4 and
    // setting unk_10c and unk_10d to 15. Defined in the stage's file.
    void Func4051b0(Sprite *vm, int script);
    int Func433150(Sprite *sprite);
    int Func433590(Sprite *sprite);
    // Frees the ANM file loaded into `slot` and clears its scripts.
    void ReleaseAnm(int slot);
    // Releases one of the loaded surfaces.
    void ReleaseSurface(int surface);
    void Func431660();
    // Copies a rectangle of one of the loaded surfaces to the back buffer.
    void Func435300(int surface, int unk1, int unk2, int unk3, int unk4);
    // Prints a printf-style text with a shadow into the sprite's texture
    // (formatted into 64 bytes) and marks the sprite for upload.
    void RenderText(Sprite *sprite, D3DCOLOR textColor, D3DCOLOR shadowColor, char *fmt, ...);

    SpriteData sprites[0x800];
    unsigned char unk_1c000[0x934];
    // The scripts of the loaded ANM files.
    void *scripts[0x800];
    unsigned char unk_1e934[0x2784];
    // The texture last set on the device (IDirect3DDevice8::SetTexture).
    IDirect3DTexture8 *currentTexture;
    // Sprite flag bit 2 as last set: the D3DRS_DESTBLEND choice.
    BYTE blendMode;
    // Sprite flag bit 3 as last set: stage 0's D3DTSS_COLOROP choice.
    BYTE colorOp;
    // Which vertex format (SetVertexShader) Draw set last.
    BYTE vertexFormat;
    // Sprite flag bit 12 as last set: D3DRS_ZWRITEENABLE off.
    BYTE zWriteDisabled;
    // The sprite entry whose texture coordinates are in the vertices.
    void *currentSpriteData;
    // Created with IDirect3DDevice8::CreateVertexBuffer and set as the
    // stream source.
    IDirect3DVertexBuffer8 *vertexBuffer;
};
extern SpriteEngine *g_SpriteEngine;
