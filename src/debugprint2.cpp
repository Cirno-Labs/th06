// cflags: /Os -/Oi
// DebugPrint2, 0x42f6c4: empty in the release build, like DebugPrint, but
// built with /Os (so /OPT:ICF can't fold the two).

#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ascii.h"
#include "features.h"
#include "gamemanager.h"
#include "resultscreen.h"
#include "unsorted.h"

void DebugPrint2(const char *fmt, ...)
{
}
