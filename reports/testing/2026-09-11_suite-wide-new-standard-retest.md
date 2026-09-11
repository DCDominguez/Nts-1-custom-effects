# Suite-wide new-standard pre-handoff retest — 2026-09-11

## Scope

Retest all 29 tracked Human Soon NTS-1 units against the new pre-handoff engineering standard as far as can be established without the physical original NTS-1 MkI.

This run deliberately separates:

- **host production-DSP correctness**;
- **fresh ARM build/package compatibility against the current official Korg logue SDK**;
- **hardware-only and listening-only checks**.

No production DSP source was changed during this retest.

## New shared test infrastructure

Added:

- `tests/pre-handoff/run_suite.py`
- `tests/pre-handoff/stubs/userosc.h`
- `tests/pre-handoff/stubs/usermodfx.h`
- `tests/pre-handoff/stubs/userdelfx.h`
- `tests/pre-handoff/stubs/userrevfx.h`
- `.github/workflows/pre-handoff-suite.yml`

The host runner compiles the actual production DSP source under AddressSanitizer + UndefinedBehaviorSanitizer and exercises deterministic input, state, parameter-sweep, reset/rest, delivery and soak checks by unit type.

The workflow separately performs fresh ARM builds and `.ntkdigunit` packaging against a newly cloned current Korg logue SDK.

## First host run and protocol corrections

The first common-gate run found three apparent failures:

- `ZEROCROSS` — normal-delivery check failed because the generic test inspected only the first 64-frame block while ZEROCROSS intentionally reads delayed history around 170 samples.
- `GLITCHREPEAT` — impulse-only delivery check was unsuitable for its beat-bound capture/repeat behavior.
- `LATTICE FIELD` — impulse-only delivery check was unsuitable for FIELD's capture/admission architecture.

Source inspection showed these were **test-protocol false negatives**, not demonstrated DSP regressions.

The common harness was corrected to:

- allow latency-bearing ModFX enough warm-up before asserting delivered output;
- use a sustained deterministic source/capture phase for DelFX before measuring post-source delayed/captured delivery;
- retain finite/bound, parameter sweep, soak and reset/rest checks.

This is itself an important standardization result: the suite protocol must test the intended processor architecture rather than assuming all delays are impulse-repeat machines or all ModFX are zero-latency.

## Final host production-DSP result

GitHub Actions workflow:

- `Pre-handoff suite`
- run **34604585462** — host production-DSP job PASS
- refined final workflow run **34605006118** — host production-DSP job PASS

Final common production-DSP result:

**29 / 29 PASS**

Units covered:

- SPECTRA
- PARALLAX
- CHORDGHOST
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
- BALLISTIC
- RAINFALL
- SWARMDELAY
- GLITCHREPEAT
- BUCKETLINE
- LONGMEMORY
- SHARD
- ABYSS
- AUREOLE
- NEBULA
- LATTICE CORE
- LATTICE ECHO
- LATTICE SPACE
- LATTICE FIELD

The common gate verifies that the production code can execute under the deterministic host corpus without sanitizer failure, non-finite output, gross unbounded output, common delivery failure, parameter-sweep/state failure, or reset/rest corruption under the exercised conditions.

## Fresh ARM build/package result

An initial combined ARM workflow failed after successfully building the baseline units through LATTICE CORE because the new workflow assumed hyphenated LATTICE output filenames while the existing LATTICE `project.mk` files produce underscore artifact basenames such as `lattice_core`.

This was a workflow naming error, not a DSP compile failure. The workflow was corrected without changing DSP source.

Final workflow:

- `Pre-handoff suite`
- run **34605006118**
- `host-production-dsp`: PASS
- `arm-build-package`: PASS

Final result:

**all 29 tracked units freshly compile and package against the current official Korg logue SDK used at CI runtime.**

The workflow records text/data/BSS sizes and uploads the resulting `.ntkdigunit` set as the `pre-handoff-arm-builds` artifact.

## What this does and does not certify

### Established by this retest

- all 29 tracked production sources pass the new shared deterministic host common gate;
- all 29 freshly ARM-build/package against the current Korg SDK at CI runtime;
- the shared protocol now correctly handles delayed ModFX and capture-based DelFX rather than generating known false negatives;
- no production DSP changes were required to obtain the common-gate PASS.

### Not established by this retest

- actual MkI load/select behavior for newly generated binaries;
- cycle-accurate MkI CPU/deadline margin;
- simultaneous-processor coexistence on the physical NTS-1;
- subjective tone, musicality, effect identity or usefulness;
- every project-specific musical/behavioral assertion from every historical QA sheet.

Therefore **common-gate PASS is not automatically the same as A-level deep coverage** in `PRE_HANDOFF_COVERAGE.md`. FIELD retains the deepest dedicated state/delivery harness. Older units keep their B/C/H depth grade until project-specific assertions are backfilled or the unit is actively reopened.

## Handoff implication

For unchanged historical builds, this sweep substantially reduces ordinary numerical/state/build uncertainty before future work.

For any unit that is actively modified from this point forward, the expected path is:

1. shared common gate PASS;
2. fresh ARM build/package PASS;
3. project-specific assertions for the changed behavior PASS;
4. then DC receives the build mainly for physical load/runtime sanity and tone/musicality.

## Status

**SUITE COMMON ENGINEERING RETEST: PASS — 29/29**

**FULL A-LEVEL UNIT-SPECIFIC COVERAGE: still tracked separately; not falsely promoted by this report.**
