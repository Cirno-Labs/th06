// The file from 0x43a7b0: the DirectX 8 SDK's dsutil.cpp, as the game
// builds it.

#include "dsutil.h"
#include "unsorted.h"

// From the SDK's dxutil.h.
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
// From the SDK's dxerr8.h, as in a release build.
#define DXTRACE_ERR(str, hr) (hr)

#define WAVEFILE_READ 1

CSoundManager::CSoundManager()
{
    m_pDS = NULL;
}

CSoundManager::~CSoundManager()
{
    SAFE_RELEASE(m_pDS);
}

HRESULT CSoundManager::Initialize(HWND hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate)
{
    HRESULT hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    SAFE_RELEASE(m_pDS);

    // Create IDirectSound using the primary sound device
    if (FAILED(hr = DirectSoundCreate8(NULL, &m_pDS, NULL)))
        return DXTRACE_ERR(TEXT("DirectSoundCreate8"), hr);

    // Set DirectSound coop level
    if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, dwCoopLevel)))
        return DXTRACE_ERR(TEXT("SetCooperativeLevel"), hr);

    // Set primary buffer format
    SetPrimaryBufferFormat(dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate);

    return S_OK;
}

HRESULT CSoundManager::SetPrimaryBufferFormat(DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate)
{
    HRESULT hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if (m_pDS == NULL)
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat = NULL;

    if (FAILED(hr = m_pDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL)))
        return DXTRACE_ERR(TEXT("CreateSoundBuffer"), hr);

    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = (WORD)dwPrimaryChannels;
    wfx.nSamplesPerSec = dwPrimaryFreq;
    wfx.wBitsPerSample = (WORD)dwPrimaryBitRate;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if (FAILED(hr = pDSBPrimary->SetFormat(&wfx)))
        return DXTRACE_ERR(TEXT("SetFormat"), hr);

    SAFE_RELEASE(pDSBPrimary);

    return S_OK;
}

CSound::CSound(LPDIRECTSOUNDBUFFER *apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile *pWaveFile)
{
    DWORD i;

    m_apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    for (i = 0; i < dwNumBuffers; i++)
        m_apDSBuffer[i] = apDSBuffer[i];

    m_dwDSBufferSize = dwDSBufferSize;
    m_dwNumBuffers = dwNumBuffers;
    m_pWaveFile = pWaveFile;

    FillBufferWithSound(m_apDSBuffer[0], FALSE);

    // Make DirectSound do pre-processing on sound effects
    for (i = 0; i < dwNumBuffers; i++)
        m_apDSBuffer[i]->SetCurrentPosition(0);
}

CSound::~CSound()
{
    for (DWORD i = 0; i < m_dwNumBuffers; i++)
    {
        SAFE_RELEASE(m_apDSBuffer[i]);
    }

    SAFE_DELETE_ARRAY(m_apDSBuffer);
    SAFE_DELETE(m_pWaveFile);
}

HRESULT CSound::FillBufferWithSound(LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger)
{
    HRESULT hr;
    VOID *pDSLockedBuffer = NULL; // Pointer to locked buffer memory
    DWORD dwDSLockedBufferSize = 0; // Size of the locked DirectSound buffer
    DWORD dwWavDataRead = 0; // Amount of data read from the wav file

    if (pDSB == NULL)
        return CO_E_NOTINITIALIZED;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if (FAILED(hr = RestoreBuffer(pDSB, NULL)))
        return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

    // Lock the buffer down
    if (FAILED(hr = pDSB->Lock(0, m_dwDSBufferSize, &pDSLockedBuffer, &dwDSLockedBufferSize, NULL, NULL, 0L)))
        return DXTRACE_ERR(TEXT("Lock"), hr);

    // Reset the wave file to the beginning
    m_pWaveFile->ResetFile(false);

    if (FAILED(hr = m_pWaveFile->Read((BYTE *)pDSLockedBuffer, dwDSLockedBufferSize, &dwWavDataRead)))
        return DXTRACE_ERR(TEXT("Read"), hr);

    if (dwWavDataRead == 0)
    {
        // Wav is blank, so just fill with silence
        FillMemory((BYTE *)pDSLockedBuffer, dwDSLockedBufferSize,
                   (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0));
    }
    else if (dwWavDataRead < dwDSLockedBufferSize)
    {
        // If the wav file was smaller than the DirectSound buffer,
        // we need to fill the remainder of the buffer with data
        if (bRepeatWavIfBufferLarger)
        {
            // Reset the file and fill the buffer with wav data
            DWORD dwReadSoFar = dwWavDataRead; // From previous call above.
            while (dwReadSoFar < dwDSLockedBufferSize)
            {
                // This will keep reading in until the buffer is full
                // for very short files
                if (FAILED(hr = m_pWaveFile->ResetFile(false)))
                    return DXTRACE_ERR(TEXT("ResetFile"), hr);

                hr = m_pWaveFile->Read((BYTE *)pDSLockedBuffer + dwReadSoFar, dwDSLockedBufferSize - dwReadSoFar,
                                       &dwWavDataRead);
                if (FAILED(hr))
                    return DXTRACE_ERR(TEXT("Read"), hr);

                dwReadSoFar += dwWavDataRead;
            }
        }
        else
        {
            // Don't repeat the wav file, just fill in silence
            FillMemory((BYTE *)pDSLockedBuffer + dwWavDataRead, dwDSLockedBufferSize - dwWavDataRead,
                       (BYTE)(m_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0));
        }
    }

    // Unlock the buffer, we don't need it anymore.
    pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

    return S_OK;
}

HRESULT CSound::RestoreBuffer(LPDIRECTSOUNDBUFFER pDSB, BOOL *pbWasRestored)
{
    HRESULT hr;

    if (pDSB == NULL)
        return CO_E_NOTINITIALIZED;
    if (pbWasRestored)
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if (FAILED(hr = pDSB->GetStatus(&dwStatus)))
        return DXTRACE_ERR(TEXT("GetStatus"), hr);

    if (dwStatus & DSBSTATUS_BUFFERLOST)
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so
        // the restoring the buffer may fail.
        // If it does, sleep until DirectSound gives us control.
        do
        {
            hr = pDSB->Restore();
            if (hr == DSERR_BUFFERLOST)
                Sleep(10);
        } while (hr = pDSB->Restore());

        if (pbWasRestored != NULL)
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}

LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer()
{
    BOOL bIsPlaying = FALSE;

    if (m_apDSBuffer == NULL)
        return FALSE;

    for (DWORD i = 0; i < m_dwNumBuffers; i++)
    {
        if (m_apDSBuffer[i])
        {
            DWORD dwStatus = 0;
            m_apDSBuffer[i]->GetStatus(&dwStatus);
            if ((dwStatus & DSBSTATUS_PLAYING) == 0)
                break;
        }
    }

    if (i != m_dwNumBuffers)
        return m_apDSBuffer[i];
    else
        return m_apDSBuffer[rand() % m_dwNumBuffers];
}

LPDIRECTSOUNDBUFFER CSound::GetBuffer(DWORD dwIndex)
{
    if (m_apDSBuffer == NULL)
        return NULL;
    if (dwIndex >= m_dwNumBuffers)
        return NULL;

    return m_apDSBuffer[dwIndex];
}

HRESULT CSound::Play(DWORD dwPriority, DWORD dwFlags)
{
    HRESULT hr;
    BOOL bRestored;

    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;

    LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

    if (pDSB == NULL)
        return DXTRACE_ERR(TEXT("GetFreeBuffer"), E_FAIL);

    // Restore the buffer if it was lost
    if (FAILED(hr = RestoreBuffer(pDSB, &bRestored)))
        return DXTRACE_ERR(TEXT("RestoreBuffer"), hr);

    if (bRestored)
    {
        // The buffer was restored, so we need to fill it with new data
        if (FAILED(hr = FillBufferWithSound(pDSB, FALSE)))
            return DXTRACE_ERR(TEXT("FillBufferWithSound"), hr);

        // Make DirectSound do pre-processing on sound effects
        Reset();
    }

    this->fading = 0;
    this->fadeFrames = 0;
    this->fadeTotal = 0;
    return pDSB->Play(0, dwPriority, dwFlags);
}

HRESULT CSound::Stop()
{
    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    DebugPrint("CSound::Stop ");
    for (DWORD i = 0; i < m_dwNumBuffers; i++)
    {
        DebugPrint("%d ", i);
        hr |= m_apDSBuffer[i]->Stop();
    }
    DebugPrint("\n");
    this->fading = 0;

    return hr;
}

HRESULT CSound::Reset()
{
    if (m_apDSBuffer == NULL)
        return CO_E_NOTINITIALIZED;

    HRESULT hr = 0;

    for (DWORD i = 0; i < m_dwNumBuffers; i++)
        hr |= m_apDSBuffer[i]->SetCurrentPosition(0);

    return hr;
}

CStreamingSound::CStreamingSound(LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, CWaveFile *pWaveFile, DWORD dwNotifySize)
    : CSound(&pDSBuffer, dwDSBufferSize, 1, pWaveFile)
{
    m_dwLastPlayPos = 0;
    m_dwPlayProgress = 0;
    m_dwNotifySize = dwNotifySize;
    m_dwNextWriteOffset = 0;
    m_bFillNextNotificationWithSilence = FALSE;
}

CStreamingSound::~CStreamingSound()
{
}

int CStreamingSound::UpdateFade()
{
    LONG lVolume;
    HRESULT hr;

    if (this->fading)
    {
        this->fadeFrames--;
        if (this->fadeFrames <= 0)
        {
            this->fading = 0;
            m_apDSBuffer[0]->Stop();
            return 1;
        }
        lVolume = this->fadeFrames * 5000 / this->fadeTotal - 5000;
        hr = m_apDSBuffer[0]->SetVolume(lVolume);
    }
    return 0;
}

CWaveFile::CWaveFile()
{
    m_pwfx = NULL;
    m_hmmio = NULL;
    m_dwSize = 0;
    m_bIsReadingFromMemory = FALSE;
    this->unk_94 = 0;
    this->unk_90 = 0;
}

DWORD CWaveFile::GetSize()
{
    return m_dwSize;
}

HRESULT CWaveFile::Close()
{
    if (m_dwFlags == WAVEFILE_READ)
    {
        mmioClose(m_hmmio, 0);
        m_hmmio = NULL;
    }

    return S_OK;
}

CWaveFile::~CWaveFile()
{
    Close();

    if (!m_bIsReadingFromMemory)
        SAFE_DELETE_ARRAY(m_pwfx);
}
