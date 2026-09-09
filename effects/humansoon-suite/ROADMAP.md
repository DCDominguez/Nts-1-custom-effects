# Human Soon Effects Suite — Roadmap

This suite studies broad DSP categories represented in the Korg logue ecosystem, including Sinevibes' public product categories, then implements **independent Human Soon designs from first principles**.

## Clean-room rule

- Do not copy Sinevibes source code, binaries, presets, UI text, or proprietary implementation details.
- Do not reverse engineer commercial binaries.
- Product names below appear only as public functional references.
- Human Soon units use original names, algorithms, control mappings and additional behavior.
- Korg's public logue SDK and published DSP literature / permissively licensed reference code may be used where appropriate and documented.

## Platform

Target first: **Korg Nu:Tekt NTS-1 digital kit MkI**, logue SDK API `1.1-0`.

Korg's SDK exposes separate `modfx`, `delfx`, and `revfx` runtimes for the original NTS-1. Custom effects use `num_param = 0` and receive the host controls through their runtime parameter callbacks. Delay and reverb units handle their own wet/dry balance through `SHIFT_DEPTH` in our designs.

## M1 completion status

**All currently planned Human Soon suite categories now have source, manifest/project overlays, CI compilation, and a project-specific physical MkI QA sheet.**

This means **M1 implementation/compile is complete; hardware validation is not.** No unit should be treated as production-validated until its QA sheet passes on the physical original NTS-1.

## Capability map

| Public functional reference | Broad DSP class | Human Soon project | Slot | M1 status / distinction |
|---|---|---|---|---|
| Blend | multi-voice chorus | **PARALLAX** | `delfx` | COMPILED / QA — independent timing field; true fixed-pitch voices remain a later milestone |
| Corrosion | distortion family | **IRONROT** | `modfx` | COMPILED / QA — CHARACTER + CORROSION; bounded tone-dependent erosion |
| DCM8 | sample-rate / bit reduction | **DUST** | `modfx` | COMPILED / QA — RATE + DAMAGE with bounded stereo clock fracture |
| Dipole | through-zero flanger | **ZEROCROSS** | `modfx` | COMPILED / QA — delayed reference enables actual zero-crossing relationship |
| Drift | chaotic stereo panner | **ATTRACTOR** | `modfx` | COMPILED / QA — coupled deterministic chaotic orbit rather than periodic LFO |
| Finite | granular pitch shifter | **SHARD** | `delfx` | COMPILED / QA — dual-grain pitch field; semitone calibration deferred |
| Ring | ring modulation | **CARRIER** | `modfx` | COMPILED / QA — dry→AM→ring POLARITY macro + stereo phase split |
| Shift | frequency shifter | **SIDEBAND** | `modfx` | COMPILED / QA — compact Hilbert/quadrature design; rejection quality must be measured |
| SVF | resonant state-variable filter | **VECTORFILTER** | `modfx` | COMPILED / QA — CUTOFF + LP→BP→HP VECTOR morph |
| Vibrant | deep phaser | **PHASEWELL** | `modfx` | COMPILED / QA — six-stage asymmetric stereo all-pass network |
| Whirl | barber-pole phaser | **ASCENDER** | `modfx` | COMPILED / QA — three crossfaded rising phase lanes |
| Eternal | barber-pole flanger | **HELIX** | `modfx` | COMPILED / QA — three windowed cyclic delay heads |
| Stator | tape wobble | **CAPSTAN** | `modfx` | COMPILED / QA — wow/flutter plus bounded deterministic wear/dropouts |
| Dispersion | bouncing/nonlinear delay | **BALLISTIC** | `delfx` | COMPILED / QA — accelerating/even/decelerating tap trajectory morph |
| Droplet | stochastic delay | **RAINFALL** | `delfx` | COMPILED / QA — independently retargeted droplet timing/stereo field |
| Time | wide delay / looper | **LONGMEMORY** | `delfx` | COMPILED / QA — delay-to-near-loop continuum with destructive aging |
| Isomer | tempo ensemble delay | **SWARMDELAY** | `delfx` | COMPILED / QA — BPM-derived four-tap constellation with microtiming divergence |
| Rerun | randomized repeater | **GLITCHREPEAT** | `delfx` | COMPILED / QA — bounded probability and finite repeat-state memory |
| Integer | digital/analog buffer delay | **BUCKETLINE** | `delfx` | COMPILED / QA — clock wobble, feedback loss and saturation AGE macro |
| Albedo | granular cloud reverb | **NEBULA** | `revfx` | COMPILED / QA — four-grain stochastic diffusion cloud |
| Hollow | large FDN reverb | **ABYSS** | `revfx` | COMPILED / QA — modulated four-line Hadamard-style FDN |
| Luminance | shimmer FDN reverb | **AUREOLE** | `revfx` | COMPILED / QA — bounded dual-grain octave feedback injected into FDN |
| Downgrade class | lo-fi degradation | **DUST** extension | `modfx` | Folded into DUST family instead of duplicating reducer architecture |
| Malfunction class | unstable/nonlinear filter | **FAULTLINE** | `modfx` | COMPILED / QA — deliberately unstable coefficient/feedback interaction with hard state bounds |

## Build waves

### Wave 1 — foundation ModFX — **M1 COMPLETE**
1. DUST
2. CARRIER
3. VECTORFILTER
4. IRONROT
5. ATTRACTOR

### Wave 2 — modulation networks — **M1 COMPLETE**
6. ZEROCROSS
7. PHASEWELL
8. ASCENDER
9. HELIX
10. CAPSTAN

Additional ModFX categories now also at M1: **SIDEBAND** and **FAULTLINE**.

### Wave 3 — delay / buffer processors — **M1 COMPLETE**
11. BALLISTIC
12. RAINFALL
13. SWARMDELAY
14. GLITCHREPEAT
15. BUCKETLINE
16. LONGMEMORY
17. SHARD

### Wave 4 — reverbs — **M1 COMPLETE**
18. ABYSS
19. AUREOLE
20. NEBULA

## Shared development rules

- Every unit must compile in GitHub Actions before hardware testing.
- Every unit gets a physical MkI test gate before feature expansion.
- Every READY FOR TEST unit must have a project-specific QA sheet.
- Parameter changes must be smoothed where discontinuities can click or explode.
- Feedback structures must have explicit gain bounds and runaway protection.
- Prefer one strong musical macro per physical control over exposing implementation detail.
- Measure code/data/BSS for every full-suite build.
- Reuse utility code only after it has passed at least one hardware test.

## Next phase

The software-development queue is now blocked intentionally behind **physical MkI QA**. Failures should be fixed unit-by-unit; successful M1 units can then advance to refinement, musical calibration, CPU profiling, and integration tests.
