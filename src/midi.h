// MIDI playback through WinMM.

#pragma once

#include <windows.h>
#include <mmsystem.h>

#include "common.h"

// A short MIDI message as midiOutShortMsg takes it: status and two data
// bytes, low byte first.
union MidiShortMsg
{
    DWORD dword;
    BYTE bytes[4];
};

// Swaps the bytes of a big-endian 16-bit value from a MIDI file.
WORD SwapBytes16(WORD value);

// Reads a MIDI variable-length quantity (7 bits per byte, high bit set on
// all but the last) and advances the cursor past it.
DWORD ReadVarLen(BYTE **cursor);

// Wraps a MIDI output device (midiOutOpen and friends).
class MidiDevice
{
public:
    MidiDevice();
    ~MidiDevice();

    // Returns -1 if no device is open, 0 once it is closed.
    int Close();

    // Opens the given device, closing another open one first. Returns
    // nonzero on failure, 0 on success or if the device is already open.
    int Open(UINT deviceId);

    // Prepares and sends a system-exclusive message. Returns 0 if no
    // device is open, nonzero if either step fails.
    int SendLongMsg(LPMIDIHDR header);

    // Sends a channel message. Returns 0 if no device is open, nonzero on
    // failure.
    int SendShortMsg(BYTE status, BYTE data1, BYTE data2);

    HMIDIOUT handle;
    UINT deviceId;
};

// A periodic WinMM multimedia timer. By default the timer calls OnTimer.
class MmTimer
{
public:
    MmTimer();
    ~MmTimer();

    // Starts a periodic timer event every `delay` ms at the finest
    // resolution, calling `callback` with `user`, or OnTimer if there is no
    // callback. Returns the timer ID, 0 on failure.
    UINT Start(UINT delay, LPTIMECALLBACK callback, DWORD_PTR user);

    // Kills the timer event, if any, and ends the timer period. Returns 1.
    int Stop();

    // Called on every tick when the timer was started without a callback
    // of its own. Empty here; the linker folded it with another empty
    // function at 0x423330.
    virtual void OnTimer();

    // The timeSetEvent callback used when Start gets none: calls OnTimer
    // on the timer passed as user data.
    static void CALLBACK DefaultCallback(UINT id, UINT msg, DWORD_PTR user, DWORD_PTR dw1, DWORD_PTR dw2);

    UINT timerId;
    TIMECAPS caps;
};
ASSERT_SIZE(MmTimer, 0x10);

// Plays MIDI files on a MidiDevice, driven by its own timer. Allocated with
// operator new(0x300).
class MidiPlayer : public MmTimer
{
public:
    MidiPlayer();

    virtual void OnTimer();

    // Loads a MIDI file into a slot; nonzero on failure.
    int LoadFile(int slot, char *path);
    // Frees the MIDI file loaded into a slot.
    void UnloadFile(int slot);
    // Parses the file in a slot into tracks.
    void ParseFile(int slot);
    // Loads and parses a MIDI file through the scratch slot 31. Returns -1
    // if it cannot be loaded, otherwise 0.
    int Load(char *path);
    // Rewinds the tracks.
    void Rewind();
    // Plays the loaded tracks on the MIDI mapper, ticking every 1 ms.
    // Returns -1 if nothing is loaded, otherwise 0.
    int Play();

    void *unk_010[0x20];
    int unk_090;
    // MIDI files loaded by 0x421ff0, freed by 0x422070.
    void *fileData[0x20];
    // The number of entries in `tracks`.
    int trackCount;
    unsigned char unk_118[0x4];
    int unk_11c;
    int unk_120;
    unsigned char unk_124[0x14];
    // 0x20-byte track records: the data at +0x10, a read cursor at +0x14.
    void *tracks;
    MidiDevice device;
    unsigned char unk_144[0x180];
    BYTE unk_2c4;
    int unk_2c8;
    int unk_2cc;
    int unk_2d0;
    int unk_2d4;
    int unk_2d8;
    int unk_2dc;
    int unk_2e0;
    unsigned char unk_2e4[0x1c];
};
ASSERT_SIZE(MidiPlayer, 0x300);
