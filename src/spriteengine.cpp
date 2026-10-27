// The file with SpriteEngine.

#include "sprite.h"
#include "unsorted.h"

int SpriteEngine::CreateTexture(int index, int width, int height, int format)
{
    D3DXCreateTexture(g_Unknown6c6d18.d3dDevice, width, height, 1, 0, g_TextureFormats[format], D3DPOOL_MANAGED,
                      &this->textures[index]);
    return 0;
}

void SpriteEngine::RotatePoint(float *out, float sine, float cosine, float x, float y, float cx, float cy)
{
    out[0] = sine * y + cosine * x + cx;
    out[1] = -sine * x + cosine * y + cy;
}
