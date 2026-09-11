# LATTICE A-class RC physical MkI observations — 2026-09-12

## Status

- Date: 2026-09-12
- Target: original Korg Nu:Tekt NTS-1 digital kit MkI
- Candidate engineering commit: `5718d12416a242137892688e240e8ac41be5a0ca`
- Pre-handoff workflow: `34613566166`
- Physical classification: **PASS WITH NOTES / FAIL-RETEST for specific combinations**

## Why this happened

After the full 29-unit A-class engineering backfill passed, DC tested the release-candidate pack on the physical MkI for loading, runtime behavior, tone and musicality.

This report records only the LATTICE-specific physical observations from that pass.

## Confirmed physical observations

### LATTICE CORE

- Loads and sounds good as a standalone effect.
- At maximum TIME it loops the note as intended/expected from the current freeze/loop behavior.
- Intermittent distortion is still audible at or near maximum TIME.
- When CORE is paired with another effect, distortion can occur; some combinations/runs are clean, so the failure is intermittent rather than universal.

Classification: **PASS WITH NOTES / RETEST**.

This supersedes any assumption that the whole high-TIME range is always unusable. The more precise current observation is that CORE is musically good and can be clean, but distortion still appears intermittently at extreme TIME and in some multi-effect use.

### LATTICE SPACE

- Loads and produces the intended echo/spatial behavior.
- Very subtle even at full MIX.
- Distorts when run with modulation according to the reported test.
- The spatial/echo effect remains audible underneath the distorted sound quality.

Classification: **FAIL / REDESIGN CANDIDATE** for musical quality and combination robustness.

This is not merely a request for more gain. SPACE needs a rework/revoice so that increasing audibility does not worsen combination headroom or distortion.

### LATTICE FIELD

No new problem was reported in this physical pass. Under the user's rule for this test, unlisted units loaded properly and sounded as intended.

Classification for this reported pass: **LOAD PASS / RUNTIME PASS / MUSICAL PASS**.

This does not alter the MkI user-delay/user-reverb slot restriction documented elsewhere.

### LATTICE ECHO

No new problem was reported in this physical pass. Under the user's rule for this test, the unit loaded properly and sounded as intended.

Classification for this reported pass: **LOAD PASS / RUNTIME PASS / MUSICAL PASS**.

ECHO remains historical rather than part of the preferred CORE → FIELD architecture.

## System-level lesson

The current physical evidence suggests two separate problems that should not be solved with one global attenuation strategy:

1. **CORE runtime/combination distortion** — intermittent and strongly associated with extreme TIME / multi-effect use.
2. **SPACE identity/headroom problem** — too subtle alone, yet distorts in modulation combinations.

Reducing every LATTICE stage globally would directly conflict with the user's request that each effect stand strongly on its own.

## Sound-quality intent for the next LATTICE work

- Every processor should be musically obvious enough to stand alone.
- Standalone audibility must not depend on maxing the NTS-1 main volume.
- Multi-effect safety should be created through local gain structure, bounded regenerative paths, workload reduction and sensible wet/dry architecture rather than making the whole effect timid.
- CORE should preserve the current sound/loop behavior while the intermittent distortion is isolated.
- SPACE should be treated as a redesign/revoice project, not a simple make-up-gain patch.

## Runtime/headroom intent

No CPU percentage or device telemetry was measured. Do not relabel CORE's intermittent distortion as a proven CPU overload.

For SPACE, the physical combination failure establishes a real headroom/runtime symptom but does not by itself identify the internal cause.

## Next step / open gates

1. Add reproducible level/loudness instrumentation to the engineering suite so obvious under-level/over-level candidates can be caught before handoff.
2. Preserve FIELD and ECHO unless a later test exposes a regression.
3. Rework SPACE separately with stronger standalone identity plus combination-safe gain structure.
4. Continue CORE isolation at maximum TIME and in representative ModFX + Delay/Reverb combinations.
5. Any changed CORE/SPACE candidate must pass A-class host/build gates again before physical handoff.
