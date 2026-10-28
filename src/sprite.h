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
    // Set by the script as it shows a sprite (instructions 1, 16, 29)
    // and by RenderText, cleared as it ends or hides it (0, 24, 29);
    // SpriteEngine::Draw skips the sprite without it.
    unsigned int shown : 1;
    // Shows the sprite.
    unsigned int visible : 1;
    // Selects SpriteEngine::Draw's D3DRS_DESTBLEND.
    unsigned int blendMode : 1;
    // Selects SpriteEngine::Draw's stage 0 D3DTSS_COLOROP.
    unsigned int colorOp : 1;
    unsigned int unk4 : 1;
    // The ANM position instructions set posOffset rather than pos.
    unsigned int usePosOffset : 1;
    // Bit 0 mirrors the sprite horizontally (negates scaleX), bit 1
    // vertically.
    unsigned int flip : 2;
    // Bit 0 anchors the sprite's left edge at its position instead of
    // centering it, bit 1 its top edge. Set to 3 throughout the music
    // screen.
    unsigned int anchor : 2;
    // How a position interpolation eases: 0 linearly, 1 out (squared),
    // 2 out (to the fourth).
    unsigned int posInterpMode : 2;
    // Turns D3DRS_ZWRITEENABLE off while drawing.
    unsigned int zWriteDisabled : 1;
    // The script reached a stop instruction and waits for an interrupt.
    unsigned int stopped : 1;
    unsigned int unk14 : 18;
};

// One instruction of an ANM script.
struct AnmInstr
{
    // The frame it runs on.
    short time;
    BYTE opcode;
    // The size of the arguments, in bytes.
    BYTE argsSize;
    int args[1];
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
    void ClearShown()
    {
        this->flags.shown = 0;
    }
    // Whether the sprite shows a sprite whose texture is loaded.
    __forceinline BOOL HasTexture();

    // The gaps are DWORDs, not bytes: a byte array would make /GS put a
    // security cookie in functions with a Sprite local.
    // The sprite's rotation about each axis; DrawRotated uses only z.
    float rotationX;
    float rotationY;
    float rotationZ;
    // Added to the rotations every frame.
    float rotationSpeedX;
    float rotationSpeedY;
    float rotationSpeedZ;
    // SpriteEngine::Draw scales the sprite's height and width by these.
    float scaleY;
    float scaleX;
    // Added to the scales every frame; while scaleInterpTime runs, the
    // scales it interpolates to.
    float scaleTargetY;
    float scaleTargetX;
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
    // The lengths of the running alpha, scale and position
    // interpolations, in frames; 0 when none runs.
    short alphaInterpTime;
    short scaleInterpTime;
    // Set by the script: when not 0, the owner turns the sprite with its
    // angle; the stage draws 2 as a screen-space sprite instead.
    short autoRotate;
    // The interrupt label to jump to on the next Step; 0 for none.
    short pendingInterrupt;
    short posInterpTime;
    short unk_08e;
    D3DXVECTOR3 pos;
    // The scales a scale interpolation started from.
    float scaleInitialY;
    float scaleInitialX;
    Timer scaleInterpTimer;
    // The sprite SpriteEngine::SetSprite set; -1 for none.
    short spriteIndex;
    // The sprite the color variants count from: bullets show
    // baseSpriteIndex + their color, menus a highlighted variant.
    short baseSpriteIndex;
    // The script SpriteEngine::StartScript started.
    short scriptIndex;
    short unk_0b6;
    // The script SetScript started, and the next instruction to run;
    // NULL once the script ends.
    AnmInstr *scriptStart;
    AnmInstr *scriptPos;
    // The entry SetSprite chose.
    SpriteData *spriteData;
    // The colors an alpha interpolation goes from and to.
    D3DCOLOR alphaInitial;
    D3DCOLOR alphaFinal;
    // The positions a position interpolation goes from and to.
    // A D3DVECTOR, without D3DXVECTOR3's (empty, inline) constructor:
    // inline Sprite constructors too large to expand still use up the
    // caller's inlining budget, by the number of inline constructors in
    // them, and one fewer lets GuiImpl's reserve slots for 9 Sprites
    // rather than 8, as the original's does.
    D3DVECTOR posInitial;
    D3DXVECTOR3 posFinal;
    // Set instead of pos while flags.usePosOffset is set.
    D3DXVECTOR3 posOffset;
    Timer posInterpTimer;
    // scriptTime when a script instruction last set the sprite.
    int spriteTime;
    Timer alphaInterpTimer;
    // The font size RenderText and its variants print with, in pixels.
    BYTE fontWidth;
    BYTE fontHeight;
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

    // Forgets scripts 0x100 to 0x179, the title menu's.
    void ClearMenuScripts()
    {
        int i;

        for (i = 0; i < 0x7a; i++)
        {
            this->scripts[i + 0x100] = NULL;
        }
    }

    // Has CopyBackBufferToTexture's copy done after the frame is drawn.
    void SetPendingCopy(int texture, int left, int top, int width, int height)
    {
        this->pendingCopyTexture = texture;
        this->pendingCopyLeft = left;
        this->pendingCopyTop = top;
        this->pendingCopyWidth = width;
        this->pendingCopyHeight = height;
    }

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

    // Like RenderText, after clearing the sprite's texture with a space,
    // aligned to the sprite's right edge. A character takes
    // (fontWidth + 1) / 2 pixels.
    void RenderTextRightAligned(Sprite *vm, D3DCOLOR color, D3DCOLOR shadowColor, char *fmt, ...);
    // RenderTextRightAligned, centered on the sprite.
    void RenderTextCentered(Sprite *vm, D3DCOLOR color, D3DCOLOR shadowColor, char *fmt, ...);

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
    // Runs a sprite's script for one frame: the instructions due (jumping
    // to a pending interrupt's label first), then the rotation, scale,
    // alpha and position changes they started. Returns 1 once the script
    // has ended, else 0.
    int Step(Sprite *vm);
    // Draws a sprite if it is shown, flat at its position, scaled and
    // anchored. Returns 0, or -1 when not shown.
    int Draw(Sprite *vm);
    // Rotates (`x`, `y`) by the angle with sine `sine` and cosine
    // `cosine`, clockwise on screen, then moves it by (`cx`, `cy`); stores
    // the result in out[0] and out[1].
    void RotatePoint(float *out, float x, float y, float sine, float cosine, float cx, float cy);
    // Like Draw, rotated by the sprite's rotation about its position,
    // with the position and size rounded to whole pixels.
    int DrawRotated(Sprite *vm);
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
    // Like StartScript, after zeroing the position and posOffset and
    // setting fontWidth and fontHeight to 15. Inline, but not expanded: its
    // copy is the stage file's.
    void StartScriptAtOrigin(Sprite *vm, int script)
    {
        vm->scriptIndex = script;
        vm->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        vm->posOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        vm->fontHeight = 15;
        vm->fontWidth = 15;
        this->SetScript(vm, this->scripts[script]);
    }
    // Draws `vm` in 3D: its matrix scaled, rotated about x, y and z and
    // moved to its position (y up), with its sprite through the texture
    // matrix. Returns 0, or -1 when not shown.
    int Draw3D(Sprite *vm);
    // Draw without snapping the corners to pixel centers.
    int DrawNoRound(Sprite *vm);
    // Like Draw3D for an unrotated sprite, with its position rounded to
    // pixel centers; Draw3D draws rotated ones. Returns 0, or -1 when
    // not shown.
    int Draw3DRounded(Sprite *vm);
    // Frees the ANM file loaded into `slot` and clears its scripts.
    void ReleaseAnm(int slot);
    // Releases one of the loaded surfaces.
    void ReleaseSurface(int surface);
    // Sets up `quad` and copies it into g_DiffuseQuadXyz; unless the
    // vertex buffer is disabled, creates the vertex buffer from it and
    // sets it as the stream source.
    void CreateQuad();
    // Copies one of the loaded surfaces, from (`left`, `top`) to its
    // bottom right corner, to (`x`, `y`) on the back buffer, recreating
    // it from its backup first if the device lost it.
    void CopySurfaceToBackBuffer(int surface, int left, int top, int x, int y);
    // Like CopySurfaceToBackBuffer, for the `width` by `height` rectangle
    // at (`left`, `top`).
    void CopySurfaceRectToBackBuffer(int surface, int x, int y, int left, int top, int width, int height);
    // Releases the loaded surfaces (not their backups), before the
    // device is reset.
    void ReleaseSurfaces();
    // Copies the `width` by `height` rectangle at (`left`, `top`) of the
    // back buffer into texture `texture`.
    void CopyBackBufferToTexture(int texture, int left, int top, int width, int height);
    // Does the copy CopyBackBufferToTexture that `pendingCopy` asks for,
    // if any, and clears it.
    void DoPendingCopy();
    // RenderTextToTexture into texture `textureIndex`, with sizes of 0 or
    // less taken as 15.
    void RenderTextToTextureIndex(int textureIndex, int x, int y, int width, int unk4, int fontHeight, int unk6,
                                  D3DCOLOR textColor, D3DCOLOR shadowColor, char *text);
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
    // Images loaded by LoadSurface, as render targets (or image
    // surfaces) to copy to the back buffer from, with system memory
    // copies to recreate them from, and the images' sizes.
    IDirect3DSurface8 *surfaces[0x20];
    IDirect3DSurface8 *surfacesBackup[0x20];
    D3DXIMAGE_INFO surfaceInfos[0x20];
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
    // A copy of the back buffer into a texture to do after the frame is
    // drawn: CopyBackBufferToTexture's arguments, the texture -1 when
    // there is none.
    int pendingCopyTexture;
    int pendingCopyLeft;
    int pendingCopyTop;
    int pendingCopyWidth;
    int pendingCopyHeight;
};
ASSERT_SIZE(SpriteEngine, 0x2112c);
extern SpriteEngine *g_SpriteEngine;

inline BOOL Sprite::HasTexture()
{
    if (this->spriteData == NULL)
    {
        return FALSE;
    }
    if (this->spriteData->sourceFileIndex < 0)
    {
        return FALSE;
    }
    return g_SpriteEngine->textures[this->spriteData->sourceFileIndex] != NULL;
}

// Transformed vertices with a color and texture coordinates, and without
// a color.
struct VertexTex1DiffuseXyzrwh
{
    D3DXVECTOR3 pos;
    float w;
    D3DCOLOR diffuse;
    D3DXVECTOR2 uv;
};
// A transformed vertex with a color.
struct VertexDiffuseXyzrwh
{
    D3DXVECTOR3 pos;
    float w;
    D3DCOLOR diffuse;
};
struct VertexTex1Xyzrwh
{
    D3DXVECTOR3 pos;
    float w;
    D3DXVECTOR2 uv;
};
// Rounds `f` to a whole number with frndint.
inline float Rint(float f)
{
    __asm
    {
        fld f
        frndint
        fstp f
    }
    return f;
}

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
