// MIDI playback through WinMM.

#pragma once

#include <windows.h>
#include <mmsystem.h>

// Wraps a MIDI output device (midiOutOpen and friends).
class MidiDevice
{
public:
    MidiDevice();

    HMIDIOUT handle;
    UINT deviceId;
};
