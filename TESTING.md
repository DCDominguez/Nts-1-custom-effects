# Hardware Testing Status

Target hardware: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

All 25 M1 units were reported functional on the physical original NTS-1 MkI. Later listening exposed a NEBULA clipping defect, so its quality gate has been reopened for the 0.1-1 hotfix. Functional operation and release-quality validation are tracked separately.

## M2 Wave 1 — 2026-09-09

| Project | Candidate | Compile | Physical MkI | Current disposition | QA |
|---|---:|---|---|---|---|
| SPECTRA | 0.2-0 | PASS | PASS | **M2 VALIDATED** | [M2 QA](oscillators/spectra/docs/qa-m2.md) |
| PARALLAX | 0.2-0 | PASS | PASS | **M2 VALIDATED** | [M2 QA](effects/parallax/docs/qa-m2.md) |
| CHORDGHOST | 0.2-0 | PASS + protocol unit test PASS | PASS | **M2 VALIDATED** | [M2 QA](effects/chordghost/docs/qa-m2.md) |
| DUST | 0.2-0 | PASS | PASS | **M2 VALIDATED** | [M2 QA](effects/dust/docs/qa-m2.md) |
| CARRIER | 0.2-0 | PASS | PASS | **M2 VALIDATED** | [M2 QA](effects/carrier/docs/qa-m2.md) |
| VECTORFILTER | 0.2-0 | PASS | PASS | **M2 VALIDATED** | [M2 QA](effects/vectorfilter/docs/qa-m2.md) |
| IRONROT | 0.2-0 | PASS | Functional, quality concern | **M2 RETEST — 0.2-1** | [M2 QA](effects/ironrot/docs/qa-m2.md) |
| ATTRACTOR | 0.2-0 | PASS | PASS | **M2 VALIDATED** | [M2 QA](effects/attractor/docs/qa-m2.md) |

IRONROT hardware note: the first M2 candidate worked, but CORROSION was reported as sounding primarily like increased loudness rather than increased nonlinear texture. Version 0.2-1 strengthens the fracture/crossover behavior and adds attenuation-only envelope matching. M3 remains blocked until the retest passes.

## Core projects — M1 baseline

| Project | Unit type | Compile | Hardware | QA sheet | Validated gate |
|---|---|---|---|---|---|
| SPECTRA | `osc` | PASS | HARDWARE PASS | [QA](oscillators/spectra/docs/qa-sheet.md) | Pitch tracking, voices, SHAPE, harmony, aliasing, stability |
| PARALLAX | `delfx` | PASS | HARDWARE PASS | [QA](effects/parallax/docs/qa-sheet.md) | Decorrelated taps, SPREAD, DIVERGENCE, MIX, stereo/mono, stability |
| CHORDGHOST | `delfx` | PASS | HARDWARE PASS | [QA](effects/chordghost/docs/qa-sheet.md) | BPM delay, divisions, MIX, stability |

## Human Soon ModFX — M1 baseline

| Project | Compile | Hardware | QA sheet | Validated gate |
|---|---|---|---|---|
| DUST | PASS | HARDWARE PASS | [QA](effects/dust/docs/qa-sheet.md) | RATE, DAMAGE, stereo fracture, silence, abuse |
| CARRIER | PASS | HARDWARE PASS | [QA](effects/carrier/docs/qa-sheet.md) | FREQUENCY, dry→AM→ring POLARITY, stereo split, aliasing |
| VECTORFILTER | PASS | HARDWARE PASS | [QA](effects/vectorfilter/docs/qa-sheet.md) | CUTOFF, LP→BP→HP VECTOR, center resonance, stability |
| IRONROT | PASS | HARDWARE PASS | [QA](effects/ironrot/docs/qa-sheet.md) | CHARACTER, CORROSION, level bounds, silence |
| ATTRACTOR | PASS | HARDWARE PASS | [QA](effects/attractor/docs/qa-sheet.md) | chaotic RATE, ORBIT width, mono collapse, motion behavior |
| ZEROCROSS | PASS | HARDWARE PASS | [QA](effects/zerocross/docs/qa-sheet.md) | through-zero cancellation, RATE, SWEEP, feedback bound |
| PHASEWELL | PASS | HARDWARE PASS | [QA](effects/phasewell/docs/qa-sheet.md) | all-pass motion, RATE, DEPTH/resonance, stereo behavior |
| ASCENDER | PASS | HARDWARE PASS | [QA](effects/ascender/docs/qa-sheet.md) | barber-pole climb, wrap masking, HEIGHT |
| HELIX | PASS | HARDWARE PASS | [QA](effects/helix/docs/qa-sheet.md) | cyclic flanger illusion, read-head wraps, feedback |
| CAPSTAN | PASS | HARDWARE PASS | [QA](effects/capstan/docs/qa-sheet.md) | wow/flutter, WEAR, bounded dropouts, coherence |
| SIDEBAND | PASS | HARDWARE PASS | [QA](effects/sideband/docs/qa-sheet.md) | frequency translation, stereo divergence, aliasing |
| FAULTLINE | PASS | HARDWARE PASS | [QA](effects/faultline/docs/qa-sheet.md) | CUTOFF, FAULT instability, self-recovery, hard bounds |

## Human Soon DelFX — M1 baseline

| Project | Compile | Hardware | QA sheet | Validated gate |
|---|---|---|---|---|
| BALLISTIC | PASS | HARDWARE PASS | [QA](effects/ballistic/docs/qa-sheet.md) | accelerating/even/decelerating tap trajectories, MIX, feedback |
| RAINFALL | PASS | HARDWARE PASS | [QA](effects/rainfall/docs/qa-sheet.md) | droplet density, WEATHER scatter, smooth retargeting |
| SWARMDELAY | PASS | HARDWARE PASS | [QA](effects/swarmdelay/docs/qa-sheet.md) | BPM/divisions, MIDI clock behavior, DIVERGENCE, MIX |
| GLITCHREPEAT | PASS | HARDWARE PASS | [QA](effects/glitchrepeat/docs/qa-sheet.md) | repeat states, SLICE, CHANCE, BPM changes |
| BUCKETLINE | PASS | HARDWARE PASS | [QA](effects/bucketline/docs/qa-sheet.md) | DELAY, AGE loss/wobble/saturation, feedback bound |
| LONGMEMORY | PASS | HARDWARE PASS | [QA](effects/longmemory/docs/qa-sheet.md) | long-buffer load, near-loop MEMORY, aging, parameter changes |
| SHARD | PASS | HARDWARE PASS | [QA](effects/shard/docs/qa-sheet.md) | down/unity/up pitch field, grain crossover, aliasing |

## Human Soon RevFX — M1 baseline

| Project | Compile | Hardware | QA sheet | Current gate |
|---|---|---|---|---|
| ABYSS | PASS | HARDWARE PASS | [QA](effects/abyss/docs/qa-sheet.md) | M1 functional baseline retained |
| AUREOLE | PASS | HARDWARE PASS | [QA](effects/aureole/docs/qa-sheet.md) | M1 functional baseline retained |
| NEBULA | PASS | Functional; clipping later observed | [QA / 0.1-1 retest](effects/nebula/docs/qa-sheet.md) | **REOPENED — clipping hotfix retest** |

## QA rule

Project-specific QA sheets remain the source of truth for regression testing. Future DSP milestones must still be rebuilt and retested on physical hardware before being marked validated.

When a regression or new milestone is tested, record exact build/commit where possible, source/monitoring conditions, PASS/FAIL for applicable rows, concrete failure settings, and whether the defect is blocking.

## Current status

- **M1:** all 25 units function on the physical original NTS-1 MkI; NEBULA has a newly identified clipping-quality defect and is awaiting 0.1-1 retest.
- **M2 Wave 1:** 7/8 validated; IRONROT awaits 0.2-1 perceptual/level-match retest.
- M3 is allowed only for units whose M2 gate has passed.

See [`HARDWARE_VALIDATION.md`](HARDWARE_VALIDATION.md) for the original suite-level validation record and GitHub Issues #7 and #8 for the active retest gates.
