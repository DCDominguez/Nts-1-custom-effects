# Hardware Testing Status

Target hardware: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

## Current engineering gate

All **29 tracked units** pass the repository's A-class pre-handoff engineering standard on candidate `5718d12416a242137892688e240e8ac41be5a0ca`.

Final consolidated workflow: **Pre-handoff suite**, run `34613566166`.

- shared production-DSP common gate: **29/29 PASS**;
- project-specific A-class harnesses: **29/29 PASS**;
- fresh ARM compile/package: **29/29 PASS**;
- sanitizer failures in exercised host paths: none.

A-class is an engineering-coverage grade. It does not replace physical MkI validation.

Read:

- [`PRE_HANDOFF_PROTOCOL.md`](PRE_HANDOFF_PROTOCOL.md)
- [`PRE_HANDOFF_COVERAGE.md`](PRE_HANDOFF_COVERAGE.md)
- [`A_CLASS_REQUIREMENTS.md`](A_CLASS_REQUIREMENTS.md)
- [`reports/testing/2026-09-11_full-a-class-suite.md`](reports/testing/2026-09-11_full-a-class-suite.md)

## Latest physical MkI test — 2026-09-12

The A-class release-candidate pack was loaded and played on the physical original NTS-1 MkI.

Tester rule for this pass: **if a unit was not specifically called out, it loaded properly and sounded as intended.**

### Reverb

- All tested reverbs loaded and worked.
- Except for LATTICE SPACE, the reverbs tested well musically.
- Common issue: perceived output level is too low; the NTS-1 main volume must be raised very high for a comfortable/enjoyable level.
- **LATTICE SPACE:** very subtle even at full MIX; audible echo/spatial behavior remains, but it distorts when run with modulation. Redesign/revoice candidate.

### Delay

- All tested delays work.
- Common issue: perceived output level is too low; same family-level calibration problem as reverb.

### ModFX

Most modulation effects loaded and sounded as intended.

Specific physical notes:

- **ASCENDER:** barely noticeable.
- **ATTRACTOR:** barely noticeable.
- **CAPSTAN:** barely noticeable.
- **IRONROT:** sounds great but can become quite loud.
- **LATTICE CORE:** sounds great; maximum TIME loops the note, with intermittent distortion. Distortion can also occur in some multi-effect use, although some runs are clean.

### Combination tests

Most modulation + delay/reverb pairings tested well. LATTICE CORE remains the primary intermittent exception.

### Current physical disposition

- Unlisted units: **LOAD PASS / RUNTIME PASS / MUSICAL PASS for the reported playing test**.
- Delay family: functional/musical PASS with **level-calibration notes**.
- Reverb family except LATTICE SPACE: functional/musical PASS with **level-calibration notes**.
- ASCENDER / ATTRACTOR / CAPSTAN: functional PASS, **musical audibility retune required**.
- IRONROT: functional/musical PASS, **level retune required**.
- LATTICE CORE: **PASS WITH NOTES / RETEST** for intermittent extreme-TIME/combination distortion.
- LATTICE SPACE: **FAIL / REDESIGN CANDIDATE** for musical identity and combination robustness.

See:

- [`reports/testing/2026-09-12_a-class-rc-physical-mki.md`](reports/testing/2026-09-12_a-class-rc-physical-mki.md)
- [`reports/lattice/2026-09-12_a-class-rc-lattice-hardware.md`](reports/lattice/2026-09-12_a-class-rc-lattice-hardware.md)
- issue **#11** for suite-wide output-level/standalone-identity calibration
- issue **#9** for LATTICE-specific runtime/history

## Handoff policy

The normal handoff is now deliberately small:

1. load/select the candidate on the physical NTS-1;
2. perform the specifically identified short hardware-runtime sanity check;
3. judge tone, musicality, identity and playability.

Compilation alone is not sufficient for normal handoff. Actual MkI loading and real-time deadline margin remain hardware-only facts.

## Testing doctrine

Every changed candidate should pass two separate layers:

### Internal engineering gate

- actual production DSP;
- deterministic input corpus;
- parameter extrema and sweeps;
- state ownership/expiry;
- wrap/feedback safety;
- finite output/rest behavior;
- project-specific behavioral assertions;
- fresh ARM build/package.

### Physical/music gate

- actual MkI load/select;
- short identified worst-case runtime check;
- tone/musicality/playability judgment;
- combination test where relevant.

A high internal activity count is not success unless the intended result reaches output. A host PASS never erases a real hardware failure.

## Next testing layer: level calibration

The latest hardware pass identified a new gap in the engineering suite: **functional A-class coverage does not yet guarantee useful perceived output level or family-wide loudness consistency**.

Next protocol work should therefore add reproducible level/loudness proxy measurements while keeping listening judgment authoritative.

Goals:

- detect obviously under-level Delay/Reverb candidates before handoff;
- detect overly subtle ModFX identity;
- detect over-loud units such as current IRONROT behavior;
- preserve dynamics and effect identity rather than normalizing every processor to identical loudness;
- avoid solving audibility by driving the whole wet bus into clipping/guard activity.

Any gain/mix-curve change reopens the physical gate for the exact affected build.

## Historical record

Earlier M1/M2 physical results remain preserved in project QA sheets and `HARDWARE_VALIDATION.md`. When older status conflicts with the 2026-09-12 exact-candidate report, the newer exact-candidate physical observation wins.
