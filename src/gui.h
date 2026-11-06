// The front: the interface drawn over the game and the dialogue.
// Included by unsorted.h where these declarations were; it relies on what
// unsorted.h declares before it.

#pragma once

// A loaded message file ("data/msg1.dat", ...): a count, then the
// offsets of its scripts, made pointers once loaded.
struct MsgFile
{
    int count;
    void *scripts[1];
};

// The arguments of most message instructions: who speaks (0 the
// player, 1 the boss; 2 and 3 select the text lines for opcode 5) and a
// sprite, script or line number, then the text.
struct MsgArgs
{
    short who;
    union
    {
        short index;
        BYTE value;
    };
    char text[1];
};

// One instruction of a message script.
struct MsgInstr
{
    // The frame it runs on.
    WORD time;
    BYTE opcode;
    // The size of the arguments, in bytes.
    BYTE argsSize;
    union
    {
        MsgArgs args;
        int i;
        BYTE b;
    };
};

// The dialogue Gui runs (GuiImpl+0x2534, 0x6a8 bytes). Its implicit
// constructor is emitted out of line (0x41b4c4), called from GuiImpl's.
struct GuiMsg
{
    // The message file loaded.
    MsgFile *msgFile;
    // The next instruction of the running script.
    MsgInstr *instr;
    // The running script; negative when none runs.
    int scriptIndex;
    Timer timer;
    // Counts the frames since the last line of text showed, for the wait
    // instruction.
    int waitFrames;
    // The player's and the boss's portraits.
    Sprite portraits[2];
    // The two lines of text.
    Sprite text[2];
    // The nameplates opcode 8 shows, right-aligned (scripts 0x704 and 0x705).
    Sprite nameplates[2];
    // The text colors and their shadows, by speaker.
    D3DCOLOR textColors[4];
    D3DCOLOR shadowColors[4];
    // The text's height in pixels (the text sprites' font size).
    int fontSize;
    // Raised by opcode 6, counted down every frame: while it is above 0
    // the enemy timeline's wait for the dialogue (MsgHoldsTimeline) ends.
    DWORD timelineRelease;
    // Lets the skip button (0x100) fast-forward the script; set by
    // opcode 13.
    BYTE skippable;
    BYTE pad_6a5[0x3];
};
ASSERT_SIZE(GuiMsg, 0x6a8);

// A message Gui shows for a while at a fixed place, with a number.
struct GuiPopup
{
    D3DXVECTOR3 pos;
    int value;
    int shown;
    Timer timer;
};

// The bulk of Gui, allocated by Gui::RegisterChain (0x2c44 bytes). Its
// implicit constructor is emitted out of line at 0x41b34b.
struct GuiImpl
{
    // Starts script `index` of msg.msgFile, if there is one: resets
    // the dialogue's state and the text's colors, and loads the effects
    // of stage 6 and the extra stage for their scripts 0 and 10.
    void StartMsgScript(int index);
    // Runs the dialogue for a frame: the instructions due (portraits,
    // text, waits for the shot button, music, the stage's end), then
    // steps its sprites. Returns 0, or -1 when no script runs.
    int RunMsg();
    // Draws the dialogue: the portraits, a dark box under the text that
    // opens over its first second, and the text. Returns 0, or -1 when no
    // script runs.
    int DrawMsg();

    // The interface's sprites (scripts from 0x600 on).
    Sprite sprites[0x1a];
    // The boss's bar (sprites[0x13]): 0 hidden, 1 appearing (script
    // 0x613), 2 shown, 3 leaving (script 0x614).
    BYTE bossBarState;
    BYTE pad_1ba1[0x3];
    // The stage's name and the song's (script 0x700 and 0x701).
    Sprite stageName;
    Sprite songName;
    // ShowBombName's portrait; ShowSpellCardName's.
    Sprite bombPortrait;
    Sprite spellPortrait;
    // The bomb's and the spell card's names, and the bars behind them.
    Sprite bombName;
    Sprite spellName;
    Sprite bombNameBar;
    Sprite spellNameBar;
    // The loading screen ("data/loading.anm", script 0x619), between
    // stages; spriteIndex is -1 when it is not shown.
    Sprite loading;
    GuiMsg msg;
    // Set by the dialogue's opcode 9: OnDraw then shows the stage clear
    // bonus, stageClearTotal.
    DWORD stageClearShown;
    DWORD stageClearTotal;
    // The bonus for bullets cleared, the full power message and the spell
    // card's bonus.
    GuiPopup bonusPopup;
    GuiPopup fullPowerPopup;
    GuiPopup spellBonusPopup;
};
ASSERT_SIZE(GuiImpl, 0x2c44);

// The in-game interface, the global at 0x69bc30: it loads
// "data/front.anm" and the dialogue portraits ("data/face00a.anm", ...)
// and draws the stage clear bonus ("Stage Clear", "Graze *   10 = %5d").
// Gui's flags, a bitfield. The player and the items set these to 2 when
// the lives, the bombs, the power, the graze and the point items
// change.
struct GuiFlags
{
    unsigned int livesChanged : 2;
    unsigned int bombsChanged : 2;
    unsigned int powerChanged : 2;
    unsigned int grazeChanged : 2;
    unsigned int pointsChanged : 2;
    unsigned int pad_10 : 22;
};

class Gui
{
public:
    // Loads the interface's sprites ("data/front.anm"), the character's
    // and the stage's portraits and the stage's message file (all but
    // the stage's kept when the scene is 3), then starts the interface's
    // scripts and shows the stage and song names. Returns 0, or -1 if a
    // file fails to load.
    int LoadResources();

    // The update element's callback: unless g_GameManager.isFrozen is
    // set, runs UpdateInterface and impl->RunMsg. Returns 1.
    static int OnUpdate(Gui *gui);
    // The draw element's callback: the stage clear bonus once
    // impl->stageClearShown is set, the dialogue, DrawBanners, DrawHud and the
    // popups, with the depth test off. Returns 1.
    static int OnDraw(Gui *gui);
    // Draws the stage's title ("STAGE %d", "FINAL STAGE", "EXTRA STAGE",
    // or " DEMO PLAY"), the song's name, the sprites ShowSpellCardName
    // and ShowBombName show with their bars, and `loading` in the play
    // area.
    void DrawBanners();
    // Draws the interface: the boss's bar, bossLives and timer
    // (ticking under 10 seconds), the frame around the play area when
    // it changed, and the score, the high score, lives, bombs, power
    // (a bar), graze and point items, each while its flag counts down.
    void DrawHud();
    // The update element's added callback (the element at 0x69bc7c):
    // runs LoadResources.
    static int AddedCallback(Gui *gui);
    // Removes the update element and the draw element.
    static void RemoveChain();
    // Unless the scene is 3, zeroes the interface and allocates impl;
    // then adds the update element (priority 12) and the draw element
    // (priority 11). Returns 0, or -1 if adding fails.
    static int RegisterChain();
    // Frees the stage's portraits and message file and, unless the
    // scene is 3, the interface's sprites and impl. Returns 0.
    static int DeletedCallback(Gui *gui);

    // Runs impl->StartMsgScript(`index`) and sets
    // g_App.redrawFrames to 3.
    void StartMsg(int index);
    // Whether impl->loading shows a sprite and has stopped (flag 13): the
    // loading screen is fully up, and the stage is not drawn.
    BOOL IsLoadingShown();
    // Shows the player's bomb, spell card `name`, with sprite `sprite`
    // (scripts 0x4a1 and 0x706), sets g_App.redrawFrames to 3 and
    // plays sound 14.
    void ShowBombName(int sprite, char *name);
    // Shows a boss's spell card, `text`, with sprite `sprite` + 0x4a8
    // (scripts 0x4a3 and 0x707), from enemy scripts, and plays sound 14.
    void ShowSpellCardName(int sprite, char *text);
    // Sets impl->bombName.pendingInterrupt, as the bomb ends.
    void EndBombName();
    // Shows fullPowerPopup, when the power reaches its maximum.
    void ShowFullPowerPopup(int unk);
    // Shows `value` in spellBonusPopup, the bonus of a spell card
    // captured, from enemy scripts.
    void ShowSpellCardBonus(int value);
    // Shows `bonus` in bonusPopup, the bonus for bullets cleared.
    void ShowBonusPopup(int bonus);
    // Sets impl->spellName.pendingInterrupt, as the spell card ends.
    void EndSpellCardName();
    // Returns impl->msg.skippable.
    BOOL IsMsgSkippable();
    // Whether a dialogue script runs and impl->msg.timelineRelease is 0:
    // the enemy timeline waits for the dialogue then.
    BOOL MsgHoldsTimeline();
    // Whether a dialogue script runs.
    BOOL IsMsgRunning();
    // Loads the message file at `path` into impl->msgFile, turning its
    // offsets into pointers. Returns 0, or -1 after logging
    // "メッセージファイル %s が読み込めませんでした".
    int LoadMsg(char *path);
    // Frees impl->msgFile, if there is one.
    void FreeMsgFile();
    BYTE IsBossPresent()
    {
        return this->bossPresent;
    }
    void SetBossTimer(int seconds)
    {
        this->bossTimer = seconds;
    }
    // Sets the boss's life bar, as a fraction of its maximum.
    int GetBossTimer()
    {
        return this->bossTimer;
    }
    void SetBossLives(int value)
    {
        this->bossLives = value;
    }
    void SetBossHealth(float health)
    {
        this->bossHealth = health;
    }
    // The update: the interface's sprites, the boss's bar, the popups,
    // and once the stage is cleared, the bonus (stage, graze, power and
    // point items, by difficulty and starting lives), added to the score.
    void UpdateInterface();

    GuiFlags flags;
    GuiImpl *impl;
    // The widths of the texts ShowBombName and ShowSpellCardName show,
    // from their lengths; DrawBanners fits the bars to them.
    float bombNameWidth;
    float spellNameWidth;
    // The boss bar's alpha, raised by 4 a frame up to 0xff while a boss
    // fight runs.
    DWORD bossBarAlpha;
    // The number shown at the boss bar's left, set by enemy scripts (the
    // boss's remaining bars, as far as can be told).
    int bossLives;
    // The seconds left of the boss's timer.
    int bossTimer;
    // bossTimer when DrawHud last ran: it ticks when the timer changes.
    DWORD lastDrawnBossTimer;
    // Set while a boss fight runs.
    BYTE bossPresent;
    BYTE pad_21[0x3];
    // The boss's life as a fraction of its maximum, for its bar.
    float bossHealth;
    // The life the bar shows, following bossHealth.
    float bossHealthShown;
};
ASSERT_SIZE(Gui, 0x2c);
extern Gui g_Gui;
// The boss timer's colors: 20 seconds and more, 10, 5, and less
// (0x4764a0).
extern D3DCOLOR g_BossTimerColors[4];

// Gui's statically allocated chain elements.
extern ChainElem g_GuiCalcElem;
extern ChainElem g_GuiDrawElem;
