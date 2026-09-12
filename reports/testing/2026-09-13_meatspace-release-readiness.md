# Meatspace calibration + community release readiness — 2026-09-13

## Physical report received

Target: original Korg Nu:Tekt NTS-1 digital kit / MkI.

Latest owner report after bulk meatspace testing:

- level calibration: **PASS across the tested suite**;
- **ASCENDER** remains the exception: the intended motion is still not clearly audible on the previously tested candidate;
- **LATTICE** still shows combination trouble when CORE is used with other LATTICE components on higher-register material;
- the same LATTICE combinations behave normally on bass/lower-register material or when oscillator shape shifts the source spectrum downward.

This observation supersedes the earlier suite-wide level-calibration backlog for the tested candidates. It does not turn changed binaries into physical passes.

## Follow-up development branch

Branch: `meatspace-register-ascender-fix`

Changes under test:

### ASCENDER 0.2-0

- retain a permanent dry reference instead of reaching effectively 100% wet;
- increase useful climb-rate range;
- add small bounded feedback and a wider stereo phase offset;
- add an A-class perceptual proxy requiring materially obvious moving notch/envelope behavior at high DEPTH.

Physical MkI retest remains required.

### LATTICE CORE 0.3-2

The higher-register observation points away from a simple CPU-only explanation because source register changes the failure while the processor topology/workload is largely unchanged.

Focused mitigation:

- cap stochastic upward history playback at **+12 semitones** instead of allowing +17/+19/+24-semitone events;
- retain downward movement to -24 semitones;
- condition only the captured wet history with a fixed two-pole 9 kHz Butterworth low-pass before variable-rate playback;
- keep the live/dry path full-band;
- add regression coverage for the upward-rate ceiling, capture-filter response, and a harmonically rich high-register source.

Physical MkI retest remains required, especially CORE + FIELD / CORE + SPACE with the same high-register source that exposed the fault.

## Community-release view

Korg's current NTS-1/logue SDK documentation supports third-party distribution of `.ntkdigunit` user units. Community packaging should explicitly state the target as original NTS-1 MkI / logue SDK API 1.1-0 and should not imply validation on other logue products without hardware testing.

### Comfortable release set

For a first public **stable/RC community pack**, include the current physically tested non-LATTICE units **except ASCENDER**. The latest bulk test reports level calibration successful, while prior reports already established load/runtime/musical operation for these units.

Candidate families:

- Oscillators: SPECTRA, PARALLAX, CHORDGHOST
- ModFX: DUST, CARRIER, VECTORFILTER, IRONROT, ATTRACTOR, ZEROCROSS, PHASEWELL, HELIX, CAPSTAN, SIDEBAND, FAULTLINE
- DelFX: BALLISTIC, RAINFALL, SWARMDELAY, GLITCHREPEAT, BUCKETLINE, LONGMEMORY, SHARD
- RevFX: ABYSS, AUREOLE, NEBULA

This is 24 units if packaged as one suite. A smaller curated Wave 1 is still preferable operationally because it reduces installation/support burden and makes community feedback easier to interpret.

### Hold from stable release

- ASCENDER 0.2-0 — changed DSP; physical musical retest required.
- LATTICE CORE 0.3-2 — changed DSP; high-register physical retest required.
- LATTICE SPACE — retain as development/lab material until the current LATTICE chain is rechecked with the new CORE and higher-register sources.
- LATTICE FIELD — standalone physical behavior is strong, but keep it with the LATTICE lab/theme release until the theme's interaction contract is settled.
- historical LATTICE ECHO — do not promote as the preferred LATTICE delay while FIELD is the reference stage.

## Recommended release structure

1. **Human Soon Community Pack v0.1 / MkI** — curated, physically validated, no known open musical failures.
2. **Human Soon Labs** — LATTICE and newly revoiced candidates with explicit RETEST/BETA labels.
3. Every release artifact should include unit version, target/API, exact binary hash, control summary, known limitations, and a short hardware-report link/template.

Do not use A-class engineering PASS alone as the public stability claim. Public stable/RC status requires the exact binary to have BUILD + LOAD + RUNTIME + MUSICAL evidence on physical MkI.
