// Sound effects and the wave music.
// Included by unsorted.h where these declarations were; it relies on what
// unsorted.h declares before it.

#pragma once

// The sound effects and the wave music, through DirectSound and the DX8
// SDK's CSoundManager; the global is at 0x6d3f50 and g_App plays wave
// music through it.
class SoundPlayer
{
public:
    // Zeroes everything; no sound is loaded.
    SoundPlayer();
    // Sets up DirectSound for `hwnd` (stereo, 44.1 kHz, 16 bits) and
    // starts a silent looping buffer and a 250 ms timer. Returns 0 or -1.
    int Initialize(HWND hwnd);
    // Releases the sounds, the wave music and DirectSound. Returns 0.
    int Release();
    // Loads the wave file at `path` as the streaming wave music, after
    // stopping what plays, and starts the thread that streams it.
    // Returns 0 or -1.
    int LoadBgm(char *path);
    // Sets the wave music's loop from the .pos file at `path`: its start
    // and end, in samples of 4 bytes. Returns 0 or -1.
    int LoadBgmLoop(char *path);
    // Plays the wave music from the start, filling the buffer with
    // `bLoop` as the repeat flag. Returns 0 or -1.
    int PlayBgm(BOOL bLoop);
    // Stops the wave music: ends the notification thread and releases
    // the stream ("stop BGM").
    void StopBgm();
    // Fades the wave music out over `seconds`, counted in frames at 60
    // per second.
    void FadeOutBgm(float seconds);
    // Loads the sound effects ("data/wav/plst00.wav", ...).
    int LoadSounds();
    // Loads the wave file at `path` into soundBuffers[`index`]. Returns
    // 0, also when there is no sound, or -1 on failure.
    int LoadSound(int index, char *path);
    // Queues sound effect `sound`, unless it is already queued or three
    // are. `unused` is not used.
    void QueueSound(int sound, int unused);
    // Plays the queued sound effects from the start, if sound effects
    // are on, and empties the queue.
    void PlayQueuedSounds();

    IDirectSound8 *dsound;
    DWORD pad_004;
    // The loaded sound files' buffers.
    IDirectSoundBuffer *soundBuffers[0x80];
    // Each sound effect's own buffer.
    IDirectSoundBuffer *sounds[0x80];
    // Set from g_SoundInfos[].unk_6 when a sound is queued (nothing reads
    // either); -1 at first.
    int unk_408[0x80];
    // A silent buffer, looping from start-up.
    IDirectSoundBuffer *silence;
    // The window the sound timer posts to.
    HWND hwnd;
    // NULL until Initialize succeeds.
    CSoundManager *manager;
    // The thread streaming the wave music and its ID.
    DWORD notifyThreadId;
    HANDLE notifyThread;
    DWORD pad_61c;
    // The sound effects to play, or -1.
    int queue[3];
    // The wave music playing; NULL when stopped.
    CStreamingSound *bgm;
    // Signalled as the wave music plays.
    HANDLE notifyEvent;
    // PlayBgm's `bLoop`.
    BOOL bgmLoop;
};
ASSERT_SIZE(SoundPlayer, 0x638);
extern SoundPlayer g_SoundPlayer;

// The thread LoadBgm starts, with the main window: streams the wave music
// each time notifyEvent is signalled, until WM_QUIT or the music is gone.
// The DX8 SDK's StreamData sample's NotificationProc, looping always.
DWORD WINAPI BgmNotificationProc(LPVOID lpParameter);

// Each sound effect's sound buffer and volume.
struct SoundInfo
{
    int buffer;
    short volume;
    short unk_6;
};
extern SoundInfo g_SoundInfos[0x20];
// The sound files LoadSounds loads, by index.
extern char *g_SoundPaths[];

// DatArchive is in archive.h.
