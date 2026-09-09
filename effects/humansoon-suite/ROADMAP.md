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

**All 25 current M1 units now have source, manifest/project overlays, CI compilation, project-specific physical MkI QA sheets, and a successful physical MkI validation report.**

M1 is the frozen known-good baseline. Any later DSP change that alters behavior, parameter mapping, memory layout, or runtime safety reopens that unit's hardware gate and requires regression testing.

For practical descriptions and control instructions, see [`../../USER_GUIDE.md`](../../USER_GUIDE.md).

## Capability map

| Public functional reference | Broad DSP class | Human Soon project | Slot | M1 distinction |
|---|---|---|---|---|
| Blend | multi-voice chorus | **PARALLAX** | `delfx` | Hardware validated — independent timing field; true fixed-pitch voices remain a later milestone |
| Corrosion | distortion family | **IRONROT** | `modfx` | Hardware validated — CHARACTER + CORROSION; bounded tone-dependent erosion |
| DCM8 | sample-rate / bit reduction | **DUST** | `modfx` | Hardware validated — RATE + DAMAGE with bounded stereo clock fracture |
| Dipole | through-zero flanger | **ZEROCROSS** | `modfx` | Hardware validated — delayed reference enables actual zero-crossing relationship |
| Drift | chaotic stereo panner | **ATTRACTOR** | `modfx` | Hardware validated — coupled deterministic chaotic orbit rather than periodic LFO |
| Finite | granular pitch shifter | **SHARD** | `delfx` | Hardware validated — dual-grain pitch field; semitone calibration remains a refinement target |
| Ring | ring modulation | **CARRIER** | `modfx` | Hardware validated — dry→AM→ring POLARITY macro + stereo phase split |
| Shift | frequency shifter | **SIDEBAND** | `modfx` | Hardware validated — compact Hilbert/quadrature design |
| SVF | resonant state-variable filter | **VECTORFILTER** | `modfx` | Hardware validated — CUTOFF + LP→BP→HP VECTOR morph |
| Vibrant | deep phaser | **PHASEWELL** | `modfx` | Hardware validated — six-stage asymmetric stereo all-pass network |
| Whirl | barber-pole phaser | **ASCENDER** | `modfx` | Hardware validated — three crossfaded rising phase lanes |
| Eternal | barber-pole flanger | **HELIX** | `modfx` | Hardware validated — three windowed cyclic delay heads |
| Stator | tape wobble | **CAPSTAN** | `modfx` | Hardware validated — wow/flutter plus bounded deterministic wear/dropouts |
| Dispersion | bouncing/nonlinear delay | **BALLISTIC** | `delfx` | Hardware validated — accelerating/even/decelerating tap trajectory morph |
| Droplet | stochastic delay | **RAINFALL** | `delfx` | Hardware validated — independently retargeted droplet timing/stereo field |
| Time | wide delay / looper | **LONGMEMORY** | `delfx` | Hardware validated — delay-to-near-loop continuum with destructive aging |
| Isomer | tempo ensemble delay | **SWARMDELAY** | `delfx` | Hardware validated — BPM-derived four-tap constellation with microtiming divergence |
| Rerun | randomized repeater | **GLITCHREPEAT** | `delfx` | Hardware validated — bounded probability and finite repeat-state memory |
| Integer | digital/analog buffer delay | **BUCKETLINE** | `delfx` | Hardware validated — clock wobble, feedback loss and saturation AGE macro |
| Albedo | granular cloud reverb | **NEBULA** | `revfx` | Hardware validated — four-grain stochastic diffusion cloud |
| Hollow | large FDN reverb | **ABYSS** | `revfx` | Hardware validated — modulated four-line Hadamard-style FDN |
| Luminance | shimmer FDN reverb | **AUREOLE** | `revfx` | Hardware validated — bounded dual-grain octave feedback injected into FDN |
| Downgrade class | lo-fi degradation | **DUST** extension | `modfx` | Folded into DUST family instead of duplicating reducer architecture |
| Malfunction class | unstable/nonlinear filter | **FAULTLINE** | `modfx` | Hardware validated — deliberately unstable coefficient/feedback interaction with hard state bounds |

## Build waves

### Wave 1 — foundation ModFX — **M1 HARDWARE VALIDATED**
1. DUST
2. CARRIER
3. VECTORFILTER
4. IRONROT
5. ATTRACTOR

### Wave 2 — modulation networks — **M1 HARDWARE VALIDATED**
6. ZEROCROSS
7. PHASEWELL
8. ASCENDER
9. HELIX
10. CAPSTAN

Additional ModFX categories also validated at M1: **SIDEBAND** and **FAULTLINE**.

### Wave 3 — delay / buffer processors — **M1 HARDWARE VALIDATED**
11. BALLISTIC
12. RAINFALL
13. SWARMDELAY
14. GLITCHREPEAT
15. BUCKETLINE
16. LONGMEMORY
17. SHARD

### Wave 4 — reverbs — **M1 HARDWARE VALIDATED**
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

## Milestone convention from M2 onward

The exact feature differs by project, but the suite uses this shared interpretation:

- **M2 — Core capability:** add the important DSP feature that moves beyond the M1 proof of concept.
- **M3 — Human Soon behavior:** add the characteristic controlled instability, stochastic behavior, alternate modes, nonlinear interactions, or evolving relationships that give the effect its own identity.
- **M4 — Instrument/release tuning:** hardware calibration, parameter ranges, gain staging, CPU optimization, smoothing, aliasing cleanup, extreme-setting behavior, default character, and integration testing.

Project-specific specs may subdivide or extend these milestones where necessary.

## Next phase

M1 is no longer blocked. Successful projects may now advance one at a time into M2 development. Each M2 candidate must compile, receive an updated QA sheet, and pass physical MkI regression testing before M3 begins.
