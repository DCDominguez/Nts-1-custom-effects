# NTS-1 Custom Effects & Oscillators

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The repository treats the NTS-1 as a programmable musical system rather than a collection of conventional pedal clones. Current projects explore harmony, spatial decorrelation, controlled instability, nonlinear timing, stochastic motion, granular processing and feedback networks.

## Current status

**25 current units have an M1 build path and hardware QA sheet.** Compilation is not physical validation: every unit remains **READY FOR TEST** until its project QA passes on the original NTS-1 MkI.

See [`TESTING.md`](TESTING.md) for the complete hardware queue and [`effects/humansoon-suite/ROADMAP.md`](effects/humansoon-suite/ROADMAP.md) for the clean-room effects roadmap.

## Oscillator

- **SPECTRA** (`osc`) — four-voice swarm oscillator with waveform morphing, detune/interval constellations, independent drift and bounded per-note mutation.

The MkI oscillator API outputs one sample per frame, so SPECTRA is intentionally mono before the NTS-1 host filter/envelope/effects.

## Core experimental effects

- **PARALLAX** (`delfx`) — four-voice decorrelated spatial chorus/doubler field. Fixed true pitch-shift voices remain a later milestone.
- **CHORDGHOST** (`delfx`) — harmonic-delay architecture; M1 is the BPM-synced delay foundation and feature development remains paused until QA.

## Human Soon ModFX

| Unit | Controls / role |
|---|---|
| **DUST** | TIME=RATE; DEPTH=DAMAGE — sample-rate/bit reduction + stereo fracture |
| **CARRIER** | TIME=FREQUENCY; DEPTH=POLARITY — dry→AM→ring + stereo phase split |
| **VECTORFILTER** | TIME=CUTOFF; DEPTH=VECTOR — LP→BP→HP morph |
| **IRONROT** | TIME=CHARACTER; DEPTH=CORROSION — bounded tone-dependent distortion |
| **ATTRACTOR** | TIME=RATE; DEPTH=ORBIT — deterministic chaotic stereo motion |
| **ZEROCROSS** | TIME=RATE; DEPTH=SWEEP — through-zero flanger architecture |
| **PHASEWELL** | TIME=RATE; DEPTH=DEPTH/RESONANCE — six-stage asymmetric phaser |
| **ASCENDER** | TIME=CLIMB; DEPTH=HEIGHT — barber-pole phaser |
| **HELIX** | TIME=ROTATION; DEPTH=HELIX — barber-pole flanger |
| **CAPSTAN** | TIME=MOTION; DEPTH=WEAR — wow/flutter/dropout tape motion |
| **SIDEBAND** | TIME=SHIFT; DEPTH=DIVERGENCE — quadrature frequency shifter |
| **FAULTLINE** | TIME=CUTOFF; DEPTH=FAULT — bounded unstable nonlinear filter |

## Human Soon DelFX

All custom delay effects use `SHIFT+DEPTH` / `k_user_delfx_param_shift_depth` for **MIX** in the current M1 designs.

| Unit | TIME | DEPTH |
|---|---|---|
| **BALLISTIC** | RANGE | TRAJECTORY |
| **RAINFALL** | DENSITY | WEATHER |
| **SWARMDELAY** | DIVISION | DIVERGENCE |
| **GLITCHREPEAT** | SLICE | CHANCE |
| **BUCKETLINE** | DELAY | AGE |
| **LONGMEMORY** | LENGTH | MEMORY |
| **SHARD** | PITCH | GRAIN |

## Human Soon RevFX

All current custom reverbs use `SHIFT+DEPTH` / `k_user_revfx_param_shift_depth` for **MIX**.

- **ABYSS** — modulated four-line FDN; TIME=DECAY, DEPTH=SPACE.
- **AUREOLE** — bounded octave-feedback shimmer FDN; TIME=DECAY, DEPTH=HALO.
- **NEBULA** — four-grain diffusion cloud; TIME=SPACE, DEPTH=CLOUD.

## Target platform

- Korg Nu:Tekt NTS-1 digital kit, original/MkI
- logue SDK API `1.1-0`
- NTS-1 firmware `>= 1.02`
- unit types: `osc`, `modfx`, `delfx`, `revfx`

Official Korg build bases:

- `platform/nutekt-digital/dummy-osc`
- `platform/nutekt-digital/dummy-modfx`
- `platform/nutekt-digital/dummy-delfx`
- `platform/nutekt-digital/dummy-revfx`

## Development rule

1. implement one bounded milestone
2. compile against Korg's current template in GitHub Actions
3. package `.ntkdigunit`
4. provide a project-specific QA sheet
5. test on the physical original NTS-1
6. only then expand/calibrate the DSP architecture

The consolidated CI workflow `.github/workflows/build-full-test-suite.yml` rebuilds every current unit in one run, records text/data/BSS sizes, and packages the binaries plus QA sheets as one test artifact.

## Clean-room policy

The Human Soon suite may use public product categories as inspiration, but it does not copy commercial source, binaries, presets, UI text or proprietary implementation details, and it does not reverse engineer commercial units. See the roadmap for the detailed rule set.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Korg unit index](https://korginc.github.io/logue-sdk/unit-index/)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
