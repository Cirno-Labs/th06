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

#if FEAT_ENGLISH
// The static English patch (th06e.exe, Touhou Wiki's v0.8 for 1.02h): its
// strings come from build/feat/english.h, which tools/build.py generates
// from tools/english.py's output; its data from th06e_*.dat.
#include "english.h"

// spellcards.eng from th06e_IN.dat: 0x50-byte names by spell card number,
// as the spell card list on the result screen shows them, then from
// 0x1400 the same names after spaces that right-align them when a boss
// declares one.
extern char *g_EnglishSpellCards;
#define ENGLISH_SPELL_CARD(index) (g_EnglishSpellCards + (index) * 0x50)
#define ENGLISH_BOSS_SPELL_CARD(index) (g_EnglishSpellCards + 0x1400 + (index) * 0x50)
// Lines too long for one sprite go whole into both sprites of a line, the
// second one's texture moved left by this much so the text continues.
#define ENGLISH_SECOND_HALF_X -247.0f
#endif
