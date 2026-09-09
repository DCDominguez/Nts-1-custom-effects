# NTS-1 Custom Effects & Oscillators

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The repository treats the NTS-1 as a programmable musical system rather than a collection of conventional pedal clones. Current projects explore harmony, spatial decorrelation, controlled instability, nonlinear timing, stochastic motion, granular processing and feedback networks.

## Current status

**All 25 current M1 units compile and have been reported working correctly on the physical original NTS-1 MkI.** The M1 suite is hardware validated at suite level.

Four experiments are now **retired from active development and moved to the backburner**: **PARALLAX, IRONROT, ZEROCROSS and NEBULA**. Their source, docs, test builds and historical validation records remain preserved, but they will not advance through M2–M4 unless they are deliberately redesigned and reopened. See **[BACKBURNER.md](BACKBURNER.md)**.

For descriptions, control mappings and practical instructions for every current unit, see the **[Human Soon NTS-1 User Guide](USER_GUIDE.md)**.

For the next-stage plan for active projects, see **[M2–M4 Milestones](MILESTONES_M2_M4.md)**. Backburnered entries in that older roadmap are superseded by `BACKBURNER.md` until explicitly reopened.

See [`TESTING.md`](TESTING.md) for the complete hardware status, [`HARDWARE_VALIDATION.md`](HARDWARE_VALIDATION.md) for the validation record, and [`effects/humansoon-suite/ROADMAP.md`](effects/humansoon-suite/ROADMAP.md) for the clean-room effects roadmap.

## Oscillator

- **SPECTRA** (`osc`) — four-voice swarm oscillator with waveform morphing, detune/interval constellations, independent drift and bounded per-note mutation.

The MkI oscillator API outputs one sample per frame, so SPECTRA is intentionally mono before the NTS-1 host filter/envelope/effects.

## Core experimental effects

- **PARALLAX** (`delfx`) — **BACKBURNER**. Intended as a delay whose trails move through stereo position and time; current versions read too strongly as chorus/ensemble.
- **CHORDGHOST** (`delfx`) — harmonic-delay architecture; M1 is the BPM-synced delay foundation and feature development remains paused after M1 validation.

## Human Soon ModFX

| Unit | Controls / role |
|---|---|
| **DUST** | TIME=RATE; DEPTH=DAMAGE — sample-rate/bit reduction + stereo fracture |
| **CARRIER** | TIME=FREQUENCY; DEPTH=POLARITY — dry→AM→ring + stereo phase split |
| **VECTORFILTER** | TIME=CUTOFF; DEPTH=VECTOR — LP→BP→HP morph |
| **IRONROT** | **BACKBURNER** — TIME=CHARACTER; DEPTH=CORROSION — current architecture lacks a strong enough corrosion identity |
| **ATTRACTOR** | TIME=RATE; DEPTH=ORBIT — deterministic chaotic stereo motion |
| **ZEROCROSS** | **BACKBURNER** — TIME=RATE; DEPTH=SWEEP — functional through-zero core, but currently too close to a conventional flanger |
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
- **NEBULA** — **BACKBURNER**. The current cloud architecture does not yet produce a convincing enough particle/grain-to-reverb identity, and an earlier build was reported to clip.

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

1. begin from a strong audible behavior or interaction idea
2. implement one bounded milestone
3. compile against Korg's current template in GitHub Actions
4. package `.ntkdigunit`
5. provide a project-specific QA sheet
6. test on the physical original NTS-1
7. only then expand/calibrate the DSP architecture

The consolidated CI workflow `.github/workflows/build-full-test-suite.yml` rebuilds every current unit in one run, records text/data/BSS sizes, and packages the binaries plus QA sheets as one test artifact.

Any M2 or later DSP change reopens the hardware gate for the affected unit.

## Clean-room policy

The Human Soon suite may use public product categories and high-level interaction ideas as inspiration, but it does not copy commercial source, binaries, presets, UI text or proprietary implementation details, and it does not reverse engineer commercial units. A future design may study the broad musical behavior of a familiar device, reduce that behavior to a small NTS-1-suitable core, and then build an original Human Soon interpretation around it. See `BACKBURNER.md` for the current design reset.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Korg unit index](https://korginc.github.io/logue-sdk/unit-index/)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
