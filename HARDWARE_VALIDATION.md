# Hardware Validation Record

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**

## Latest A-class release-candidate physical test — 2026-09-12

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

- **LATTICE SPACE** — very subtle even at full MIX; echo/spatial behavior is audible, but it distorts when run with modulation. **Redesign/revoice candidate.**
- **Other reverbs** — tested well, but perceived output level is too low.

#### Delay

- **All tested delays work.**
- Family-wide perceived output is too low; level/mix calibration is required.

#### Modulation

- **ASCENDER** — barely noticeable.
- **ATTRACTOR** — barely noticeable.
- **CAPSTAN** — barely noticeable.
- **IRONROT** — sounds great, but can become quite loud.
- **LATTICE CORE** — sounds great; maximum TIME loops the note, with intermittent distortion. Distortion can also occur in some combinations with another effect, although some runs are clean.
- Other modulation effects not listed above were reported to load and sound as intended.

### Combination testing

Most ModFX + Delay/Reverb combinations tested well. The main recurring exception is LATTICE CORE, where intermittent distortion remains possible.

### Status rule

This physical report is separate from the repository's A-class engineering grade. A-class host/build PASS does not erase physical findings.

Any DSP change made to address level, identity or distortion reopens the physical gate for that exact changed candidate.

See `reports/testing/2026-09-12_a-class-rc-physical-mki.md`, `reports/testing/2026-09-12_spectra-three-voice-cap.md`, and `reports/lattice/2026-09-12_a-class-rc-lattice-hardware.md`.

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
