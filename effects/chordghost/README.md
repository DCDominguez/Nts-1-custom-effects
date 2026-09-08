# CHORDGHOST

CHORDGHOST is an experimental harmonic delay for the original Korg Nu:Tekt NTS-1.

The dry signal remains untouched. The delayed signal becomes a harmony voice that is retuned against a chord progression supplied by an external sequencer, initially the OXI One MKII.

## Current implementation stage

The first source scaffold implements **M1: a safe BPM-synced stereo delay** plus the state decoder required for M2.

It does **not** yet pitch-shift the repeats. Harmonic audio processing comes after the plain delay and MIDI protocol are proven on hardware.

## Build model

This folder is designed to be overlaid onto Korg's official:

```text
logue-sdk/platform/nutekt-digital/dummy-delfx
```

Recommended workflow:

1. Clone Korg's `logue-sdk` and initialize its submodules.
2. Copy `platform/nutekt-digital/dummy-delfx` to `platform/nutekt-digital/chordghost`.
3. Replace its `manifest.json` and `project.mk` with the files from `nts1/` here.
4. Copy `nts1/src/` into that project.
5. Run `make`.
6. Run `make install` to package `chordghost.ntkdigunit`.

The official template provides the Korg-owned `Makefile`, linker files, and `_unit.c` hook layer. We intentionally do not duplicate those files here.

## Controls in the current scaffold

| Parameter | Role |
|---|---|
| TIME / CC30 | synced delay division |
| DEPTH / CC31 | CHORDGHOST chord-state code |
| SHIFT+DEPTH / CC33 | wet/dry mix |

Feedback is temporarily fixed while the protocol is being validated.

## Important

`DEPTH` no longer behaves like conventional feedback in CHORDGHOST. It becomes the external harmonic-state input.

See:

- [`SPEC.md`](SPEC.md)
- [`docs/midi-protocol.md`](docs/midi-protocol.md)
- [`docs/test-plan.md`](docs/test-plan.md)
