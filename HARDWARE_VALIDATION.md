# Hardware Validation Record

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**

## Latest focused LATTICE follow-up — 2026-09-12

### LATTICE CORE 0.3-1

Exact handed-off binary SHA-256: `1d91d643775e26bb698cd24c0980109561ebfbc18c5811c19c15d067ddaddaee`  
Pre-handoff workflow: `34628866540`

The focused physical regression covered the two checks requested at handoff:

1. CORE 0.3-1 standalone through upper/max TIME, including the previous intermittent-distortion region and max-TIME loop/freeze;
2. CORE 0.3-1 + FIELD 0.1-2 through the upper/max-TIME region.

Tester report: **“okay pass for both.”**

Disposition:

- CORE 0.3-1 standalone focused playing test: **PASS**
- CORE 0.3-1 + FIELD 0.1-2 focused combination test: **PASS**
- CORE 0.3-1 is the current MkI reference unless later physical evidence reopens it.

No 30-minute soak or unreported effect combinations are inferred.

See `reports/lattice/2026-09-12_core-0.3-1-physical-pass.md`.

### LATTICE SPACE 0.4-0

Exact handed-off binary SHA-256: `de4dd4c7ccef1c435939b0c617df5a12532b29a4d3bf8af061604c5e46405bc7`  
Pre-handoff workflow: `34635428252`

The tester reports that SPACE 0.4-0 still distorts in combination with modulation:

> “still hitting distortion on core with space. actually any modulation plus space i get distortion”

Disposition:

- SPACE 0.4-0 + CORE 0.3-1: **FAIL / RETEST** due to distortion;
- more broadly, tester reports the same distortion with **any modulation + SPACE** in the current physical test;
- this does **not** reopen CORE 0.3-1 by itself because CORE 0.3-1 separately passed standalone and CORE + FIELD 0.1-2 immediately beforehand;
- no exact CPU/deadline, clipping, guard or analog-level cause is inferred from the sound alone.

SPACE 0.4-0 is superseded for development by the focused 0.4-1 guard/headroom diagnostic. The 0.4-1 physical gate is open.

See `reports/lattice/2026-09-12_space-0.4-1-guard-isolation.md` and issue #9 comment `5639386820`.

### Earlier LATTICE SPACE 0.3-0 characterization

The earlier 0.3-0 candidate was physically characterized as:

- load/basic function PASS;
- too subtle even at full MIX;
- echo/spatial behavior audible;
- distorted sound quality when run with modulation;
- redesign/revoice required.

The 0.4-0 revoice solved its engineering/presence gates but **did not solve the physical modulation-combination distortion**, as recorded above.

---

## A-class release-candidate physical test — 2026-09-12

Candidate engineering commit: `5718d12416a242137892688e240e8ac41be5a0ca`  
Pre-handoff workflow: `34613566166`

The tester loaded the A-class release-candidate pack on the physical original NTS-1 MkI. Units not specifically called out below were reported to **load properly and sound as intended**.

### Suite-wide result

- **All tested units loaded and functioned.**
- Most units were musically successful in the reported playing test.
- The principal remaining suite-wide issue is **perceived level calibration** rather than basic functionality.
- Delays and reverbs generally require the NTS-1 main volume to be raised very high for a comfortable/enjoyable listening level.
- ModFX also need a level-consistency pass.

### Specific notes

#### SPECTRA

- Prior 0.2-0 hardware result: 1–3 voices sounded great; 4 voices degraded the sound.
- SPECTRA was therefore revised to **0.2-1**, capped at three internal voices.
- Exact SPECTRA 0.2-1 follow-up: tester reported **“spectra runs great.”**
- Current disposition: **LOAD / RUNTIME / MUSICAL PASS** for the reported 0.2-1 playing test; three voices is the approved MkI maximum.

#### Reverb

- **LATTICE SPACE** — the 0.3-0 and exact 0.4-0 candidates both physically exhibited distortion when run with modulation; 0.4-0 tester report broadens this to “any modulation plus space” in the current test. Current development candidate is 0.4-1; physical gate open.
- **Other reverbs** — tested well, but perceived output level is too low.

#### Delay

- **All tested delays work.**
- Family-wide perceived output is too low; level/mix calibration is required.

#### Modulation

- **ASCENDER** — barely noticeable.
- **ATTRACTOR** — barely noticeable.
- **CAPSTAN** — barely noticeable.
- **IRONROT** — sounds great, but can become quite loud.
- **LATTICE CORE 0.3-0** — sounded great; maximum TIME looped the note, with intermittent distortion. This exact finding is superseded for current development by the successful focused CORE 0.3-1 retest above.
- Other modulation effects not listed above were reported to load and sound as intended.

### Combination testing

Most ModFX + Delay/Reverb combinations tested well in the original RC pass. Later focused testing established:

- CORE 0.3-1 + FIELD 0.1-2: **PASS**;
- SPACE 0.4-0 after modulation: **FAIL / RETEST**, with tester reporting distortion for any modulation + SPACE in the current test.

### Status rule

This physical report is separate from the repository's A-class engineering grade. A-class host/build PASS does not erase physical findings.

Any DSP change made to address level, identity or distortion reopens the physical gate for that exact changed candidate.

See `reports/testing/2026-09-12_a-class-rc-physical-mki.md`, `reports/testing/2026-09-12_spectra-three-voice-cap.md`, `reports/lattice/2026-09-12_a-class-rc-lattice-hardware.md`, `reports/lattice/2026-09-12_core-0.3-1-physical-pass.md`, and `reports/lattice/2026-09-12_space-0.4-1-guard-isolation.md`.

---

## Original suite-level validation — 2026-09-09

The project owner reported that **all 25 current M1 units work correctly on the physical NTS-1 MkI**, with no hardware defects reported at that time.

This original suite-level record remains useful historical evidence, but later listening and newer candidates supersede it where more specific findings exist.

### Validated units

#### Core
- SPECTRA
- PARALLAX
- CHORDGHOST

#### ModFX
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

#### DelFX
- BALLISTIC
- RAINFALL
- SWARMDELAY
- GLITCHREPEAT
- BUCKETLINE
- LONGMEMORY
- SHARD

#### RevFX
- ABYSS
- AUREOLE
- NEBULA

## General validation rule

Hardware validation applies to the tested build/candidate only. Any later source change that alters DSP behavior, memory layout, parameter mapping, output level, or runtime safety reopens the applicable hardware gate and requires regression testing.
