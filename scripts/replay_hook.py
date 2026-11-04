"""th06's replay hook for tools/replay/replay.py (replay.hook in
project.toml): the T6RP replay format, what the tracer
(scripts/replay_trace.cpp) patches and reads, the configuration a run
uses, and the checks of a stage's recorded inputs.
"""
import os, struct

TRACER = ["scripts/replay_trace.cpp"]
WR_TABLE = "EoSD_table"

FUNCTIONS = {
    "RunFrame": "?RunFrame@GameWindow@@QAEHXZ",
    "RunCalcChain": "?RunCalcChain@Chain@@QAEHXZ",
    "SetUpTitleScene": "?SetUpTitleScene@@YAHH@Z",
    "MenuOnUpdate": "?OnUpdate@MainMenu@@SAHPAV1@@Z",
    "RegisterChain": "?RegisterChain@ReplayManager@@SAHHPAD@Z",
    "PlayOnUpdate": "?PlayOnUpdate@ReplayManager@@SAHPAV1@@Z",
    "PlayOnUpdate2": "?PlayOnUpdate2@ReplayManager@@SAHPAV1@@Z",
    "PlayAddedCallback": "?PlayAddedCallback@ReplayManager@@SAHPAV1@@Z",
    "DeletedCallback": "?DeletedCallback@ReplayManager@@SAHPAV1@@Z",
}
SIZED = ["RunFrame", "SetUpTitleScene", "RegisterChain"]
GLOBALS = {
    "GameManager": "?g_GameManager@@3VGameManager@@A",
    "App": "?g_App@@3VApp@@A",
    "Random": "?g_Random@@3VRng@@A",
    "Buttons": "?g_Buttons@@3GA",
    "Player": "?g_Player@@3VPlayer@@A",
    "BulletManager": "?g_BulletManager@@3VBulletManager@@A",
    "EnemyManager": "?g_EnemyManager@@3VEnemyManager@@A",
    "ItemManager": "?g_ItemManager@@3VItemManager@@A",
}

SHOTS = ["ReimuA", "ReimuB", "MarisaA", "MarisaB"]
DIFFICULTIES = ["Easy", "Normal", "Hard", "Lunatic", "Extra"]
END_FRAME = 9999999
CONFIG = "東方紅魔郷.cfg"
REPLAY_PATH = "replay/harness.rpy"


class Replay:
    """A decrypted T6RP replay (ReplayHeader and ReplayStage in
    src/replay.h)."""

    def __init__(self, path):
        self.path = path
        with open(path, "rb") as f:
            data = bytearray(f.read())
        if data[:4] != b"T6RP":
            raise ValueError(f"{path}: not a th06 replay")
        key = data[0xe]
        for i in range(0xf, len(data)):
            data[i] = (data[i] - key) & 0xff
            key = (key + 7) & 0xff
        self.version, self.shot, self.difficulty, checksum = struct.unpack_from("<HBBi", data, 4)
        if (0x3f000318 + sum(data[0xe:])) & 0xffffffff != checksum & 0xffffffff:
            raise ValueError(f"{path}: bad checksum")
        if self.version != 0x102:
            raise ValueError(f"{path}: version {self.version:#x}, not 1.02")
        self.date = data[0x10:0x19].split(b"\0")[0].decode("ascii", "replace")
        self.name = data[0x19:0x22].split(b"\0")[0].decode("ascii", "replace").strip()
        self.score, self.slowdown = struct.unpack_from("<I4xf", data, 0x24)
        offsets = struct.unpack_from("<8I", data, 0x30)
        # Each stage's entries run to the next stage's data. While
        # recording, GameManager::DeletedCallback ends each stage with
        # ReplayManager::EndStage: an entry with no buttons at the frame
        # count, then one at END_FRAME.
        starts = sorted(o for o in offsets[1:] if o)
        self.stages = {}
        for stage in range(1, 8):
            off = offsets[stage]
            if not off:
                continue
            score, seed, points, power, lives, bombs, rank, combo = struct.unpack_from("<IHHBBBBB", data, off)
            end = next((o for o in starts if o > off), len(data))
            entries = []
            for at in range(off + 0x10, end - 7, 8):
                frame, buttons = struct.unpack_from("<iH", data, at)
                if frame == END_FRAME:
                    break
                entries.append((frame, buttons))
            self.stages[stage] = dict(score=score, frames=entries[-1][0] if entries else 0, entries=len(entries),
                                      seed=seed, pointItemsTotal=points, power=power, lives=lives, bombs=bombs,
                                      rank=rank, powerItemCombo=combo)

    @property
    def label(self):
        return f"{SHOTS[self.shot]} {DIFFICULTIES[self.difficulty]}"

    def describe(self):
        yield f"{self.name} on {self.date}, score {self.score}, slowdown {self.slowdown:.2f}%"
        for stage, s in self.stages.items():
            yield (f"stage {stage}: ends with {s['score']} after {s['frames']} frames, {s['entries']} input "
                   f"entries; starts with lives {s['lives']} bombs {s['bombs']} power {s['power']} rank "
                   f"{s['rank']} seed {s['seed']:#06x}")

    def start(self):
        """stage,difficulty,shot,lives,bombs as the replay menu sets them
        for the first stage."""
        first = min(self.stages)
        s = self.stages[first]
        return f"{first - 1},{self.difficulty},{self.shot},{s['lives']},{s['bombs']}"


def config_bytes():
    """The game's configuration (GameConfig in src/unsorted.h): the
    shipped key mapping and defaults, windowed, no frame skip, no music,
    no sound effects, 32-bit colors."""
    mapping = bytes.fromhex("0000010002000400ffffffffffffffff0300")
    cfg = mapping + bytes(2) + struct.pack("<I8B2h", 0x102, 2, 3, 0, 0, 0, 1, 1, 0, 600, 600)
    cfg += bytes(0x10) + struct.pack("<I", 1)  # flags: emulateColorAdd
    assert len(cfg) == 0x38
    return cfg


def setup(game_dir, replay):
    """Writes the configuration and the replay; the tracer finds the
    replay at TRACE_REPLAY and starts it with TRACE_START."""
    with open(replay.path, "rb") as f:
        rpy = f.read()
    for name, data in ((CONFIG, config_bytes()), (REPLAY_PATH, rpy)):
        path = os.path.join(game_dir, name)
        if os.path.lexists(path):
            os.remove(path)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "wb") as f:
            f.write(data)
    return dict(TRACE_REPLAY=REPLAY_PATH, TRACE_START=replay.start())


def stage_failures(stage, recorded, last, end, get):
    """The stage's last calculation leaves the replay's frame count on the
    frame of EndStage's entry, with every entry before that one played.
    Playing, the same EndStage writes the same two entries over that one
    and the end marker, so the stage's end record is on the marker; if
    the frame counts differed, it would also write into the next stage's
    data."""
    failures = []
    if get(last, "entry") != recorded["entries"] - 2:
        failures.append(f"the last calculation played input entry {get(last, 'entry')}, the replay's last is "
                        f"{recorded['entries'] - 2}")
    if get(end, "entry") != recorded["entries"] or get(end, "entryFrame") != END_FRAME:
        failures.append(f"ended at input entry {get(end, 'entry')}, not at the replay's end marker, entry "
                        f"{recorded['entries']}")
    return failures

