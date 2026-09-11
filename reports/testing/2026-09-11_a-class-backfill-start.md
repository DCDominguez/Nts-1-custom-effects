# A-class backfill start + platform expansion — 2026-09-11

## Purpose

Begin the project-specific testing layer required to move the Human Soon suite from shared common-gate coverage toward A-class engineering coverage, while also defining a safe path to additional Korg logue SDK platforms.

No production DSP source was changed in this cycle.

## Shared measurement library

Added `tests/pre-handoff/common/measure.hpp` with reusable host-side measurements for:

- mean/RMS/peak/energy;
- stereo channel extraction;
- maximum signal difference;
- normalized correlation;
- arbitrary-frequency tone power;
- zero-crossing frequency estimation;
- transition/update counting;
- approximate output-level counting;
- windowed frequency variance.

These helpers are intended to keep project-specific assertions small and auditable rather than creating one bespoke signal-analysis implementation per unit.

## Project-specific runner

Added:

- `tests/pre-handoff/run_unit_specific.py`
- `tests/pre-handoff/unit_specific/test_spectra.cpp`
- `tests/pre-handoff/unit_specific/test_dust.cpp`

The main `Pre-handoff suite` workflow now runs:

1. the 29-unit shared production-DSP common gate;
2. checked-in project-specific A-class harnesses;
3. fresh ARM compile/package against the current official Korg logue SDK.

## SPECTRA pilot

The SPECTRA harness uses the actual production source and currently verifies:

- equal-tempered single-voice pitch sanity at 220/440/880 Hz test notes;
- SHAPE endpoint harmonic enrichment;
- Voices + Spread producing measurable detuned side energy;
- HarmMode + ALT delivering the documented root/fifth/octave/sub-octave relationship for the tested mode;
- Drift producing bounded pitch wander;
- Motion changing drift rate;
- Chaos=0 remaining seed-independent after identical initialization;
- Chaos=max producing bounded seeded variation.

An early Chaos assertion failed because the test compared two runs without resetting all patch/slew state. The harness was corrected to compare independently initialized identical patches. This was a test setup defect, not a production DSP regression.

Current result: **project-specific harness PASS**.

This is strong A-class progress, but coverage grade promotion should still be reviewed against the full A1-A8 checklist before changing `PRE_HANDOFF_COVERAGE.md`.

## DUST pilot

The DUST harness uses the actual production source and currently verifies:

- DEPTH=0 dry-path integrity;
- RATE materially changes sample-hold update cadence;
- DAMAGE materially increases destructive deviation from dry;
- maximum DAMAGE exposes bounded low-bit staircase behavior;
- stereo fracture is absent at mild settings and measurable/bounded at destructive settings;
- low-level DC remains bounded;
- suspend/resume clears stale held audio;
- repeated control abuse/soak remains finite and bounded.

### Important test-design finding

The first RATE assertion incorrectly counted microscopic residual dry-path changes at maximum DAMAGE as sample-hold updates. Because DUST slews the wet coefficient toward its target, the output can retain a numerically tiny dry contribution even when perceptually fully damaged. A threshold intended to detect held/quantized output changes must sit above that residual floor.

The test was corrected to count output changes above a musically/materially relevant threshold rather than weakening the production contract.

Current result: **project-specific harness PASS**.

No production DSP changes were required for this RATE finding.

## Final CI result for pilot

Workflow: **Pre-handoff suite**  
Run: **34608296543**

- shared 29-unit production-DSP common gate: **PASS**;
- SPECTRA project-specific harness: **PASS**;
- DUST project-specific harness: **PASS**;
- fresh ARM compile/package job: **PASS**.

## Next A-class work

Priority remains:

1. review SPECTRA and DUST against full A1-A8 requirements before grade promotion;
2. LATTICE CORE deep runtime/state harness because its physical high-TIME failure remains open;
3. CARRIER, VECTORFILTER, ATTRACTOR and CHORDGHOST project-specific contracts;
4. reusable family analyzers for modulation, delay, reverb, nonlinear and capture processors;
5. then the remaining active M1 units.

A failing project-specific test should be treated as evidence to investigate, not as a threshold to relax until green.

---

# Multi-platform expansion

Added `PLATFORM_SUPPORT.md` after checking the current official Korg `logue-sdk` repository and platform READMEs.

Current official targets include:

- prologue;
- minilogue xd;
- NTS-1 MkI;
- NTS-1 MkII;
- NTS-3 kaoss pad kit;
- microKORG2;
- drumlogue.

The expansion strategy is **portable DSP core + thin product adapter**, with the MkI production behavior protected by A-class regression tests before refactoring.

Key conclusions:

- minilogue xd is the lowest-friction additional v1 target;
- prologue shares the v1 generation but ModFX needs explicit main/sub-timbre handling rather than blindly shipping MkI code that ignores the secondary buffer;
- NTS-1 MkII is a high-priority target because it retains osc/modfx/delfx/revfx, provides substantially larger effect memory, and has independent custom Delay/Reverb runtimes;
- NTS-3 exposes four `genericfx` runtimes with touch/XY events and large external memory, making it attractive for performance-oriented Human Soon ports;
- microKORG2 exposes the familiar four unit categories through the newer v2.1 generation;
- drumlogue DelFX/RevFX are direct conceptual candidates, while oscillator/ModFX concepts require synth/masterfx-specific redesign.

Korg's current SDK also contains a browser/WebAudio simulator path for NTS-1 MkII and NTS-3; this should become an additional community pre-hardware test layer for those targets.

Cross-platform build or host PASS never inherits hardware validation from another product.
