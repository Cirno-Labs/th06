// The standalone exe's optional features (see README.md). tools/build.py
// defines FEAT_<NAME> for the exe only, so the DLL build, which must match
// the original, never sees any of this.

#pragma once

#if FEAT_FRAME_CAP
#include <windows.h>

// The frame limiter of VsyncPatch (vpatch_th06.dll, rev4, with its
// default vpatch.ini): the game runs at 60 frames per second, timed with
// QueryPerformanceCounter instead of the display's vsync. Each frame is
// presented at its deadline, and the next one starts as late as it can
// before its own (by the time recent frames took to build), so the input
// it reads is as fresh as possible.

// Sets up the timers. Called once, first thing in WinMain.
void FrameCapInit();
// Presents the frame drawn last (unless this is the first), then waits
// until it is time to start the next one. Called before the calc chain.
void FrameCapBeforeCalc();
// The buttons held, as read from the devices (a replay replaces
// g_Buttons with its own): picks the replay speed.
extern WORD g_FrameCapButtons;
// The frame rate shown in the corner: returns nonzero and the rate over
// the last half second in `fps` when it is due for an update.
int FrameCapFps(float *fps);
#endif

