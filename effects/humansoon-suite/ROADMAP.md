# Human Soon Effects Suite — Roadmap

This suite studies the broad DSP categories represented by Sinevibes' Korg logue effects, then implements **independent Human Soon designs from first principles**.

## Clean-room rule

- Do not copy Sinevibes source code, binaries, presets, UI text, or proprietary implementation details.
- Do not reverse engineer commercial binaries.
- Product names below appear only as public functional references.
- Our units use original names, algorithms, parameter mappings, and additional behavior.
- Korg's public logue SDK and published DSP literature / permissively licensed reference code may be used where appropriate and documented.

## Platform

Target first: **Korg Nu:Tekt NTS-1 digital kit MkI**, logue SDK API `1.1-0`.

Korg's SDK exposes separate `modfx`, `delfx`, and `revfx` project templates for the original NTS-1. Custom effects have `num_param = 0` in their manifest; host effect controls arrive through the corresponding runtime parameter callback.

## Reference baseline

Korg's public unit index (last updated 2025-02-26) lists 22 Sinevibes effects for the original NTS-1 across modulation, delay, and reverb slots. Recent 2026 reporting also describes two newer Sinevibes logue effects, Downgrade and Malfunction; keep those as reference-watch items until the official index catches up.

## Capability map

| Public reference | Broad DSP class | Human Soon project | Intended slot | Status / distinction |
|---|---|---|---|---|
| Blend | multi-voice chorus | **PARALLAX** | `delfx` | Already built; spatially independent voices rather than a direct chorus recreation |
| Corrosion | multi-algorithm distortion | **IRONROT** | `modfx` | Planned: nonlinear stages + tone-dependent mutation |
| DCM8 | sample-rate / bit-depth reduction | **DUST** | `modfx` | **M1 compiled; ready for MkI QA.** RATE + DAMAGE macros with bounded stereo clock fracture |
| Dipole | through-zero flanger | **ZEROCROSS** | `modfx` | Planned: true through-zero sweep with asymmetric feedback |
| Drift | chaotic stereo panner | **ATTRACTOR** | `modfx` | Planned: deterministic chaotic motion with controllable orbit geometry |
| Finite | granular pitch shifter | **SHARD** | `modfx` or `delfx` after profiling | Planned: multi-grain pitch field, not a direct clone |
| Ring | ring modulation | **CARRIER** | `modfx` | **M1 compiled; ready for MkI QA.** Dry→AM→ring POLARITY macro + bounded stereo carrier-phase split |
| Shift | frequency shifter | **SIDEBAND** | `modfx` | Planned: quadrature frequency shift + stereo divergence |
| SVF | resonant state-variable filter | **VECTORFILTER** | `modfx` | Planned: morphing multimode filter with controlled instability |
| Vibrant | deep phaser | **PHASEWELL** | `modfx` | Planned: asymmetric all-pass network |
| Whirl | barber-pole phaser | **ASCENDER** | `modfx` | Planned: continuously rising/falling phase illusion |
| Eternal | barber-pole flanger | **HELIX** | `modfx` | Planned: multi-head cyclic flanger illusion |
| Stator | tape wobble | **CAPSTAN** | `modfx` | Planned: wow/flutter/dropout model with correlated drift |
| Dispersion | bouncing-ball delay | **BALLISTIC** | `delfx` | Planned: accelerating/decelerating repeat trajectories |
| Droplet | rain-drop delay | **RAINFALL** | `delfx` | Planned: stochastic droplet timing / stereo / damping |
| Time | wide-range delay / looper | **LONGMEMORY** | `delfx` | Planned: delay-to-loop continuum with destructive ageing |
| Isomer | tempo-synced ensemble delay | **SWARMDELAY** | `delfx` | Planned: clocked multi-tap ensemble with independent microtiming |
| Rerun | self-randomizing repeater | **GLITCHREPEAT** | `delfx` | Planned: bounded probability repeater with repeat-state memory |
| Integer | digital/analog buffer delay | **BUCKETLINE** | `delfx` | Planned: digital buffer with analog-style loss / clock modulation |
| Albedo | granular cloud reverb | **NEBULA** | `revfx` | Planned: granular diffusion cloud with density transitions |
| Hollow | large FDN reverb | **ABYSS** | `revfx` | Planned: modulated FDN for huge non-metallic spaces |
| Luminance | shimmer FDN reverb | **AUREOLE** | `revfx` | Planned: FDN + pitched feedback with bounded shimmer |
| Downgrade (2026 watch) | lo-fi degradation | **DUST** extension | `modfx` | Folded into DUST family rather than duplicating the reducer class |
| Malfunction (2026 watch) | unstable / nonlinear filter | **FAULTLINE** | `modfx` | Planned: deliberately unstable but bounded filter topology |

## Build order

### Wave 1 — low-risk modulation processors

1. **DUST** — bit-depth + sample-rate reduction — **M1 compiled / ready for QA**
2. **CARRIER** — ring / amplitude modulation — **M1 compiled / ready for QA**
3. **VECTORFILTER** — state-variable filter
4. **IRONROT** — distortion family
5. **ATTRACTOR** — chaotic stereo panner

Purpose: establish a reusable `modfx` project template, parameter smoothing, stereo utilities, saturation / safety utilities, and automated compilation before heavier time-domain effects.

### Wave 2 — modulation networks

6. **ZEROCROSS** — through-zero flanger
7. **PHASEWELL** — phaser
8. **ASCENDER** — barber-pole phaser
9. **HELIX** — barber-pole flanger
10. **CAPSTAN** — tape motion

### Wave 3 — delay / buffer processors

11. **BALLISTIC**
12. **RAINFALL**
13. **SWARMDELAY**
14. **GLITCHREPEAT**
15. **BUCKETLINE**
16. **LONGMEMORY**
17. **SHARD**

### Wave 4 — reverbs

18. **ABYSS**
19. **AUREOLE**
20. **NEBULA**

Reverbs and granular units come last because they are the strongest SRAM/SDRAM and CPU-risk projects on the MkI.

## Shared development rules

- Every unit must compile in GitHub Actions before hardware testing.
- Every unit gets a physical MkI test gate before feature expansion.
- Every READY FOR TEST unit must have a project-specific QA sheet.
- Parameter changes must be smoothed where discontinuities can click or explode.
- Feedback structures must have explicit gain bounds and runaway protection.
- Prefer one strong musical macro per physical control over exposing implementation detail.
- Measure code/data/BSS and inspect linker maps for every build.
- Reuse utility code only after it has passed at least one hardware test.
