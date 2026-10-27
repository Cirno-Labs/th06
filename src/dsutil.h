// The DirectSound helpers of the DirectX 8 SDK samples (dsutil.h), as the
// game builds them: the same classes and layout, with fields of its own.

#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include "common.h"

// Reads a wave file.
class CWaveFile
{
public:
    CWaveFile();
    ~CWaveFile();

    // Opens `strFileName` for reading; the game drops the SDK's resource
    // fallback and writing, and logs why mmioOpen fails.
    HRESULT Open(LPTSTR strFileName, WAVEFORMATEX *pwfx, DWORD dwFlags);
    HRESULT ReadMMIO();
    // Closes a file opened for reading; the game drops the SDK's writing.
    HRESULT Close();
    DWORD GetSize();
    // Not in the SDK: the reset limits the data to `loopEnd` bytes and,
    // when `bLoop` is set, skips its first `loopStart` bytes.
    HRESULT ResetFile(bool bLoop);
    HRESULT Read(BYTE *pBuffer, DWORD dwSizeToRead, DWORD *pdwSizeRead);

    WAVEFORMATEX *m_pwfx;
    HMMIO m_hmmio;
    MMCKINFO m_ck;
    MMCKINFO m_ckRiff;
    DWORD m_dwSize;
    MMIOINFO m_mmioinfoOut;
    DWORD m_dwFlags;
    BOOL m_bIsReadingFromMemory;
    BYTE *m_pbData;
    BYTE *m_pbDataCur;
    ULONG m_ulDataSize;
    // Not in the SDK: the loop's start and end in bytes of the data, for
    // ResetFile. An end of 0 or less plays to the end of the file.
    int loopStart;
    int loopEnd;
};
ASSERT_SIZE(CWaveFile, 0x98);
// Owns the DirectSound object.
class CSoundManager
{
public:
    CSoundManager();
    ~CSoundManager();

    HRESULT Initialize(HWND hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate);
    HRESULT SetPrimaryBufferFormat(DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate);

    LPDIRECTSOUND8 m_pDS;
};

// A DirectSound buffer playing a wave file.
class CSound
{
public:
    CSound(LPDIRECTSOUNDBUFFER *apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile *pWaveFile);
    virtual ~CSound();

    HRESULT FillBufferWithSound(LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger);
    HRESULT RestoreBuffer(LPDIRECTSOUNDBUFFER pDSB, BOOL *pbWasRestored);
    LPDIRECTSOUNDBUFFER GetFreeBuffer();
    LPDIRECTSOUNDBUFFER GetBuffer(DWORD dwIndex);

    // Also stops any fade.
    HRESULT Play(DWORD dwPriority, DWORD dwFlags);
    // Also stops a fade, and logs the buffers it stops.
    HRESULT Stop();
    HRESULT Reset();

    LPDIRECTSOUNDBUFFER *m_apDSBuffer;
    DWORD m_dwDSBufferSize;
    CWaveFile *m_pWaveFile;
    DWORD m_dwNumBuffers;
    // Not in the SDK. While `fading`, each frame counts `fadeFrames` down
    // and lowers the volume by 5000 * (1 - fadeFrames / fadeTotal). Play
    // clears all three, Stop clears `fading`.
    int fadeFrames;
    int fadeTotal;
    int fading;
};

// A CSound refilled from its wave file as it plays.
class CStreamingSound : public CSound
{
public:
    CStreamingSound(LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, CWaveFile *pWaveFile, DWORD dwNotifySize);
    ~CStreamingSound();

    // Not in the SDK: steps a fade begun with `fading`, run once a frame.
    // Stops the buffer and returns 1 when the fade ends, otherwise 0.
    int UpdateFade();

    HRESULT Reset();

    DWORD m_dwLastPlayPos;
    DWORD m_dwPlayProgress;
    DWORD m_dwNotifySize;
    DWORD m_dwNextWriteOffset;
    BOOL m_bFillNextNotificationWithSilence;
};
