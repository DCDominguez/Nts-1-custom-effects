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

CORE 0.3-1 is now the current MkI reference.

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

**Previous physically tested version:** `0.3-0`  
**Current development candidate:** `0.4-0` (`LatSpace`, custom `revfx`)  
**0.4-0 physical status:** **RETEST REQUIRED**.

### Why 0.3-0 was rejected musically

Physical MkI observations:

- loaded and produced the intended echo/spatial behavior;
- very subtle even at full MIX;
- distorted when run after modulation;
- the echo effect remained audible underneath the distorted quality.

### SPACE 0.4-0 revoice

0.4 is a real architecture/runtime rework rather than a global gain boost:

- one stereo diffusion stage instead of two;
- four fixed integer FDN read heads instead of continuously interpolated/modulated reads;
- FDN backing buffers reduced from 8192 to 4096 floats each;
- DRIFT now moves damping/stereo geometry instead of FDN delay time;
- feedback is mostly linear with an emergency state bound rather than continuous feedback soft limiting;
- full MIX is intentionally wet-dominant: only 12% dry remains at maximum;
- output guard recovery now tracks the currently safe target gain rather than staying stale until raw peak drops below the ceiling.

The A-class SPACE harness now also checks:

- middle-MIX presence;
- full-MIX wet identity and useful level;
- explicit early echo delivery;
- post-overload guard recovery;
- actual production CORE 0.3-1 → SPACE host-chain boundedness/delivery;
- existing decay, DRIFT, stereo, reset and long-soak contracts.

The host chain test does not claim ARM timing equivalence.

See `reports/lattice/2026-09-12_space-0.4-0-revoice.md`.

## ECHO

**Version:** historical `0.3-0` custom `delfx`  
**Physical status:** **LOAD / RUNTIME / MUSICAL PASS for latest reported playing test**.

ECHO remains historical rather than the preferred LATTICE delay stage.

## Current doctrine

- each LATTICE processor must have obvious standalone musical identity;
- multi-effect safety should come from correct local gain structure and bounded runtime/state behavior, not blanket attenuation;
- CORE 0.3-1 and FIELD 0.1-2 are the current preferred MkI pair;
- SPACE is a standalone reverb experiment and must be robust after ModFX even though it is not part of the preferred CORE → FIELD system;
- do not infer support for simultaneous user DELAY + user REVERB on original MkI.

## Next gate

1. Complete SPACE 0.4-0 common/A-class/ARM CI.
2. Hand over only the exact SPACE 0.4-0 binary after those gates pass.
3. Physical MkI test:
   - standalone middle MIX;
   - standalone full MIX;
   - TIME/SPACE and DEPTH/DRIFT sweeps;
   - run after a ModFX and listen for the prior distorted quality;
   - confirm level does not remain collapsed after a loud passage.
4. If SPACE passes, record/freeze it and return to suite-wide delay/reverb/ModFX level calibration.
