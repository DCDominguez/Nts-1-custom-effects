# PARALLAX

PARALLAX is an experimental multi-voice spatial chorus / swarm doubler for the original Korg Nu:Tekt NTS-1.

Instead of treating chorus as one modulated delay, PARALLAX treats the wet signal as several independent performers. Each voice can have its own delay time, pitch offset, motion rate, motion phase, and stereo anchor.

```text
INPUT
  ├─ voice A: pitch A / time A / pan A
  ├─ voice B: pitch B / time B / pan B
  ├─ voice C: pitch C / time C / pan C
  └─ voice D: pitch D / time D / pan D
                     ↓
                 stereo sum
                     ↓
                    OUT
```

## Current implementation stage

The first source scaffold implements **M1: a four-voice spatial ensemble**.

It already separates the voices in time, modulation phase/rate, and stereo position. It does **not** yet implement true fixed pitch offsets. M2 adds the pitch-shifter core after the multi-tap field is stable on hardware.

That staging is deliberate: a moving delay line creates chorus pitch motion, but it is not the same thing as a voice held at a fixed offset such as `-9 cents` or `+7 semitones`. PARALLAX will use a real multi-read-head pitch-shift stage for that.

## Build model

This folder is designed to be overlaid onto Korg's official:

```text
logue-sdk/platform/nutekt-digital/dummy-delfx
```

Recommended workflow:

1. Clone Korg's `logue-sdk` and initialize its submodules.
2. Copy `platform/nutekt-digital/dummy-delfx` to `platform/nutekt-digital/parallax`.
3. Replace its `manifest.json` and `project.mk` with the files from `nts1/` here.
4. Copy `nts1/src/` into that project.
5. Run `make`.
6. Run `make install` to package `parallax.ntkdigunit`.

## Controls in the current scaffold

| NTS-1 parameter | PARALLAX role |
|---|---|
| TIME / CC30 | **SPREAD** — separates voice delay times |
| DEPTH / CC31 | **DIVERGENCE** — increases independent motion and stereo separation |
| SHIFT+DEPTH / CC33 | **MIX** — dry/wet balance |

## Why `delfx`

Korg's SDK allows custom delay units to do more than conventional echoes, and the current logue SDK unit index includes NTS-1 unison/pitch-shift and ensemble-delay units in the delay category. PARALLAX also needs several independently addressed delay taps, so `delfx` is the cleanest first target.

See:

- [`SPEC.md`](SPEC.md)
- [`docs/dsp-architecture.md`](docs/dsp-architecture.md)
- [`docs/test-plan.md`](docs/test-plan.md)
