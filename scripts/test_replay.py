"""Tests for th06's replay hook (scripts/replay_hook.py) and the trace
checks of tools/replay/replay.py, on the checked-in replays and on small
made-up traces. They run no game:
`python3 -m unittest scripts/test_replay.py`."""
import os, sys, unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "tools", "replay"))
import replay  # noqa: E402

hook = replay.hook()
NORMAL = os.path.join(replay.REPLAYS, "th6_udNoRA.rpy")


class ReplayTest(unittest.TestCase):
    def test_normal_record(self):
        r = hook.Replay(NORMAL)
        self.assertEqual(r.label, "ReimuA Normal")
        self.assertEqual(list(r.stages), [1, 2, 3, 4, 5, 6])
        # The header's score is the last stage's.
        self.assertEqual(r.score, r.stages[6]["score"])
        self.assertEqual(r.start(), "0,1,0,2,3")
        scores = [s["score"] for s in r.stages.values()]
        self.assertEqual(scores, sorted(scores))

    def test_manifest(self):
        import json
        with open(replay.MANIFEST) as f:
            manifest = json.load(f)
        for entry in manifest["replays"]:
            path = os.path.join(replay.REPLAYS, entry["file"])
            self.assertEqual(replay.sha1(path), entry["sha1"], entry["file"])
            r = hook.Replay(path)
            self.assertEqual(r.label, f"{entry['shot']} {entry['difficulty']}")
            self.assertEqual(r.score, entry["score"])

    def test_bad_checksum(self):
        data = bytearray(open(NORMAL, "rb").read())
        data[-1] ^= 1
        path = os.path.join(os.environ.get("TMPDIR", "/tmp"), "th06_bad_checksum.rpy")
        with open(path, "wb") as f:
            f.write(data)
        try:
            with self.assertRaises(ValueError):
                hook.Replay(path)
        finally:
            os.remove(path)


class FakeReplay:
    def __init__(self, stages):
        self.stages = stages


def record(fields, **values):
    return tuple(values.get(f, 0) for f in fields)


FIELDS = ["kind", "seq", "calcs", "passes", "fastForwards", "replayFrame", "entry", "entryFrame",
          "nextEntryFrame", "stage", "flags", "score"]


def stage_trace(frames=3, entries=2, score=100, end=replay.REPLAY_END, gap=False, tail=True):
    """A one-stage trace: the start, `frames` calculations, the end."""
    rs = [record(FIELDS, kind=replay.STAGE_START, seq=0, calcs=10, passes=10, stage=1)]
    seq = 1
    for frame in range(1, frames + 1):
        if gap and frame == 2:
            continue
        rs.append(record(FIELDS, kind=replay.CALC, seq=seq, calcs=10 + seq, passes=10 + seq, replayFrame=frame,
                         entry=entries - 2, stage=1, flags=1, score=score))
        seq += 1
    rs.append(record(FIELDS, kind=end, seq=seq, calcs=10 + seq, passes=10 + seq, replayFrame=frames,
                     entry=entries, entryFrame=hook.END_FRAME, stage=1, score=score))
    if tail:
        rs.append(record(FIELDS, kind=replay.TITLE, seq=seq + 1, calcs=80 + seq, passes=80 + seq))
    return replay.Trace(FIELDS, "test", rs)


STAGES = {1: dict(score=100, entries=2, frames=3)}


class ValidateTest(unittest.TestCase):
    def test_pass(self):
        v = replay.validate(FakeReplay(STAGES), stage_trace())
        self.assertEqual(v["failures"], [])
        self.assertEqual(v["playing_calcs"], 3)

    def test_wrong_score(self):
        v = replay.validate(FakeReplay(STAGES), stage_trace(score=99))
        self.assertTrue(any("recorded 100" in f for f in v["failures"]), v["failures"])

    def test_gap(self):
        v = replay.validate(FakeReplay(STAGES), stage_trace(gap=True))
        self.assertTrue(any("gaps" in f for f in v["failures"]), v["failures"])

    def test_short(self):
        v = replay.validate(FakeReplay(STAGES), stage_trace(frames=2))
        self.assertTrue(any("played 2 frames" in f for f in v["failures"]), v["failures"])

    def test_no_title(self):
        v = replay.validate(FakeReplay(STAGES), stage_trace(tail=False))
        self.assertIn("the game did not get back to the title", v["failures"])

    def test_missing_stage(self):
        v = replay.validate(FakeReplay({**STAGES, 2: STAGES[1]}), stage_trace())
        self.assertTrue(any("stages played" in f for f in v["failures"]), v["failures"])


class CompareTest(unittest.TestCase):
    def test_identical(self):
        self.assertTrue(replay.compare(stage_trace(), stage_trace())["identical"])

    def test_difference(self):
        result = replay.compare(stage_trace(), stage_trace(score=101))
        self.assertFalse(result["identical"])
        self.assertEqual(result["first_difference"]["record"], 1)
        self.assertEqual(result["first_difference"]["fields"], {"score": [100, 101]})

    def test_length(self):
        result = replay.compare(stage_trace(), stage_trace(tail=False))
        self.assertFalse(result["identical"])


class MapTest(unittest.TestCase):
    def test_sizes(self):
        path = os.path.join(os.environ.get("TMPDIR", "/tmp"), "th06_test.map")
        with open(path, "w") as f:
            f.write(" 0001:00000000       ?A@@YAXXZ                  00401000 f   a.obj\n"
                    " 0001:00000040       ?B@@YAXXZ                  00401040 f   a.obj\n"
                    " 0003:00000010       ?g_C@@3HA                  00500010     b.obj\n")
        try:
            symbols = replay.map_symbols(path)
        finally:
            os.remove(path)
        self.assertEqual(symbols["?A@@YAXXZ"], (0x401000, 0x40))
        self.assertEqual(symbols["?g_C@@3HA"][0], 0x500010)


if __name__ == "__main__":
    unittest.main()
