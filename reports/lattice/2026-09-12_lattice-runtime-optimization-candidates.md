# LATTICE behavior-preserving runtime optimization candidates — 2026-09-12

## Status

- Target: original Korg Nu:Tekt NTS-1 digital kit MkI
- Base branch/head: `field-0.1-0-test` at `c665a7824c3179b281c6057ca865b76deacabc28`
- Work branch: `lattice-runtime-optimization`
- Accepted controls remain unchanged: CORE 0.3-1, FIELD 0.1-2, SPACE 0.4-1, SPECTRA 0.2-1
- New candidates: CORE 0.3-2, FIELD 0.1-3, SPACE 0.4-2, SPECTRA 0.2-2
- Classification: **HOST/A-CLASS PASS; ARM BUILD AND PHYSICAL MKI CHECK REQUIRED**

This pass deliberately changes no gain constants, event timing, pitch/interval rules, feedback coefficients, mix laws, voice ceiling, control mapping, or effect topology. The current physical references remain the control group until the candidates pass on the original MkI.

## Optimizations

### CORE 0.3-2

- caches the reciprocal of the captured freeze length once when freeze engages instead of expressing two divisions in the per-sample freeze path;
- skips ordinary-engine voice-count, modulation, and pattern calculations while full freeze owns the processor;
- replaces the constant 320 ms readiness conversion with its exact 48 kHz sample count (`15360`);
- makes the output guard helper `void` because its returned peak was unused.

The history format, ten-voice ceiling, microloop scheduler, wet normalization, freeze playback/crossfade, limiter, and controls are unchanged.

### FIELD 0.1-3

- factors the four-line Hadamard bloom matrix into shared pair sums/differences;
- reuses those pairs for tail readout.

Capture admission, phrase grammar, eight-voice service, foreground protection, bloom coefficients, gain structure, termination, and controls are unchanged.

### SPACE 0.4-2

- factors the accepted four-line FDN Hadamard matrix into shared pair sums/differences;
- compiles hidden-clipping counters only into `LATTICE_TEST` host builds, retaining all safety bounds in production.

The accepted 0.4-1 predelay, diffusion, feedback, damping, drift, wet readout, mix law, emergency bounds, and controls are unchanged.

### SPECTRA 0.2-2

- renders only the waveform pair used by the current SHAPE region rather than calculating sine, triangle, saw, and square for every voice and discarding two or three of them;
- computes the active three-voice ceiling and constant per-block level sum once outside the sample loop;
- avoids per-block increment/table setup for inactive voices.

The intentional three-voice cap, oscillator phases, band-limited table selection, drift, spread, chaos, interval modes, normalization order, soft clipping, and controls are unchanged.

## Differential render evidence

The accepted sources and candidate sources were compiled into separate namespaces and driven with identical deterministic input, parameter changes, clock, and initialization.

| Unit | Comparison | Maximum sample difference | RMS difference |
| --- | --- | ---: | ---: |
| CORE 0.3-1 → 0.3-2 | ordinary field, freeze, and release | `0` | `0` |
| FIELD 0.1-2 → 0.1-3 | capture/mode/clock sweep | `5.96046448e-08` | `1.61504015e-09` |
| SPACE 0.4-1 → 0.4-2 | room/drift sweep with dense stereo input | `5.96046448e-08` | `2.63496366e-09` |
| SPECTRA 0.2-1 → 0.2-2 | all three SHAPE regions, three voices | `0` Q31 units | `0` |

CORE and SPECTRA were bit-identical in these comparisons. FIELD and SPACE differed by at most one single-precision float quantum in the tested signal range due to reassociation of algebraically equivalent sums.

## Existing project-specific test result

- CORE A-class harness: PASS
- FIELD full host suite: PASS, including all 32 mode/division lifecycles, replacement, finite-state containment, headroom, and rest
- SPACE A-class harness: PASS, including mid/full MIX audibility, tail decay, drift/stereo behavior, CORE → SPACE, IRONROT → SPACE, and zero hidden clamp hits
- SPECTRA A-class harness: PASS, including the hard three-voice ceiling
- warning build: PASS; warnings are pre-existing numeric-conversion warnings in the host test code, not new production-source warnings

These host results do not establish original-MkI callback margin or replace physical listening.

## Required gates

1. Fresh ARM compile/package against the current official Korg logue SDK.
2. Confirm manifest/API/category and image sizes.
3. Focused original-MkI A/B against the accepted controls:
   - CORE: upper TIME and maximum freeze, then CORE + FIELD;
   - FIELD: center controls, dense overlap, then CORE + FIELD;
   - SPACE: center/full MIX and the residual top-end region after CORE or another ModFX;
   - SPECTRA: three voices in each SHAPE region and one-to-three-voice switching.
4. Promote individually. A failure in one candidate does not block the other three.

No release or replacement of the accepted reference line is claimed in this report.
