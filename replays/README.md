# Replay corpus

These 17 player recordings were downloaded from the
[Embodiment of Scarlet Devil world record table](https://maribelhearn.com/wr)
on 2026-11-04 for regression testing. They cover all four shot types on
Easy, Normal, Hard and Lunatic, and ReimuA on Extra: the table's other
Extra records link to videos only.

`manifest.json` retains each recording's source URL, credited player,
date, shot, difficulty, score and SHA-1. The recordings belong to their
players; inclusion here documents test provenance, not authorship.

Refresh with `python3 tools/replay/replay.py fetch`. Run with `just replay`.
See [the harness documentation](../docs/replay.md) for validation and
coverage. Downloading or parsing a recording alone does not count as a
successful parity check.
