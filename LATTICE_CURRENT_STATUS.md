# Human Soon LATTICE — Current Status

**Date:** 2026-09-11  
**Target:** original Korg Nu:Tekt NTS-1 digital kit MkI  
**Active development branch:** `field-0.1-0-test`  
**Stable/reference branch:** `lattice-suite`

This file is the fastest current-state entry point. It summarizes the latest implemented versions, physical MkI findings, unresolved failures, and the next approved decision point. For historical reasoning, read `LATTICE_HISTORY.md`; for chronological evidence, read `reports/lattice/`.

## Current architecture

Preferred MkI system remains:

```text
AUDIO IN / OSC
      ↓
LATTICE CORE   [custom MOD]
      ↓
LATTICE FIELD  [custom DELAY]
      ↓
optional Korg built-in REVERB only
      ↓
OUTPUT
```

User DELAY and user REVERB are not treated as a supported simultaneous pair on MkI because they share SDK memory regions. Albedo is a user REVERB and therefore replaces FIELD rather than proving coexistence with it.

## FIELD

**Current implemented version:** `0.1-2` (`LatField`, custom `delfx`)  
**Branch:** `field-0.1-0-test`  
**Hardware validation:** OPEN overall; standalone listening behavior PASS for the reported test.

FIELD 0.1-2 supersedes the earlier attack-only capture rules. It admits fresh attacks plus CLOCK-spaced captures while source input qualifies, keeps two owned captures, protects the first clear answer from replacement, and may retire lower-priority later events when busy.

### Confirmed physical MkI observations

- FIELD 0.1-2 standalone: user reported it was “perfect” and played exactly as intended. Treat this as a standalone listening PASS, not a 30-minute stability certification.
- Corrosion + FIELD: reported clean / working perfectly in the tested playing context.
- Tested built-in MOD effects + FIELD: reported without the CORE-specific distortion seen later.
- Albedo + FIELD: does not operate as an independent simultaneous pair; this is consistent with the MkI user DELAY / user REVERB slot-memory limitation.
- Earlier reports of occasional distortion with some built-in effects and other custom combinations are not fully resolved; do not generalize beyond the exact confirmed pairings above.

### FIELD status rule

Do not reduce FIELD sound quality merely to compensate for failures proven to originate in CORE or in unsupported DELAY+REVERB combinations. FIELD 0.1-2 is the current standalone sonic reference.

## CORE

**Current implemented version:** `0.3-0`  
**Hardware validation:** OPEN for high TIME.

### Confirmed physical MkI observation

CORE alone is clean through much of the TIME range, but distortion appears around the user's approximate **3 o'clock and higher** TIME region. Lowering incoming source level did **not** remove that distortion.

This materially weakens a simple input/headroom-overload explanation for the CORE-alone failure.

Current source maps that region to roughly 14 active microloop voices, rising to 16 at maximum TIME. The per-sample workload and granular density both increase with TIME. A real-time workload failure is therefore the leading hypothesis, but there is no measured CPU percentage and dense granular playback artifacts are still a possible contributor.

## CORE + FIELD

**Status:** FAIL / RETEST.

Physical testing identified CORE + FIELD as the supported pair with the most distortion. A host-side chain probe using the actual production DSP reproduced substantial guard engagement as CORE activity rose, but did not produce full-scale clipping or non-finite output. That probe diagnoses a strong level/work interaction; it does not reproduce the exact audible hardware failure or prove a CPU cause.

The probe also confirmed that CORE continues ordinary microloop scheduling during settled freeze even when that work contributes negligibly to the audible output.

## Proposed next diagnostic build — pending approval

Proposed `CORE 0.3-1` runtime profile:

1. Cap the ordinary microloop engine at **10 active voices** instead of 16.
2. Preserve current gain, patterns, history, loop rules, and FIELD 0.1-2.
3. When full freeze is engaged, suspend ordinary microloop scheduling/processing and keep only the frozen-loop output path active.
4. Add automated checks for the 10-voice ceiling, freeze work suspension, state lifetime, and finite/bounded output.
5. Physically test CORE alone from the reported ~3 o'clock region through maximum, then repeat with FIELD enabled.

This is a scoped runtime diagnostic, not yet an implemented fix and not a claim that 10 voices will solve the hardware issue.

## Current evidence hierarchy

### Physical MkI — strongest evidence

- FIELD 0.1-2 standalone: PASS for reported listening behavior.
- Corrosion + FIELD: PASS for reported playing test.
- CORE 0.3-0 high TIME: FAIL / RETEST; distortion persists at lower input.
- CORE + FIELD: FAIL / RETEST.
- Albedo + FIELD: unsupported user REVERB + user DELAY arrangement.

### Host/CI evidence — useful but not hardware validation

- FIELD 0.1-2 builds successfully through the dedicated workflow.
- Host-side production-DSP chain probe shows increasing CORE/FIELD guard activity with CORE TIME and confirms avoidable ordinary CORE work during freeze.
- No ARM cycle measurement or hardware CPU telemetry exists.

## Guardrails carried forward

- Compile/CI success is never hardware validation.
- Preserve FIELD 0.1-2 as the current sound-quality reference unless hardware evidence requires a FIELD change.
- Do not call the CORE failure “CPU overload” as a measured fact; call it the leading runtime-workload hypothesis.
- Do not solve supported-pair failures by degrading audible quality before removing demonstrably unnecessary work.
- Keep update reports under `reports/lattice/` for every meaningful build, test, failure, diagnosis, or decision.

## Next decision point

Await approval before implementing CORE 0.3-1. If approved, the primary success question is:

> Does CORE become clean from the reported ~3 o'clock TIME region through maximum on the physical MkI, first standalone and then with FIELD 0.1-2?

If not, preserve the failure and investigate granular playback/runtime structure rather than immediately lowering FIELD quality or overall output level.
