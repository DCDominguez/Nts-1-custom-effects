# Hardware Testing Queue

Target hardware: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

Compilation is not hardware validation. A unit stays in this queue until it loads on the physical NTS-1 and passes its project QA sheet for the current milestone.

## Core projects

| Project | Unit type | Compile | Hardware | QA sheet | Primary gate |
|---|---|---|---|---|---|
| SPECTRA | `osc` | PASS | READY FOR TEST | [QA](oscillators/spectra/docs/qa-sheet.md) | Pitch tracking, voices, SHAPE, harmony, aliasing, stability |
| PARALLAX | `delfx` | PASS | READY FOR TEST | [QA](effects/parallax/docs/qa-sheet.md) | Decorrelated taps, SPREAD, DIVERGENCE, MIX, stereo/mono, stability |
| CHORDGHOST | `delfx` | PASS | READY FOR TEST | [QA](effects/chordghost/docs/qa-sheet.md) | BPM delay, divisions, MIX, stability; later OXI chord decode |

## Human Soon ModFX

| Project | Compile | Hardware | QA sheet | Primary gate |
|---|---|---|---|---|
| DUST | PASS | READY FOR TEST | [QA](effects/dust/docs/qa-sheet.md) | RATE, DAMAGE, stereo fracture, silence, abuse |
| CARRIER | PASS | READY FOR TEST | [QA](effects/carrier/docs/qa-sheet.md) | FREQUENCY, dry→AM→ring POLARITY, stereo split, aliasing |
| VECTORFILTER | PASS | READY FOR TEST | [QA](effects/vectorfilter/docs/qa-sheet.md) | CUTOFF, LP→BP→HP VECTOR, center resonance, stability |
| IRONROT | PASS | READY FOR TEST | [QA](effects/ironrot/docs/qa-sheet.md) | CHARACTER, CORROSION, level bounds, silence |
| ATTRACTOR | PASS | READY FOR TEST | [QA](effects/attractor/docs/qa-sheet.md) | chaotic RATE, ORBIT width, mono collapse, no simple periodicity |
| ZEROCROSS | PASS | READY FOR TEST | [QA](effects/zerocross/docs/qa-sheet.md) | through-zero cancellation, RATE, SWEEP, feedback bound |
| PHASEWELL | PASS | READY FOR TEST | [QA](effects/phasewell/docs/qa-sheet.md) | all-pass motion, RATE, DEPTH/resonance, stereo behavior |
| ASCENDER | PASS | READY FOR TEST | [QA](effects/ascender/docs/qa-sheet.md) | barber-pole climb, wrap masking, HEIGHT |
| HELIX | PASS | READY FOR TEST | [QA](effects/helix/docs/qa-sheet.md) | cyclic flanger illusion, read-head wraps, feedback |
| CAPSTAN | PASS | READY FOR TEST | [QA](effects/capstan/docs/qa-sheet.md) | wow/flutter, WEAR, bounded dropouts, coherence |
| SIDEBAND | PASS | READY FOR TEST | [QA](effects/sideband/docs/qa-sheet.md) | frequency translation, sideband rejection, stereo divergence, aliasing |
| FAULTLINE | PASS | READY FOR TEST | [QA](effects/faultline/docs/qa-sheet.md) | CUTOFF, FAULT instability, self-recovery, hard bounds |

## Human Soon DelFX

| Project | Compile | Hardware | QA sheet | Primary gate |
|---|---|---|---|---|
| BALLISTIC | PASS | READY FOR TEST | [QA](effects/ballistic/docs/qa-sheet.md) | accelerating/even/decelerating tap trajectories, MIX, feedback |
| RAINFALL | PASS | READY FOR TEST | [QA](effects/rainfall/docs/qa-sheet.md) | droplet density, WEATHER scatter, smooth retargeting |
| SWARMDELAY | PASS | READY FOR TEST | [QA](effects/swarmdelay/docs/qa-sheet.md) | BPM/divisions, MIDI clock behavior, DIVERGENCE, MIX |
| GLITCHREPEAT | PASS | READY FOR TEST | [QA](effects/glitchrepeat/docs/qa-sheet.md) | finite repeat states, SLICE, CHANCE, BPM changes |
| BUCKETLINE | PASS | READY FOR TEST | [QA](effects/bucketline/docs/qa-sheet.md) | DELAY, AGE loss/wobble/saturation, feedback bound |
| LONGMEMORY | PASS | READY FOR TEST | [QA](effects/longmemory/docs/qa-sheet.md) | long-buffer load, near-loop MEMORY, aging, parameter changes |
| SHARD | PASS | READY FOR TEST | [QA](effects/shard/docs/qa-sheet.md) | down/unity/up pitch field, grain crossover, aliasing |

## Human Soon RevFX

| Project | Compile | Hardware | QA sheet | Primary gate |
|---|---|---|---|---|
| ABYSS | PASS | READY FOR TEST | [QA](effects/abyss/docs/qa-sheet.md) | FDN decay, SPACE modulation, metallic ringing/runaway |
| AUREOLE | PASS | READY FOR TEST | [QA](effects/aureole/docs/qa-sheet.md) | octave HALO, grain crossover, shimmer feedback bounds |
| NEBULA | PASS | READY FOR TEST | [QA](effects/nebula/docs/qa-sheet.md) | cloud density, grain wraps, diffuse tail, CPU/stability |

## QA rule

Every item marked **READY FOR TEST** must have a project-specific QA sheet before physical testing begins.

Each QA pass should record:

1. Date, tester, NTS-1 firmware, NTS-1 Sound Librarian version and exact build/commit.
2. Test source, controller/MIDI setup and monitoring chain.
3. PASS/FAIL result for every applicable row.
4. Concrete failure settings: note, control value, BPM, source type, stereo/mono state, or reproduction sequence.
5. Blocking vs non-blocking defects and whether a retest is required.
6. Final `Hardware validated for current milestone` status.

## Test rule

1. Load the packaged `.ntkdigunit` with the NTS-1 Sound Librarian.
2. Start at conservative monitoring level.
3. Follow the exact controls, ranges and acceptance criteria in the project's QA sheet and test plan.
4. Record failures by note/range/setting rather than only describing the sound generally.
5. Do not advance a project milestone until the physical NTS-1 passes the current gate.
6. Future milestone rows remain `N/A` until that implementation exists.

## Current status

All **25 current units** have an M1 source/build path and a project-specific hardware QA sheet. The complete ModFX matrix, DelFX matrix and RevFX matrix compile against Korg's current `nutekt-digital` SDK templates. None is marked hardware-validated until tested on the physical MkI.
