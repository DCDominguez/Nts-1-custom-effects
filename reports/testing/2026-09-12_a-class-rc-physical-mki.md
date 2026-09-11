# A-class release-candidate physical MkI test — 2026-09-12

## Status

- Target: original Korg Nu:Tekt NTS-1 digital kit MkI
- Candidate branch: `field-0.1-0-test`
- A-class engineering candidate: `5718d12416a242137892688e240e8ac41be5a0ca`
- Pre-handoff workflow: `34613566166`
- ARM artifact: `10269557698` (`pre-handoff-arm-builds`)
- Engineering coverage before handoff: 29/29 shared common gate PASS; 29/29 project-specific A-class harness PASS; 29/29 ARM build/package PASS
- Physical tester report: DC
- Classification: **PASS WITH NOTES / RETEST FOR SPECIFIC UNITS**

## Scope of the tester report

The tester loaded the A-class release-candidate pack on the physical original NTS-1 MkI and reported that units not specifically called out below loaded correctly and sounded as intended.

This report preserves the tester's observations without turning subjective level/tone comments into unmeasured numerical claims.

## SPECTRA

- SPECTRA loaded and the tester reported that it sounds great overall.
- 1–3 internal voices were reported as working well / musically successful.
- At 4 voices the sound starts becoming bad.
- Decision: cap SPECTRA at 3 voices for the MkI release line rather than trying to preserve a fourth voice at the cost of the approved sound.
- Follow-up candidate: SPECTRA `0.2-1`, with a hard 1–3 voice range. See `reports/testing/2026-09-12_spectra-three-voice-cap.md`.

## Suite-wide observations

### Reverb family

- The reverbs loaded and functioned.
- Except for LATTICE SPACE, the reverbs tested well in tone/behavior.
- Common issue: perceived output level is too low; the tester reported needing the NTS-1 main volume near maximum to reach a comfortable/enjoyable listening level.
- This is now a suite-level voicing/gain-structure issue, not a functional-load failure.

### Delay family

- All tested delays were reported working.
- Common issue: perceived output level is too low; the tester reported needing the NTS-1 main volume near maximum to hear them at a comfortable/enjoyable level.
- This is a family-wide output/gain-structure tuning target.

### Modulation family

Most modulation effects loaded and behaved as intended, but overall level consistency needs another pass.

Specific notes:

- **ASCENDER** — effect is barely noticeable.
- **ATTRACTOR** — effect is barely noticeable.
- **CAPSTAN** — effect is barely noticeable.
- **IRONROT** — sounds great, but can become quite loud.
- **LATTICE CORE** — sounds great; with TIME at maximum it loops the note, with intermittent distortion reported.

The combination tests indicate most modulation + delay/reverb pairings work well. LATTICE CORE remains the main exception: distortion occurs at times when paired with another effect, although some runs are clean.

## LATTICE SPACE

Tester report:

- very subtle even at full MIX;
- the echo/spatial effect is audible;
- distorted sound quality when run with modulation;
- described as needing a rework rather than merely a level increase.

Disposition: **FAIL / REDESIGN CANDIDATE** for musical quality/combination robustness, while load/basic function passes.

## Units not listed above

The tester explicitly stated that units not listed in the notes tested well: they loaded properly and sounded as intended.

Therefore, for this candidate, unlisted units receive:

- **LOAD PASS**
- **RUNTIME PASS for the reported playing test**
- **MUSICAL PASS for the reported playing test**

This is not a 30-minute soak certification unless separately reported.

## What this changes

The A-class engineering grade remains separate and intact. The new physical test primarily identifies a **post-A-class calibration layer**:

1. preserve SPECTRA's approved sound by limiting it to 3 voices;
2. family-wide perceived-level normalization for Delay and Reverb;
3. family-wide level consistency review for ModFX;
4. increase audible identity for ASCENDER, ATTRACTOR and CAPSTAN;
5. tame IRONROT's loudness without losing its newly successful corrosion character;
6. redesign/revoice LATTICE SPACE;
7. continue isolating LATTICE CORE high-TIME / combination distortion.

## Recommended development order

1. Lock SPECTRA to the approved 3-voice ceiling and re-run its exact-build engineering/hardware gate.
2. Build a reproducible loudness/level measurement protocol that supplements the existing peak/bounds tests without replacing listening judgment.
3. Calibrate Delay and Reverb output/mix curves as families, preserving each effect's identity and dry path.
4. Calibrate ModFX wet contribution / apparent loudness, with dedicated targets for ASCENDER, ATTRACTOR, CAPSTAN and IRONROT.
5. Rework LATTICE SPACE separately; do not treat it as a simple gain-fix candidate.
6. Keep LATTICE CORE combination/runtime distortion as an independent diagnostic problem.

Any DSP change made from these findings reopens the affected unit's exact-build physical gate and must retain A-class automated coverage.
