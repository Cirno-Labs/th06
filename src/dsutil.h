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
    ~CWaveFile();

    // The SDK's, with a flag of the game's.
    HRESULT ResetFile(bool unk);
    HRESULT Read(BYTE *pBuffer, DWORD dwSizeToRead, DWORD *pdwSizeRead);

    WAVEFORMATEX *m_pwfx;
};

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

    LPDIRECTSOUNDBUFFER *m_apDSBuffer;
    DWORD m_dwDSBufferSize;
    CWaveFile *m_pWaveFile;
    DWORD m_dwNumBuffers;
    // Not in the SDK. While `fading`, each frame counts `fadeFrames` down
    // and lowers the volume by 5000 * (1 - fadeFrames / fadeTotal); Stop
    // clears `fading`.
    int fadeFrames;
    int fadeTotal;
    int fading;
};

// A CSound refilled from its wave file as it plays.
class CStreamingSound : public CSound
{
};
