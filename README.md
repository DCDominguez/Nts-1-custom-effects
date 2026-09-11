# NTS-1 Custom Effects & Oscillators

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The repository treats the NTS-1 as a programmable musical system rather than a collection of conventional pedal clones. Current projects explore harmony, spatial decorrelation, controlled instability, nonlinear timing, stochastic motion, granular processing and feedback networks.

## Current status

**All 25 current M1 units compile and have been reported working correctly on the physical original NTS-1 MkI.** The M1 suite is hardware validated at suite level.

Four experiments are now **retired from active development and moved to the backburner**: **PARALLAX, IRONROT, ZEROCROSS and NEBULA**. Their source, docs, test builds and historical validation records remain preserved, but they will not advance through M2–M4 unless they are deliberately redesigned and reopened. See **[BACKBURNER.md](BACKBURNER.md)**.

For descriptions, control mappings and practical instructions for every current unit, see the **[Human Soon NTS-1 User Guide](USER_GUIDE.md)**.

For the next-stage plan for active projects, see **[M2–M4 Milestones](MILESTONES_M2_M4.md)**. Backburnered entries in that older roadmap are superseded by `BACKBURNER.md` until explicitly reopened.

See [`TESTING.md`](TESTING.md) for the complete hardware status, [`HARDWARE_VALIDATION.md`](HARDWARE_VALIDATION.md) for the validation record, and [`effects/humansoon-suite/ROADMAP.md`](effects/humansoon-suite/ROADMAP.md) for the clean-room effects roadmap.

## Pre-handoff engineering protocol

FIELD's internal-debugging workflow is now the suite-wide standard.

Before a normal hardware handoff, changed units should be exercised with deterministic tests using the **actual production DSP source** wherever possible: input-level cases, parameter extremes/sweeps, state delivery/expiry, buffer wraps, feedback/rest behavior, finite-output checks and unit-specific behavioral assertions. ARM compile/package still remains required, but compilation alone is not a normal handoff gate.

The intended DC handoff is now mostly:

1. load/select the `.ntkdigunit` on the physical NTS-1;
2. perform one short hardware-only worst-case/runtime sanity sweep identified in the handoff notes;
3. judge **tone, musicality, identity and playability**.

Actual MkI loading and real-time CPU/deadline margin cannot be certified by desktop tests, so those remain physical checks. The goal is to stop using DC's hardware time for bugs we can reproduce internally.

- [`PRE_HANDOFF_PROTOCOL.md`](PRE_HANDOFF_PROTOCOL.md) — suite-wide test and handoff requirements
- [`PRE_HANDOFF_COVERAGE.md`](PRE_HANDOFF_COVERAGE.md) — current per-unit coverage/backfill matrix
- [`A_CLASS_REQUIREMENTS.md`](A_CLASS_REQUIREMENTS.md) — exact requirements for deep A-class engineering coverage

## Community testing

The test system is intended to be reproducible outside the Human Soon development chat.

Community members can run the shared production-DSP host gate locally or through GitHub Actions, contribute deeper project-specific regression tests, and submit physical original-NTS-1 MkI reports using a standard template.

- [`COMMUNITY_TESTING.md`](COMMUNITY_TESTING.md) — local/CI testing instructions and contribution rules
- [`community/HARDWARE_TEST_REPORT.md`](community/HARDWARE_TEST_REPORT.md) — physical MkI report template

Current suite-wide baseline: **29/29 tracked units pass the shared production-DSP common gate and fresh ARM build/package gate.** This does not mean all 29 are A-class yet: most still need deeper unit-specific behavioral assertions. Engineering coverage and physical/music validation are deliberately tracked separately.

## LATTICE — active MkI system development

LATTICE is the current Human Soon generative phrase-and-space system for the original NTS-1 MkI.

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

The earlier three-custom-unit CORE + ECHO + SPACE architecture is historical and no longer the preferred MkI target.

### Current LATTICE state

- **CORE 0.3-0** — composer / microloop / freeze processor. Physical MkI testing shows distortion around the user's approximate **3 o'clock and higher** TIME region; lowering source level did not remove it. High-TIME hardware validation remains open.
- **FIELD 0.1-2** — finite clocked phrase + spatial bloom processor. Standalone listening behavior was reported as playing exactly as intended; overall hardware validation remains open.
- **Corrosion + FIELD** — reported clean in the tested playing context.
- **CORE + FIELD** — **FAIL / RETEST**; currently the supported pairing with the strongest reported distortion.
- **Albedo + FIELD** — not treated as a supported simultaneous pair on MkI because user DELAY and user REVERB share the first-generation SDK memory regions.
- **CORE 0.3-1** — proposed runtime diagnostic only; not implemented. The proposal caps ordinary microloop playback at 10 voices and suspends ordinary microloop work during full freeze.

Compile/CI success is not hardware validation. The current physical MkI evidence remains authoritative.

For current LATTICE work, read in this order:

1. [`LATTICE_CURRENT_STATUS.md`](LATTICE_CURRENT_STATUS.md) — fastest current-state snapshot
2. [`LATTICE_SYSTEM.md`](LATTICE_SYSTEM.md) — current architecture and doctrine
3. [`LATTICE_HISTORY.md`](LATTICE_HISTORY.md) — full historical context and engineering lessons
4. [`LATTICE_FIELD_SPEC.md`](LATTICE_FIELD_SPEC.md) — FIELD design/implementation guardrails
5. [`LATTICE_REPORTING.md`](LATTICE_REPORTING.md) — mandatory update-reporting protocol
6. [`reports/lattice/`](reports/lattice/) — chronological build, CI, diagnosis and hardware reports

Draft PR **#10** remains the active FIELD integration boundary into `lattice-suite` while CORE/FIELD hardware validation is unresolved.

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

1. begin from a strong audible behavior or interaction idea;
2. implement one bounded milestone;
3. create/extend deterministic production-DSP tests for the changed behavior;
4. reproduce and resolve internally reproducible failures before normal handoff;
5. compile against Korg's current original-NTS-1 template/API in GitHub Actions;
6. package `.ntkdigunit` and record text/data/BSS;
7. provide project-specific QA plus an exact handoff note stating only the remaining hardware/listening checks;
8. DC performs load/select, the identified brief hardware-runtime sanity check, and tone/musicality judgment;
9. only then expand/calibrate the DSP architecture.

The consolidated CI workflow `.github/workflows/build-full-test-suite.yml` rebuilds every current unit in one run, records text/data/BSS sizes, and packages the binaries plus QA sheets as one test artifact. The suite-wide testing roadmap now also tracks deterministic host coverage in `PRE_HANDOFF_COVERAGE.md`.

Any M2 or later DSP change reopens the hardware gate for the affected unit.

## Clean-room policy

The Human Soon suite may use public product categories and high-level interaction ideas as inspiration, but it does not copy commercial source, binaries, presets, UI text or proprietary implementation details, and it does not reverse engineer commercial units. A future design may study the broad musical behavior of a familiar device, reduce that behavior to a small NTS-1-suitable core, and then build an original Human Soon interpretation around it. See `BACKBURNER.md` for the current design reset.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Korg unit index](https://korginc.github.io/logue-sdk/unit-index/)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
