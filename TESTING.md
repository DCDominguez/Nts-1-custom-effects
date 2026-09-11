# Hardware Testing Status

Target hardware: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

## Suite-wide pre-handoff policy — 2026-09-11

FIELD's debugging workflow is now the default engineering model for **all Human Soon NTS-1 units**.

Before a normal hardware handoff, we should reproduce and eliminate functional problems internally wherever possible by running the **actual production DSP** under deterministic host tests, then compile/package against the current original-NTS-1 SDK target.

Read:

- [`PRE_HANDOFF_PROTOCOL.md`](PRE_HANDOFF_PROTOCOL.md) — required internal layers and handoff contract;
- [`PRE_HANDOFF_COVERAGE.md`](PRE_HANDOFF_COVERAGE.md) — per-unit depth/backfill status;
- [`reports/testing/2026-09-11_suite-wide-new-standard-retest.md`](reports/testing/2026-09-11_suite-wide-new-standard-retest.md) — first full-suite retest under the new shared gate.

The intended DC handoff is now mostly:

1. load/select the candidate on the physical NTS-1;
2. perform the specifically identified short hardware-runtime sanity sweep;
3. judge **tone, musicality, identity and playability**.

Compilation alone is no longer sufficient for a normal handoff. A changed unit whose internal coverage is incomplete may still be handed over as an explicitly labeled **DIAGNOSTIC** build, but the missing coverage must be stated.

Actual MkI loading and real-time deadline margin remain hardware-only facts; desktop tests must not be presented as proof of device CPU stability.

### First suite-wide new-standard retest

On 2026-09-11 all **29 tracked units** were run through the shared deterministic production-DSP common gate and rebuilt/packaged against a fresh clone of the current official Korg logue SDK.

Final workflow: **Pre-handoff suite**, run **34605006118**.

- host production-DSP common gate: **29/29 PASS**;
- fresh ARM compile/package: **29/29 PASS**;
- no production DSP source changes were required to achieve the final common-gate pass.

This common-gate result does **not** automatically promote every unit to A-level depth. Project-specific behavioral coverage is still tracked in `PRE_HANDOFF_COVERAGE.md`; FIELD remains the deepest reference harness.

---

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

Project-specific QA sheets remain the source of truth for musical/device regression checks, but new or changed candidates also inherit the suite-wide internal pre-handoff gate.

Future DSP milestones should therefore pass two different layers:

- **internal engineering gate** — deterministic production-DSP tests + ARM build/package;
- **physical/music gate** — actual MkI load/runtime sanity + DC's tone/musicality judgment.

When a regression or new milestone is tested, record exact build/commit where possible, source/monitoring conditions, PASS/FAIL for applicable rows, concrete failure settings, and whether the defect is blocking.

## Current status

- **Suite common engineering gate:** 29/29 PASS on run 34605006118.
- **M1:** all 25 historical M1 units function on the physical original NTS-1 MkI; NEBULA has a clipping-quality defect and is awaiting redesign/retest if reopened.
- **M2 Wave 1:** historical validation remains recorded; future changes must adopt the new pre-handoff protocol.
- **LATTICE FIELD:** first unit at deep A-level pre-handoff coverage and the reference workflow for future backfill.
- **LATTICE CORE:** common gate PASS, but physical high-TIME distortion remains open and deeper project-specific backfill is still highest priority.

See [`HARDWARE_VALIDATION.md`](HARDWARE_VALIDATION.md) for the original suite-level validation record.

## Required testing layers before hardware handoff

The detailed requirements now live in [`PRE_HANDOFF_PROTOCOL.md`](PRE_HANDOFF_PROTOCOL.md).

In short:

1. **Internal correctness and delivery:** actual production DSP, deterministic input corpus, parameter extremes/sweeps, state ownership/expiry, wrap/feedback safety, finite output, rest behavior and unit-specific assertions.
2. **Build/package integrity:** current official original-NTS-1 build target, manifest/API checks, memory sizes and `.ntkdigunit` packaging.
3. **Physical MkI acceptance:** load/select, one short known worst-case runtime sweep, then tone/musicality/playability.

A high internal activity count is not success unless the intended result reaches the output. This rule comes directly from FIELD's C-D-E failure investigation.

Every handoff identifies the build, internal evidence, known limitations, and **only the remaining physical/listening checks**. Reproduce deterministic failures internally before another normal device build. Preserve hardware feedback literally and distinguish inference from observation.
