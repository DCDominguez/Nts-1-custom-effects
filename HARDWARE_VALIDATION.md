# Hardware Validation Record

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**

Validation date: **2026-09-09**

## Suite-level result

The project owner reported that **all 25 current M1 units work correctly on the physical NTS-1 MkI**, with no hardware defects reported.

This is a suite-level hardware validation record. It does not invent or backfill tester metadata that was not captured at the time of the report. Project-specific QA sheets remain in place for regression testing, future firmware changes and later DSP milestones.

## Validated units

### Core
- SPECTRA
- PARALLAX
- CHORDGHOST

### ModFX
- DUST
- CARRIER
- VECTORFILTER
- IRONROT
- ATTRACTOR
- ZEROCROSS
- PHASEWELL
- ASCENDER
- HELIX
- CAPSTAN
- SIDEBAND
- FAULTLINE

### DelFX
- BALLISTIC
- RAINFALL
- SWARMDELAY
- GLITCHREPEAT
- BUCKETLINE
- LONGMEMORY
- SHARD

### RevFX
- ABYSS
- AUREOLE
- NEBULA

## Status rule

These M1 builds may now be treated as **hardware validated** for the tested MkI target. Any later source change that alters DSP behavior, memory layout, parameter mapping or runtime safety reopens the applicable hardware gate and requires regression testing against that project's QA sheet.
