// FEAT_FRAME_CAP: VsyncPatch's frame limiter (see features.h).
//
// Reproduces vpatch_th06.dll (rev4) with the settings of the vpatch.ini
// it ships with: Vsync = 0 (time frames with the performance counter),
// SleepType = 1 (sleep on a waitable timer, then spin for the last
// millisecond), AutoBltPrepareTime = 1, GameFPS = 60, ReplaySkipFPS = 240,
// ReplaySlowFPS = 30, CalcFPS = 1; no window mode, LockBackBuffer = 0.
// The changes vpatch makes to the game's own code are in its sources,
// under #if FEAT_FRAME_CAP.

// Waitable timers need NT 4.
#define _WIN32_WINNT 0x0400
#include <stdlib.h>

#include "features.h"
#include "gamemanager.h"
#include "unsorted.h"

#if FEAT_FRAME_CAP

// Frame rates: normally, in a replay while skipping (Ctrl) and while
// slowed down (Shift).
#define GAME_FPS 60
#define REPLAY_SKIP_FPS 240
#define REPLAY_SLOW_FPS 30
// The frame rate shown is measured over this many milliseconds of frames.
#define FPS_WINDOW_MS 500
// Ring sizes: present times and frame build times.
#define NUM_PRESENTS 0x200
#define NUM_BUILD_TIMES 0x400

static __int64 s_freq;
// One millisecond, the least time the next frame is started early, and
// the counter's ticks in 100 ns units, in counter ticks.
static __int64 s_oneMs;
static __int64 s_minPrepare;
static double s_ticksTo100ns;
static HANDLE s_timer;
static BOOL s_started;

WORD g_FrameCapButtons;

// The frame rate and why (0 normal, 1 replay skip, 2 replay slow).
static int s_fps = GAME_FPS;
static int s_rateMode;
// Frames are due at s_base + n / s_fps seconds; the next present time.
static __int64 s_base;
static DWORD s_frame;
static __int64 s_presentAt;

// When the last few frames were presented, for the frame rate shown.
static __int64 s_presents[NUM_PRESENTS];
static DWORD s_numPresents;
static DWORD s_fpsShownAt;

// How long recent frames took from the end of the wait to their present,
// in order and sorted, and when the current frame's wait ended.
static __int64 s_buildTimes[NUM_BUILD_TIMES];
static __int64 s_sortedBuildTimes[NUM_BUILD_TIMES];
static DWORD s_numBuildTimes;
static __int64 s_buildStart;

static __int64 Now()
{
    LARGE_INTEGER now;

    QueryPerformanceCounter(&now);
    return now.QuadPart;
}

static void FrameCapRelease()
{
    timeEndPeriod(1);
    if (s_timer != NULL)
    {
        CloseHandle(s_timer);
        s_timer = NULL;
    }
}

void FrameCapInit()
{
    LARGE_INTEGER freq;

    QueryPerformanceFrequency(&freq);
    s_freq = freq.QuadPart;
    s_oneMs = s_freq / 1000;
    s_minPrepare = s_freq * 1500 / 1000000;
    s_ticksTo100ns = 10000000.0 / (double)s_freq;
    s_timer = CreateWaitableTimerA(NULL, TRUE, NULL);
    // vpatch keeps the timer resolution at 1 ms for the whole run (and
    // turns the game's own timeBeginPeriod/timeEndPeriod pairs, which
    // then change nothing, into no-ops).
    timeBeginPeriod(1);
    atexit(FrameCapRelease);
}

// Sleeps until the counter reaches `target`: on the waitable timer until a
// millisecond before, then spinning.
static void SleepUntil(__int64 target)
{
    __int64 left;
    LARGE_INTEGER due;

    left = target - Now();
    if (left <= 0)
    {
        return;
    }
    left -= s_oneMs;
    if (left > 0)
    {
        due.QuadPart = -(__int64)(left * s_ticksTo100ns);
        if (due.QuadPart < 0)
        {
            SetWaitableTimer(s_timer, &due, 0, NULL, NULL, FALSE);
            WaitForSingleObject(s_timer, INFINITE);
        }
    }
    while (Now() < target)
    {
    }
}

// Records how long the frame took to build, keeping the last
// NUM_BUILD_TIMES in order and sorted.
static void RecordBuildTime()
{
    __int64 time;
    __int64 *oldest;
    DWORD count;
    DWORD i;

    if (s_buildStart == 0)
    {
        return;
    }
    time = Now() - s_buildStart;
    count = s_numBuildTimes < NUM_BUILD_TIMES ? s_numBuildTimes : NUM_BUILD_TIMES;
    if (s_numBuildTimes >= NUM_BUILD_TIMES)
    {
        // Drop the time about to be overwritten from the sorted ones.
        oldest = &s_buildTimes[s_numBuildTimes % NUM_BUILD_TIMES];
        for (i = 0; i < count && s_sortedBuildTimes[i] != *oldest; i++)
        {
        }
        for (; i + 1 < count; i++)
        {
            s_sortedBuildTimes[i] = s_sortedBuildTimes[i + 1];
        }
        count--;
    }
    for (i = count; i > 0 && s_sortedBuildTimes[i - 1] > time; i--)
    {
        s_sortedBuildTimes[i] = s_sortedBuildTimes[i - 1];
    }
    s_sortedBuildTimes[i] = time;
    s_buildTimes[s_numBuildTimes % NUM_BUILD_TIMES] = time;
    s_numBuildTimes++;
    s_buildStart = 0;
}

// How long before its deadline to start the next frame: the 80th
// percentile of the recent build times plus 1.5 ms, or the longer of the
// last two plus 1 ms if that is more, at least 1.5 ms and at most 1 s.
static __int64 PrepareTime()
{
    __int64 time;
    __int64 last;
    __int64 previous;
    DWORD count = s_numBuildTimes;

    if (count == 0)
    {
        return s_minPrepare;
    }
    if (count >= NUM_BUILD_TIMES)
    {
        time = s_sortedBuildTimes[NUM_BUILD_TIMES * 80 / 100] + s_minPrepare;
    }
    else
    {
        time = s_sortedBuildTimes[count * 80 / 100] + s_minPrepare;
    }
    if (count > 3)
    {
        last = s_buildTimes[(count - 1) % NUM_BUILD_TIMES] + s_oneMs;
        previous = s_buildTimes[(count - 2) % NUM_BUILD_TIMES] + s_oneMs;
        if (previous > last)
        {
            last = previous;
        }
        if (last > time)
        {
            time = last;
        }
    }
    if (time < s_minPrepare)
    {
        return s_minPrepare;
    }
    if (time > s_freq)
    {
        return s_freq;
    }
    return time;
}

// Presents the frame at its deadline.
static void Present()
{
    RecordBuildTime();
    SleepUntil(s_presentAt);
    s_numPresents++;
    s_presents[s_numPresents % NUM_PRESENTS] = Now();
    g_App.d3dDevice->Present(NULL, NULL, NULL, NULL);
}

// Picks the frame rate: faster while Ctrl is held in a replay, slower
// while Shift is. A new rate restarts the deadlines from now.
static void SelectRate()
{
    int mode = 0;
    int fps;

    if (g_GameManager.isInReplay == 1)
    {
        if (g_FrameCapButtons & 0x100)
        {
            mode = 1;
        }
        else if (g_FrameCapButtons & 4)
        {
            mode = 2;
        }
    }
    if (mode == s_rateMode)
    {
        return;
    }
    s_rateMode = mode;
    fps = mode == 1 ? REPLAY_SKIP_FPS : mode == 2 ? REPLAY_SLOW_FPS : GAME_FPS;
    s_fps = fps;
    s_frame = 0;
    s_base = Now();
    s_presentAt = s_base;
}

// Sets the next deadline and waits until the next frame should start. A
// frame that is already late starts the deadlines over from now.
static void WaitForNextFrame()
{
    __int64 deadline;
    __int64 now;

    s_frame++;
    deadline = s_base + (__int64)s_frame * s_freq / s_fps;
    now = Now();
    if (deadline < now)
    {
        s_frame = 0;
        s_base = now;
        s_presentAt = now;
    }
    else
    {
        s_presentAt = deadline;
        SleepUntil(deadline - PrepareTime());
    }
    s_buildStart = Now();
}

void FrameCapBeforeCalc()
{
    if (s_started)
    {
        Present();
    }
    s_started = TRUE;
    SelectRate();
    WaitForNextFrame();
}

int FrameCapFps(float *fps)
{
    DWORD count = s_numPresents;
    DWORD frames;
    __int64 time;

    if (s_presents[count % NUM_PRESENTS] - s_presents[s_fpsShownAt % NUM_PRESENTS] < s_freq * FPS_WINDOW_MS / 1000)
    {
        return 0;
    }
    s_fpsShownAt = count;
    frames = FPS_WINDOW_MS * s_fps / 1000;
    if (frames > NUM_PRESENTS)
    {
        frames = NUM_PRESENTS;
    }
    if (count < frames)
    {
        if (count <= 1)
        {
            *fps = 0.0f;
            return 1;
        }
        time = s_presents[count] - s_presents[1];
        frames = count - 1;
    }
    else
    {
        time = s_presents[count % NUM_PRESENTS] - s_presents[(count - frames) % NUM_PRESENTS];
    }
    *fps = time == 0 ? 0.0f : (float)((double)s_freq * frames / (double)time);
    return 1;
}

#endif
