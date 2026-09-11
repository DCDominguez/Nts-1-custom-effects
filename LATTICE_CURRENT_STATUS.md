# Human Soon LATTICE — Current Status

**Date:** 2026-09-12  
**Target:** original Korg Nu:Tekt NTS-1 digital kit MkI  
**Active development branch:** `field-0.1-0-test`  
**Stable/reference branch:** `lattice-suite`

This is the fastest current-state entry point. For chronological evidence, read `reports/lattice/`; for historical reasoning, read `LATTICE_HISTORY.md`.

## Current architecture

Preferred MkI system:

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

User DELAY and user REVERB are not treated as a supported independent simultaneous pair on MkI because they share first-generation SDK memory regions.

## CORE

**Version:** `0.3-1`  
**Engineering status:** **A-CLASS / ARM PASS**  
**Physical status:** **PASS for reported focused playing test**  
**CORE + FIELD 0.1-2:** **PASS for reported focused combination test**.

CORE 0.3-1 is the current MkI reference.

Changes from 0.3-0:

1. ordinary microloop ceiling reduced from 16 to 10 voices;
2. full freeze suspends ordinary microloop scheduling, reads, phase advancement and repeat/wait state advancement;
3. history format/length, patterns, pitch rules, loop rules, wet-drive formula, freeze path, guard and limiter were preserved.

The exact candidate passed workflow `34628866540` and then passed the requested original-NTS-1 MkI checks at upper/max TIME both standalone and with FIELD 0.1-2.

This supports keeping the reduced-workload profile but does not prove a measured CPU cause; no MkI cycle/CPU telemetry exists.

See:

- `reports/lattice/2026-09-12_core-0.3-1-runtime-profile.md`
- `reports/lattice/2026-09-12_core-0.3-1-physical-pass.md`

## FIELD

**Version:** `0.1-2` (`LatField`, custom `delfx`)  
**Physical status:** **LOAD / RUNTIME / MUSICAL PASS for reported playing tests**.

FIELD remains the preferred delay/spatial stage for the MkI LATTICE architecture and passed the latest focused pairing with CORE 0.3-1.

Preserve FIELD's current sound unless later physical evidence requires a change.

## SPACE

**Last physically tested candidate:** `0.4-0` (`LatSpace`, custom `revfx`)  
**0.4-0 engineering status:** **A-CLASS / ARM PASS**  
**0.4-0 physical combination status:** **FAIL / RETEST**  
**Current development candidate:** `0.4-1` guard/headroom diagnostic  
**0.4-1 physical status:** **NOT YET TESTED**.

### New 0.4-0 physical result

Literal tester report:

> “still hitting distortion on core with space. actually any modulation plus space i get distortion”

This means the current failure should be localized to SPACE rather than reopening CORE generally:

- CORE 0.3-1 standalone: PASS in the immediately preceding focused test;
- CORE 0.3-1 + FIELD 0.1-2: PASS;
- CORE 0.3-1 + SPACE 0.4-0: distortion reported;
- tester further reports distortion with **any modulation + SPACE** in the current MkI test.

No exact CPU/deadline, clipping or analog cause is inferred from the sound alone.

Issue #9 comment `5639386820` records this result.

### Why 0.4-1 exists

SPACE 0.4-0 still had a sample-by-sample dynamic output guard. Whenever the raw wet/dry sum exceeded the ceiling, gain was changed immediately according to the current peak. That can create an audible nonlinear amplitude process on complex ModFX output even if the final samples stay numerically bounded.

The 0.4-0 A-class chain test checked finite/bounded/useful output, but it did not reject frequent internal FDN safety-bound hits or repeated guard action. A numerically bounded signal can still sound distorted.

0.4-1 therefore isolates this mechanism before a more destructive topology rewrite:

1. removes the dynamic output guard completely;
2. keeps only an emergency final output bound;
3. reduces diffusion/FDN injection and maximum feedback for more internal headroom;
4. keeps strong wet presence by increasing readout weighting instead of driving the feedback network harder;
5. adds explicit counters for FDN state-bound hits and emergency output-bound hits;
6. expands the A-class harness so CORE → SPACE and IRONROT → SPACE must produce useful audio with **zero hidden clamp hits**.

Production source commit: `15382a34c25b94aefa36c84f2056ab7f32d42354`  
Manifest bump: `d6d9566cf9499498fbbd48a5306bd07e05a4b3c0`  
Harness update: `03d2ec273bbe4aea56aacb447610ca46194350c7`.

See `reports/lattice/2026-09-12_space-0.4-1-guard-isolation.md`.

### Fallback if 0.4-1 still distorts

If the exact 0.4-1 build passes its host/ARM gates but still distorts after ModFX on physical MkI, stop tuning gain/limiting and move to a substantially cheaper SPACE topology:

- two-line cross-coupled room instead of four-line FDN;
- fewer SDRAM reads/writes;
- no separate diffusion bank if possible;
- preserve early echo identity, dark stereo movement and long-room control with a lower runtime footprint.

That next step would specifically test the remaining runtime/deadline hypothesis.

## ECHO

**Version:** historical `0.3-0` custom `delfx`  
**Physical status:** **LOAD / RUNTIME / MUSICAL PASS for latest reported playing test**.

ECHO remains historical rather than the preferred LATTICE delay stage.

## Current doctrine

- each LATTICE processor must have obvious standalone musical identity;
- multi-effect safety should come from correct local gain structure and bounded runtime/state behavior, not blanket attenuation;
- CORE 0.3-1 and FIELD 0.1-2 are the current preferred MkI pair;
- SPACE is a standalone reverb experiment and must be robust after ModFX even though it is not part of the preferred CORE → FIELD system;
- physical MkI evidence outranks host-model assumptions;
- do not infer support for simultaneous user DELAY + user REVERB on original MkI.

## Next gate

1. Complete host/A-class and fresh ARM build/package for SPACE 0.4-1.
2. If engineering gates pass, physically test the exact 0.4-1 binary after at least one built-in ModFX and CORE 0.3-1.
3. If distortion is gone, record whether middle/full MIX and SPACE/DRIFT musical identity remain acceptable.
4. If distortion remains, move directly to the lower-runtime two-line SPACE diagnostic instead of further gain tweaks.
