# SPECTRA

SPECTRA is a four-voice swarm oscillator for the **Korg Nu:Tekt NTS-1 digital kit (MkI)**.

A single played note becomes a small internal ensemble. The voices can be detuned, drift independently, and move into interval structures while the NTS-1 continues to provide the surrounding filter, envelope, LFO, modulation, delay, and reverb stages.

```text
MIDI pitch
   |
   +-- voice 1: root / detune / drift
   +-- voice 2: interval / detune / drift
   +-- voice 3: interval / detune / drift
   +-- voice 4: interval / detune / drift
             |
         mono swarm
             |
       NTS-1 voice path
```

## Important hardware/API constraint

The original NTS-1 user-oscillator callback writes **one sample per frame** to a mono oscillator output buffer. SPECTRA can generate multiple internal pitches, but it cannot place those voices independently in stereo inside the oscillator unit itself.

Stereo placement belongs downstream — for example PARALLAX, another modulation/delay effect, or the external studio chain.

## Current stage

The first scaffold implements:

- 1–4 internal oscillator voices
- waveform morphing from sine → triangle → band-limited saw → band-limited square
- static detune spread
- eight interval constellations
- per-voice low-rate pitch drift
- per-note bounded chaos for phase, tuning, and level
- NTS-1 `SHAPE` modulation support
- NTS-1 `ALT` as harmonic-interval amount

This is an oscillator source only. The NTS-1 host still handles articulation and filtering.

## Controls

### Performance controls

| NTS-1 control | SPECTRA role |
|---|---|
| `SHAPE` | waveform morph |
| `ALT` | harmonic interval amount |

### User OSC edit parameters

| Edit parameter | Role |
|---|---|
| `Voices` | 1–4 internal voices |
| `Spread` | static unison detune width |
| `Drift` | independent slow pitch movement |
| `HarmMode` | interval constellation 1–8 |
| `Motion` | drift speed |
| `Chaos` | per-note phase/tuning/level mutation |

See [`SPEC.md`](SPEC.md) and [`docs/test-plan.md`](docs/test-plan.md).

## Build model

This folder is an overlay/scaffold for Korg's official:

```text
logue-sdk/platform/nutekt-digital/dummy-osc
```

Recommended workflow:

1. Clone Korg's `logue-sdk` and initialize submodules.
2. Copy `platform/nutekt-digital/dummy-osc` to `platform/nutekt-digital/spectra`.
3. Replace its `manifest.json` and `project.mk` with the files in `nts1/` here.
4. Copy `nts1/src/` into the project.
5. Run `make`.
6. Run `make install` to package `spectra.ntkdigunit`.

Do not skip the official `dummy-osc` hardware test before debugging SPECTRA.
