// The game in play: score, lives, stage.

#pragma once

#include <windows.h>

#include "chain.h"
#include "common.h"
#include "resultscreen.h"

// The 3D view of the stage, set up by SetupCamera.
struct Camera
{
    // The camera's distance from the screen plane, set by SetupCamera.
    float distance;
    // Moves the point looked at from the center of the viewport.
    float offsetX;
    float offsetY;
    // Scales the camera's distance.
    float zoom;
};

// Runs the game in play, the global at 0x69bca0. Its added callback
// reads "score.dat" and sets up the replay ("replay/th6_00.rpy").
class GameManager
{
public:
    // Zeroes the manager and sets the play area to (32, 16), 384 by 448.
    GameManager();

    // Whether a `width` by `height` rectangle centred on (x, y) touches
    // the play area.
    BOOL IsInPlayArea(float x, float y, float width, float height);
    // Inline; `points` is an int and `score` unsigned, so even a
    // constant goes through a temporary.
    void AddScore(int points)
    {
        this->score += points;
    }
    // Whether shot `shot` of `character` cleared the game on normal,
    // hard or lunatic (clrds' 99), which opens the extra stage.
    BOOL IsExtraUnlocked(int character, int shot);
    // Adds `amount` hundredths to the rank, up to maxRank.
    void AddSubRank(int amount);
    // Takes `amount` hundredths from the rank, down to minRank.
    void SubSubRank(int amount);

    // Adds the update element (priority 4) and the draw element
    // (priority 2). Returns 0, or -1 if adding fails.
    static int RegisterChain();
    // The update element's callback: ends a demo on a key press or after
    // 3720 frames, opens the pause menu, sets up the viewport and the
    // camera, counts the shown score up to the score, and gives a life
    // at each of g_ExtendScores. Returns 3 while paused, else 1.
    static int OnUpdate(GameManager *mgr);
    // The update element's added callback: for a new game (scene not 3)
    // resets the play area, the score, the rank and the spell card
    // chunks and loads "score.dat"; then goes to the next stage, records
    // its progress in clrds, loads the archives, starts the replay and
    // adds the game's elements (stage, player, bullets, enemies, effects,
    // interface) and the stage's music. Returns 0, or -1 after logging
    // which part failed.
    static int AddedCallback(GameManager *mgr);
    // Stops the music unless isInDemo is set, removes the game's chain
    // elements and resets the ASCII text. Returns 0.
    static int DeletedCallback(GameManager *mgr);
    // Removes the two chain elements.
    static void RemoveChain();
    // The draw element's callback: sets isPaused to 2 while it is set.
    // Returns 1.
    static int OnDraw(GameManager *mgr);

    // The score shown, and the score, which it counts up to.
    unsigned int guiScore;
    unsigned int score;
    // How far guiScore moves towards score a frame.
    unsigned int scoreIncrement;
    // The high score shown.
    unsigned int highScore;
    int difficulty;
    // Grazes, up to 9999 and 999999.
    int graze;
    int grazeTotal;
    // Set while a replay or the demo plays (1 plays replayFile); no
    // replay is recorded and nothing is saved.
    DWORD isInReplay;
    // Counts the player's deaths.
    int deaths;
    // Counts the bombs used.
    int bombsUsed;
    // Spell cards captured.
    int spellCardsCaptured;
    // Set by an enemy script (with an effect): the interface, the bullets,
    // the player, the stage and the enemies' timers stop updating while
    // it is set.
    char isFrozen;
    BYTE unk_002d[0x3];
    // The score file's spell card, clear and practice chunks
    // (SaveScoreDat).
    Catk catks[0x40];
    Clrd clrds[4];
    // By shot, stage and difficulty.
    Pscr pscrs[4][6][4];
    // What FireShot compares the shot tables' levels with.
    WORD power;
    WORD unk_1812;
    // Point items collected.
    WORD pointItems;
    WORD pointItemsTotal;
    // Continues used; a replay of a continued game cannot be saved.
    BYTE continuesUsed;
    // Power items collected in a row at full power, up to 30.
    char powerItemCombo;
    // Lives and bombs left.
    char lives;
    char bombs;
    // The next of g_ExtendScores to reach; -1 once there are no more.
    char nextExtend;
    // The replay header keeps character * 2 + shotType.
    BYTE character;
    BYTE shotType;
    // The pause menu is open (OnDraw makes 1 into 2).
    BYTE isPaused;
    // The continue menu is open, after the last life is lost.
    BYTE isInContinueMenu;
    // Neither of the two menus is open: the replay records and DrawFps
    // measures how far the frame rate falls short only then.
    BYTE isPlaying;
    // Set once the game is cleared (the ending starts, or the extra stage
    // ends): the score entry gets no stage.
    BYTE isGameCleared;
    // A practice game: no continues, and its own scores.
    BYTE isInPractice;
    // The title screen's demo plays (data/demo/demo00.rpy).
    BYTE isInDemo;
    BYTE unk_1825[0x3];
    // Counts a demo's frames: it fades out at 3600 and ends at 3720.
    int demoFrames;
    // The replay file to play (ReplayManager::RegisterChain mode 1).
    char replayFile[0x200];
    // g_Rng's seed when the stage started; replays keep it.
    WORD rngSeed;
    WORD unk_1a2e;
    // Counts OnUpdate's calls since RegisterChain.
    int stageFrames;
    // The stage being played: 1 to 6, 7 for the extra stage.
    int stage;
    // The stage last picked in practice.
    int lastPracticeStage;
    // The play area: x, y, width and height.
    float arena[4];
    // Where the player may move: x, y, width and height.
    float playerArea[4];
    Camera camera;
    // Counts the frames the stages' enemy timelines ran outside boss
    // fights; the result screen shows it as a share of the game.
    int progressFrames;
    // A level from minRank to maxRank, moved in hundredths through
    // subRank by AddSubRank and SubSubRank.
    int rank;
    int maxRank;
    int minRank;
    int subRank;
};
ASSERT_SIZE(GameManager, 0x1a80);
extern GameManager g_GameManager;
// The scores giving an extra life, in order (0x4764b0).
extern unsigned int g_ExtendScores[];

// The rank a game starts at and its bounds, by difficulty.
struct RankSettings
{
    int rank;
    int minRank;
    int maxRank;
};
// Replays keep the rank they recorded but take these bounds (0x476528).
extern RankSettings g_ReplayRanks[5];
extern RankSettings g_Ranks[5];

// Each stage's enemy sprites (EnemyManager::RegisterChain's paths;
// stage 0 has "dummy" ones) and enemy script ("data/ecldata1.ecl", ...).
struct EnemyAnmPaths
{
    char *anmPath;
    char *anmPath2;
};
extern EnemyAnmPaths g_EnemyAnmPaths[8];
extern char *g_EclPaths[8];

// The 3D camera the stage backgrounds are seen through (0x69d6fc).

// Looks at the viewport's center from a distance where a 30° field of
// view covers its height, and sets the view and projection matrices,
// with the far plane at 10000 + `extraFar`.
void SetupCamera(float extraFar);

// GameManager's statically allocated chain elements.
extern ChainElem g_GameManagerCalcChain;
extern ChainElem g_GameManagerDrawChain;
