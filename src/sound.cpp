// The file with SoundPlayer, the sound effects and wave music, from
// 0x430270.

#include "unsorted.h"

// The game's data, as the original defines it here. The DLL build uses
// the original's copies instead.
#ifdef TH06_EXE
SoundInfo g_SoundInfos[0x20] = {
    {0, -1500, 0}, {0, -2000, 0}, {1, -1200, 5}, {1, -1400, 5},
    {2, -1000, 100}, {3, -500, 100}, {4, -500, 100}, {5, -1700, 50},
    {6, -1700, 50}, {7, -1700, 50}, {8, -1000, 100}, {9, -1000, 100},
    {10, -1900, 10}, {11, -1200, 10}, {12, -900, 100}, {5, -1500, 50},
    {13, -900, 50}, {14, -900, 50}, {15, -600, 100}, {16, -400, 100},
    {17, -1100, 0}, {18, -900, 0}, {5, -1800, 20}, {6, -1800, 20},
    {7, -1800, 20}, {19, -300, 50}, {20, -600, 50}, {21, -800, 50},
    {22, -100, 140}, {23, -500, 100}, {24, -1000, 20}, {25, -1000, 90},
};

char *g_SoundPaths[] = {
    "data/wav/plst00.wav",
    "data/wav/enep00.wav",
    "data/wav/pldead00.wav",
    "data/wav/power0.wav",
    "data/wav/power1.wav",
    "data/wav/tan00.wav",
    "data/wav/tan01.wav",
    "data/wav/tan02.wav",
    "data/wav/ok00.wav",
    "data/wav/cancel00.wav",
    "data/wav/select00.wav",
    "data/wav/gun00.wav",
    "data/wav/cat00.wav",
    "data/wav/lazer00.wav",
    "data/wav/lazer01.wav",
    "data/wav/enep01.wav",
    "data/wav/nep00.wav",
    "data/wav/damage00.wav",
    "data/wav/item00.wav",
    "data/wav/kira00.wav",
    "data/wav/kira01.wav",
    "data/wav/kira02.wav",
    "data/wav/extend.wav",
    "data/wav/timeout.wav",
    "data/wav/graze.wav",
    "data/wav/powerup.wav",
};
#endif // TH06_EXE

char *FindRiffChunk(char *data, char *name, int *chunkSize, unsigned int size);

// Spins until `ms` milliseconds have passed, or the clock wraps.
#define WAIT_FOR(ms)                                                                                                   \
    {                                                                                                                  \
        DWORD start = timeGetTime();                                                                                   \
        DWORD now = start;                                                                                             \
        DWORD delay = ms;                                                                                              \
        while (now < start + delay && now >= start)                                                                    \
        {                                                                                                              \
            now = timeGetTime();                                                                                       \
        }                                                                                                              \
    }

int SoundPlayer::Initialize(HWND hwnd)
{
    DSBUFFERDESC params;
    LPVOID audioPtrTwo;
    DWORD size2;
    DWORD sizeOne;
    LPVOID audioPtr_1;
    WAVEFORMATEX formatInfo;

    this->manager = new CSoundManager();
    if (this->manager->Initialize(hwnd, DSSCL_PRIORITY, 2, 44100, 16) < 0)
    {
        LogPrintf(&g_Log, "DirectSound オブジェクトの初期化が失敗したよ\n");
        if (this->manager != NULL)
        {
            delete this->manager;
            this->manager = NULL;
        }
        return -1;
    }
    this->dsound = this->manager->GetDirectSound();
    this->notifyThread = NULL;
    memset(&params, 0, sizeof(params));
    params.dwSize = sizeof(params);
    params.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCSOFTWARE;
    params.dwBufferBytes = 0x8000;
    memset(&formatInfo, 0, sizeof(formatInfo));
    formatInfo.cbSize = 0;
    formatInfo.wFormatTag = WAVE_FORMAT_PCM;
    formatInfo.nChannels = 2;
    formatInfo.nSamplesPerSec = 44100;
    formatInfo.nAvgBytesPerSec = 44100 * 4;
    formatInfo.nBlockAlign = 4;
    formatInfo.wBitsPerSample = 16;
    params.lpwfxFormat = &formatInfo;
    if (this->dsound->CreateSoundBuffer(&params, &this->silence, NULL) < 0)
    {
        return -1;
    }
    if (this->silence->Lock(0, 0x8000, &audioPtr_1, &sizeOne, &audioPtrTwo, &size2, 0) < 0)
    {
        return -1;
    }
    memset(audioPtr_1, 0, 0x8000);
    this->silence->Unlock(audioPtr_1, sizeOne, audioPtrTwo, size2);
    this->silence->Play(0, 0, DSBPLAY_LOOPING);
    SetTimer(hwnd, 0, 250, NULL);
    this->hwnd = hwnd;
    LogPrintf(&g_Log, "DirectSound は正常に初期化されました\n");
    return 0;
}

int SoundPlayer::Release()
{
    int i;

    if (!this->manager)
    {
        return 0;
    }
    for (i = 0; i < 0x80; i++)
    {
        if (this->sounds[i] != NULL)
        {
            this->sounds[i]->Release();
            this->sounds[i] = NULL;
        }
        if (this->soundBuffers[i] != NULL)
        {
            this->soundBuffers[i]->Release();
            this->soundBuffers[i] = NULL;
        }
    }
    KillTimer(this->hwnd, 1);
    this->StopBgm();
    this->dsound = NULL;
    this->silence->Stop();
    if (this->silence != NULL)
    {
        this->silence->Release();
        this->silence = NULL;
    }
    if (this->bgm != NULL)
    {
        delete this->bgm;
        this->bgm = NULL;
    }
    if (this->manager != NULL)
    {
        delete this->manager;
        this->manager = NULL;
    }
    return 0;
}

int SoundPlayer::LoadSound(int index, char *path)
{
    char *cursor;
    DSBUFFERDESC params;
    char *chunk;
    int size;
    LPVOID audioPtrTwo;
    DWORD size2;
    DWORD sizeOne;
    LPVOID audioPtr_1;
    char *wavBuf;
    WAVEFORMATEX formatInfo;
    int totalSize2;

    if (!this->manager)
    {
        return 0;
    }
    if (this->soundBuffers[index] != NULL)
    {
        this->soundBuffers[index]->Release();
        this->soundBuffers[index] = NULL;
    }
    wavBuf = (char *)LoadFileData(path, 0);
    cursor = wavBuf;
    if (cursor == NULL)
    {
        return -1;
    }
    if (strncmp(cursor, "RIFF", 4))
    {
        LogPrintf(&g_Log, "Wav ファイルじゃない %s\n", path);
        free(wavBuf);
        return -1;
    }
    cursor += 4;
    totalSize2 = *(int *)cursor;
    cursor += 4;
    if (strncmp(cursor, "WAVE", 4))
    {
        LogPrintf(&g_Log, "Wav ファイルじゃない? %s\n", path);
        free(wavBuf);
        return -1;
    }
    cursor += 4;
    chunk = FindRiffChunk(cursor, "fmt ", &size, totalSize2 - 12);
    if (chunk == NULL)
    {
        LogPrintf(&g_Log, "Wav ファイルじゃない? %s\n", path);
        free(wavBuf);
        return -1;
    }
    formatInfo = *(WAVEFORMATEX *)chunk;
    chunk = FindRiffChunk(cursor, "data", &size, totalSize2 - 12);
    if (chunk == NULL)
    {
        LogPrintf(&g_Log, "Wav ファイルじゃない? %s\n", path);
        free(wavBuf);
        return -1;
    }
    memset(&params, 0, sizeof(params));
    params.dwSize = sizeof(params);
    params.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
    params.dwBufferBytes = size;
    params.lpwfxFormat = &formatInfo;
    if (this->dsound->CreateSoundBuffer(&params, &this->soundBuffers[index], NULL) < 0)
    {
        free(wavBuf);
        return -1;
    }
    if (this->soundBuffers[index]->Lock(0, size, &audioPtr_1, &sizeOne, &audioPtrTwo, &size2, 0) < 0)
    {
        free(wavBuf);
        return -1;
    }
    memcpy(audioPtr_1, chunk, sizeOne);
    if (size2 != 0)
    {
        memcpy(audioPtrTwo, chunk + sizeOne, size2);
    }
    this->soundBuffers[index]->Unlock(audioPtr_1, sizeOne, audioPtrTwo, size2);
    free(wavBuf);
    return 0;
}

// Finds chunk `name` among the RIFF chunks at `data`, `size` bytes of
// them. Returns its contents and sets *chunkSize, or returns NULL.
char *FindRiffChunk(char *data, char *name, int *chunkSize, unsigned int size)
{
    while (size > 0)
    {
        *chunkSize = *(int *)(data + 4);
        if (strncmp(data, name, 4) == 0)
        {
            return data + 8;
        }
        size -= *chunkSize + 8;
        data += *chunkSize + 8;
    }
    return NULL;
}

int SoundPlayer::LoadBgm(char *path)
{
    DWORD dwNotifySize;
    CWaveFile waveFile;
    HRESULT hr;
    INT nSamplesPerSec;
    DWORD nBlockAlign;

    if (!this->manager)
    {
        return -1;
    }
    if (!g_App.cfg.playSounds)
    {
        return -1;
    }
    if (this->dsound == NULL)
    {
        return -1;
    }
    this->StopBgm();
    DebugPrint("load BGM\n");
    hr = waveFile.Open(path, NULL, WAVEFILE_READ);
    if (hr < 0)
    {
        DebugPrint("error : wav file load error %s\n", path);
        waveFile.Close();
        return -1;
    }
    if (waveFile.GetSize() == 0)
    {
        waveFile.Close();
        return -1;
    }
    WAIT_FOR(100);
    waveFile.Close();
    nBlockAlign = waveFile.m_pwfx->nBlockAlign;
    nSamplesPerSec = waveFile.m_pwfx->nSamplesPerSec;
    dwNotifySize = nSamplesPerSec * 2 * nBlockAlign / 4;
    dwNotifySize -= dwNotifySize % nBlockAlign;
    this->notifyEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    this->notifyThread = CreateThread(NULL, 0, BgmNotificationProc, g_App.hwnd, 0, &this->notifyThreadId);
    hr = this->manager->CreateStreaming(&this->bgm, path, DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2,
                                        GUID_NULL, 4, dwNotifySize, this->notifyEvent);
    if (hr < 0)
    {
        DebugPrint("error : ストリーミング用サウンドバッファを作成出来ませんでした\n");
        return -1;
    }
    DebugPrint("comp\n");
    WAIT_FOR(100);
    return 0;
}

int SoundPlayer::LoadBgmLoop(char *path)
{
    int *posData;
    CWaveFile *pWaveFile;
    int loopEnd;
    int loopBegin;

    if (!this->manager)
    {
        return -1;
    }
    if (!g_App.cfg.playSounds)
    {
        return -1;
    }
    if (this->bgm == NULL)
    {
        return -1;
    }
    posData = (int *)LoadFileData(path, 0);
    if (posData == NULL)
    {
        return -1;
    }
    pWaveFile = this->bgm->m_pWaveFile;
    loopEnd = posData[1] * 4;
    loopBegin = posData[0] * 4;
    pWaveFile->loopStart = loopBegin;
    pWaveFile->loopEnd = loopEnd;
    free(posData);
    return 0;
}

int SoundPlayer::PlayBgm(BOOL bLoop)
{
    LPDIRECTSOUNDBUFFER pDSB;
    HRESULT hr;

    DebugPrint("play BGM\n");
    if (this->bgm == NULL)
    {
        return -1;
    }
    hr = this->bgm->Reset();
    if (hr < 0)
    {
        return -1;
    }
    pDSB = this->bgm->GetBuffer(0);
    hr = this->bgm->FillBufferWithSound(pDSB, bLoop);
    if (hr < 0)
    {
        return -1;
    }
    hr = this->bgm->Play(0, DSBPLAY_LOOPING);
    if (hr < 0)
    {
        return -1;
    }
    DebugPrint("comp\n");
    this->bgmLoop = bLoop;
    return 0;
}

void SoundPlayer::StopBgm()
{
    if (this->bgm != NULL)
    {
        this->bgm->Stop();
        if (this->notifyThread != NULL)
        {
            PostThreadMessageA(this->notifyThreadId, WM_QUIT, 0, 0);
            DebugPrint("stop m_dwNotifyThreadID\n");
            WaitForSingleObject(this->notifyThread, INFINITE);
            DebugPrint("comp\n");
            CloseHandle(this->notifyThread);
            CloseHandle(this->notifyEvent);
            this->notifyThread = NULL;
        }
        if (this->bgm != NULL)
        {
            delete this->bgm;
            this->bgm = NULL;
        }
        DebugPrint("stop BGM\n");
    }
}

int SoundPlayer::LoadSounds()
{
    int i;

    if (!this->manager)
    {
        return -1;
    }
    if (this->dsound == NULL)
    {
        return 0;
    }
    for (i = 0; i < 3; i++)
    {
        this->queue[i] = -1;
    }
    for (i = 0; i < 0x1a; i++)
    {
        if (this->LoadSound(i, g_SoundPaths[i]) != 0)
        {
            LogPrintf(&g_Log, "error : Sound ファイルが読み込めない データを確認 %s\n", g_SoundPaths[i]);
            return -1;
        }
    }
    for (i = 0; i < ARRAY_SIZE(g_SoundInfos); i++)
    {
        this->dsound->DuplicateSoundBuffer(this->soundBuffers[g_SoundInfos[i].buffer], &this->sounds[i]);
        this->sounds[i]->SetCurrentPosition(0);
        this->sounds[i]->SetVolume(g_SoundInfos[i].volume);
    }
    return 0;
}

void SoundPlayer::QueueSound(int sound, int unused)
{
    int i;
    int unk;

    unk = g_SoundInfos[sound].unk_6;
    for (i = 0; i < 3; i++)
    {
        if (this->queue[i] < 0)
        {
            break;
        }
        if (this->queue[i] == sound)
        {
            return;
        }
    }
    if (i >= 3)
    {
        return;
    }
    this->queue[i] = sound;
    this->unk_408[sound] = unk;
}

void SoundPlayer::PlayQueuedSounds()
{
    int i;
    int sound;

    if (!this->manager)
    {
        return;
    }
    if (!g_App.cfg.playSounds)
    {
        return;
    }
    for (i = 0; i < 3; i++)
    {
        if (this->queue[i] < 0)
        {
            break;
        }
        sound = this->queue[i];
        this->queue[i] = -1;
        if (this->sounds[sound] == NULL)
        {
            continue;
        }
        this->sounds[sound]->Stop();
        this->sounds[sound]->SetCurrentPosition(0);
        this->sounds[sound]->Play(0, 0, 0);
    }
}

DWORD WINAPI BgmNotificationProc(LPVOID lpParameter)
{
    HRESULT hr;
    HWND hDlg = (HWND)lpParameter;
    MSG msg;
    DWORD dwResult;
    BOOL bDone = FALSE;
    BOOL bLooped = TRUE;

    while (!bDone)
    {
        dwResult = MsgWaitForMultipleObjects(1, &g_SoundPlayer.notifyEvent, FALSE, INFINITE, QS_ALLEVENTS);
        if (g_SoundPlayer.bgm == NULL)
        {
            bDone = TRUE;
        }
        switch (dwResult)
        {
        case WAIT_OBJECT_0 + 0:
            if (g_SoundPlayer.bgm != NULL)
            {
                hr = g_SoundPlayer.bgm->HandleWaveStreamNotification(bLooped);
            }
            break;
        case WAIT_OBJECT_0 + 1:
            while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    bDone = TRUE;
                }
            }
            break;
        }
    }
    return 0;
}

SoundPlayer::SoundPlayer()
{
    int i;

    memset(this, 0, sizeof(*this));
    for (i = 0; i < 0x80; i++)
    {
        this->unk_408[i] = -1;
    }
}
