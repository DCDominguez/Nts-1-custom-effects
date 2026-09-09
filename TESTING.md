# Hardware Testing Status

Target hardware: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

All 25 current M1 units have now been reported working correctly on the physical original NTS-1 MkI. Compilation and physical hardware validation are recorded separately; the detailed project QA sheets remain available for regression testing and future milestones.

## Core projects

| Project | Unit type | Compile | Hardware | QA sheet | Validated gate |
|---|---|---|---|---|---|
| SPECTRA | `osc` | PASS | HARDWARE PASS | [QA](oscillators/spectra/docs/qa-sheet.md) | Pitch tracking, voices, SHAPE, harmony, aliasing, stability |
| PARALLAX | `delfx` | PASS | HARDWARE PASS | [QA](effects/parallax/docs/qa-sheet.md) | Decorrelated taps, SPREAD, DIVERGENCE, MIX, stereo/mono, stability |
| CHORDGHOST | `delfx` | PASS | HARDWARE PASS | [QA](effects/chordghost/docs/qa-sheet.md) | BPM delay, divisions, MIX, stability |

## Human Soon ModFX

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

## Human Soon DelFX

| Project | Compile | Hardware | QA sheet | Validated gate |
|---|---|---|---|---|
| BALLISTIC | PASS | HARDWARE PASS | [QA](effects/ballistic/docs/qa-sheet.md) | accelerating/even/decelerating tap trajectories, MIX, feedback |
| RAINFALL | PASS | HARDWARE PASS | [QA](effects/rainfall/docs/qa-sheet.md) | droplet density, WEATHER scatter, smooth retargeting |
| SWARMDELAY | PASS | HARDWARE PASS | [QA](effects/swarmdelay/docs/qa-sheet.md) | BPM/divisions, MIDI clock behavior, DIVERGENCE, MIX |
| GLITCHREPEAT | PASS | HARDWARE PASS | [QA](effects/glitchrepeat/docs/qa-sheet.md) | repeat states, SLICE, CHANCE, BPM changes |
| BUCKETLINE | PASS | HARDWARE PASS | [QA](effects/bucketline/docs/qa-sheet.md) | DELAY, AGE loss/wobble/saturation, feedback bound |
| LONGMEMORY | PASS | HARDWARE PASS | [QA](effects/longmemory/docs/qa-sheet.md) | long-buffer load, near-loop MEMORY, aging, parameter changes |
| SHARD | PASS | HARDWARE PASS | [QA](effects/shard/docs/qa-sheet.md) | down/unity/up pitch field, grain crossover, aliasing |

## Human Soon RevFX

| Project | Compile | Hardware | QA sheet | Validated gate |
|---|---|---|---|---|
| ABYSS | PASS | HARDWARE PASS | [QA](effects/abyss/docs/qa-sheet.md) | FDN decay, SPACE modulation, ringing/runaway behavior |
| AUREOLE | PASS | HARDWARE PASS | [QA](effects/aureole/docs/qa-sheet.md) | octave HALO, grain crossover, shimmer feedback bounds |
| NEBULA | PASS | HARDWARE PASS | [QA](effects/nebula/docs/qa-sheet.md) | cloud density, grain wraps, diffuse tail, stability |

## QA rule

Project-specific QA sheets remain the source of truth for regression testing. Future DSP milestones must still be rebuilt and retested on physical hardware before being marked validated.

When a regression or new milestone is tested, record:

1. Date, tester, NTS-1 firmware, Sound Librarian version and exact build/commit where available.
2. Test source, controller/MIDI setup and monitoring chain.
3. PASS/FAIL for applicable rows.
4. Concrete failure settings when any defect is found.
5. Blocking vs non-blocking status and whether a retest is required.

## Current status

**2026-09-09: all 25 current M1 units reported working correctly on the physical original NTS-1 MkI.** No hardware defects were reported. This records M1 hardware validation at suite level; detailed QA templates are retained for future regression and M2 testing.

See [`HARDWARE_VALIDATION.md`](HARDWARE_VALIDATION.md) for the suite-level validation record.
