// The file with SpriteEngine.

#include "sprite.h"

void SpriteEngine::RotatePoint(float *out, float sine, float cosine, float x, float y, float cx, float cy)
{
    out[0] = sine * y + cosine * x + cx;
    out[1] = -sine * x + cosine * y + cy;
}
