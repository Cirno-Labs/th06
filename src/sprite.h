// Sprites animated by ANM scripts.

#pragma once

#include <d3dx8.h>

#include "common.h"
#include "timer.h"

// One of SpriteEngine's sprites: a rectangle of a texture.
// A sprite in an ANM file: its number and rectangle, in pixels.
struct AnmSprite
{
    int id;
    float x;
    float y;
    float width;
    float height;
};
ASSERT_SIZE(AnmSprite, 0x14);

// An ANM file's header. The sprites' and then the scripts' offsets
// follow it.
struct AnmFile
{
    int numSprites;
    int numScripts;
    // Set to the slot it is loaded into.
    int textureIndex;
    int width;
    int height;
    int format;
    D3DCOLOR colorKey;
    // The image's file name, or "@" for a texture created blank.
    int nameOffset;
    // Set to the first sprite number.
    int spriteOffset;
    // The file name of an image to take the alpha from, or 0.
    int alphaNameOffset;
    DWORD unk_28[6];
    int spriteOffsets[1];
};

struct SpriteData
{
    // The ANM slot the sprite comes from; negative when unused.
    int sourceFileIndex;
    // The sprite's rectangle in the texture, in pixels.
    float startX;
    float startY;
    float endX;
    float endY;
    // The texture's size.
    float textureHeight;
    float textureWidth;
    // The rectangle in texture coordinates.
    float uvStartX;
    float uvStartY;
    float uvEndX;
    float uvEndY;
    float height;
    float width;
    // Numbers the sprites in the order they were loaded.
    int spriteId;
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
    unsigned int unk4 : 2;
    unsigned int unk6 : 2;
    // Bit 0 anchors the sprite's left edge at its position instead of
    // centering it, bit 1 its top edge. Set to 3 throughout the music
    // screen.
    unsigned int anchor : 2;
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
    DWORD unk_020[0x2];
    // Added to the sprite's texture coordinates.
    float uvScrollX;
    float uvScrollY;
    // Counts the frames since the script started.
    Timer scriptTime;
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
    // The script SetScript started, and the next instruction to run;
    // NULL once the script ends.
    void *scriptStart;
    void *scriptPos;
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

// An untransformed vertex with texture coordinates.
struct VertexTex1Xyz
{
    D3DXVECTOR3 pos;
    D3DXVECTOR2 uv;
};

// Runs and draws Sprites.
class SpriteEngine
{
public:
    // Zeroes everything, marks every sprite unused and sets up the quads'
    // w and texture coordinates.
    SpriteEngine();
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
    // Resets `vm` and runs `script` on it from the start: runs its first
    // frame now unless `script` is NULL.
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

    // Writes printf-style text in `color` with shadow `shadowColor` into
    // `vm`'s texture.
    void Func434c40(Sprite *vm, D3DCOLOR color, D3DCOLOR shadowColor, char *fmt, ...);

    // Releases the vertex buffer, if there is one.
    void ReleaseVertexBuffer();

    // Draws `vm` as a screen-space quad from g_Quad's corners, with
    // `vm`'s z, sprite and render state; snaps the corners to pixel
    // centers when `roundToPixel` is set. Returns 0.
    int DrawQuad(Sprite *vm, int roundToPixel);
    // Sets the blend mode, color operation, color and z writes `vm`
    // wants on the device, those that changed since the last sprite and
    // that the configuration allows.
    void SetRenderStateForSprite(Sprite *vm);
    // Runs a sprite's script for one frame.
    int Step(Sprite *vm);
    // Draws a sprite if it is shown, flat at its position, scaled and
    // anchored. Returns 0, or -1 when not shown.
    int Draw(Sprite *vm);
    // Rotates (`x`, `y`) by the angle with sine `sine` and cosine
    // `cosine`, clockwise on screen, then moves it by (`cx`, `cy`); stores
    // the result in out[0] and out[1].
    void RotatePoint(float *out, float sine, float cosine, float x, float y, float cx, float cy);
    int Func432cc0(Sprite *sprite);
    // Loads the image file at `path` into texture `index` in the format
    // g_TextureFormats[`format`] (16 bits when forced), with `colorKey`
    // transparent. Returns 0 or -1.
    int LoadTexture(int index, char *path, int format, D3DCOLOR colorKey);
    // Sets texture `index`'s alpha from the blue channel of the image file
    // at `path`, loaded in the texture's format with `colorKey`
    // transparent. `format` is unused. Returns 0 or -1.
    int LoadTextureAlpha(int index, char *path, int format, D3DCOLOR colorKey);
    // Sets up sprites[`index`] from `sprite`: its texture coordinates,
    // size and id.
    void LoadSprite(int index, SpriteData *sprite);
    // Releases texture `index` and frees its image file.
    void ReleaseTexture(int index);
    // Creates texture `index` at `width` by `height` in the format
    // g_TextureFormats[`format`]. Returns 0.
    int CreateTexture(int index, int width, int height, int format);
    // Like StartScript, after zeroing the position and unk_0e4 and
    // setting unk_10c and unk_10d to 15. Inline, but not expanded: its
    // copy is the stage file's.
    void Func4051b0(Sprite *vm, int script)
    {
        vm->scriptIndex = script;
        vm->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        vm->unk_0e4 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        vm->unk_10d = 15;
        vm->unk_10c = 15;
        this->SetScript(vm, this->scripts[script]);
    }
    int Func433150(Sprite *sprite);
    int Func432fa0(Sprite *sprite);
    int Func433590(Sprite *sprite);
    // Frees the ANM file loaded into `slot` and clears its scripts.
    void ReleaseAnm(int slot);
    // Releases one of the loaded surfaces.
    void ReleaseSurface(int surface);
    // Sets up `quad` and copies it into g_DiffuseQuadXyz; unless the
    // vertex buffer is disabled, creates the vertex buffer from it and
    // sets it as the stream source.
    void CreateQuad();
    // Copies a rectangle of one of the loaded surfaces to the back buffer.
    void Func435300(int surface, int unk1, int unk2, int unk3, int unk4);
    // Prints a printf-style text with a shadow into the sprite's texture
    // (formatted into 64 bytes) and marks the sprite for upload.
    void RenderText(Sprite *sprite, D3DCOLOR textColor, D3DCOLOR shadowColor, char *fmt, ...);

    SpriteData sprites[0x800];
    Sprite unk_1c000;
    IDirect3DTexture8 *textures[0x108];
    // The image files the textures were created from, kept loaded.
    void *textureFiles[0x100];
    // Counts the sprites loaded, for SpriteData::spriteId.
    int spriteCount;
    // The scripts of the loaded ANM files.
    void *scripts[0x800];
    // The first sprite number of each script's ANM file.
    int scriptSpriteOffsets[0x800];
    // The ANM files loaded into each slot, and their first sprite numbers.
    AnmFile *anmFiles[0x80];
    int anmFileSpriteOffsets[0x80];
    unsigned char unk_20d34[0x380];
    // The texture factor last set: the sprites' color.
    D3DCOLOR currentColor;
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
    SpriteData *currentSpriteData;
    // Created with IDirect3DDevice8::CreateVertexBuffer and set as the
    // stream source.
    IDirect3DVertexBuffer8 *vertexBuffer;
    // A 256 by 256 quad around the origin, and the vertex buffer's
    // contents.
    VertexTex1Xyz quad[4];
    int unk_21118;
    unsigned char unk_2111c[0x10];
};
ASSERT_SIZE(SpriteEngine, 0x2112c);
extern SpriteEngine *g_SpriteEngine;

// Transformed vertices with a color and texture coordinates, and without
// a color.
struct VertexTex1DiffuseXyzrwh
{
    D3DXVECTOR3 pos;
    float w;
    D3DCOLOR diffuse;
    D3DXVECTOR2 uv;
};
struct VertexTex1Xyzrwh
{
    D3DXVECTOR3 pos;
    float w;
    D3DXVECTOR2 uv;
};
// 0.5f, the float constant the game's hand-written assembly reads.
extern float g_Half;
// A quad of each, set up by SpriteEngine's constructor.
extern VertexTex1DiffuseXyzrwh g_DiffuseQuad[4];
extern VertexTex1Xyzrwh g_Quad[4];
// An untransformed vertex with a color and texture coordinates.
struct VertexTex1DiffuseXyz
{
    D3DXVECTOR3 pos;
    D3DCOLOR diffuse;
    D3DXVECTOR2 uv;
};
// SpriteEngine::quad with colors.
extern VertexTex1DiffuseXyz g_DiffuseQuadXyz[4];

// The texture formats ANM files choose from: unknown (the surface's own),
// A8R8G8B8, A1R5G5B5, R5G6B5, R8G8B8 and A4R4G4B4.
extern D3DFORMAT g_TextureFormats[];
