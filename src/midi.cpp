#include "midi.h"
#include "unsorted.h"

MidiDevice::MidiDevice()
{
    this->handle = 0;
    this->deviceId = 0;
}

MidiDevice::~MidiDevice()
{
    this->Close();
}

int MidiDevice::Close()
{
    if (this->handle == 0)
    {
        return -1;
    }
    midiOutReset(this->handle);
    midiOutClose(this->handle);
    this->handle = 0;
    return 0;
}

int MidiDevice::Open(UINT deviceId)
{
    if (this->handle != 0)
    {
        if (this->deviceId != deviceId)
        {
            this->Close();
        }
        else
        {
            return 0;
        }
    }
    this->deviceId = deviceId;
    return midiOutOpen(&this->handle, deviceId, (DWORD_PTR)g_Hwnd6c6d5c, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR;
}

int MidiDevice::SendLongMsg(LPMIDIHDR header)
{
    if (this->handle == 0)
    {
        return 0;
    }
    if (midiOutPrepareHeader(this->handle, header, sizeof(MIDIHDR)) != MMSYSERR_NOERROR)
    {
        return 1;
    }
    return midiOutLongMsg(this->handle, header, sizeof(MIDIHDR)) != MMSYSERR_NOERROR;
}

int MidiDevice::SendShortMsg(BYTE status, BYTE data1, BYTE data2)
{
    MidiShortMsg msg;

    if (this->handle == 0)
    {
        return 0;
    }
    msg.bytes[0] = status;
    msg.bytes[1] = data1;
    msg.bytes[2] = data2;
    return midiOutShortMsg(this->handle, msg.dword) != MMSYSERR_NOERROR;
}

MmTimer::MmTimer()
{
    timeGetDevCaps(&this->caps, sizeof(this->caps));
    this->timerId = 0;
}

MmTimer::~MmTimer()
{
    this->Stop();
    timeEndPeriod(this->caps.wPeriodMin);
}

UINT MmTimer::Start(UINT delay, LPTIMECALLBACK callback, DWORD_PTR user)
{
    this->Stop();
    timeBeginPeriod(this->caps.wPeriodMin);
    if (callback != NULL)
    {
        this->timerId = timeSetEvent(delay, this->caps.wPeriodMin, callback, user, TIME_PERIODIC);
    }
    else
    {
        this->timerId = timeSetEvent(delay, this->caps.wPeriodMin, DefaultCallback, (DWORD_PTR)this, TIME_PERIODIC);
    }
    return this->timerId;
}

int MmTimer::Stop()
{
    if (this->timerId != 0)
    {
        timeKillEvent(this->timerId);
    }
    timeEndPeriod(this->caps.wPeriodMin);
    this->timerId = 0;
    return 1;
}

void MmTimer::OnTimer()
{
}

void CALLBACK MmTimer::DefaultCallback(UINT id, UINT msg, DWORD_PTR user, DWORD_PTR dw1, DWORD_PTR dw2)
{
    MmTimer *timer = (MmTimer *)user;
    timer->OnTimer();
}

WORD SwapBytes16(WORD value)
{
    union
    {
        WORD word;
        BYTE bytes[2];
    } swapped;

    swapped.bytes[0] = ((BYTE *)&value)[1];
    swapped.bytes[1] = ((BYTE *)&value)[0];
    return swapped.word;
}

DWORD ReadVarLen(BYTE **cursor)
{
    DWORD value = 0;
    BYTE byte;

    do
    {
        byte = **cursor;
        (*cursor)++;
        value = (value << 7) + (byte & 0x7f);
    } while (byte & 0x80);
    return value;
}

MidiPlayer::MidiPlayer()
{
    int i;
    int j;

    this->tracks = NULL;
    this->unk_11c = 0;
    this->unk_120 = 0;
    this->trackCount = 0;
    this->unk_2c4 = 0;
    this->unk_2c8 = 0.0f;
    this->unk_2cc = 0;
    this->unk_2d0 = 0;
    this->unk_2d4 = 0;
    this->unk_2d8 = 0;
    this->unk_2dc = 0;
    this->unk_2e0 = 0;
    for (i = 0; i < 0x20; i++)
    {
        this->fileData[i] = NULL;
    }
    for (j = 0; j < 0x20; j++)
    {
        this->unk_010[j] = NULL;
    }
    this->unk_090 = 0;
}

void MidiPlayer::UnloadFile(int slot)
{
    MemFree(this->fileData[slot]);
    this->fileData[slot] = NULL;
}

int MidiPlayer::LoadFile(int slot, char *path)
{
    if (g_MusicMode != 2)
    {
        return 0;
    }
    this->Stop();
    this->UnloadFile(slot);
    this->fileData[slot] = Func41e290(path, 0);
    if (this->fileData[slot] == NULL)
    {
        Func41e4d0(&g_Unknown69d998, "error : MIDI File が読み込めない %s \n", path);
        return -1;
    }
    return 0;
}

int MidiPlayer::Load(char *path)
{
    if (this->LoadFile(0x1f, path) != 0)
    {
        return -1;
    }
    this->ParseFile(0x1f);
    this->UnloadFile(0x1f);
    return 0;
}

int MidiPlayer::Play()
{
    if (this->tracks == NULL)
    {
        return -1;
    }
    this->Rewind();
    this->device.Open(MIDI_MAPPER);
    this->Start(1, NULL, 0);
    return 0;
}

void MidiPlayer::Rewind()
{
    int i;
    MidiTrack *track;

    track = this->tracks;
    this->unk_2c8 = 1.0f;
    this->unk_2dc = 0;
    this->unk_2e0 = 0;
    this->unk_128 = 0;
    this->unk_130 = 0;
    for (i = 0; i < this->trackCount; i++, track++)
    {
        track->cursor = track->data;
        track->unk_18 = track->cursor;
        track->active = 1;
        track->delta = ReadVarLen(&track->cursor);
    }
}

int MidiPlayer::FadeOut(int duration)
{
    this->unk_2c8 = 0.0f;
    this->fadeDuration = duration;
    this->unk_2e8 = 0;
    this->unk_2dc = 0;
    this->unk_2e0 = 1;
    return 0;
}
