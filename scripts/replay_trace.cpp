// th06's replay tracer: plays one replay in an unmodified game and
// writes a trace of its state, one record per calculation
// (tools/replay/trace.h has the format and the runtime it uses).
//
// tools/replay/replay.py adds this DLL to a copy of an executable's
// import table, so it loads before the game starts, and tells it where
// the game's code and data are (TRACE_ADDR, from symbols.csv or the
// build's map file); it uses the project's headers only for the layouts
// of the game's structures. DllMain:
//
// - replaces the update callback SetUpTitleScene gives the main menu
//   with one that starts the replay as the demo does, once the title
//   screen has waited a second for a key;
// - redirects the call to Chain::RunCalcChain in GameWindow::RunFrame,
//   to stop the game after it returned to the title, and to give up
//   after a limit;
// - replaces the callbacks ReplayManager::RegisterChain installs while
//   playing (PlayOnUpdate, PlayOnUpdate2, PlayAddedCallback and
//   DeletedCallback) with wrappers that call them and record the state;
// - installs the runtime's clock (a replay sets the game speed to 1, so
//   no game logic reads it), its null renderer, and its crash handler.
//
// Besides the runtime's settings, from the environment:
//
//   TRACE_REPLAY  the replay's path, from the game's directory
//   TRACE_START   stage,difficulty,shot,lives,bombs as the replay menu
//                 sets them
//   TRACE_TAIL    calculations to run after the game is back on the
//                 title (default 60)
//   TRACE_LIMIT   passes of the main loop into the update chain after
//                 which it gives up (dialogue going fast makes several
//                 calculations a pass)
//
// A calc record is taken after PlayOnUpdate, in the middle of a
// calculation (update chain priority 5): its game state is how the
// calculation before ended, with this one's input. `calcs` is
// App::frames, which counts every pass of the update chain, and
// `fastForwards` counts PlayOnUpdate2 starting the chain over during
// dialogue.

// cflags: -/GS
// No CRT: everything below runs on kernel32.

#include <windows.h>

#include "trace.h"

#include "bullet.h"
#include "enemy.h"
#include "gamemanager.h"
#include "item.h"
#include "mainmenu.h"
#include "player.h"
#include "replay.h"
#include "unsorted.h"

extern "C" int _fltused = 0;

#define FIELDS(X)                                                                                                      \
    X(kind)                                                                                                            \
    X(seq)                                                                                                             \
    X(calcs)                                                                                                           \
    X(passes)                                                                                                          \
    X(fastForwards)                                                                                                    \
    X(replayFrame)                                                                                                     \
    X(entry)                                                                                                           \
    X(entryFrame)                                                                                                      \
    X(nextEntryFrame)                                                                                                  \
    X(stage)                                                                                                           \
    X(stageFrames)                                                                                                     \
    X(flags)                                                                                                           \
    X(scene)                                                                                                           \
    X(score)                                                                                                           \
    X(guiScore)                                                                                                        \
    X(lives)                                                                                                           \
    X(bombs)                                                                                                           \
    X(power)                                                                                                           \
    X(graze)                                                                                                           \
    X(grazeTotal)                                                                                                      \
    X(pointItems)                                                                                                      \
    X(pointItemsTotal)                                                                                                 \
    X(rank)                                                                                                            \
    X(subRank)                                                                                                         \
    X(powerItemCombo)                                                                                                  \
    X(nextExtend)                                                                                                      \
    X(deaths)                                                                                                          \
    X(bombsUsed)                                                                                                       \
    X(spellCardsCaptured)                                                                                              \
    X(progressFrames)                                                                                                  \
    X(rngSeed)                                                                                                         \
    X(rngGenerated)                                                                                                    \
    X(buttons)                                                                                                         \
    X(playerX)                                                                                                         \
    X(playerY)                                                                                                         \
    X(playerState)                                                                                                     \
    X(bulletCount)                                                                                                     \
    X(enemyCount)                                                                                                      \
    X(itemCount)                                                                                                       \
    X(spellCard)                                                                                                       \
    X(spellCardBonus)                                                                                                  \
    X(bulletHash)                                                                                                      \
    X(laserHash)                                                                                                       \
    X(enemyHash)                                                                                                       \
    X(itemHash)                                                                                                        \
    X(shotHash)

#define FIELD_MEMBER(name) DWORD name;
#define FIELD_NAME(name) #name "\n"

struct Record
{
    FIELDS(FIELD_MEMBER)
};

static const char s_fieldNames[] = FIELDS(FIELD_NAME);

// What replay.py passes in TRACE_ADDR, by name.
#define ADDRS(X)                                                                                                       \
    X(RunFrame)                                                                                                        \
    X(RunFrameSize)                                                                                                    \
    X(RunCalcChain)                                                                                                    \
    X(SetUpTitleScene)                                                                                                 \
    X(SetUpTitleSceneSize)                                                                                             \
    X(MenuOnUpdate)                                                                                                    \
    X(RegisterChain)                                                                                                   \
    X(RegisterChainSize)                                                                                               \
    X(PlayOnUpdate)                                                                                                    \
    X(PlayOnUpdate2)                                                                                                   \
    X(PlayAddedCallback)                                                                                               \
    X(DeletedCallback)                                                                                                 \
    X(GameManager)                                                                                                     \
    X(App)                                                                                                             \
    X(Random)                                                                                                          \
    X(Buttons)                                                                                                         \
    X(Player)                                                                                                          \
    X(BulletManager)                                                                                                   \
    X(EnemyManager)                                                                                                    \
    X(ItemManager)

#define ADDR_ENUM(name) A_##name,
#define ADDR_NAME(name) #name,

enum
{
    ADDRS(ADDR_ENUM) NUM_ADDRS
};

static const char *s_addrNames[] = {ADDRS(ADDR_NAME)};
static DWORD s_addrs[NUM_ADDRS];

#define GM ((GameManager *)s_addrs[A_GameManager])
#define APP ((App *)s_addrs[A_App])
#define RNG ((Rng *)s_addrs[A_Random])
#define BUTTONS (*(WORD *)s_addrs[A_Buttons])
#define PLAYER ((Player *)s_addrs[A_Player])
#define BULLETS ((BulletManager *)s_addrs[A_BulletManager])
#define ENEMIES ((EnemyManager *)s_addrs[A_EnemyManager])
#define ITEMS ((ItemManager *)s_addrs[A_ItemManager])

typedef int(__cdecl *ReplayCallback)(ReplayManager *replay);
typedef int(__cdecl *MenuCallback)(MainMenu *menu);

// Passes of the main loop into the update chain since the start, and
// the settings.
static DWORD s_seq;
static DWORD s_passes;
static DWORD s_tail;
static DWORD s_limit;
static char s_rpy[MAX_PATH];
static int s_start[5];

// 0 waiting for the title, 1 playing, 2 back on the title after the
// replay; the calculations the title was up for.
static int s_phase;
static DWORD s_titlePasses;
static DWORD s_fastForwards;
// A crash's code and address.
static DWORD s_crashCode;
static DWORD s_crashAddress;

// The stage PlayAddedCallback last started, whose entries `entry`
// counts.
static int s_stage;

static DWORD Bits(float f)
{
    return *(DWORD *)&f;
}

// FNV-1a over the bytes of `v`.
static DWORD Mix(DWORD h, DWORD v)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        h = (h ^ (v & 0xff)) * 16777619;
        v >>= 8;
    }
    return h;
}

static DWORD MixVector(DWORD h, D3DXVECTOR3 *v)
{
    return Mix(Mix(Mix(h, Bits(v->x)), Bits(v->y)), Bits(v->z));
}

static DWORD BulletHash()
{
    DWORD h = 2166136261u;
    int i;
    Bullet *b;

    for (i = 0; i < (int)ARRAY_SIZE(BULLETS->bullets); i++)
    {
        b = &BULLETS->bullets[i];
        if (b->state == 0)
        {
            continue;
        }
        h = Mix(h, i);
        h = Mix(h, b->state | b->flags << 16);
        h = MixVector(h, &b->pos);
        h = MixVector(h, &b->velocity);
        h = Mix(h, Bits(b->speed));
        h = Mix(h, Bits(b->angle));
        h = Mix(h, b->timer.current);
        h = Mix(h, b->grazed);
    }
    return h;
}

static DWORD LaserHash()
{
    DWORD h = 2166136261u;
    int i;
    Laser *l;

    for (i = 0; i < (int)ARRAY_SIZE(BULLETS->lasers); i++)
    {
        l = &BULLETS->lasers[i];
        if (!l->active)
        {
            continue;
        }
        h = Mix(h, i);
        h = Mix(h, l->state);
        h = MixVector(h, &l->pos);
        h = Mix(h, Bits(l->angle));
        h = Mix(h, Bits(l->startOffset));
        h = Mix(h, Bits(l->endOffset));
        h = Mix(h, l->timer.current);
    }
    return h;
}

static DWORD EnemyHash()
{
    DWORD h = 2166136261u;
    int i;
    Enemy *e;

    for (i = 0; i < (int)ARRAY_SIZE(ENEMIES->enemies); i++)
    {
        e = &ENEMIES->enemies[i];
        if (!e->flags.active)
        {
            continue;
        }
        h = Mix(h, i);
        h = MixVector(h, &e->pos);
        h = Mix(h, e->life);
    }
    return h;
}

static DWORD ItemHash()
{
    DWORD h = 2166136261u;
    int i;
    Item *it;

    for (i = 0; i < (int)ARRAY_SIZE(ITEMS->items); i++)
    {
        it = &ITEMS->items[i];
        if (!it->active)
        {
            continue;
        }
        h = Mix(h, i);
        h = Mix(h, it->type | it->state << 8);
        h = MixVector(h, &it->pos);
    }
    return h;
}

static DWORD ShotHash()
{
    DWORD h = 2166136261u;
    int i;
    PlayerBullet *b;

    for (i = 0; i < (int)ARRAY_SIZE(PLAYER->bullets); i++)
    {
        b = &PLAYER->bullets[i];
        if (b->state == 0)
        {
            continue;
        }
        h = Mix(h, i);
        h = Mix(h, b->state | b->damage << 16);
        h = MixVector(h, &b->pos);
    }
    return h;
}

static void Emit(int kind, ReplayManager *replay)
{
    Record r;
    ReplayStage *stage;

    ZeroMemory(&r, sizeof(r));
    r.kind = kind;
    r.seq = s_seq++;
    r.calcs = APP->frames;
    r.passes = s_passes;
    r.fastForwards = s_fastForwards;
    if (kind == TRACE_KIND_CRASH)
    {
        r.passes = s_crashCode;
        r.calcs = s_crashAddress;
    }
    if (replay != NULL)
    {
        r.replayFrame = replay->frame;
        stage = replay->data != NULL && s_stage > 0 && s_stage < 8 ? replay->data->stages[s_stage] : NULL;
        if (stage != NULL && replay->cursors[0] != NULL)
        {
            r.entry = replay->cursors[0] - stage->frames;
            r.entryFrame = replay->cursors[0]->frame;
            r.nextEntryFrame = (replay->cursors[0] + 1)->frame;
        }
    }
    r.stage = GM->stage;
    r.stageFrames = GM->stageFrames;
    r.flags = GM->isPlaying | GM->isPaused << 8 | GM->isInContinueMenu << 16 | (BYTE)GM->isFrozen << 24;
    r.scene = (WORD)APP->scene | APP->curScene << 16;
    r.score = GM->score;
    r.guiScore = GM->guiScore;
    r.lives = GM->lives;
    r.bombs = GM->bombs;
    r.power = GM->power;
    r.graze = GM->graze;
    r.grazeTotal = GM->grazeTotal;
    r.pointItems = GM->pointItems;
    r.pointItemsTotal = GM->pointItemsTotal;
    r.rank = GM->rank;
    r.subRank = GM->subRank;
    r.powerItemCombo = GM->powerItemCombo;
    r.nextExtend = GM->nextExtend;
    r.deaths = GM->deaths;
    r.bombsUsed = GM->bombsUsed;
    r.spellCardsCaptured = GM->spellCardsCaptured;
    r.progressFrames = GM->progressFrames;
    r.rngSeed = RNG->seed;
    r.rngGenerated = RNG->generated;
    r.buttons = BUTTONS;
    r.playerX = Bits(PLAYER->position.x);
    r.playerY = Bits(PLAYER->position.y);
    r.playerState = (BYTE)PLAYER->playerState;
    r.bulletCount = BULLETS->bulletCount;
    r.enemyCount = ENEMIES->enemyCount;
    r.itemCount = ITEMS->activeCount;
    r.spellCard = (BYTE)ENEMIES->spellCardActive | (BYTE)ENEMIES->spellCardCapture << 8 |
                  (WORD)ENEMIES->spellCardIndex << 16;
    r.spellCardBonus = ENEMIES->spellCardBonus;
    r.bulletHash = BulletHash();
    r.laserHash = LaserHash();
    r.enemyHash = EnemyHash();
    r.itemHash = ItemHash();
    r.shotHash = ShotHash();
    TraceWrite(&r, sizeof(r));
    if (kind != TRACE_KIND_CALC)
    {
        TraceFlush();
    }
}

static void Finish(int kind, UINT code)
{
    Emit(kind, NULL);
    TraceExit(code);
}

static int __cdecl PlayOnUpdate(ReplayManager *replay)
{
    int res = ((ReplayCallback)s_addrs[A_PlayOnUpdate])(replay);

    Emit(TRACE_KIND_CALC, replay);
    return res;
}

static int __cdecl PlayOnUpdate2(ReplayManager *replay)
{
    int res = ((ReplayCallback)s_addrs[A_PlayOnUpdate2])(replay);

    // 6 starts the update chain over: dialogue going three times as fast.
    if (res == 6)
    {
        s_fastForwards++;
    }
    return res;
}

static int __cdecl PlayAddedCallback(ReplayManager *replay)
{
    int res;

    if (replay->mode == 1 && replay->data != NULL)
    {
        Emit(TRACE_KIND_STAGE_END, replay);
    }
    res = ((ReplayCallback)s_addrs[A_PlayAddedCallback])(replay);
    s_stage = GM->stage;
    Emit(TRACE_KIND_STAGE_START, replay);
    return res;
}

static int __cdecl DeletedCallback(ReplayManager *replay)
{
    if (replay->mode == 1 && s_phase == 1)
    {
        Emit(TRACE_KIND_REPLAY_END, replay);
        s_phase = 2;
        s_titlePasses = 0;
    }
    return ((ReplayCallback)s_addrs[A_DeletedCallback])(replay);
}

// Starts the replay with the settings the replay menu takes from it
// (MainMenu::UpdateReplayMenu). The menu's update callback then returns
// 0, which removes the menu, as for the demo.
static void StartReplay()
{
    int i;

    GM->isInReplay = 1;
    APP->gameSpeed = 1.0f;
    for (i = 0; s_rpy[i] != '\0' && i < sizeof(GM->replayFile) - 1; i++)
    {
        GM->replayFile[i] = s_rpy[i];
    }
    GM->replayFile[i] = '\0';
    GM->stage = s_start[0];
    GM->difficulty = s_start[1];
    GM->character = s_start[2] / 2;
    GM->shotType = s_start[2] % 2;
    GM->lives = s_start[3];
    GM->bombs = s_start[4];
    APP->scene = 2;
}

// Replaces MainMenu::OnUpdate. States 1 and 2 wait for a key on the
// title screen and start the demo after 720 idle frames; a second in, the
// replay starts the same way.
static int __cdecl MenuOnUpdate(MainMenu *menu)
{
    if (s_phase == 0 && (menu->state == 1 || menu->state == 2) && ++s_titlePasses >= 60)
    {
        StartReplay();
        s_phase = 1;
        return 0;
    }
    return ((MenuCallback)s_addrs[A_MenuOnUpdate])(menu);
}

static void __cdecl BeforeCalc()
{
    s_passes++;
    if (s_passes >= s_limit)
    {
        Finish(TRACE_KIND_LIMIT, 3);
    }
    if (s_phase == 2 && APP->curScene == 1 && APP->scene == 1 && ++s_titlePasses >= s_tail)
    {
        Finish(TRACE_KIND_TITLE, 0);
    }
}

static void Crashed(DWORD code, DWORD address)
{
    s_crashCode = code;
    s_crashAddress = address;
    Finish(TRACE_KIND_CRASH, 4);
}

static DWORD s_runCalcChain;

// Replaces `call Chain::RunCalcChain` (thiscall, `this` in ecx).
static __declspec(naked) void RunCalcChainHook()
{
    __asm
    {
        push ecx
        call BeforeCalc
        mov ecx, [esp]
        call dword ptr [s_runCalcChain]
        pop ecx
        ret
    }
}

// Patches the code and checks each patch found what it expected.
static BOOL Patch()
{
    DWORD reg = s_addrs[A_RegisterChain];
    DWORD regSize = s_addrs[A_RegisterChainSize];
    int n;
    int calls;

    s_runCalcChain = s_addrs[A_RunCalcChain];
    n = TracePatchCalls(s_addrs[A_RunFrame], s_addrs[A_RunFrameSize], s_runCalcChain, (DWORD)RunCalcChainHook);
    TraceInfo("RunCalcChain=%d", n);
    if (n != 1)
    {
        return FALSE;
    }
    n = TracePatchImmediates(s_addrs[A_SetUpTitleScene], s_addrs[A_SetUpTitleSceneSize], s_addrs[A_MenuOnUpdate],
                             (DWORD)MenuOnUpdate);
    TraceInfo(" MenuOnUpdate=%d", n);
    if (n != 1)
    {
        return FALSE;
    }
    n = TracePatchImmediates(reg, regSize, s_addrs[A_PlayOnUpdate], (DWORD)PlayOnUpdate);
    TraceInfo(" PlayOnUpdate=%d", n);
    if (n != 1)
    {
        return FALSE;
    }
    n = TracePatchImmediates(reg, regSize, s_addrs[A_PlayOnUpdate2], (DWORD)PlayOnUpdate2);
    TraceInfo(" PlayOnUpdate2=%d", n);
    if (n != 1)
    {
        return FALSE;
    }
    n = TracePatchImmediates(reg, regSize, s_addrs[A_PlayAddedCallback], (DWORD)PlayAddedCallback);
    calls = TracePatchCalls(reg, regSize, s_addrs[A_PlayAddedCallback], (DWORD)PlayAddedCallback);
    TraceInfo(" PlayAddedCallback=%d+%d", n, calls);
    if (n != 1 || calls < 1)
    {
        return FALSE;
    }
    n = TracePatchImmediates(reg, regSize, s_addrs[A_DeletedCallback], (DWORD)DeletedCallback);
    TraceInfo(" DeletedCallback=%d", n);
    return n >= 1;
}

static BOOL Install()
{
    char path[MAX_PATH];
    char start[64];
    const char *p;
    int i;

    if (!TraceReadAddrs(s_addrNames, s_addrs, NUM_ADDRS) || !TraceEnvString("TRACE_REPLAY", s_rpy, sizeof(s_rpy)) ||
        !TraceEnvString("TRACE_START", start, sizeof(start)) || !TraceEnvString("TRACE_OUT", path, sizeof(path)))
    {
        OutputDebugStringA("th06trace: settings missing\n");
        return FALSE;
    }
    for (p = start, i = 0; i < 5; i++)
    {
        s_start[i] = TraceNextNumber(&p);
    }
    s_tail = TraceEnvNumber("TRACE_TAIL", 60);
    s_limit = TraceEnvNumber("TRACE_LIMIT", 2000000);
    if (!TraceOpen(path))
    {
        OutputDebugStringA("th06trace: cannot create the trace\n");
        return FALSE;
    }
    if (!Patch() || !TraceInstallClock() || !TraceInstallRenderer())
    {
        TraceInfo(" failed");
        return FALSE;
    }
    TraceOnCrash(Crashed);
    return TRUE;
}

extern "C" __declspec(dllexport) void Init()
{
}

extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    BOOL ok;

    if (reason == DLL_PROCESS_DETACH)
    {
        TraceFlush();
        return TRUE;
    }
    if (reason != DLL_PROCESS_ATTACH)
    {
        return TRUE;
    }
    ok = Install();
    TraceWriteHeader(s_fieldNames, sizeof(Record) / 4);
    return ok;
}
