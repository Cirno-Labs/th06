# Replay parity

`tools/replay/replay.py` plays recorded replays from start to finish in
the original executable and in rebuilt ones, records the game's state
at every calculation, and compares the runs. A replay counts as covered
only when every run plays all of it, by itself, to the title screen.
A run that stops early, or that matches the original only up to some
point, does not count.

```sh
just replay                          # every replay in replays/
just replay replays/th6_udNoRA.rpy   # one
python3 tools/replay/replay.py info replays/th6_udNoRA.rpy
python3 tools/replay/replay.py run replays/th6_udNoRA.rpy --exe standalone --speed 1 --show
python3 -m unittest scripts.test_replay        # the parser and the checks, no game
python3 -m unittest discover -s tools/replay   # the shared harness's tests
```

The harness is shared by the games that use decomp-tools. What every
game has in common lives in `tools/replay/`: running the games, the
tracer's runtime (`trace.h`, `trace.c`) and null renderer
(`null_d3d8.c`), the trace format, the checks every replay gets, the
comparison and the evidence. That part holds no game code and no
symbols. What is th06's is here: `scripts/replay_hook.py`
(`replay.hook` in `project.toml`) reads T6RP replays, names what the
tracer patches and reads, writes the run's configuration and checks
each stage's inputs, and `scripts/replay_trace.cpp` is the tracer.

`just replay` runs each replay in the original twice and in the
standalone `build/th06.exe`, three games at a time (`-j`). The two
original runs show the harness itself is deterministic, and the
standalone run is the parity check. `--exe PATH=MAP` adds another build,
such as a feature build with its map file. A run's trace, the game's
directory and `run.json` go to `build/replay/<replay>/<exe>/`, and
`build/replay/<replay>/evidence.json` collects the hashes, checks and
comparisons. `build/` is not tracked, so evidence never gets committed.

## How a run works

`replay.py` copies the executable and adds `th06trace.dll` to the
copy's imports. Nothing else in the executable changes. The DLL, built
from `scripts/replay_trace.cpp` and the runtime, is compiled with the
project's own compiler against the project's headers, which gives it
the layouts of the game's structures. It gets the addresses of
functions and globals at run time (`TRACE_ADDR`): for the original from
`config/symbols.csv`, for a rebuilt executable from that build's own
map file. Each address is resolved by symbol name, so it is never
assumed to be the same across builds.

The game runs under Wine on its own Xvfb display, in a directory of its
own with:

- the game's archives and directories, linked from `orig/`, and copies
  of its other files;
- a fixed configuration: windowed, no frame skip, no music, no sound
  effects, 32-bit colors, the shipped key mapping;
- the replay, as `replay/harness.rpy`.

Wine's audio drivers are disabled for the process
(`WINEDLLOVERRIDES`). DirectSound then fails to start, which the game
allows, and nothing reaches the speakers. (Leaving the driver empty in
the registry instead makes Wine's mmdevapi fail an assertion, and the
game exits.) Wine's helper processes still log an ALSA error or two
while looking for a device.

Games that run at the same time need Wine prefixes of their own. One
wineserver gives all its processes one desktop with one active window,
and the game only runs frames while its window is the active one. It
also holds a named mutex so it can't start twice. `check -j N` makes
N prefixes under `build/replay/wine/` on first use (13 s and 384 MB
each, without Mono or Gecko) and gives each run a free one. None of
this touches the project's `.wine`.

From DllMain the DLL:

- starts the replay from the title screen the way the demo does: it
  replaces the main menu's update callback, and a second into the title
  screen's wait for a key it sets the fields the replay menu sets
  (`MainMenu::UpdateReplayMenu`) and returns 0, which removes the menu.
  The menu's own replay screens are not exercised;
- wraps the callbacks `ReplayManager::RegisterChain` installs for
  playback, by rewriting their addresses in its code, to write records;
- redirects the call to `Chain::RunCalcChain` in `GameWindow::RunFrame`,
  so it can stop the game once it is back on the title after the
  replay, and give up after a limit;
- replaces `Direct3DCreate8` in the import table, unless `--render` is
  given, with the runtime's null renderer (`null_d3d8.c`): devices
  that work as usual, but whose `Present`, `Clear` and four
  `Draw*Primitive*` methods do nothing and succeed. The game still runs
  all of its own code, draw callbacks included (some change state:
  `GameManager::OnDraw` moves a pause along). Only the rasterizing goes,
  which under Xvfb is llvmpipe on the CPU and most of a run's time: a run
  of the Normal replay takes 40 seconds instead of 13 minutes, and its
  trace matched a run that draws, record for record. What the game reads
  back from the screen (the back buffer copy behind its menus) has
  nothing drawn on it, which only shows on screen. Mesa's no-op driver
  (`GALLIUM_NOOP`) would do the same below Wine, but wined3d crashes on
  it, and wined3d's `no3d` renderer gives the game no device;
- replaces `timeGetTime` in the import table. With `--speed 0` the clock
  moves 17 ms every time it is read, so the windowed main loop never
  waits, and every run sees the same times. With `--speed N` it runs N
  times as fast as real time. Playing a replay pins the game speed to 1,
  so the clock doesn't reach game logic either way;
- on a crash, writes a last record and exits instead of waiting in
  Wine's debugger.

## The trace

Each record has 46 32-bit fields. Their names are in the trace's header
(`trace.h` has the format, and the fields every game's trace has), and
`FIELDS` in `replay_trace.cpp` defines them:

- counters: the record's sequence number, `calcs` (`App::frames`, one
  per pass of the update chain, so every calculation), the main loop's
  passes into the update chain, and the dialogue fast-forwards;
- the replay: its frame counter, the input entry it is on, and that
  entry's frame and the next one's;
- the game: stage, stage frames, play state (playing, paused, continue
  menu, frozen), scene, score and shown score, lives, bombs, power,
  graze, point items, rank, the power item combo, the next extend,
  deaths, bombs used, spell cards captured, the progress counter, the
  RNG's seed and count, and the buttons;
- the player's position (float bits) and state;
- the numbers of bullets, enemies and items, and the spell card's
  state and bonus;
- FNV-1a hashes over every active bullet (slot, state, flags, position,
  velocity, speed, angle, timer, grazed), laser, enemy (position, life),
  item and player shot, with floats hashed as their bits.

The kinds of records:

| kind | when |
| --- | --- |
| calc | after `PlayOnUpdate`: one per calculation the replay plays |
| stage end | before `PlayAddedCallback` for each later stage: how the stage before it ended |
| stage start | after `PlayAddedCallback` |
| replay end | before the playback `DeletedCallback`: how the last stage ended |
| title | `--tail` calculations after the game is back on the title |
| limit, crash | the run gave up or crashed |

The calc record is taken in the middle of a calculation: after the
input, the interface and the game manager (update chain priorities 0 to
5), before the stage, the player, the enemies and the rest. Its
game-state fields therefore show how the previous calculation ended.
The stage end and replay end records show how the last calculation of
a stage ended.

During dialogue a replay plays three times as fast:
`ReplayManager::PlayOnUpdate2` makes the update chain start over on
two calculations out of three. Several calculations then happen in one
pass of the main loop. They are still separate records, the replay's
frame counter and `App::frames` count each of them, and `fastForwards`
counts the restarts.

## When a run passes

- The replay ended by itself (the playback `DeletedCallback` ran) and
  the game got back to the title screen. A run that stops on the
  limit, the timeout or a crash fails.
- The stages played are the replay's, in order.
- Every stage ends with the score the replay recorded for it. Each
  stage after the first starts from the recorded score, so a stage that
  went wrong would otherwise go unnoticed after its end.
- Each stage plays exactly its recorded number of frames. Its frame
  counter runs from 1 without a gap, and its last calculation is on the
  replay's last real input entry. While recording,
  `GameManager::DeletedCallback` ends every stage with
  `ReplayManager::EndStage`: an entry with no buttons at the frame
  count, then one at frame 9999999. During playback the same call
  writes those two entries into the loaded replay again, so the stage
  end record has to be on the end marker. If the counts differ, the
  rewrite would also land in the next stage's header.
- `calcs` goes up at every calculation, and the main loop's counter
  never goes back.

All but the input entry checks are `tools/replay/`'s, for every game;
`stage_failures` in the hook adds those.

A comparison passes when both traces have the same records, field for
field, from the first stage's start to the title screen. The trace ends
`--tail` calculations after the title, so the return to the title is
compared too.

## What this does not cover

- Rendering and audio. By default nothing is drawn; with `--render`
  frames are drawn (llvmpipe), but nothing compares them. Audio is off.
- Hooked builds (`build/game/th06.exe` with `th06.dll`): their replaced
  functions live in the DLL, which these hooks don't reach.
- The replay menu, saving replays, and recording new ones.
- Sound effects and music being on, which could change timing but not
  logic.
- The standalone exe is byte-identical to the original for now
  (`just check-exe`), so its parity is expected. The check is there for
  when the code changes, and for feature builds (`--exe PATH=MAP`).

## The replays

`replays/` holds the world record replays from Maribel Hearn's table
(https://maribelhearn.com/wr) that are hosted there; the others link
to videos only. `replay.py fetch` downloads them again and rewrites
`replays/manifest.json`, which lists each replay's source, player, date,
score and SHA-1 ([`replays/README.md`](../replays/README.md)). The
players made the replays, and the repository keeps them for testing
only. The hook's `WR_TABLE` picks the game's table.

## Provenance

The harness was written for this repository. Its requirements came
from a description of N0zoM1z0/th08's replay tests: complete runs,
natural exits, stage order and end scores, input consumption,
continuous counters, dialogue fast-forward, and parity counted over
whole routes rather than a fixed-length prefix. None of their code or
documents were used.
