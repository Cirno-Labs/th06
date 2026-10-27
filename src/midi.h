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

// One track of a parsed MIDI file. Size from the stride of
// MidiPlayer::tracks.
struct MidiTrack
{
    // Set when the track is rewound.
    int active;
    // Delta time of the next event, read by ReadVarLen.
    DWORD delta;
    unsigned char unk_08[0x8];
    // The track's event data.
    BYTE *data;
    // Read position in `data`.
    BYTE *cursor;
    // Set to the start of the data on rewind.
    BYTE *unk_18;
    unsigned char unk_1c[0x4];
};
ASSERT_SIZE(MidiTrack, 0x20);

// Plays MIDI files on a MidiDevice, driven by its own timer. Allocated with
// operator new(0x300).
class MidiPlayer : public MmTimer
{
public:
    MidiPlayer();

    virtual void OnTimer();

    // Loads a MIDI file into a slot, stopping playback first. Returns -1
    // on failure, 0 otherwise or if the game doesn't play MIDI.
    int LoadFile(int slot, char *path);
    // Stops playback and closes the device.
    int Stop();
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
    // Starts fading the music out over `duration` ms. Returns 0.
    int FadeOut(int duration);

    void *unk_010[0x20];
    int unk_090;
    // MIDI files loaded by 0x421ff0, freed by 0x422070.
    void *fileData[0x20];
    // The number of entries in `tracks`.
    int trackCount;
    unsigned char unk_118[0x4];
    int unk_11c;
    int unk_120;
    unsigned char unk_124[0x4];
    __int64 unk_128;
    __int64 unk_130;
    // Parsed tracks, `trackCount` of them.
    MidiTrack *tracks;
    MidiDevice device;
    unsigned char unk_144[0x180];
    BYTE unk_2c4;
    float unk_2c8;
    int unk_2cc;
    int unk_2d0;
    int unk_2d4;
    int unk_2d8;
    int unk_2dc;
    // Set while fading out.
    int unk_2e0;
    // In ms, set by FadeOut.
    int fadeDuration;
    int unk_2e8;
    unsigned char unk_2ec[0x14];
};
ASSERT_SIZE(MidiPlayer, 0x300);
