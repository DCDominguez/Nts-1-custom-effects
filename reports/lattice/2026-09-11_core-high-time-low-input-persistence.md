# CORE high-TIME distortion persists at lower input

## Status / physical result
2026-09-11; CORE 0.3-0. DC tested CORE alone around TIME 3 o'clock and higher at a lower incoming source level. Distortion remained. Classification: CORE standalone high-TIME FAIL / RETEST. No source or binary change.

## Updated diagnosis
This materially weakens the signal-overload explanation for the observed high-TIME distortion. Source mapping gives approximately 14 active voices at TIME .75 (roughly 3 o'clock): 1 + floor((((.75-.035)/.865) * 15.999)) = 14. Maximum TIME reaches 16 voices. Each active voice, per 48 kHz sample, checks scheduling, reads/interpolates Q15 history from SDRAM, applies a grain window, mixes stereo and advances state. Higher TIME also increases pitch/rhythm behavior. Hardware evidence and source cost align with a real-time workload failure as the leading hypothesis.

This is not a measured CPU percentage or proof of the exact mechanism. Granular playback behavior at dense high TIME remains a possible contributor. The source's CORE guard and FIELD guard level interaction remain relevant for the pair but no longer explain CORE-alone distortion's input-level independence.

## Proposed scoped fix, pending DC approval
CORE 0.3-1 runtime profile:
1. Cap the ordinary microloop engine at 10 active voices, reducing the reported 3-o'clock workload from about 14 to 10 (about 29% fewer per-sample voice passes). TIME remains a density/behavior control; high TIME reaches a stable rich ceiling rather than a 16-voice overload zone.
2. When full freeze is fully engaged, suspend ordinary voice scheduling and processing. The frozen loop remains audible; this removes confirmed unused work in freeze.
3. Preserve current gain, patterns, loop rules, history and FIELD 0.1-2. Do not merge an unrelated level retune into this diagnostic build.
4. Add tests for 10-voice ceiling, high-TIME state lifetime, freeze ordinary-work suspension and output finite/bounded behavior; ARM build and hardware check at 3 o'clock through maximum with FIELD off, then FIELD on.

Tradeoff: high TIME has fewer independent simultaneous fragments, but should remain dense because existing voice loops repeat and are randomized. No claim that this will fully solve the physical issue until tested.

## Next step
Await DC approval before implementation, consistent with the pause-before-new-improvements instruction. The required physical question for 0.3-1 is whether CORE becomes clean from 3 o'clock through maximum at the same source and listening setup; then test CORE + FIELD.
