# NTS-1 Custom Effects & Oscillators

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The repository treats the NTS-1 as a programmable musical system rather than a collection of conventional pedal clones. Current projects explore harmony, spatial decorrelation, controlled instability, nonlinear timing, stochastic motion, granular processing and feedback networks.

## Current status

### Engineering

All **29 tracked units are A-class for pre-handoff engineering coverage** on the current MkI candidate.

Final consolidated A-class evidence:

- candidate: `5718d12416a242137892688e240e8ac41be5a0ca`;
- workflow: **Pre-handoff suite** run `34613566166`;
- shared production-DSP common gate: **29/29 PASS**;
- project-specific A-class harnesses: **29/29 PASS**;
- fresh ARM compile/package: **29/29 PASS**;
- AddressSanitizer/UndefinedBehaviorSanitizer: no failure in the exercised host paths.

A-class is an engineering grade, not a claim that hardware runtime or musical quality is perfect. See [`A_CLASS_REQUIREMENTS.md`](A_CLASS_REQUIREMENTS.md) and [`PRE_HANDOFF_COVERAGE.md`](PRE_HANDOFF_COVERAGE.md).

### Latest physical MkI pass — 2026-09-12

The A-class release-candidate pack was loaded and played on the physical original NTS-1 MkI. Units not specifically called out below were reported to **load properly and sound as intended**.

Main findings:

- **Delay:** all tested delays work, but the family is generally too quiet; the NTS-1 main volume must be raised very high for an enjoyable listening level.
- **Reverb:** all tested reverbs work; except for LATTICE SPACE they tested well musically, but the family is also generally too quiet.
- **ModFX:** overall level consistency needs another pass. ASCENDER, ATTRACTOR and CAPSTAN are barely noticeable; IRONROT sounds great but can become quite loud.
- **LATTICE CORE:** sounds great and loops the note at maximum TIME, but intermittent distortion remains at extreme TIME and in some multi-effect use.
- **LATTICE SPACE:** very subtle even at full MIX and distorts when paired with modulation; it is now a redesign/revoice candidate.
- Most other ModFX + Delay/Reverb combinations tested well.

See [`HARDWARE_VALIDATION.md`](HARDWARE_VALIDATION.md), [`reports/testing/2026-09-12_a-class-rc-physical-mki.md`](reports/testing/2026-09-12_a-class-rc-physical-mki.md), and issue **#11** for the suite-wide level-calibration work.

## Pre-handoff engineering protocol

FIELD's debugging workflow is now the suite-wide standard.

Before normal hardware handoff, changed units are exercised using the **actual production DSP source** with deterministic host tests: input-level cases, parameter extremes/sweeps, state delivery/expiry, buffer wraps, feedback/rest behavior, finite-output checks and unit-specific behavioral assertions. ARM compile/package is required, but compilation alone is not a normal handoff gate.

The intended physical handoff is mainly:

1. load/select the `.ntkdigunit` on the physical NTS-1;
2. perform one short hardware-only worst-case/runtime sanity sweep identified in the handoff notes;
3. judge **tone, musicality, identity and playability**.

- [`PRE_HANDOFF_PROTOCOL.md`](PRE_HANDOFF_PROTOCOL.md) — suite-wide test and handoff requirements
- [`PRE_HANDOFF_COVERAGE.md`](PRE_HANDOFF_COVERAGE.md) — current per-unit engineering coverage
- [`A_CLASS_REQUIREMENTS.md`](A_CLASS_REQUIREMENTS.md) — A-class requirements
- [`COMMUNITY_TESTING.md`](COMMUNITY_TESTING.md) — reproducible community test instructions
- [`community/HARDWARE_TEST_REPORT.md`](community/HARDWARE_TEST_REPORT.md) — physical-hardware report template

## LATTICE — active MkI system development

Preferred architecture:

```text
AUDIO IN / OSC
      ↓
LATTICE CORE   [custom MOD]
      ↓
LATTICE FIELD  [custom DELAY]
      ↓
optional Korg built-in REVERB only
      ↓
OUTPUT
```

The earlier CORE + ECHO + SPACE three-custom-unit architecture is historical and is not the preferred MkI target.

Current physical state:

- **CORE 0.3-0** — sounds good and its max-TIME loop/freeze behavior works, but intermittent distortion remains at extreme TIME and in some multi-effect use. **PASS WITH NOTES / RETEST.**
- **FIELD 0.1-2** — no new problem reported in the latest RC pass. **LOAD / RUNTIME / MUSICAL PASS for that playing test.**
- **LATTICE ECHO 0.3-0** — historical; no new problem reported in the latest RC pass.
- **LATTICE SPACE 0.3-0** — historical; very subtle even at full MIX and distorts with modulation. **REDESIGN/REVOICE candidate.**

For current LATTICE work, read:

1. [`LATTICE_CURRENT_STATUS.md`](LATTICE_CURRENT_STATUS.md)
2. [`LATTICE_SYSTEM.md`](LATTICE_SYSTEM.md)
3. [`LATTICE_HISTORY.md`](LATTICE_HISTORY.md)
4. [`LATTICE_FIELD_SPEC.md`](LATTICE_FIELD_SPEC.md)
5. [`LATTICE_REPORTING.md`](LATTICE_REPORTING.md)
6. [`reports/lattice/`](reports/lattice/)

Draft PR **#10** remains the current FIELD/LATTICE integration boundary into `lattice-suite` while focused runtime/voicing work continues.

## Oscillator

- **SPECTRA** (`osc`) — four-voice swarm oscillator with waveform morphing, detune/interval constellations, independent drift and bounded per-note mutation.

The MkI oscillator API outputs one sample per frame, so SPECTRA is intentionally mono before the NTS-1 host filter/envelope/effects.

## Core experimental effects

- **PARALLAX** (`delfx`) — backburnered musically; retained with A-class regression coverage.
- **CHORDGHOST** (`delfx`) — harmonic-delay architecture with BPM-synced delay foundation.

## Human Soon ModFX

| Unit | Controls / role |
|---|---|
| **DUST** | TIME=RATE; DEPTH=DAMAGE — sample-rate/bit reduction + stereo fracture |
| **CARRIER** | TIME=FREQUENCY; DEPTH=POLARITY — dry→AM→ring + stereo phase split |
| **VECTORFILTER** | TIME=CUTOFF; DEPTH=VECTOR — LP→BP→HP morph |
| **IRONROT** | TIME=CHARACTER; DEPTH=CORROSION — nonlinear corrosion; current RC sounds good but can get loud |
| **ATTRACTOR** | TIME=RATE; DEPTH=ORBIT — deterministic chaotic stereo motion; current RC is too subtle |
| **ZEROCROSS** | TIME=RATE; DEPTH=SWEEP — through-zero modulation; backburnered musically |
| **PHASEWELL** | TIME=RATE; DEPTH=DEPTH/RESONANCE — six-stage asymmetric phaser |
| **ASCENDER** | TIME=CLIMB; DEPTH=HEIGHT — barber-pole phaser; current RC is too subtle |
| **HELIX** | TIME=ROTATION; DEPTH=HELIX — barber-pole flanger |
| **CAPSTAN** | TIME=MOTION; DEPTH=WEAR — wow/flutter/dropout tape motion; current RC is too subtle |
| **SIDEBAND** | TIME=SHIFT; DEPTH=DIVERGENCE — quadrature frequency shifter |
| **FAULTLINE** | TIME=CUTOFF; DEPTH=FAULT — bounded unstable nonlinear filter |

## Human Soon DelFX

All custom delay effects use `SHIFT+DEPTH` / `k_user_delfx_param_shift_depth` for **MIX** in the current designs.

| Unit | TIME | DEPTH |
|---|---|---|
| **BALLISTIC** | RANGE | TRAJECTORY |
| **RAINFALL** | DENSITY | WEATHER |
| **SWARMDELAY** | DIVISION | DIVERGENCE |
| **GLITCHREPEAT** | SLICE | CHANCE |
| **BUCKETLINE** | DELAY | AGE |
| **LONGMEMORY** | LENGTH | MEMORY |
| **SHARD** | PITCH | GRAIN |

Current family note: functionality is good, but perceived output level needs calibration upward without sacrificing headroom.

## Human Soon RevFX

All current custom reverbs use `SHIFT+DEPTH` / `k_user_revfx_param_shift_depth` for **MIX**.

- **ABYSS** — modulated four-line FDN; TIME=DECAY, DEPTH=SPACE.
- **AUREOLE** — bounded octave-feedback shimmer FDN; TIME=DECAY, DEPTH=HALO.
- **NEBULA** — cloud/grain reverb; retained with A-class engineering coverage despite earlier musical/backburner history.

Current family note: functionality/tone is generally good, but perceived output level needs calibration upward. LATTICE SPACE is handled separately because it also has a combination-distortion/identity problem.

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

1. begin from a strong audible behavior or interaction idea;
2. implement one bounded milestone;
3. create/extend deterministic production-DSP tests for the changed behavior;
4. reproduce and resolve internally reproducible failures before normal handoff;
5. compile against Korg's current original-NTS-1 template/API in CI;
6. package `.ntkdigunit` and record memory/build evidence;
7. identify only the remaining physical/runtime/listening checks;
8. run physical load/runtime/music validation;
9. record the exact hardware result;
10. only then freeze or expand the DSP architecture.

Any DSP change reopens the physical gate for the affected exact build.

## Platform expansion

Other logue SDK targets are documented in [`PLATFORM_SUPPORT.md`](PLATFORM_SUPPORT.md), but production porting remains gated behind MkI suite maturity and calibration.

## Clean-room policy

The Human Soon suite may use public product categories and high-level interaction ideas as inspiration, but it does not copy commercial source, binaries, presets, UI text or proprietary implementation details, and it does not reverse engineer commercial units.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Korg unit index](https://korginc.github.io/logue-sdk/unit-index/)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
